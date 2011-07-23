/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RNDPOOL_H_HEADER_INCLUDED_BAEC29C7
#define RNDPOOL_H_HEADER_INCLUDED_BAEC29C7

#include "..\BaseCrypt\SHA256_HASH.h"

#define	POOL_BLOCKS			8
#define	POOL_BLOCK_SIZE		SHA256_DIDGEST_SIZE
#define	POOL_SIZE			(POOL_BLOCKS*POOL_BLOCK_SIZE)

namespace CryptoLib
{
class RndPool
{
public:
	RndPool()
		: m_dataCount(0)
		, m_counterAdd(0)
	{}

	~RndPool()
	{
		RtlSecureZeroMemory(m_pool, sizeof(m_pool));
	}

	// Add random data. Size must be SHA256_DIDGEST_SIZE.
	void AddData(void *pData);

	// Generates key and remix pool
	void GenKey(void *pKey);

	void Remix();

	int GetDataCount()
	{ return m_dataCount;}

protected:
	void GetKeyInternal(void *pKey);

	// How many times data was added to pool
	int		m_dataCount;

	// Current pool pointer
	ULONG	m_counterAdd;
	
	UCHAR	m_pool[POOL_SIZE];
};
};


#endif /* RNDPOOL_H_HEADER_INCLUDED_BAEC29C7 */
