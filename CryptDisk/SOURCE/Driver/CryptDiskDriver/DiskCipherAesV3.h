///////////////////////////////////////////////////////////
//  DiscCipherAesV3.h
//  Implementation of the Class DiscCipherAesV3
//  Created on:      13-dec-2009 23:35:21
//  Original author: nobody
///////////////////////////////////////////////////////////

#pragma once

#include <BaseCrypt\DNAES.h>
#include <BaseCrypt\LRWMode.h>
#include <BaseCrypt\LRWAES.h>

#include "DiskParamatersV3.h"
#include "IDiskCipher.h"

class DiscCipherAesV3 : public IDiskCipher
{
public:
	DiscCipherAesV3(const DiskParamatersV3& diskParameters);
	virtual ~DiscCipherAesV3();
	DiscCipherAesV3(const DiscCipherAesV3& theDiscCipherAesV3);

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pCipherData, PVOID pPlainData);

#if	!defined(_USER_MODE_)
	// Placement new
	void* operator new(size_t size, void* pBuff) throw() { return pBuff; }
	void operator delete(void *ptr, void* pBuff) throw() { }
#endif

#if 0	//defined(_USER_MODE_)
	void* operator new(size_t size) throw()
	{
		void* pvTemp = malloc(size);
		if (pvTemp != 0)
			memset(pvTemp, 0, size);
		return pvTemp;
	}

	void operator delete(void* ptr) noexcept { delete ptr; };
#endif

private:
	CryptoLib::LRWAES	m_cipher;
};
