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
	static void MountImage(DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter,
		const unsigned char* password, size_t passwordLength, ULONG mountOptions);

	static void MountVolume(DNDriverControl& driverControl, const WCHAR* volumePath, WCHAR driveLetter,
		const unsigned char* password, size_t passwordLength, ULONG mountOptions);

	static bool CheckImage(const WCHAR* imagePath, const unsigned char* password, size_t passwordLength);
	
	static bool CheckVolume(const WCHAR* volumeId, const unsigned char* password, size_t passwordLength);

	static void ChangePasswordVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeId, const unsigned char* password,
		size_t passwordLength, const unsigned char* passwordNew, size_t passwordNewLength);

	static void ChangePassword(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* imagePath, const unsigned char* password,
		size_t passwordLength, const unsigned char* passwordNew, size_t passwordNewLength);

	static std::vector<MountedImageInfo> ListMountedImages(DNDriverControl& driverControl);

	static void UnmountImage(DNDriverControl& driverControl, ULONG id, bool forceUnmount);

	static void CreateImage(CryptoLib::IRandomGenerator* pRndGen,const WCHAR* imagePath, INT64 imageSize,
		DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool (double)> callback);

	static void EncryptVolume(CryptoLib::IRandomGenerator* pRndGen, const WCHAR* volumeName, DISK_CIPHER cipherAlgorithm,
		const unsigned char* password, size_t passwordLength, bool fillImageWithRandom, std::function<bool(double)> callback);

	static boost::optional<long long> getVolumeCapacity(const std::string& volumeId);

	static std::vector<unsigned char> ReadVolumeHeader(const WCHAR* volumeId);

	static std::vector<unsigned char> ReadImageHeader(const WCHAR* imagePath);

	static void RestoreImageHeader(const std::string& imageFile, const std::string& backupFile);

	static void RestoreVolumeHeader(const std::wstring& volumeId, const std::string& backupFile);

	static void BackupImageHeader(const std::string& imageFile, const std::string& backupFile, DISK_VERSION diskVersion);

	static void BackupVolumeHeader(const std::wstring& volumeId, const std::string& backupFile, DISK_VERSION diskVersion);

	static void DecryptImage(const std::wstring& imagePath, const std::wstring& outputImagePath, const std::string& password);
};
