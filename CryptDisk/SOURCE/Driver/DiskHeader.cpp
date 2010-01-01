/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"

#include "DiskHeader.h"
#include "..\Version.h"

using namespace CryptoLib;

// This function prepares disk header, but not encipher it
#ifdef	_USER_MODE_
BOOL CDiskHeader::Create(CRandom *pRndGen)
{
	SHA256_HASH		hash;

	// Fill all header with random data
	if(!pRndGen->GenRandom(m_pHeader, sizeof(DISK_HEADER)))
	{
		// If not enough randomness exit with error
		return FALSE;
	}

	// Set disk version number
	m_pHeader->DiskVersion.formatVersion=DISK_FORMAT_VERSION;

	// Calculate hash of magic value
	hash.Init();
	hash.Update(m_pHeader->MagicValue, sizeof(m_pHeader->MagicValue));
	hash.Final(m_pHeader->MagicHash);

	return TRUE;
}

void CDiskHeader::Seal(UCHAR *passwordData, ULONG passwordLength, DISK_CIPHER algoId)
{
	// Derivate key from password and salt
	DeriveKey(passwordData, passwordLength, m_pHeader->DiskSalt,
		sizeof(m_pHeader->DiskSalt), m_userKey, sizeof(m_userKey), 50000);

	m_algoId=algoId;
	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			DNAES	cipher;

			// Encipher with disk key
			cipher.SetupKey(m_pHeader->DiskKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
				m_pHeader->TweakKey);
			// Encipher with password key
			cipher.SetupKey(m_userKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
				m_pHeader->DiskKey);
			
		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			DNTwofish	cipher;

			// Encipher with disk key
			cipher.SetupKey(m_pHeader->DiskKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_SIZE/TWOFISH_BLOCK_SIZE,
				m_pHeader->TweakKey);
			// Encipher with deriver key
			cipher.SetupKey(m_userKey);
			cipher.EncipherDataECB(DISK_HEADER_ENC_PASS_SIZE/TWOFISH_BLOCK_SIZE,
				m_pHeader->DiskKey);
		}
		break;
	}
}

BOOL CDiskHeader::Open(UCHAR *passwordData, ULONG passwordLength)
{
	UCHAR			didgest[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	DISK_HEADER		header;

	// Derive user key
	DeriveKey(passwordData, passwordLength, m_pHeader->DiskSalt,
		sizeof(m_pHeader->DiskSalt), m_userKey, sizeof(m_userKey), 50000);

	// Try AES
	memcpy(&header, m_pHeader, sizeof(DISK_HEADER));
	{
		DNAES	cipher;

		cipher.SetupKey(m_userKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
			header.DiskKey);
		cipher.SetupKey(header.DiskKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
			header.TweakKey);

		// Check version first
		if((header.DiskVersion.formatVersion != DISK_FORMAT_VERSION))
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

		m_algoId=DISK_CIPHER_AES;
		// Copy header and return true
		memcpy(m_pHeader, &header, sizeof(DISK_HEADER));
		RtlSecureZeroMemory(&header, sizeof(DISK_HEADER));
		return TRUE;
	}
try_twofish:
	// Try Twofish
	memcpy(&header, m_pHeader, sizeof(DISK_HEADER));
	{
		DNTwofish	cipher;

		cipher.SetupKey(m_userKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/TWOFISH_BLOCK_SIZE,
			header.DiskKey);
		cipher.SetupKey(header.DiskKey);
		cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE/TWOFISH_BLOCK_SIZE,
			header.TweakKey);

		// Check version first
		if((header.DiskVersion.formatVersion != DISK_FORMAT_VERSION))
		{
			return FALSE;
		}

		// Check hash
		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			m_algoId=DISK_CIPHER_TWOFISH;
			// Copy header and return true
			memcpy(m_pHeader, &header, sizeof(DISK_HEADER));
			RtlSecureZeroMemory(&header, sizeof(DISK_HEADER));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDiskHeader::Open(UCHAR *pUserKey, DISK_CIPHER algoId)
{
	UCHAR			didgest[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	DISK_HEADER		header;
	BOOL			bRez=FALSE;

	// Copy user key
	memcpy(m_userKey, pUserKey, sizeof(m_userKey));

	m_algoId=algoId;

	// Copy header to temporary buffer
	memcpy(&header, m_pHeader, sizeof(DISK_HEADER));

	
	// Decipher header using specified algorithm
	switch(algoId)
	{
		case DISK_CIPHER_AES:
			{
				DNAES	cipher;

				cipher.SetupKey(m_userKey);
				cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
					header.DiskKey);
				cipher.SetupKey(header.DiskKey);
				cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
					header.TweakKey);
			}
			break;
		case DISK_CIPHER_TWOFISH:
			{
				DNTwofish	cipher;

				cipher.SetupKey(m_userKey);
				cipher.DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
					header.DiskKey);
				cipher.SetupKey(header.DiskKey);
				cipher.DecipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
					header.TweakKey);
			}
			break;
	}

	
	// Check deciphered header
	// Check version
	if((header.DiskVersion.formatVersion == DISK_FORMAT_VERSION))
	{
		// Check hash
		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			// Copy header and return true
			memcpy(m_pHeader, &header, sizeof(DISK_HEADER));
			bRez=TRUE;
		}
	}

	RtlSecureZeroMemory(&header, sizeof(DISK_HEADER));
	
	return bRez;
}

#else
BOOL CDiskHeader::Open(UCHAR *pUserKey, DISK_CIPHER algoId)
{
	UCHAR			didgest[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	DISK_HEADER		header;
	BOOL			bRez=FALSE;

	// Copy user key
	memcpy(m_userKey, pUserKey, sizeof(m_userKey));

	// Copy header to temporary buffer
	memcpy(&header, m_pHeader, sizeof(DISK_HEADER));

	m_algoId=algoId;

	// Decipher header using specified algorithm
	switch(algoId)
	{
	case DISK_CIPHER_AES:
		{
			DNAES	*pCipher;

			pCipher=(DNAES*)ExAllocatePoolWithTag(PagedPool, sizeof(DNAES), MEM_TAG);
			if(!pCipher)
			{
				return FALSE;
			}

			pCipher->SetupKey(m_userKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
				header.DiskKey);
			pCipher->SetupKey(header.DiskKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
				header.TweakKey);

			ExFreePoolWithTag(pCipher, MEM_TAG);
		}
		break;
	case DISK_CIPHER_TWOFISH:
		{
			DNTwofish	*pCipher;

			pCipher=(DNTwofish*)ExAllocatePoolWithTag(PagedPool,
				sizeof(DNTwofish), MEM_TAG);
			if(!pCipher)
			{
				return FALSE;
			}

			pCipher->SetupKey(m_userKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_PASS_SIZE/AES_BLOCK_SIZE,
				header.DiskKey);
			pCipher->SetupKey(header.DiskKey);
			pCipher->DecipherDataECB(DISK_HEADER_ENC_SIZE/AES_BLOCK_SIZE,
				header.TweakKey);

			ExFreePoolWithTag(pCipher, MEM_TAG);
		}
		break;
	}


	// Check deciphered header
	// Check version
	if((header.DiskVersion.formatVersion == DISK_FORMAT_VERSION))
	{
		// Check hash
		hash.Init();
		hash.Update(header.MagicValue, sizeof(header.MagicValue));
		hash.Final(didgest);
		if(!memcmp(header.MagicHash, didgest, SHA256_DIDGEST_SIZE))
		{
			// Copy header and return true
			memcpy(m_pHeader, &header, sizeof(DISK_HEADER));
			bRez=TRUE;
		}
	}

	RtlSecureZeroMemory(&header, sizeof(DISK_HEADER));

	return bRez;
}
#endif	//_USER_MODE_
