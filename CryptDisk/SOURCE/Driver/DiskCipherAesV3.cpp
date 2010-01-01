///////////////////////////////////////////////////////////
//  DiscCipherAesV3.cpp
//  Implementation of the Class DiscCipherAesV3
//  Created on:      13-dec-2009 23:35:22
//  Original author: nobody
///////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DiskCipherAesV3.h"
#include "format.h"

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

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex*BYTES_PER_SECTOR/16;

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

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex*BYTES_PER_SECTOR/16;

	m_cipher.EncipherFirstBlock(blockNumberByte, pPlainData, pCipherData);

	const UCHAR *plainDataPtr = (const UCHAR *)pPlainData;
	UCHAR *cipherDataPtr = (UCHAR *)pCipherData;
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

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex*BYTES_PER_SECTOR/16;

	m_cipher.DecipherFirstBlock(blockNumberByte, pData);

	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		pData=((UCHAR*)pData)+CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.DecipherNextBlock(pData);
	}
}

void DiscCipherAesV3::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
{
	union
	{
		UCHAR	blockNumberByte[16]; // 128-bit number
		UINT64	blockNumberInt64;    // 64-bit number
	};

	if(blocksCount == 0)
		return;

	memset(blockNumberByte, 0, sizeof(blockNumberByte));

	// Calculate first block number

	blockNumberInt64 = firstBlockIndex*BYTES_PER_SECTOR/16;

	m_cipher.EncipherFirstBlock(blockNumberByte, pPlainData, pCipherData);

	UCHAR *plainDataPtr = (UCHAR *)pPlainData;
	const UCHAR *cipherDataPtr = (const UCHAR *)pCipherData;
	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::RijndaelEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		plainDataPtr = plainDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		cipherDataPtr = cipherDataPtr + CryptoLib::RijndaelEngine::BlockSize;
		m_cipher.DecipherNextBlock(cipherDataPtr, plainDataPtr);
	}
}
