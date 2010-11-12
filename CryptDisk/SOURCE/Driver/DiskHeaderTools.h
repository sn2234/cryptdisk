
#pragma once

#include "DiskFormat.h"

#if	defined(_USER_MODE_) || defined(_TEST_MODE_)
#include <Random/Random.h>
#endif

class DiskHeaderTools
{
public:
#if	defined(_USER_MODE_) || defined(_TEST_MODE_)

	static bool Initialize(DISK_HEADER_V3* pHeader, CryptoLib::CRandom* pRndGen);
	static bool Initialize(DISK_HEADER_V4* pHeader, CryptoLib::CRandom* pRndGen);

	static void Encipher(DISK_HEADER_V3* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER algoId);
	static void Encipher(DISK_HEADER_V4* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER algoId);

	static bool Decipher(void* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER* pDiskCipher, VERSION_INFO* pDiskVersion, UCHAR* pUserKey); // Decipher with password

private:
	static bool DecipherV3(DISK_HEADER_V3* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER* pDiskCipher, VERSION_INFO* pDiskVersion, UCHAR* pUserKey);
	static bool DecipherV4(DISK_HEADER_V4* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER* pDiskCipher, VERSION_INFO* pDiskVersion, UCHAR* pUserKey);
#endif


#if	!defined(_USER_MODE_) || defined(_TEST_MODE_)
public:
	static bool Decipher(DISK_HEADER_V3* pHeader, const UCHAR* pUserKey, DISK_CIPHER algoId); // Decipher with key
	static bool Decipher(DISK_HEADER_V4* pHeader, const UCHAR* pUserKey, DISK_CIPHER algoId); // Decipher with key
#endif
};

