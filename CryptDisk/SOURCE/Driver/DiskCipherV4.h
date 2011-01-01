///////////////////////////////////////////////////////////
//  DiskCipherV4.h
//  Implementation of the Class DiskCipherV4
//  Created on:      13-dec-2009 23:35:23
//  Original author: nobody
///////////////////////////////////////////////////////////

#pragma once

#include <BaseCrypt\ModeXts.h>

#include "DiskParametersV4.h"
#include "IDiskCipher.h"

template <class Engine>
class DiskCipherV4 : public IDiskCipher
{
private:
	DiskCipherV4(const DiskCipherV4& theDiskCipherAesV4);
	const DiskCipherV4& operator =(const DiskCipherV4& theDiskCipherAesV4);
public:
	DiskCipherV4(const DiskParametersV4& diskParameters);
	virtual ~DiskCipherV4(){}

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);

	// Placement new
	void* operator new(size_t size, void* pBuff) throw() { return pBuff; }
	void operator delete(void *ptr, void* pBuff) throw() { }

//private:
	static void AddToInt128(unsigned char* i128, UINT64 x);
	static void IncInt128(unsigned char* i128);
private:
	DiskParametersV4			m_diskParameters;
	Engine						m_diskKeyCipher;
	Engine						m_tweakKeyCipher;
	UINT32						m_cipherBlocksPerSector;
};

template <class Engine>
DiskCipherV4<Engine>::DiskCipherV4(const DiskParametersV4& diskParameters)
	: m_diskParameters(diskParameters)
	, m_cipherBlocksPerSector(diskParameters.getDiskSectorSize()/Engine::BlockSize)
{
	m_diskKeyCipher.SetupKey(m_diskParameters.getDiskKey());
	m_tweakKeyCipher.SetupKey(m_diskParameters.getDiskTweakKey());
}

template <class Engine>
void DiskCipherV4<Engine>::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
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

template <class Engine>
void DiskCipherV4<Engine>::EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
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

template <class Engine>
void DiskCipherV4<Engine>::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData)
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

template <class Engine>
void DiskCipherV4<Engine>::DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData)
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

template <class Engine>
void DiskCipherV4<Engine>::AddToInt128( unsigned char* i128, UINT64 x )
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

template <class Engine>
void DiskCipherV4<Engine>::IncInt128( unsigned char* i128 )
{
	UINT64* src = reinterpret_cast<UINT64*>(i128);
	src[0]++;
	if(src[0] == 0)
	{
		src[1]++;
	}
}
