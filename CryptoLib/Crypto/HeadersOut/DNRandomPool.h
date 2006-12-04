
/*********************************************************************
	
	Random pool
	
	nobody, 21.03.2005

*********************************************************************/

#ifndef _DNRANDOMPOOL_H_INCLUDED
#define	_DNRANDOMPOOL_H_INCLUDED

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#pragma pack(push)
#pragma pack(1)

#define POOL_BLOCKS		8
#define POOL_SIZE		(POOL_BLOCKS*AES_BLOCK_SIZE)

class DNRandomPool
{
public:
	UCHAR	Pool[POOL_SIZE];
	UCHAR	Buff[AES_KEY_SIZE];
	ULONG	Counter;				// Used for getting gandom data
	int		bytesInBuff;
	int		currentBlock;			// Used for adding random samle

	void	Init();
	void	Clear();

	void	Remix();
	void	AddByte(UCHAR Byte);
	void	FlushBuff();
	
	__forceinline void AddDword(ULONG Dword)
	{
		AddByte((UCHAR)Dword);
		AddByte((UCHAR)(Dword>>8)&0xFF);
		AddByte((UCHAR)(Dword>>16)&0xFF);
		AddByte((UCHAR)(Dword>>24));
	}

	void	GetRandomBytes(void *pData,ULONG Size);		// Get random data immediately from pool
	void	GetRandomBytesFast(void *pData,ULONG Size);	// Generate random data using RC4

};

#pragma pack(pop)

#endif	//_DNRANDOMPOOL_H_INCLUDED
