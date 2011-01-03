
#include "StdAfx.h"
#include "CryptDiskHelpers.h"
#include "DriverProtocol.h"
#include "DiskFormat.h"
#include "winapi_exception.h"
#include "DiskHeaderTools.h"

using namespace std;
using namespace CryptoLib;

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
