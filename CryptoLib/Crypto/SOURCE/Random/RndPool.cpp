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

#include <Windows.h>

#include "SHA256_HASH.h"
#include "DNAES.h"

#include "RndPool.h"

namespace CryptoLib
{
void RndPool::AddData(void *pData)
{
	PUINT64	ptr;

	ptr=(PUINT64)(m_pool+m_counterAdd*POOL_BLOCK_SIZE);

	m_counterAdd++;
	if(m_counterAdd>=POOL_BLOCKS)
	{
		m_counterAdd=0;
	}

	m_dataCount++;

	for(int i=0;i<POOL_BLOCK_SIZE/sizeof(UINT64);i++)
	{
		ptr[i]^=((PUINT64)pData)[i];
	}

	Remix();
}

void RndPool::GenKey(void *pKey)
{
	GetKeyInternal(pKey);
	Remix();
}

void RndPool::Remix()
{
	SHA256_HASH		hash;
	BYTE			didgest[SHA256_DIDGEST_SIZE];
	PUINT64			ptr;

	for(int i=0;i<POOL_BLOCKS;i++)
	{
		hash.Init();
		hash.Update(m_pool, POOL_SIZE);
		hash.Final(didgest);

		ptr=(PUINT64)(m_pool+i*POOL_BLOCK_SIZE);
		for(int j=0;j<SHA256_DIDGEST_SIZE/sizeof(UINT64);j++)
		{
			ptr[j]^=((PUINT64)didgest)[j];
		}
	}

	RtlSecureZeroMemory(didgest, sizeof(didgest));
}

void RndPool::GetKeyInternal(void *pKey)
{
	SHA256_HASH		hash;

	hash.Init();
	hash.Update(m_pool, POOL_SIZE);
	hash.Final(pKey);
}
};
