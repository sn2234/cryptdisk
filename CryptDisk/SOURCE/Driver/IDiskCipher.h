
#pragma once

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
