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

#include "stdafx.h"

#include "DiskCipher.h"

using namespace CryptoLib;

void DiskCipher::Init(DISK_HEADER* pHeader, DISK_CIPHER cipher)
{
	m_currentCipher=cipher;
	switch(cipher)
	{
	case DISK_CIPHER_AES:
		m_LrwAES.SetupKey(pHeader->DiskKey, pHeader->TweakKey);
		break;
	case DISK_CIPHER_TWOFISH:
		m_LrwTwofish.SetupKey(pHeader->DiskKey, pHeader->TweakKey);
		break;
	}
}

void DiskCipher::DecipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pData)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwAES.DecipherFirstBlock(blockNumberByte, pData);
	
	numOfSectors*=BYTES_PER_SECTOR/AES_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pData=((UCHAR*)pData)+AES_BLOCK_SIZE;
		m_LrwAES.DecipherNextBlock(pData);
	}
}

void DiskCipher::DecipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pData)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwTwofish.DecipherFirstBlock(blockNumberByte, pData);

	numOfSectors*=BYTES_PER_SECTOR/TWOFISH_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pData=((UCHAR*)pData)+TWOFISH_BLOCK_SIZE;
		m_LrwTwofish.DecipherNextBlock(pData);
	}
}

void DiskCipher::EncipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pPlain, void *pEnc)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwAES.EncipherFirstBlock(blockNumberByte, pPlain, pEnc);

	numOfSectors*=BYTES_PER_SECTOR/AES_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pPlain=((UCHAR*)pPlain)+AES_BLOCK_SIZE;
		pEnc=((UCHAR*)pEnc)+AES_BLOCK_SIZE;
		m_LrwAES.EncipherNextBlock(pPlain, pEnc);
	}
}

void DiskCipher::EncipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pPlain, void *pEnc)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwTwofish.EncipherFirstBlock(blockNumberByte, pPlain, pEnc);

	numOfSectors*=BYTES_PER_SECTOR/TWOFISH_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pPlain=((UCHAR*)pPlain)+TWOFISH_BLOCK_SIZE;
		pEnc=((UCHAR*)pEnc)+TWOFISH_BLOCK_SIZE;
		m_LrwTwofish.EncipherNextBlock(pPlain, pEnc);
	}
}

void DiskCipher::EncipherSectorsAES(ULONG beginSector, ULONG numOfSectors, void* pData)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwAES.EncipherFirstBlock(blockNumberByte, pData);

	numOfSectors*=BYTES_PER_SECTOR/AES_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pData=((UCHAR*)pData)+AES_BLOCK_SIZE;
		m_LrwAES.EncipherNextBlock(pData);
	}
}

void DiskCipher::EncipherSectorsTwofish(ULONG beginSector, ULONG numOfSectors, void* pData)
{
	union
	{
		UCHAR				blockNumberByte[16];		// 128-bit number
		unsigned __int64	blockNumberInt64;	// 64-bit number
	};

	if(numOfSectors == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64=beginSector*BYTES_PER_SECTOR/16;

	m_LrwTwofish.EncipherFirstBlock(blockNumberByte, pData);

	numOfSectors*=BYTES_PER_SECTOR/TWOFISH_BLOCK_SIZE;
	while(--numOfSectors)
	{
		pData=((UCHAR*)pData)+TWOFISH_BLOCK_SIZE;
		m_LrwTwofish.EncipherNextBlock(pData);
	}
}
