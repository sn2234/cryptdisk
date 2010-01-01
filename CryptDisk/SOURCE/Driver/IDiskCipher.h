///////////////////////////////////////////////////////////
//  IDiskCipher.h
//  Implementation of the Interface IDiskCipher
//  Created on:      09-dec-2009 1:17:11
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_80FA169D_11FA_41ae_BAA1_84F3B2C5970B__INCLUDED_)
#define EA_80FA169D_11FA_41ae_BAA1_84F3B2C5970B__INCLUDED_

/**
 * Provides common interface for disk block cipher/decipher operations.
 */
class IDiskCipher
{

public:
	IDiskCipher()
	{

	}

	virtual ~IDiskCipher()
	{

	}

	IDiskCipher(const IDiskCipher& theIDiskCipher)
	{

	}

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData) =0;
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData) =0;
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData) =0;
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData) =0;

};
#endif // !defined(EA_80FA169D_11FA_41ae_BAA1_84F3B2C5970B__INCLUDED_)
