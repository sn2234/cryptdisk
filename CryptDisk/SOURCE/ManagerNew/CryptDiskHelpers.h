#pragma once

#include "DNDriverControl.h"
#include "DiskFormat.h"

class MountedImageInfo
{
public:
	MountedImageInfo(ULONG id, WCHAR letter, ULONG openCount, UINT64 size, const WCHAR* path)
		: diskId(id)
		, driveLetter(1, letter)
		, openCount(openCount)
		, size(size)
		, imageFilePath(path)
	{}

	ULONG			diskId;
	std::wstring	driveLetter;
	ULONG			openCount;
	UINT64			size;
	std::wstring	imageFilePath;
};

class CryptDiskHelpers
{
public:
	static void MountImage(DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const unsigned char* password, size_t passwordLength);
	static bool CheckImage(const WCHAR* imagePath, const unsigned char* password, size_t passwordLength);
	static void ChangePassword(CryptoLib::CRandom* pRndGen, const WCHAR* imagePath, const unsigned char* password, size_t passwordLength, const unsigned char* passwordNew, size_t passwordNewLength);
	static std::vector<MountedImageInfo> ListMountedImages(DNDriverControl& driverControl);
	static void UnmountImage(DNDriverControl& driverControl, ULONG id, bool forceUnmount);
	static void CreateImage(CryptoLib::CRandom* pRndGen, const WCHAR* imagePath, INT64 imageSize, DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength);
	static std::vector<unsigned char> ReadImageHeader(const WCHAR* imagePath);
};
