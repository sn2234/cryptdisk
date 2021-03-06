
#include "stdafx.h"
#include "CryptDiskHelpers.h"
#include "DriverProtocol.h"
#include "winapi_exception.h"
#include "DiskHeaderTools.h"

#include "SafeHandle.h"
#include "DNDriverControl.h"
#include "VolumeTools.h"

#include "IDiskCipher.h"
#include "DiskCipherAesV3.h"
#include "DiskCipherTwofishV3.h"
#include "DiskCipherV4.h"
#include "DiskFormat.h"

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

	void DoEncrypt(HANDLE hVolume, __int64 length, CryptoLib::IRandomGenerator* pRndGen, DISK_CIPHER cipherAlgorithm,
		const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool(double)> callback)
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
			}

			if (callbackCounter++ > 10)
			{
				if (!callback(min((double) bytesFilled / (double) bytesToFill, 1.0)))
				{
					break;
				}
				callbackCounter = 0;
			}

			{
				DWORD dwResult;

				DWORD bytesToWrite = static_cast<DWORD>(min(bytesToFill, writeBuffer.size()));

				if (bytesFilled > bytesToFill)
				{
					bytesToWrite -= static_cast<DWORD>(bytesFilled - bytesToFill);
				}
				else if ((bytesFilled + writeBuffer.size()) > bytesToFill)
				{
					bytesToWrite = static_cast<DWORD>(bytesToFill - bytesFilled);
				}

				if (!WriteFile(hVolume, static_cast<LPCVOID>(&writeBuffer[0]),
					bytesToWrite, &dwResult, nullptr) || dwResult != bytesToWrite)
				{
					throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
				}

				bytesFilled += bytesToWrite;
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
		case static_cast<uint_8t>(DISK_VERSION::DISK_VERSION_3):
		{
			DISK_HEADER_V3 *pHeader = reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]);
			pRndGen->GenerateRandomBytes(pHeader->DiskSalt, sizeof(pHeader->DiskSalt));
			DiskHeaderTools::Encipher(pHeader, passwordNew, static_cast<ULONG>(passwordNewLength), cipherInfo.diskCipher);
		}
		break;
		case static_cast<uint_8t>(DISK_VERSION::DISK_VERSION_4):
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

	size_t getHeaderSize(DISK_VERSION diskVersion)
	{
		size_t headerSize = 0;

		switch (diskVersion)
		{
		case DISK_VERSION::DISK_VERSION_3:
			headerSize = sizeof(DISK_HEADER_V3);
			break;
		case DISK_VERSION::DISK_VERSION_4:
			headerSize = sizeof(DISK_HEADER_V4) * 2;
			break;
		}
		assert(headerSize);

		return headerSize;
	}

	void performBackup(HANDLE hImage, const std::string& backupFile, DISK_VERSION diskVersion)
	{
		// Read header
		size_t headerSize = getHeaderSize(diskVersion);
		vector<unsigned char> headerBuff(headerSize);

		{
			DWORD bytesRead;
			BOOL result = ReadFile(hImage, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesRead, NULL);
			if (!result || !(bytesRead == headerSize))
			{
				int err = GetLastError();
				throw winapi_exception("Error reading disk header", err);
			}
		}

		// Write backup
		{
			HANDLE hBackupFile(CreateFileA(backupFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL));
			if (hBackupFile == INVALID_HANDLE_VALUE)
			{
				int err = GetLastError();
				throw winapi_exception("Unable to open backup file", err);
			}

			SCOPE_EXIT{ CloseHandle(hBackupFile); };

			DWORD bytesWrite;
			BOOL result = WriteFile(hBackupFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesWrite, NULL);
			if (!result || !(bytesWrite == headerSize))
			{
				int err = GetLastError();
				throw winapi_exception("Unable to write data", err);
			}
		}
	}
}

void CryptDiskHelpers::MountImage( DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const unsigned char* password,
	size_t passwordLength, ULONG mountOptions )
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

void CryptDiskHelpers::MountVolume(DNDriverControl& driverControl, const WCHAR* volumePath, WCHAR driveLetter,
	const unsigned char* password, size_t passwordLength, ULONG mountOptions)
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
	const WCHAR* imagePath, INT64 imageSize, DISK_CIPHER cipherAlgorithm,
	const unsigned char* password, size_t passwordLength, bool fillImageWithRandom,
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

void CryptDiskHelpers::ChangePasswordVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeId,
	const unsigned char* password, size_t passwordLength,
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

void CryptDiskHelpers::ChangePassword( CryptoLib::IRandomGenerator* pRndGen, const WCHAR* imagePath,
	const unsigned char* password, size_t passwordLength,
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
	std::wstring volumeFileName(VolumeTools::prepareVolumeName(volumeId.c_str()));
	HANDLE hVolumeFile = ::CreateFileW(volumeFileName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
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
			throw winapi_exception("Unable to write data");
		}
	}
}

void CryptDiskHelpers::BackupImageHeader(const std::string& imageFile, const std::string& backupFile, DISK_VERSION diskVersion)
{
	HANDLE hFile = CreateFileA(imageFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image");
	}

	SCOPE_EXIT{ ::CloseHandle(hFile); };

	performBackup(hFile, backupFile, diskVersion);
}

void CryptDiskHelpers::BackupVolumeHeader(const std::wstring& volumeId, const std::string& backupFile, DISK_VERSION diskVersion)
{
	std::wstring volumeName = VolumeTools::prepareVolumeName(volumeId);

	HANDLE hFile = CreateFileW(volumeName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image");
	}

	SCOPE_EXIT{ ::CloseHandle(hFile); };

	performBackup(hFile, backupFile, diskVersion);
}

void CryptDiskHelpers::EncryptVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeName, DISK_CIPHER cipherAlgorithm,
	const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool(double)> callback)
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

boost::optional<long long> CryptDiskHelpers::getVolumeCapacity(const std::string& volumeId)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring volumeFileName(VolumeTools::prepareVolumeName(conv.from_bytes(volumeId)));

	HANDLE hVolumeFile = ::CreateFileW(volumeFileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hVolumeFile == INVALID_HANDLE_VALUE)
	{
		return boost::none;
	}

	SCOPE_EXIT{ ::CloseHandle(hVolumeFile); };

	try
	{
		return GetVolumeLength(hVolumeFile);
	}
	catch (const std::exception&)
	{
		return boost::none;
	}
}

std::unique_ptr<IDiskCipher> MakeDiskCipher(const DiskHeaderTools::CIPHER_INFO& cipherInfo, const void* headerBuff)
{
	switch (cipherInfo.versionInfo.formatVersion)
	{
		case static_cast<UCHAR>(DISK_VERSION::DISK_VERSION_3):
		{
			const DISK_HEADER_V3* header = static_cast<const DISK_HEADER_V3*>(headerBuff);
			switch (cipherInfo.diskCipher)
			{
			case DISK_CIPHER::DISK_CIPHER_AES:
				return std::make_unique<DiscCipherAesV3>(DiskParamatersV3{ header->DiskKey, header->TweakKey });
			case DISK_CIPHER::DISK_CIPHER_TWOFISH:
				return std::make_unique<DiskCipherTwofishV3>(DiskParamatersV3{ header->DiskKey, header->TweakKey });
			}
		}
		break;
		case static_cast<UCHAR>(DISK_VERSION::DISK_VERSION_4) :
		{
			const DISK_HEADER_V4* header = static_cast<const DISK_HEADER_V4*>(headerBuff);
			switch (cipherInfo.diskCipher)
			{
			case DISK_CIPHER::DISK_CIPHER_AES:
				return std::make_unique<DiskCipherV4<RijndaelEngine>>(
					DiskParametersV4{ header->DiskKey, header->TweakKey, header->TweakNumber, header->DiskSectorSize });
			case DISK_CIPHER::DISK_CIPHER_TWOFISH:
				return std::make_unique<DiskCipherV4<TwofishEngine>>(
					DiskParametersV4{ header->DiskKey, header->TweakKey, header->TweakNumber, header->DiskSectorSize });
			}
		}
		break;
	}

	throw logic_error("Unsupported disk format");
}

size_t constexpr CalculateHeaderSize(const DiskHeaderTools::CIPHER_INFO& cipherInfo)
{
	switch (cipherInfo.versionInfo.formatVersion)
	{
		case static_cast<UCHAR>(DISK_VERSION::DISK_VERSION_3) :
		{
			return sizeof(DISK_HEADER_V3);
		}
		break;
		case static_cast<UCHAR>(DISK_VERSION::DISK_VERSION_4) :
		{
			return sizeof(DISK_HEADER_V4) * 2;
		}
		break;
	}

	throw logic_error("Unsupported disk format");
}

void CryptDiskHelpers::DecryptImage(const std::wstring& imagePath, const std::wstring& outputImagePath, const std::string& password)
{
	// Read image header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath.c_str())));

	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if (!DiskHeaderTools::Decipher(&headerBuff[0],
		reinterpret_cast<const UCHAR*>(password.c_str()),
		static_cast<ULONG>(password.size()),
		&cipherInfo))
	{
		throw logic_error("Wrong password or damaged header");
	}

	// Open files
	HANDLE hOriginalFile = CreateFileW(imagePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOriginalFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image");
	}

	SCOPE_EXIT{ CloseHandle(hOriginalFile); };

	HANDLE hDecryptedFile{ ::CreateFileW(outputImagePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL) };
	if (hDecryptedFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		throw winapi_exception("Unable to create decypted image file", err);
	}

	SCOPE_EXIT{ CloseHandle(hDecryptedFile); };

	// Init disk cipher
	std::unique_ptr<IDiskCipher> diskCipher = MakeDiskCipher(cipherInfo, headerBuff.data());

	size_t headerSize = CalculateHeaderSize(cipherInfo);

	LARGE_INTEGER imageDataOffset;
	imageDataOffset.QuadPart = headerSize;

	// Adjust data pointer
	if (!::SetFilePointerEx(hOriginalFile, imageDataOffset, nullptr, FILE_BEGIN))
	{
		int err = GetLastError();
		throw winapi_exception("File seek error", err);
	}

	// Do decryption
	vector<uint8_t> dataBuffer(0x10000);
	SCOPE_EXIT{
		RtlSecureZeroMemory(&dataBuffer[0], dataBuffer.size());
	};

	uint64_t blockIndex = 0;

	for (;;)
	{
		DWORD bytesRead;
		{
			BOOL result = ReadFile(hOriginalFile, dataBuffer.data(), static_cast<DWORD>(dataBuffer.size()), &bytesRead, NULL);
			if (!result)
			{
				int err = GetLastError();
				throw winapi_exception("Error reading disk header", err);
			}
		}

		if (bytesRead == 0)
		{
			// We've reached the end of file
			break;
		}

		// bytesRead contains the number of bytes read from file, it might be less than size of buffer,
		//  but it must contain a whole number of blocks
		if ((bytesRead % 512) != 0)
		{
			throw logic_error("Bad image size");
		}

		uint64_t numberOfBlocks = bytesRead / 512;

		if (numberOfBlocks * 512 != bytesRead)
		{
			throw logic_error("Image decryt error");
		}

		diskCipher->DecipherDataBlocks(blockIndex, numberOfBlocks, dataBuffer.data());

		{
			DWORD bytesWrite;
			BOOL result = WriteFile(hDecryptedFile, dataBuffer.data(), bytesRead, &bytesWrite, NULL);
			if (!result || !(bytesWrite == bytesRead))
			{
				int err = GetLastError();
				throw winapi_exception("Unable to write data", err);
			}
		}

		blockIndex += numberOfBlocks;
	}
}
