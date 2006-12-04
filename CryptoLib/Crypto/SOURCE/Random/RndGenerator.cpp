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

#include "RndGenerator.h"

void CRndGenerator::Init(void *pKey)
{
	m_cipher.SetupKey(pKey);
	memset(m_counter, 0, sizeof(m_counter));
	m_cipher.EncipherDataECB(1, m_counter);
}

void CRndGenerator::Generate(void *pBuff, ULONG buffSize)
{
	while(buffSize >= AES_BLOCK_SIZE)
	{
		// Increment counter
		IncCounter();

		// Encipher
		m_cipher.EncipherDataECB(1, m_counter, pBuff);

		// Next block
		buffSize-=AES_BLOCK_SIZE;
		pBuff=(UCHAR*)pBuff+AES_BLOCK_SIZE;
	}

	if(buffSize)
	{
		UCHAR	tmp[AES_BLOCK_SIZE];

		IncCounter();
		m_cipher.EncipherDataECB(1, m_counter, tmp);
		memcpy(pBuff, tmp, buffSize);
		RtlSecureZeroMemory(tmp, sizeof(tmp));
	}
}

void CRndGenerator::Clear()
{
	m_cipher.Clear();
	RtlSecureZeroMemory(m_counter, sizeof(m_counter));
}
