///////////////////////////////////////////////////////////
//  DiscCipherAesV3.h
//  Implementation of the Class DiscCipherAesV3
//  Created on:      13-dec-2009 23:35:21
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_A72286B6_4BAF_4d3a_8D2A_B5ECF3242C5C__INCLUDED_)
#define EA_A72286B6_4BAF_4d3a_8D2A_B5ECF3242C5C__INCLUDED_

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
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);

private:
	CryptoLib::LRWAES	m_cipher;
};

#endif // !defined(EA_A72286B6_4BAF_4d3a_8D2A_B5ECF3242C5C__INCLUDED_)
