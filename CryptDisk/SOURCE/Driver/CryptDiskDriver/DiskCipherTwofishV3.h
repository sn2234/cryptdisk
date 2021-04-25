///////////////////////////////////////////////////////////
//  DiskCipherV3.h
//  Implementation of the Class DiskCipherV3
//  Created on:      09-dec-2009 1:17:13
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_DC82544F_8D00_40ba_AC14_221DAFAB7011__INCLUDED_)
#define EA_DC82544F_8D00_40ba_AC14_221DAFAB7011__INCLUDED_

#include <BaseCrypt\DNTwofish.h>
#include <BaseCrypt\LRWMode.h>
#include <BaseCrypt\LRWTwofish.h>

#include "DiskParamatersV3.h"
#include "IDiskCipher.h"

class DiskCipherTwofishV3 : public IDiskCipher
{

public:
	DiskCipherTwofishV3(const DiskParamatersV3& diskParameters);
	virtual ~DiskCipherTwofishV3();
	DiskCipherTwofishV3(const DiskCipherTwofishV3& theDiskCipherTwofishV3);

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);

#if	!defined(_USER_MODE_)
	// Placement new
	void* operator new(size_t size, void* pBuff) throw() { return pBuff; }
	void operator delete(void *ptr, void* pBuff) throw() { }
#endif

private:
	CryptoLib::LRWTwofish	m_cipher;
};

#endif // !defined(EA_DC82544F_8D00_40ba_AC14_221DAFAB7011__INCLUDED_)
