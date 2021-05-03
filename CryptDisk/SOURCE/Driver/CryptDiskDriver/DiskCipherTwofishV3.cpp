///////////////////////////////////////////////////////////
//  DiskCipherTwofishV3.cpp
//  Implementation of the Class DiskCipherTwofishV3
//  Created on:      09-dec-2009 1:17:13
//  Original author: nobody
///////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DiskCipherTwofishV3.h"
#include "format.h"

#if	defined(_USER_MODE_)
#include <assert.h>
#if !defined(ASSERT)
#define ASSERT assert
#endif
#endif

DiskCipherTwofishV3::~DiskCipherTwofishV3()
{
	m_cipher.Clear();
}

DiskCipherTwofishV3::DiskCipherTwofishV3(const DiskCipherTwofishV3& theDiskCipherTwofishV3)
{

}

DiskCipherTwofishV3::DiskCipherTwofishV3(const DiskParamatersV3& diskParameters)
{
	ASSERT(diskParameters.getDiskKeyLength() == CryptoLib::TwofishEngine::KeySize);
	ASSERT(diskParameters.getDiskTweakKeyLength() == 16);

	m_cipher.SetupKey(diskParameters.getDiskKey(), diskParameters.getDiskTweakKey());
}

void DiskCipherTwofishV3::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
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

	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::TwofishEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		pData = ((UCHAR*)pData) + CryptoLib::TwofishEngine::BlockSize;
		m_cipher.EncipherNextBlock(pData);
	}
}

void DiskCipherTwofishV3::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
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

	const UCHAR *plainDataPtr = (const UCHAR *)pPlainData;
	UCHAR *cipherDataPtr = (UCHAR *)pCipherData;
	UINT64 cipherBlocksCount = blocksCount * (BYTES_PER_SECTOR/CryptoLib::TwofishEngine::BlockSize);
	while(--cipherBlocksCount)
	{
		plainDataPtr = plainDataPtr + CryptoLib::TwofishEngine::BlockSize;
		cipherDataPtr = cipherDataPtr + CryptoLib::TwofishEngine::BlockSize;
		m_cipher.EncipherNextBlock(plainDataPtr, cipherDataPtr);
	}
}

void DiskCipherTwofishV3::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
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
		pData=((UCHAR*)pData)+CryptoLib::TwofishEngine::BlockSize;
		m_cipher.DecipherNextBlock(pData);
	}
}

void DiskCipherTwofishV3::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
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
