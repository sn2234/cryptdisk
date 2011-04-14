
#include "stdafx.h"
#include "CryptDiskHelpers.h"
#include "DriverProtocol.h"
#include "winapi_exception.h"
#include "DiskHeaderTools.h"

#include "SafeHandle.h"

using namespace std;
using namespace CryptoLib;
using namespace boost;

void CryptDiskHelpers::MountImage( DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const char* password )
{
	static const WCHAR pathPrefix[] =  L"\\??\\";

	size_t	structSize = sizeof(DISK_ADD_INFO) + sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR);

	vector<unsigned char> buffer(structSize);

	DISK_ADD_INFO* pInfo = reinterpret_cast<DISK_ADD_INFO*>(&buffer[0]);

	pInfo->DriveLetter = driveLetter;
	pInfo->MountOptions = MOUNT_VIA_MOUNTMGR | MOUNT_SAVE_TIME;
	pInfo->PathSize = sizeof(pathPrefix) + wcslen(imagePath)*sizeof(WCHAR) + sizeof(WCHAR);
	wcscpy_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), pathPrefix);
	wcscat_s(pInfo->FilePath, pInfo->PathSize / sizeof(WCHAR), imagePath);

	// Read disk header
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

	// Decipher disk header
	DiskHeaderTools::CIPHER_INFO cipherInfo;
	if(!DiskHeaderTools::Decipher(&headerBuff[0], reinterpret_cast<const UCHAR*>(password), strlen(password), &cipherInfo))
	{
		throw logic_error("Wrong passrwod");
	}

	// Fill cryptographic parameters
	pInfo->DiskFormatVersion = cipherInfo.versionInfo.formatVersion;
	copy(begin(cipherInfo.initVector), end(cipherInfo.initVector), pInfo->InitVector);
	copy(begin(cipherInfo.userKey), end(cipherInfo.userKey), pInfo->UserKey);
	pInfo->wAlgoId = cipherInfo.diskCipher;

	DWORD drvResult = driverControl.Control(IOCTL_VDISK_ADD_DISK, &buffer[0], buffer.size());
	if(!drvResult)
	{
		throw winapi_exception("DeviceIoControl error");
	}
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
	DISK_DELETE_INFO info;

	info.DiskId = id;
	info.bForce = forceUnmount ? TRUE : FALSE;

	if(!driverControl.Control(IOCTL_VDISK_DELETE_DISK, &info, sizeof(info)))
	{
		throw winapi_exception("UnmountImage: DeviceIoControl failed");
	}
}

void CryptDiskHelpers::CreateImage( CryptoLib::CRandom* pRndGen, const WCHAR* imagePath, INT64 imageSize, DISK_CIPHER cipherAlgorithm, const char* password )
{
	// Prepare disk header
	DISK_HEADER_V4 header;

	DiskHeaderTools::Initialize(&header, pRndGen);

	const size_t  sectorSize = 512;

	header.DiskSectorSize = sectorSize;
	header.ImageOffset = 2*sizeof(DISK_HEADER_V4); // Reserve place for another header for hidden volume

	DiskHeaderTools::Encipher(&header, (UCHAR*)password, strlen(password), cipherAlgorithm);

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

		pRndGen->GenRandom(key, sizeof(key));

		RijndaelEngine cipher;

		cipher.SetupKey(key);

		union
		{
			UINT64 counter;
			unsigned char buffer[RijndaelEngine::BlockSize];
		};

		pRndGen->GenRandom(buffer, sizeof(buffer));

		unsigned char *pBlock = (unsigned char*)pMap + sizeof(DISK_HEADER_V4);
		UINT64 bytesToFill = fileSize.QuadPart;
		UINT64 bytesFilled = sizeof(DISK_HEADER_V4);
		size_t bytesFilledInCurrentView = sizeof(DISK_HEADER_V4);

		do 
		{
			cipher.EncipherBlock(buffer, pBlock);
			counter++;
			pBlock += RijndaelEngine::BlockSize;
			bytesFilledInCurrentView += RijndaelEngine::BlockSize;
			bytesFilled += RijndaelEngine::BlockSize;
			if(bytesFilledInCurrentView >= bytesToMap)
			{
				UnmapViewOfFile(pMap);
				offsetMapped.QuadPart += bytesToMap;
				pMap = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, offsetMapped.HighPart, offsetMapped.LowPart,
					min(fileSize.QuadPart - offsetMapped.QuadPart, bytesToMap));
				pBlock = (unsigned char*)pMap;
				bytesFilledInCurrentView = 0;
			}
		} while (bytesFilled < bytesToFill);

		UnmapViewOfFile(pMap);
	}
}
