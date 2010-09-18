///////////////////////////////////////////////////////////
//  DiskCipherAesV4.h
//  Implementation of the Class DiskCipherAesV4
//  Created on:      13-dec-2009 23:35:23
//  Original author: nobody
///////////////////////////////////////////////////////////

#pragma once

#include <BaseCrypt\RijndaelEngine.h>

#include "DiskParametersV4.h"
#include "IDiskCipher.h"

class DiskCipherAesV4 : public IDiskCipher
{
private:
	DiskCipherAesV4(const DiskCipherAesV4& theDiskCipherAesV4);
	const DiskCipherAesV4& operator =(const DiskCipherAesV4& theDiskCipherAesV4);
public:
	DiskCipherAesV4(const DiskParametersV4& diskParameters);
	virtual ~DiskCipherAesV4();

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);

//private:
	static void AddToInt128(unsigned char* i128, UINT64 x);
	static void IncInt128(unsigned char* i128);
private:
	DiskParametersV4			m_diskParameters;
	CryptoLib::RijndaelEngine	m_diskKeyCipher;
	CryptoLib::RijndaelEngine	m_tweakKeyCipher;
	UINT32						m_cipherBlocksPerSector;
};
