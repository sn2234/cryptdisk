
#include "StdAfx.h"

#include "..\Version.h"
#include "DiskHeaderTools.h"

#if	defined(_USER_MODE_) || defined(_TEST_MODE_)
#include <CryptoLib.h>
#endif

#include <BaseCrypt/SHA256_HASH.h>
#include <BaseCrypt/DNAES.h>
#include <BaseCrypt/DNTwofish.h>
#include <BaseCrypt/RijndaelEngine.h>
#include <BaseCrypt/TwofishEngine.h>
#include <BaseCrypt/EncryptionModes.h>
using namespace CryptoLib;

#if defined(_TEST_MODE_)
#endif

#if	defined(_USER_MODE_) || defined(_TEST_MODE_)

bool DiskHeaderTools::Decipher( void* pHeader, const UCHAR* passwordData, ULONG passwordLength, CIPHER_INFO* pCipherInfo )
{

	// Try do decipher disk as v3
	{
		DISK_HEADER_V3 headerBuff;

		headerBuff = *(reinterpret_cast<DISK_HEADER_V3*>(pHeader));

		if (DecipherV3(&headerBuff, passwordData, passwordLength, &pCipherInfo->diskCipher , &pCipherInfo->versionInfo, pCipherInfo->userKey))
		{
			*(reinterpret_cast<DISK_HEADER_V3*>(pHeader)) = headerBuff;
			RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
			return true;
		}

		RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
	}


	// Try do decipher disk as v4
	{
		DISK_HEADER_V4 headerBuff;

		headerBuff = *(reinterpret_cast<DISK_HEADER_V4*>(pHeader));

		if (DecipherV4(&headerBuff, passwordData, passwordLength, &pCipherInfo->diskCipher , &pCipherInfo->versionInfo, pCipherInfo->userKey, pCipherInfo->initVector))
		{
			*(reinterpret_cast<DISK_HEADER_V4*>(pHeader)) = headerBuff;
			RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));

			return true;
		}

		RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
	}

	return false;
}

bool DiskHeaderTools::DecipherV3( DISK_HEADER_V3* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER* pDiskCipher, VERSION_INFO* pDiskVersion, UCHAR* pUserKey )
{

	UCHAR			didgest[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	DISK_HEADER_V3	header;
	UCHAR			userKey[RijndaelEngine::KeySize];

	// Derive user key
	DeriveKey(passwordData, passwordLength, pHeader->DiskSalt,
		sizeof(pHeader->DiskSalt), userKey, sizeof(userKey), 50000);

	// Try AES
	memcpy(&header, pHeader, sizeof(DISK_HEADER_V3));
	{
		DNAES	cipher;

		cipher.SetupKey(userKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/AES_BLOCK_SIZE,
			header.DiskKey);
		cipher.SetupKey(header.DiskKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE_V3/AES_BLOCK_SIZE,
			header.TweakKey);

		// Check version first
		if((header.DiskVersion.formatVersion != DISK_VERSION_3))
		{
			goto try_twofish;
		}

		// Check hash
		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			goto try_twofish;
		}

		// Copy header and return true
		*pDiskCipher = DISK_CIPHER_AES;
		*pDiskVersion = header.DiskVersion;
		memcpy(pHeader, &header, sizeof(DISK_HEADER_V3));
		memcpy(pUserKey, userKey, sizeof(userKey));
		RtlSecureZeroMemory(&header, sizeof(DISK_HEADER_V3));
		return true;
	}
try_twofish:
	// Try Twofish
	memcpy(&header, pHeader, sizeof(DISK_HEADER_V3));
	{
		DNTwofish	cipher;

		cipher.SetupKey(userKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/TWOFISH_BLOCK_SIZE,
			header.DiskKey);
		cipher.SetupKey(header.DiskKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE_V3/TWOFISH_BLOCK_SIZE,
			header.TweakKey);

		// Check version first
		if((header.DiskVersion.formatVersion != DISK_VERSION_3))
		{
			return false;
		}

		// Check hash
		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			// Copy header and return true
			*pDiskCipher=DISK_CIPHER_TWOFISH;
			*pDiskVersion = header.DiskVersion;
			memcpy(pHeader, &header, sizeof(DISK_HEADER_V3));
			memcpy(pUserKey, userKey, sizeof(userKey));
			RtlSecureZeroMemory(&header, sizeof(DISK_HEADER_V3));
			return true;
		}
	}

	return false;
}

bool DiskHeaderTools::DecipherV4( DISK_HEADER_V4* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER* pDiskCipher, VERSION_INFO* pDiskVersion, UCHAR* pUserKey , UCHAR* pInitVector)
{
#pragma pack(push, 1)
	struct
	{
		UCHAR			userKey[RijndaelEngine::KeySize];
		UCHAR			userIV[RijndaelEngine::BlockSize];
	}usedEncData;
#pragma pack(pop)

	// Derivate key from password and salt
	Pkcs5DeriveKey<SHA256_HASH>(passwordLength, passwordData, sizeof(pHeader->DiskSalt), pHeader->DiskSalt, 50000, sizeof(usedEncData), &usedEncData);

	// Try AES
	{
		// Create a copy of header
		DISK_HEADER_V4 header;
		memcpy(&header, pHeader, sizeof(header));

		// Initialize cipher
		RijndaelEngine engine;

		engine.SetupKey(usedEncData.userKey);
		DecipherDataCbc(engine, DISK_HEADER_ENC_PASS_SIZE_V4/RijndaelEngine::BlockSize, usedEncData.userIV, header.DiskKey);

		engine.SetupKey(header.DiskKey);
		DecipherDataCbc(engine, DISK_HEADER_ENC_SIZE_V4/RijndaelEngine::BlockSize, header.TweakNumber, &header.DiskVersion);

		// Check hash
		SHA256_HASH hash;
		UCHAR didgest[SHA256_DIDGEST_SIZE];

		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			// Copy header and return true
			*pDiskCipher=DISK_CIPHER_AES;
			*pDiskVersion = header.DiskVersion;
			memcpy(pHeader, &header, sizeof(header));
			memcpy(pUserKey, usedEncData.userKey, sizeof(usedEncData.userKey));
			memcpy(pInitVector, usedEncData.userIV, sizeof(usedEncData.userIV));
			RtlSecureZeroMemory(&header, sizeof(header));
			RtlSecureZeroMemory(&usedEncData, sizeof(usedEncData));
			return true;
		}
		RtlSecureZeroMemory(&header, sizeof(header));
	}
	// Try twofish
	{
		// Create a copy of header
		DISK_HEADER_V4 header;
		memcpy(&header, pHeader, sizeof(header));

		// Initialize cipher
		TwofishEngine engine;

		engine.SetupKey(usedEncData.userKey);
		DecipherDataCbc(engine, DISK_HEADER_ENC_PASS_SIZE_V4/TwofishEngine::BlockSize, usedEncData.userIV, header.DiskKey);

		engine.SetupKey(header.DiskKey);
		DecipherDataCbc(engine, DISK_HEADER_ENC_SIZE_V4/TwofishEngine::BlockSize, header.TweakNumber, &header.DiskVersion);

		// Check hash
		SHA256_HASH hash;
		UCHAR didgest[SHA256_DIDGEST_SIZE];

		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			// Copy header and return true
			*pDiskCipher=DISK_CIPHER_TWOFISH;
			*pDiskVersion = header.DiskVersion;
			memcpy(pHeader, &header, sizeof(header));
			memcpy(pUserKey, usedEncData.userKey, sizeof(usedEncData.userKey));
			memcpy(pInitVector, usedEncData.userIV, sizeof(usedEncData.userIV));
			RtlSecureZeroMemory(&header, sizeof(header));
			RtlSecureZeroMemory(&usedEncData, sizeof(usedEncData));
			return true;
		}
		RtlSecureZeroMemory(&header, sizeof(header));
	}

	RtlSecureZeroMemory(&usedEncData, sizeof(usedEncData));
	return false;
}

bool DiskHeaderTools::Initialize( DISK_HEADER_V3* pHeader, CryptoLib::IRandomGenerator* pRndGen )
{
	SHA256_HASH		hash;

	// Fill all pHeaderBuff with random data
	if(!pRndGen->GenerateRandomBytes(pHeader, sizeof(DISK_HEADER_V3)))
	{
		// If not enough randomness exit with error
		return false;
	}

	// Set disk version number
	pHeader->DiskVersion.formatVersion = DISK_VERSION_3;

	// Calculate hash of magic value
	hash.Init();
	hash.Update(pHeader->MagicValue, sizeof(pHeader->MagicValue));
	hash.Final(pHeader->MagicHash);

	return true;
}

bool DiskHeaderTools::Initialize( DISK_HEADER_V4* pHeader, CryptoLib::IRandomGenerator* pRndGen )
{
	SHA256_HASH		hash;

	// Fill all pHeaderBuff with random data
	if(!pRndGen->GenerateRandomBytes(pHeader, sizeof(DISK_HEADER_V4)))
	{
		// If not enough randomness exit with error
		return false;
	}

	// Set disk version number
	pHeader->DiskVersion.formatVersion = DISK_VERSION_4;

	// Calculate hash of magic value
	hash.Init();
	hash.Update(pHeader->MagicValue, sizeof(pHeader->MagicValue));
	hash.Final(pHeader->MagicHash);

	return true;
}

void DiskHeaderTools::Encipher( DISK_HEADER_V3* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER algoId )
{
	UCHAR			userKey[RijndaelEngine::KeySize];

	// Derivate key from password and salt
	DeriveKey(passwordData, passwordLength, pHeader->DiskSalt,
		sizeof(pHeader->DiskSalt), userKey, sizeof(userKey), 50000);

	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			DNAES	cipher;

			// Encipher with disk key
			cipher.SetupKey(pHeader->DiskKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->TweakKey);
			// Encipher with password key
			cipher.SetupKey(userKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->DiskKey);

		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			DNTwofish	cipher;

			// Encipher with disk key
			cipher.SetupKey(pHeader->DiskKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_SIZE_V3/TWOFISH_BLOCK_SIZE,
				pHeader->TweakKey);
			// Encipher with derived key
			cipher.SetupKey(userKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/TWOFISH_BLOCK_SIZE,
				pHeader->DiskKey);
		}
		break;
	}
}

void DiskHeaderTools::Encipher( DISK_HEADER_V4* pHeader, const UCHAR* passwordData, ULONG passwordLength, DISK_CIPHER algoId )
{
#pragma pack(push, 1)
	struct
	{
		UCHAR			userKey[RijndaelEngine::KeySize];
		UCHAR			userIV[RijndaelEngine::BlockSize];
	}usedEncData;
#pragma pack(pop)

	// Derivate key from password and salt
	Pkcs5DeriveKey<SHA256_HASH>(passwordLength, passwordData, sizeof(pHeader->DiskSalt), pHeader->DiskSalt, 50000, sizeof(usedEncData), &usedEncData);

	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			RijndaelEngine engine;

			// Encipher with disk key
			engine.SetupKey(pHeader->DiskKey);
			EncipherDataCbc(engine, DISK_HEADER_ENC_SIZE_V4/RijndaelEngine::BlockSize, pHeader->TweakNumber, &pHeader->DiskVersion);

			// Encipher with derived key
			engine.SetupKey(usedEncData.userKey);
			EncipherDataCbc(engine, DISK_HEADER_ENC_PASS_SIZE_V4/RijndaelEngine::BlockSize, usedEncData.userIV, pHeader->DiskKey);
		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			TwofishEngine engine;

			// Encipher with disk key
			engine.SetupKey(pHeader->DiskKey);
			EncipherDataCbc(engine, DISK_HEADER_ENC_SIZE_V4/TwofishEngine::BlockSize, pHeader->TweakNumber, &pHeader->DiskVersion);

			// Encipher with derived key
			engine.SetupKey(usedEncData.userKey);
			EncipherDataCbc(engine, DISK_HEADER_ENC_PASS_SIZE_V4/TwofishEngine::BlockSize, usedEncData.userIV, pHeader->DiskKey);
		}
		break;
	}

	RtlSecureZeroMemory(&usedEncData, sizeof(usedEncData));
}

#endif

#if	!defined(_USER_MODE_) || defined(_TEST_MODE_)

bool DiskHeaderTools::Decipher( DISK_HEADER_V3* pHeader, const UCHAR* pUserKey, DISK_CIPHER algoId )
{
	UCHAR			didgest[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	bool			result = false;

	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			DNAES* pCipher = (DNAES*)ExAllocatePoolWithTag(PagedPool, sizeof(DNAES), MEM_TAG);
			if(!pCipher)
			{
				return false;
			}

			pCipher->SetupKey(pUserKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->DiskKey);
			pCipher->SetupKey(pHeader->DiskKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->TweakKey);

			// Check version first
			if((pHeader->DiskVersion.formatVersion == DISK_VERSION_3))
			{
				// Check hash
				hash.Init();
				hash.Update(pHeader->MagicValue, sizeof(pHeader->MagicValue));
				hash.Final(didgest);
				hash.Clear();
				if(!memcmp(pHeader->MagicHash, didgest, SHA256_DIDGEST_SIZE))
				{
					result = true;
				}
			}
			pCipher->Clear();
			ExFreePoolWithTag(pCipher, MEM_TAG);
			RtlSecureZeroMemory(didgest, sizeof(didgest));
		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			DNTwofish* pCipher = (DNTwofish*)ExAllocatePoolWithTag(PagedPool, sizeof(DNTwofish), MEM_TAG);
			if(!pCipher)
			{
				return false;
			}

			pCipher->SetupKey(pUserKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->DiskKey);
			pCipher->SetupKey(pHeader->DiskKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_SIZE_V3/AES_BLOCK_SIZE,
				pHeader->TweakKey);

			// Check version first
			if((pHeader->DiskVersion.formatVersion == DISK_VERSION_3))
			{
				// Check hash
				hash.Init();
				hash.Update(pHeader->MagicValue, sizeof(pHeader->MagicValue));
				hash.Final(didgest);
				hash.Clear();
				if(!memcmp(pHeader->MagicHash, didgest, SHA256_DIDGEST_SIZE))
				{
					result = true;
				}
			}
			pCipher->Clear();
			ExFreePoolWithTag(pCipher, MEM_TAG);
			RtlSecureZeroMemory(didgest, sizeof(didgest));
		}
		break;
	}
	return result;
}

bool DiskHeaderTools::Decipher( DISK_HEADER_V4* pHeader, const UCHAR* pUserKey, const UCHAR* pInitVector, DISK_CIPHER algoId )
{
	bool			result = false;

	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			// Create a copy of pHeaderBuff
			DISK_HEADER_V4 *pHeaderBuff = (DISK_HEADER_V4*)ExAllocatePoolWithTag(PagedPool, sizeof(DISK_HEADER_V4), MEM_TAG);
			memcpy(pHeaderBuff, pHeader, sizeof(DISK_HEADER_V4));
			if(!pHeaderBuff)
			{
				return false;
			}

			// Initialize cipher
			RijndaelEngine* pEngine = (RijndaelEngine*)ExAllocatePoolWithTag(PagedPool, sizeof(RijndaelEngine), MEM_TAG);
			if(!pEngine)
			{
				ExFreePoolWithTag(pHeaderBuff, MEM_TAG);
				return false;
			}

			pEngine->SetupKey(pUserKey);
			DecipherDataCbc(*pEngine, DISK_HEADER_ENC_PASS_SIZE_V4/RijndaelEngine::BlockSize, pInitVector, pHeaderBuff->DiskKey);

			pEngine->SetupKey(pHeaderBuff->DiskKey);
			DecipherDataCbc(*pEngine, DISK_HEADER_ENC_SIZE_V4/RijndaelEngine::BlockSize, pHeaderBuff->TweakNumber, &pHeaderBuff->DiskVersion);

			// Check hash
			SHA256_HASH hash;
			UCHAR didgest[SHA256_DIDGEST_SIZE];

			hash.Init();
			hash.Update(pHeaderBuff->MagicValue, sizeof(pHeaderBuff->MagicValue));
			hash.Final(didgest);
			if(!memcmp(pHeaderBuff->MagicHash, didgest, SHA256_DIDGEST_SIZE))
			{
				// Copy pHeaderBuff and return true
				memcpy(pHeader, pHeaderBuff, sizeof(DISK_HEADER_V4));
				result = true;
			}
			RtlSecureZeroMemory(pHeaderBuff, sizeof(DISK_HEADER_V4));
			pEngine->Clear();
			ExFreePoolWithTag(pHeaderBuff, MEM_TAG);
			ExFreePoolWithTag(pEngine, MEM_TAG);
		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			// Create a copy of pHeaderBuff
			DISK_HEADER_V4 *pHeaderBuff = (DISK_HEADER_V4*)ExAllocatePoolWithTag(PagedPool, sizeof(DISK_HEADER_V4), MEM_TAG);
			memcpy(pHeaderBuff, pHeader, sizeof(DISK_HEADER_V4));
			if(!pHeaderBuff)
			{
				return false;
			}

			// Initialize cipher
			TwofishEngine* pEngine = (TwofishEngine*)ExAllocatePoolWithTag(PagedPool, sizeof(TwofishEngine), MEM_TAG);
			if(!pEngine)
			{
				ExFreePoolWithTag(pHeaderBuff, MEM_TAG);
				return false;
			}

			pEngine->SetupKey(pUserKey);
			DecipherDataCbc(*pEngine, DISK_HEADER_ENC_PASS_SIZE_V4/TwofishEngine::BlockSize, pInitVector, pHeaderBuff->DiskKey);

			pEngine->SetupKey(pHeaderBuff->DiskKey);
			DecipherDataCbc(*pEngine, DISK_HEADER_ENC_SIZE_V4/TwofishEngine::BlockSize, pHeaderBuff->TweakNumber, &pHeaderBuff->DiskVersion);

			// Check hash
			SHA256_HASH hash;
			UCHAR didgest[SHA256_DIDGEST_SIZE];

			hash.Init();
			hash.Update(pHeaderBuff->MagicValue, sizeof(pHeaderBuff->MagicValue));
			hash.Final(didgest);
			if(!memcmp(pHeaderBuff->MagicHash, didgest, SHA256_DIDGEST_SIZE))
			{
				// Copy pHeaderBuff and return true
				memcpy(pHeader, pHeaderBuff, sizeof(DISK_HEADER_V4));
				result = true;
			}
			RtlSecureZeroMemory(pHeaderBuff, sizeof(DISK_HEADER_V4));
			pEngine->Clear();
			ExFreePoolWithTag(pHeaderBuff, MEM_TAG);
			ExFreePoolWithTag(pEngine, MEM_TAG);
		}
		break;
	}

	return result;
}

#endif
