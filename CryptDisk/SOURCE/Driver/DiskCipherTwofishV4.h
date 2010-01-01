///////////////////////////////////////////////////////////
//  DiskCipherTwofishV4.h
//  Implementation of the Class DiskCipherTwofishV4
//  Created on:      09-dec-2009 1:17:11
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_276F74A9_49C0_4b4e_8C16_3DC9B023BB83__INCLUDED_)
#define EA_276F74A9_49C0_4b4e_8C16_3DC9B023BB83__INCLUDED_

#include "DiskParametersV4.h"
#include "IDiskCipher.h"

class DiskCipherTwofishV4 : public IDiskCipher
{

public:
	DiskCipherTwofishV4(const DiskParametersV4 diskParameters);
	virtual ~DiskCipherTwofishV4();
	DiskCipherTwofishV4(const DiskCipherTwofishV4& theDiskCipherV4);

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
};

#endif // !defined(EA_276F74A9_49C0_4b4e_8C16_3DC9B023BB83__INCLUDED_)
