///////////////////////////////////////////////////////////
//  DiscCipherAesV3.cpp
//  Implementation of the Class DiscCipherAesV3
//  Created on:      13-dec-2009 23:35:22
//  Original author: nobody
///////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DiskCipherAesV3.h"
#include "format.h"

#if	defined(_USER_MODE_)
#include <assert.h>
#if !defined(ASSERT)
#define ASSERT assert
#endif
#endif

DiscCipherAesV3::~DiscCipherAesV3()
{
	m_cipher.Clear();
}

DiscCipherAesV3::DiscCipherAesV3(const DiscCipherAesV3& theDiscCipherAesV3)
{
}

DiscCipherAesV3::DiscCipherAesV3(const DiskParamatersV3& diskParameters)
{
	ASSERT(diskParameters.getDiskKeyLength() == CryptoLib::RijndaelEngine::KeySize);
	ASSERT(diskParameters.getDiskTweakKeyLength() == 16);

	m_cipher.SetupKey(diskParameters.getDiskKey(), diskParameters.getDiskTweakKey());
}

void DiscCipherAesV3::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
{
	union
	{
		UCHAR	blockNumberByte[16]; // 128-bit number
		UINT64	blockNumberInt64;    // 64-bit number
	};

	if(blocksCount == 0)
		return;

	UINT32 firstBlockIndex32 = static_cast<UINT32>(firstBlockIndex & 0xFFFFFFFF); // Compatibility with previous version

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex32*BYTES_PER_SECTOR/16;

	m_cipher.EncipherFirstBlock(blockNumberByte, pData);

	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		pData = ((UCHAR*)pData) + CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.EncipherNextBlock(pData);
	}
}

void DiscCipherAesV3::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
{
	union
	{
		UCHAR	blockNumberByte[16]; // 128-bit number
		UINT64	blockNumberInt64;    // 64-bit number
	};

	if(blocksCount == 0)
		return;

	UINT32 firstBlockIndex32 = static_cast<UINT32>(firstBlockIndex & 0xFFFFFFFF); // Compatibility with previous version

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex32*BYTES_PER_SECTOR/16;

	m_cipher.EncipherFirstBlock(blockNumberByte, pPlainData, pCipherData);

	const UCHAR *plainDataPtr = static_cast<const UCHAR *>(pPlainData);
	UCHAR *cipherDataPtr = static_cast<UCHAR *>(pCipherData);
	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		plainDataPtr = plainDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		cipherDataPtr = cipherDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.EncipherNextBlock(plainDataPtr, cipherDataPtr);
	}
}

void DiscCipherAesV3::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
{
	union
	{
		UCHAR	blockNumberByte[16]; // 128-bit number
		UINT64	blockNumberInt64;    // 64-bit number
	};

	if(blocksCount == 0)
		return;

	UINT32 firstBlockIndex32 = static_cast<UINT32>(firstBlockIndex & 0xFFFFFFFF); // Compatibility with previous version

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex32*BYTES_PER_SECTOR/16;

	m_cipher.DecipherFirstBlock(blockNumberByte, pData);

	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		pData=((UCHAR*)pData)+CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.DecipherNextBlock(pData);
	}
}

void DiscCipherAesV3::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pCipherData, PVOID pPlainData)
{
	union
	{
		UCHAR	blockNumberByte[16]; // 128-bit number
		UINT64	blockNumberInt64;    // 64-bit number
	};

	if(blocksCount == 0)
		return;

	UINT32 firstBlockIndex32 = static_cast<UINT32>(firstBlockIndex & 0xFFFFFFFF); // Compatibility with previous version

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex32*BYTES_PER_SECTOR/16;

	m_cipher.DecipherFirstBlock(blockNumberByte, pCipherData, pPlainData);

	UCHAR *plainDataPtr = static_cast<UCHAR *>(pPlainData);
	const UCHAR *cipherDataPtr = static_cast<const UCHAR *>(pCipherData);
	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		plainDataPtr = plainDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		cipherDataPtr = cipherDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.DecipherNextBlock(cipherDataPtr, plainDataPtr);
	}
}
