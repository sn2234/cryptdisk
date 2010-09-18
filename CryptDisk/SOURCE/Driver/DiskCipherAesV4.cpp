///////////////////////////////////////////////////////////
//  DiskCipherAesV4.cpp
//  Implementation of the Class DiskCipherAesV4
//  Created on:      13-dec-2009 23:35:24
//  Original author: nobody
///////////////////////////////////////////////////////////

#include "stdafx.h"

#include <BaseCrypt\ModeXts.h>

#include "DiskCipherAesV4.h"

DiskCipherAesV4::~DiskCipherAesV4()
{

}

DiskCipherAesV4::DiskCipherAesV4(const DiskParametersV4& diskParameters)
	: m_diskParameters(diskParameters)
	, m_cipherBlocksPerSector(diskParameters.getDiskSectorSize()/CryptoLib::RijndaelEngine::BlockSize)
{
	m_diskKeyCipher.SetupKey(m_diskParameters.getDiskKey());
	m_tweakKeyCipher.SetupKey(m_diskParameters.getDiskTweakKey());
}

void DiskCipherAesV4::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
{
	unsigned char indexBytes[DiskParametersV4::TweakNumberLength];

	memcpy(indexBytes, m_diskParameters.getDiskTweakNumber(), DiskParametersV4::TweakNumberLength);
	AddToInt128(indexBytes, firstBlockIndex);

	unsigned char* pBuff = static_cast<unsigned char*>(pData);

	for (UINT32 i = 0; i < blocksCount; i++)
	{
		CryptoLib::XtsSectorEncipher(m_diskKeyCipher, m_tweakKeyCipher, indexBytes, m_diskParameters.getDiskSectorSize(), pBuff);
		pBuff += m_diskParameters.getDiskSectorSize();
		IncInt128(indexBytes);
	}
}

void DiskCipherAesV4::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
{
	unsigned char indexBytes[DiskParametersV4::TweakNumberLength];

	memcpy(indexBytes, m_diskParameters.getDiskTweakNumber(), DiskParametersV4::TweakNumberLength);
	AddToInt128(indexBytes, firstBlockIndex);

	unsigned char* pPlainBuff = static_cast<unsigned char*>(pPlainData);
	unsigned char* pCipherBuff = static_cast<unsigned char*>(pCipherData);

	for (UINT32 i = 0; i < blocksCount; i++)
	{
		CryptoLib::XtsSectorEncipher(m_diskKeyCipher, m_tweakKeyCipher, indexBytes, m_diskParameters.getDiskSectorSize(), pPlainBuff, pCipherBuff);
		pPlainBuff += m_diskParameters.getDiskSectorSize();
		pCipherBuff += m_diskParameters.getDiskSectorSize();
		IncInt128(indexBytes);
	}
}

void DiskCipherAesV4::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
{
	unsigned char indexBytes[DiskParametersV4::TweakNumberLength];
	
	memcpy(indexBytes, m_diskParameters.getDiskTweakNumber(), DiskParametersV4::TweakNumberLength);

	AddToInt128(indexBytes, firstBlockIndex);
	unsigned char* pBuff = static_cast<unsigned char*>(pData);

	for (UINT32 i = 0; i < blocksCount; i++)
	{
		CryptoLib::XtsSectorDecipher(m_diskKeyCipher, m_tweakKeyCipher, indexBytes, m_diskParameters.getDiskSectorSize(), pBuff);
		pBuff += m_diskParameters.getDiskSectorSize();
		IncInt128(indexBytes);
	}
}

void DiskCipherAesV4::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
{
	unsigned char indexBytes[DiskParametersV4::TweakNumberLength];

	memcpy(indexBytes, m_diskParameters.getDiskTweakNumber(), DiskParametersV4::TweakNumberLength);

	AddToInt128(indexBytes, firstBlockIndex);

	unsigned char* pPlainBuff = static_cast<unsigned char*>(pPlainData);
	unsigned char* pCipherBuff = static_cast<unsigned char*>(pCipherData);

	for (UINT32 i = 0; i < blocksCount; i++)
	{
		CryptoLib::XtsSectorDecipher(m_diskKeyCipher, m_tweakKeyCipher, indexBytes, m_diskParameters.getDiskSectorSize(), pPlainBuff, pCipherBuff);
		pPlainBuff += m_diskParameters.getDiskSectorSize();
		pCipherBuff += m_diskParameters.getDiskSectorSize();
		IncInt128(indexBytes);
	}
}

void DiskCipherAesV4::AddToInt128( unsigned char* i128, UINT64 x )
{
	UINT64* src = reinterpret_cast<UINT64*>(i128);
	UINT64	tmp;

	tmp = src[0] + x;

	if(tmp < src[0] || tmp < x)
	{
		src[1]++;
	}
	src[0] = tmp;
}

void DiskCipherAesV4::IncInt128( unsigned char* i128 )
{
	UINT64* src = reinterpret_cast<UINT64*>(i128);
	src[0]++;
	if(src[0] == 0)
	{
		src[1]++;
	}
}
