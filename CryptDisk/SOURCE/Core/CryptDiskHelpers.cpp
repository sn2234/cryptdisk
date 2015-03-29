
#include "stdafx.h"
#include "CryptDiskHelpers.h"
#include "DriverProtocol.h"
#include "winapi_exception.h"
#include "DiskHeaderTools.h"

#include "SafeHandle.h"
#include "DNDriverControl.h"
#include "VolumeTools.h"

using namespace std;
using namespace CryptoLib;
using namespace boost;
namespace bsys = boost::system;
namespace bfs = boost::filesystem;

namespace
{
	static const WCHAR pathPrefix [] = L"\\??\\";

	void SendBroadcastMessage(WPARAM message, WCHAR driveLetter)
	{
		std::string driveRoot;

		driveRoot += (char) driveLetter;
		driveRoot += ":\\";

		switch (message)
		{
		case DBT_DEVICEARRIVAL:
			SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, driveRoot.c_str(), NULL);
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, driveRoot.c_str(), NULL);
			break;
		}

		DEV_BROADCAST_VOLUME	data;

		data.dbcv_size = sizeof(data);
		data.dbcv_devicetype = DBT_DEVTYP_VOLUME;
		data.dbcv_reserved = 0;
		data.dbcv_flags = 0;
		data.dbcv_unitmask = 1 << (driveLetter - L'A');

		DWORD_PTR dwResult;
		SendMessageTimeout(HWND_BROADCAST, WM_DEVICECHANGE, message, (LPARAM) (&data), SMTO_ABORTIFHUNG, 1000, &dwResult);
	}

	void DoEncrypt(HANDLE hVolume, __int64 length, CryptoLib::IRandomGenerator* pRndGen, DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool(double)> callback)
	{
		// Prepare disk header
		DISK_HEADER_V4 header;

		DiskHeaderTools::Initialize(&header, pRndGen);

		const size_t  sectorSize = 512;

		header.DiskSectorSize = sectorSize;
		header.ImageOffset = 2 * sizeof(DISK_HEADER_V4); // Reserve place for another header for hidden volume

		DiskHeaderTools::Encipher(&header, password, static_cast<ULONG>(passwordLength), cipherAlgorithm);

		// Write header
		{
			DWORD dwResult;
			if (!WriteFile(hVolume, static_cast<LPCVOID>(&header), sizeof(header), &dwResult, nullptr))
			{
				throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
			}
		}

		// Init CTR cipher
		unsigned char key[RijndaelEngine::KeySize];

		pRndGen->GenerateRandomBytes(key, sizeof(key));

		RijndaelEngine cipher;

		cipher.SetupKey(key);

		union
		{
			UINT64 counter;
			unsigned char buffer[RijndaelEngine::BlockSize];
		};

		pRndGen->GenerateRandomBytes(buffer, sizeof(buffer));

		UINT64 bytesToFill = fillImageWithRandom ? length : 2 * sizeof(DISK_HEADER_V4); // Write random data over reserved area always
		UINT64 bytesFilled = sizeof(DISK_HEADER_V4);
		std::vector<unsigned char> writeBuffer(0x10000);

		int callbackCounter = 0;
		do
		{
			for (size_t i = 0; i < writeBuffer.size() / RijndaelEngine::BlockSize; i++)
			{
				cipher.EncipherBlock(buffer, &writeBuffer[0] + i * RijndaelEngine::BlockSize);
				counter++;
				bytesFilled += RijndaelEngine::BlockSize;
			}

			if (callbackCounter++ > 255)
			{
				if (!callback(min((double) bytesFilled / (double) bytesToFill, 1.0)))
				{
					break;
				}
				callbackCounter = 0;
			}

			{
				DWORD dwResult;

				DWORD bytesToWrite = static_cast<DWORD>(min(bytesToFill, writeBuffer.size()) -
					(bytesFilled > bytesToFill ? bytesFilled - bytesToFill : 0));

				if (!WriteFile(hVolume, static_cast<LPCVOID>(&writeBuffer[0]),
					bytesToWrite, &dwResult, nullptr))
				{
					throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
				}
			}

		} while (bytesFilled < bytesToFill);
	}

	__int64 GetVolumeLength(HANDLE hVolume)
	{
		DWORD dwResult;
		GET_LENGTH_INFORMATION	lengthInfo;

		if (!::DeviceIoControl(hVolume, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof(lengthInfo), &dwResult, NULL))
		{
			throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
		}

		return lengthInfo.Length.QuadPart;
	}

	bool CheckHeader(const vector<unsigned char>& header, const unsigned char* password, size_t passwordLength)
	{
		vector<unsigned char> headerBuff(header);

		// Decipher disk header
		DiskHeaderTools::CIPHER_INFO cipherInfo;
		bool bResult = DiskHeaderTools::Decipher(&headerBuff[0], password, static_cast<ULONG>(passwordLength), &cipherInfo);

		RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
		RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());

		return bResult;

	}

	void ChangeHeaderPassowrd(CryptoLib::IRandomGenerator* pRndGen, const unsigned char* password, size_t passwordLength,
		const unsigned char* passwordNew, size_t passwordNewLength, vector<unsigned char>& headerBuff)
	{
		// Decipher disk header
		DiskHeaderTools::CIPHER_INFO cipherInfo;
		if (!DiskHeaderTools::Decipher(&headerBuff[0], password, static_cast<ULONG>(passwordLength), &cipherInfo))
		{
			throw logic_error("Wrong password");
		}

		SCOPE_EXIT{
			RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
		};

		// Encipher with new password
		switch (cipherInfo.versionInfo.formatVersion)
		{
		case DISK_VERSION::DISK_VERSION_3:
		{
			DISK_HEADER_V3 *pHeader = reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]);
			pRndGen->GenerateRandomBytes(pHeader->DiskSalt, sizeof(pHeader->DiskSalt));
			DiskHeaderTools::Encipher(pHeader, passwordNew, static_cast<ULONG>(passwordNewLength), cipherInfo.diskCipher);
		}
		break;
		case DISK_VERSION::DISK_VERSION_4:
		{
			DISK_HEADER_V4 *pHeader = reinterpret_cast<DISK_HEADER_V4*>(&headerBuff[0]);
			pRndGen->GenerateRandomBytes(pHeader->DiskSalt, sizeof(pHeader->DiskSalt));
			DiskHeaderTools::Encipher(pHeader, passwordNew, static_cast<ULONG>(passwordNewLength), cipherInfo.diskCipher);
		}
		break;
		default:
			throw logic_error("Wrong passrwod");
			break;
		}

	}
}

void CryptDiskHelpers::MountImage( DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const unsigned char* password, size_t passwordLength, ULONG mountOptions )
{
	size_t	structSize = sizeof(DISK_ADD_INFO) + sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR);

	vector<unsigned char> buffer(structSize);

	DISK_ADD_INFO* pInfo = reinterpret_cast<DISK_ADD_INFO*>(&buffer[0]);

	pInfo->DriveLetter = driveLetter;
	pInfo->MountOptions = mountOptions;
	pInfo->PathSize = static_cast<ULONG>(sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR));
	wcscpy_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), pathPrefix);
	wcscat_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), imagePath);

	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));


	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if(!DiskHeaderTools::Decipher(&headerBuff[0], password, static_cast<ULONG>(passwordLength), &cipherInfo))
	{
		throw logic_error("Wrong password");
	}

	// Fill cryptographic parameters
	pInfo->DiskFormatVersion = cipherInfo.versionInfo.formatVersion;
	copy(begin(cipherInfo.initVector), end(cipherInfo.initVector), pInfo->InitVector);
	copy(begin(cipherInfo.userKey), end(cipherInfo.userKey), pInfo->UserKey);
	pInfo->wAlgoId = static_cast<USHORT>(cipherInfo.diskCipher);

	RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
	RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());

	DWORD drvResult = driverControl.Control(IOCTL_VDISK_ADD_DISK, &buffer[0], static_cast<ULONG>(buffer.size()));
	RtlSecureZeroMemory(&buffer[0], buffer.size());
	if(!drvResult)
	{
		throw winapi_exception("DeviceIoControl error");
	}

	SendBroadcastMessage(DBT_DEVICEARRIVAL, driveLetter);
}

void CryptDiskHelpers::MountVolume(DNDriverControl& driverControl, const WCHAR* volumePath, WCHAR driveLetter, const unsigned char* password, size_t passwordLength, ULONG mountOptions)
{
	std::wstring volumeName = VolumeTools::prepareVolumeName(volumePath);

	size_t	structSize = sizeof(DISK_ADD_INFO) + volumeName.size()*sizeof(WCHAR) + sizeof(WCHAR);

	vector<unsigned char> buffer(structSize);

	DISK_ADD_INFO* pInfo = reinterpret_cast<DISK_ADD_INFO*>(&buffer[0]);

	pInfo->DriveLetter = driveLetter;
	pInfo->MountOptions = mountOptions;
	pInfo->PathSize = static_cast<ULONG>(volumeName.size()*sizeof(WCHAR) + sizeof(WCHAR));
	wcscpy_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), volumeName.c_str());

	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(volumeName.c_str())));


	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if (!DiskHeaderTools::Decipher(&headerBuff[0], password, static_cast<ULONG>(passwordLength), &cipherInfo))
	{
		throw logic_error("Wrong password");
	}

	// Fill cryptographic parameters
	pInfo->DiskFormatVersion = cipherInfo.versionInfo.formatVersion;
	copy(begin(cipherInfo.initVector), end(cipherInfo.initVector), pInfo->InitVector);
	copy(begin(cipherInfo.userKey), end(cipherInfo.userKey), pInfo->UserKey);
	pInfo->wAlgoId = static_cast<USHORT>(cipherInfo.diskCipher);

	RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
	RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());

	DWORD drvResult = driverControl.Control(IOCTL_VDISK_ADD_DISK, &buffer[0], static_cast<ULONG>(buffer.size()));
	RtlSecureZeroMemory(&buffer[0], buffer.size());
	if (!drvResult)
	{
		throw winapi_exception("DeviceIoControl error");
	}

	SendBroadcastMessage(DBT_DEVICEARRIVAL, driveLetter);
}

std::vector<MountedImageInfo> CryptDiskHelpers::ListMountedImages( DNDriverControl& driverControl )
{
	std::vector<MountedImageInfo> tmp;

	// Get images count
	DISKS_COUNT_INFO disksCount;
	{
		DWORD result;
		if(!driverControl.Control(IOCTL_VDISK_GET_DISKS_COUNT, &disksCount, sizeof(disksCount), &result))
		{
			throw winapi_exception("DeviceIoControl error when querying count of mounted images");
		}
	}

	// Get images
	{
		size_t bufferSize = 0;
		DWORD result = 0;
		if(!driverControl.Control(IOCTL_VDISK_GET_DISKS_INFO, &bufferSize, sizeof(bufferSize), &result))
		{
			if(GetLastError() == ERROR_MORE_DATA)
			{
				vector<unsigned char> buffer(bufferSize);

				if(!driverControl.Control(IOCTL_VDISK_GET_DISKS_INFO, &buffer[0], static_cast<ULONG>(buffer.size()), &result))
				{
					throw winapi_exception("DeviceIoControl error when querying mounted images");
				}

				size_t i = 0;
				while (i < (buffer.size() - sizeof(DISK_BASIC_INFO)))
				{
					DISK_BASIC_INFO* pInfo;

					pInfo = reinterpret_cast<DISK_BASIC_INFO*>(&buffer[i]);
					tmp.push_back(MountedImageInfo(pInfo->DiskId, pInfo->DriveLetter, pInfo->OpenCount, pInfo->FileSize.QuadPart, pInfo->FilePath));

					i += sizeof(DISK_BASIC_INFO) + pInfo->PathSize - sizeof(WCHAR);
				}
			}
			else
			{
				throw winapi_exception("DeviceIoControl error when querying mounted images");
			}
		}
	}

	return std::move(tmp);
}

void CryptDiskHelpers::UnmountImage( DNDriverControl& driverControl, ULONG id, bool forceUnmount )
{
	// Get disk letter

	auto images = ListMountedImages(driverControl);

	auto it = std::find_if(images.cbegin(), images.cend(), [id](const MountedImageInfo& i){
		return i.diskId == id;
	});

	if(it == images.cend())
	{
		return;
	}

	SendBroadcastMessage(DBT_DEVICEREMOVECOMPLETE, it->driveLetter[0]);

	DISK_DELETE_INFO info;

	info.DiskId = id;
	info.bForce = forceUnmount ? TRUE : FALSE;

	if(!driverControl.Control(IOCTL_VDISK_DELETE_DISK, &info, sizeof(info)))
	{
		throw winapi_exception("UnmountImage: DeviceIoControl failed");
	}

	//SendBroadcastMessage(DBT_DEVICEREMOVECOMPLETE, driveLetter);
}

void CryptDiskHelpers::CreateImage( CryptoLib::IRandomGenerator* pRndGen,
	const WCHAR* imagePath, INT64 imageSize, DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool fillImageWithRandom,
	std::function<bool (double)> callback)
{
	// Prepare disk header
	DISK_HEADER_V4 header;

	DiskHeaderTools::Initialize(&header, pRndGen);

	const size_t  sectorSize = 512;

	header.DiskSectorSize = sectorSize;
	header.ImageOffset = 2*sizeof(DISK_HEADER_V4); // Reserve place for another header for hidden volume

	DiskHeaderTools::Encipher(&header, password, static_cast<ULONG>(passwordLength), cipherAlgorithm);

	// Create file
	HANDLE hFile(CreateFileW(imagePath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL));
	if(hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("CreateImage: unable to create image file");
	}

	SCOPE_EXIT { CloseHandle(hFile); };

	// Correct image size
	imageSize -= imageSize % sectorSize;

	LARGE_INTEGER fileSize;
	LARGE_INTEGER oldSize;

	fileSize.QuadPart = imageSize + 2*sizeof(DISK_HEADER_V4);

	if(!SetFilePointerEx(hFile, fileSize, &oldSize, FILE_BEGIN) || !SetEndOfFile(hFile))
	{
		throw winapi_exception("CreateImage: unable to set file size");
	}

	{
		HANDLE hFileMapping(CreateFileMapping(hFile, NULL, PAGE_READWRITE, fileSize.HighPart, fileSize.LowPart, NULL));
		if(hFileMapping == 0)
		{
			throw winapi_exception("CreateImage: unable to create file mapping");
		}

		SCOPE_EXIT { CloseHandle(hFileMapping); };

		// Write header
		LARGE_INTEGER offsetMapped;
		const size_t bytesToMap = 1024*1024*4;

		offsetMapped.QuadPart = 0;

		void *pMap = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, offsetMapped.HighPart, offsetMapped.LowPart, bytesToMap);

		SCOPE_EXIT { UnmapViewOfFile(pMap); };

		*((DISK_HEADER_V4*)pMap) = header;

		// Init CTR cipher
		unsigned char key[RijndaelEngine::KeySize];

		pRndGen->GenerateRandomBytes(key, sizeof(key));

		RijndaelEngine cipher;

		cipher.SetupKey(key);

		union
		{
			UINT64 counter;
			unsigned char buffer[RijndaelEngine::BlockSize];
		};

		pRndGen->GenerateRandomBytes(buffer, sizeof(buffer));

		unsigned char *pBlock = (unsigned char*)pMap + sizeof(DISK_HEADER_V4);
		UINT64 bytesToFill = fillImageWithRandom ? fileSize.QuadPart : 2*sizeof(DISK_HEADER_V4); // Write random data over reserved area always
		UINT64 bytesFilled = sizeof(DISK_HEADER_V4);
		size_t bytesFilledInCurrentView = sizeof(DISK_HEADER_V4);

		int callbackCounter = 0;
		do 
		{
			cipher.EncipherBlock(buffer, pBlock);
			counter++;
			pBlock += RijndaelEngine::BlockSize;
			bytesFilledInCurrentView += RijndaelEngine::BlockSize;
			bytesFilled += RijndaelEngine::BlockSize;

			if(callbackCounter++ > 255)
			{
				if(!callback((double)bytesFilled/(double)bytesToFill))
				{
					break;
				}
				callbackCounter = 0;
			}

			if(bytesFilledInCurrentView >= bytesToMap)
			{
				UnmapViewOfFile(pMap);
				offsetMapped.QuadPart += bytesToMap;
				pMap = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, offsetMapped.HighPart, offsetMapped.LowPart,
					static_cast<size_t>(min(fileSize.QuadPart - offsetMapped.QuadPart, bytesToMap)));
				pBlock = (unsigned char*)pMap;
				bytesFilledInCurrentView = 0;
			}
		} while (bytesFilled < bytesToFill);
	}
}

bool CryptDiskHelpers::CheckImage( const WCHAR* imagePath, const unsigned char* password, size_t passwordLength )
{
	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));

	return CheckHeader(headerBuff, password, passwordLength);
}

bool CryptDiskHelpers::CheckVolume(const WCHAR* volumeId, const unsigned char* password, size_t passwordLength)
{
	std::wstring volumeName = VolumeTools::prepareVolumeName(volumeId);

	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(volumeName.c_str())));

	return CheckHeader(headerBuff, password, passwordLength);
}

void CryptDiskHelpers::ChangePasswordVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeId, const unsigned char* password, size_t passwordLength,
	const unsigned char* passwordNew, size_t passwordNewLength)
{
	std::wstring volumeName = VolumeTools::prepareVolumeName(volumeId);

	vector<unsigned char> headerBuff((ReadImageHeader(volumeName.c_str())));

	SCOPE_EXIT{
		RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());
	};

	ChangeHeaderPassowrd(pRndGen, password, passwordLength, passwordNew, passwordNewLength, headerBuff);

	HANDLE hFile = ::CreateFileW(volumeName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT{ ::CloseHandle(hFile); };

	// Lock
	DWORD dwResult;
	if (!::DeviceIoControl(hFile, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL))
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT{
		DWORD dwResult;
		::DeviceIoControl(hFile, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL);
	};

	DWORD bytesWritten;
	BOOL result = WriteFile(hFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesWritten, NULL);
	if (!result)
	{
		int err = GetLastError();
		throw winapi_exception("Error writing disk header", err);
	}
}

void CryptDiskHelpers::ChangePassword( CryptoLib::IRandomGenerator* pRndGen, const WCHAR* imagePath, const unsigned char* password, size_t passwordLength,
	const unsigned char* passwordNew, size_t passwordNewLength )
{
	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));

	SCOPE_EXIT{
		RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());
	};

	ChangeHeaderPassowrd(pRndGen, password, passwordLength, passwordNew, passwordNewLength, headerBuff);

	// Write header
	HANDLE hFile = CreateFileW(imagePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image for write");
	}

	SCOPE_EXIT{
		CloseHandle(hFile);
	};

	DWORD bytesWritten;
	BOOL result = WriteFile(hFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesWritten, NULL);
	if(!result)
	{
		int err = GetLastError();
		throw winapi_exception("Error writing disk header", err);
	}
}

std::vector<unsigned char> CryptDiskHelpers::ReadVolumeHeader(const WCHAR* volumeId)
{
	std::wstring volumeName = VolumeTools::prepareVolumeName(volumeId);

	return ReadImageHeader(volumeName.c_str());
}

std::vector<unsigned char> CryptDiskHelpers::ReadImageHeader( const WCHAR* imagePath )
{
	size_t headerSize = max(sizeof(DISK_HEADER_V3), sizeof(DISK_HEADER_V4));
	vector<unsigned char> headerBuff(headerSize);

	HANDLE hFile = CreateFileW(imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image");
	}

	SCOPE_EXIT { CloseHandle(hFile); };

	DWORD bytesRead;
	BOOL result = ReadFile(hFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesRead, NULL);
	if(!result)
	{
		int err = GetLastError();
		throw winapi_exception("Error reading disk header", err);
	}
	
	return std::move(headerBuff);
}

void CryptDiskHelpers::RestoreImageHeader(const std::string& imageFile, const std::string& backupFile)
{
	if (!bfs::exists(imageFile))
	{
		throw logic_error("Image file doesn't exist");
	}

	if (!bfs::exists(backupFile))
	{
		throw logic_error("Backup file doesn't exist");
	}

	HANDLE hBackupFile = CreateFileA(backupFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hBackupFile == INVALID_HANDLE_VALUE)
	{
		throw logic_error("Unable to open backup file");
	}

	SCOPE_EXIT { CloseHandle(hBackupFile); };


	HANDLE hImageFile = CreateFileA(imageFile.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hImageFile == INVALID_HANDLE_VALUE)
	{
		throw logic_error("Unable to open image file");
	}

	SCOPE_EXIT { CloseHandle(hImageFile); };

	size_t backupFileSize = GetFileSize(hBackupFile, NULL);

	std::vector<unsigned char> backupBuff(backupFileSize);
	{
		DWORD bytesRead;
		BOOL result = ReadFile(hBackupFile, &backupBuff[0], static_cast<DWORD>(backupBuff.size()), &bytesRead, NULL);
		if (!result || !(bytesRead == backupFileSize))
		{
			throw logic_error("Unable to read data");
		}
	}

	{
		DWORD bytesWrite;
		BOOL result = WriteFile(hImageFile, &backupBuff[0], static_cast<DWORD>(backupBuff.size()), &bytesWrite, NULL);
		if (!result || !(bytesWrite == backupFileSize))
		{
			throw logic_error("Unable to write data");
		}
	}
}

void CryptDiskHelpers::RestoreVolumeHeader(const std::wstring& volumeId, const std::string& backupFile)
{
	// Open and read backup file
	if (!bfs::exists(backupFile))
	{
		throw logic_error("Backup file doesn't exist");
	}

	HANDLE hBackupFile = CreateFileA(backupFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hBackupFile == INVALID_HANDLE_VALUE)
	{
		throw logic_error("Unable to open backup file");
	}

	SCOPE_EXIT{ CloseHandle(hBackupFile); };

	size_t backupFileSize = GetFileSize(hBackupFile, NULL);

	std::vector<unsigned char> backupBuff(backupFileSize);
	{
		DWORD bytesRead;
		BOOL result = ReadFile(hBackupFile, &backupBuff[0], static_cast<DWORD>(backupBuff.size()), &bytesRead, NULL);
		if (!result || !(bytesRead == backupFileSize))
		{
			throw logic_error("Unable to read data");
		}
	}

	// Open volume
	HANDLE hVolumeFile = ::CreateFileW(volumeId.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hVolumeFile == INVALID_HANDLE_VALUE)
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT{ ::CloseHandle(hVolumeFile); };


	// Lock
	DWORD dwResult;
	if (!::DeviceIoControl(hVolumeFile, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL))
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT{
		DWORD dwResult;
		::DeviceIoControl(hVolumeFile, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL);
	};

	// Write header from backup file
	{
		DWORD bytesWrite;
		BOOL result = WriteFile(hVolumeFile, &backupBuff[0], static_cast<DWORD>(backupBuff.size()), &bytesWrite, NULL);
		if (!result || !(bytesWrite == backupFileSize))
		{
			throw logic_error("Unable to write data");
		}
	}
}

void CryptDiskHelpers::EncryptVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeName, DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool(double)> callback)
{
	// Open
	HANDLE hFile = ::CreateFileW(volumeName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT { ::CloseHandle(hFile); };


	// Lock
	DWORD dwResult;
	if(!::DeviceIoControl(hFile, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL))
	{
		throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
	}

	SCOPE_EXIT {
		DWORD dwResult;
		::DeviceIoControl(hFile, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL);
	};

	DoEncrypt(hFile, GetVolumeLength(hFile), pRndGen, cipherAlgorithm, password, passwordLength, fillImageWithRandom, callback);
}
