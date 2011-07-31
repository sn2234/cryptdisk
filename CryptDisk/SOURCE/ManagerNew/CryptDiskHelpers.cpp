
#include "stdafx.h"
#include "CryptDiskHelpers.h"
#include "DriverProtocol.h"
#include "winapi_exception.h"
#include "DiskHeaderTools.h"

#include "SafeHandle.h"

using namespace std;
using namespace CryptoLib;
using namespace boost;

static const WCHAR pathPrefix[] =  L"\\??\\";

static void SendBroadcastMessage( WPARAM message, WCHAR driveLetter );

void CryptDiskHelpers::MountImage( DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const unsigned char* password, size_t passwordLength, ULONG mountOptions )
{
	size_t	structSize = sizeof(DISK_ADD_INFO) + sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR);

	vector<unsigned char> buffer(structSize);

	DISK_ADD_INFO* pInfo = reinterpret_cast<DISK_ADD_INFO*>(&buffer[0]);

	pInfo->DriveLetter = driveLetter;
	pInfo->MountOptions = mountOptions;
	pInfo->PathSize = sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR);
	wcscpy_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), pathPrefix);
	wcscat_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), imagePath);

	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));


	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if(!DiskHeaderTools::Decipher(&headerBuff[0], password, passwordLength, &cipherInfo))
	{
		throw logic_error("Wrong password");
	}

	// Fill cryptographic parameters
	pInfo->DiskFormatVersion = cipherInfo.versionInfo.formatVersion;
	copy(begin(cipherInfo.initVector), end(cipherInfo.initVector), pInfo->InitVector);
	copy(begin(cipherInfo.userKey), end(cipherInfo.userKey), pInfo->UserKey);
	pInfo->wAlgoId = cipherInfo.diskCipher;

	RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
	RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());

	DWORD drvResult = driverControl.Control(IOCTL_VDISK_ADD_DISK, &buffer[0], buffer.size());
	RtlSecureZeroMemory(&buffer[0], buffer.size());
	if(!drvResult)
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

				if(!driverControl.Control(IOCTL_VDISK_GET_DISKS_INFO, &buffer[0], buffer.size(), &result))
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

	return tmp;
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

	DiskHeaderTools::Encipher(&header, password, passwordLength, cipherAlgorithm);

	// Create file
	SafeHandle hFile(CreateFileW(imagePath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL));
	if(hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("CreateImage: unable to create image file");
	}

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
		SafeHandle hFileMapping(CreateFileMapping(hFile, NULL, PAGE_READWRITE, fileSize.HighPart, fileSize.LowPart, NULL));
		if(hFileMapping == 0)
		{
			throw winapi_exception("CreateImage: unable to create file mapping");
		}

		// Write header
		LARGE_INTEGER offsetMapped;
		const size_t bytesToMap = 1024*1024*4;

		offsetMapped.QuadPart = 0;

		void *pMap = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, offsetMapped.HighPart, offsetMapped.LowPart, bytesToMap);

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

		UnmapViewOfFile(pMap);
	}
}

bool CryptDiskHelpers::CheckImage( const WCHAR* imagePath, const unsigned char* password, size_t passwordLength )
{
	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));

	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	bool bResult = DiskHeaderTools::Decipher(&headerBuff[0], password, passwordLength, &cipherInfo);

	RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
	RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());

	return bResult;
}

void CryptDiskHelpers::ChangePassword( CryptoLib::IRandomGenerator* pRndGen, const WCHAR* imagePath, const unsigned char* password, size_t passwordLength,
	const unsigned char* passwordNew, size_t passwordNewLength )
{
	// Read disk header
	vector<unsigned char> headerBuff((ReadImageHeader(imagePath)));

	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if(!DiskHeaderTools::Decipher(&headerBuff[0], password, passwordLength, &cipherInfo))
	{
		throw logic_error("Wrong password");
	}

	// Encipher with new password
	switch (cipherInfo.versionInfo.formatVersion)
	{
	case DISK_VERSION_3:
		{
			DISK_HEADER_V3 *pHeader = reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]);
			pRndGen->GenerateRandomBytes(pHeader->DiskSalt, sizeof(pHeader->DiskSalt));
			DiskHeaderTools::Encipher(pHeader, passwordNew, passwordNewLength, cipherInfo.diskCipher);
		}
		break;
	case DISK_VERSION_4:
		{
			DISK_HEADER_V4 *pHeader = reinterpret_cast<DISK_HEADER_V4*>(&headerBuff[0]);
			pRndGen->GenerateRandomBytes(pHeader->DiskSalt, sizeof(pHeader->DiskSalt));
			DiskHeaderTools::Encipher(pHeader, passwordNew, passwordNewLength, cipherInfo.diskCipher);
		}
		break;
	default:
		throw logic_error("Wrong passrwod");
		break;
	}

	// Write header
	HANDLE hFile = CreateFileW(imagePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening image for write");
	}

	DWORD bytesWritten;
	BOOL result = WriteFile(hFile, &headerBuff[0], headerBuff.size(), &bytesWritten, NULL);
	if(!result)
	{
		int err = GetLastError();
		CloseHandle(hFile);
		throw winapi_exception("Error writing disk header", err);
	}
	CloseHandle(hFile);


	RtlSecureZeroMemory(&cipherInfo, sizeof(cipherInfo));
	RtlSecureZeroMemory(&headerBuff[0], headerBuff.size());
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

	DWORD bytesRead;
	BOOL result = ReadFile(hFile, &headerBuff[0], headerBuff.size(), &bytesRead, NULL);
	if(!result)
	{
		int err = GetLastError();
		CloseHandle(hFile);
		throw winapi_exception("Error reading disk header", err);
	}
	CloseHandle(hFile);
	
	return headerBuff;
}

void SendBroadcastMessage( WPARAM message, WCHAR driveLetter )
{
	std::string driveRoot;

	driveRoot += (char)driveLetter;
	driveRoot += ":\\";

	switch(message)
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

	DWORD dwResult;
	SendMessageTimeout(HWND_BROADCAST, WM_DEVICECHANGE, message, (LPARAM)(&data), SMTO_ABORTIFHUNG, 1000, &dwResult);

}
