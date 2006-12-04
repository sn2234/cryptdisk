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

#ifndef CRYPTO_DISKCIPHER_H_HEADER_INCLUDED_BD126D1B
#define CRYPTO_DISKCIPHER_H_HEADER_INCLUDED_BD126D1B

#pragma once

enum DISK_CIPHER
{
	DISK_CIPHER_AES=1,
	DISK_CIPHER_TWOFISH=2
};

class DiskCipher
{
public:
	void Init(DISK_HEADER* pHeader, DISK_CIPHER cipher);

	void Clear()
	{
		switch(m_currentCipher)
		{
		case DISK_CIPHER_AES:
			m_LrwAES.Clear();
			break;
		case DISK_CIPHER_TWOFISH:
			m_LrwTwofish.Clear();
			break;
		}
	}

	void DecipherSectors(ULONG beginSector, ULONG numOfSectors, void* pData)
	{
		switch(m_currentCipher)
		{
		case DISK_CIPHER_AES:
			DecipherSectorsAES(beginSector, numOfSectors, pData);
			break;
		case DISK_CIPHER_TWOFISH:
			DecipherSectorsTwofish(beginSector, numOfSectors, pData);
			break;
		}
	}

	void EncipherSectors(ULONG beginSector, ULONG numOfSectors, void* pPlain, void *pEnc)
	{
		switch(m_currentCipher)
		{
		case DISK_CIPHER_AES:
			EncipherSectorsAES(beginSector, numOfSectors, pPlain, pEnc);
			break;
		case DISK_CIPHER_TWOFISH:
			EncipherSectorsTwofish(beginSector, numOfSectors, pPlain, pEnc);
			break;
		}
	}

	void EncipherSectors(ULONG beginSector, ULONG numOfSectors, void* pData)
	{
		switch(m_currentCipher)
		{
		case DISK_CIPHER_AES:
			EncipherSectorsAES(beginSector, numOfSectors, pData);
			break;
		case DISK_CIPHER_TWOFISH:
			EncipherSectorsTwofish(beginSector, numOfSectors, pData);
			break;
		}
	}

protected:

	void DecipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pData);
	void DecipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pData);
	void EncipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pPlain, void *pEnc);
	void EncipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pPlain, void *pEnc);
	void EncipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pData);
	void EncipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pData);

	DISK_CIPHER		m_currentCipher;

	union
	{
		LRWAES			m_LrwAES;
		LRWTwofish		m_LrwTwofish;
	};
};



#endif /* CRYPTO_DISKCIPHER_H_HEADER_INCLUDED_BD126D1B */
