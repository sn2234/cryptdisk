///////////////////////////////////////////////////////////
//  DiskCipherAesV4.h
//  Implementation of the Class DiskCipherAesV4
//  Created on:      13-dec-2009 23:35:23
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_65A25D2E_A7B9_4f8a_BEB0_71ECCE625038__INCLUDED_)
#define EA_65A25D2E_A7B9_4f8a_BEB0_71ECCE625038__INCLUDED_

#include "DiskParametersV4.h"
#include "IDiskCipher.h"

class DiskCipherAesV4 : public IDiskCipher
{

public:
	DiskCipherAesV4(const DiskParametersV4 diskParameters);
	virtual ~DiskCipherAesV4();
	DiskCipherAesV4(const DiskCipherAesV4& theDiskCipherAesV4);

	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void EncipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, PVOID pData);
	virtual void DecipherDataBlocks(UINT64 firstBlockIndex, UINT32 blocksCount, const PVOID pPlainData, PVOID pCipherData);
};
#endif // !defined(EA_65A25D2E_A7B9_4f8a_BEB0_71ECCE625038__INCLUDED_)
