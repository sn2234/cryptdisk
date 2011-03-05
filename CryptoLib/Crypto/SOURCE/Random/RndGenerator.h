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

#ifndef RNDGENERATOR_H_HEADER_INCLUDED_BAEC1B09
#define RNDGENERATOR_H_HEADER_INCLUDED_BAEC1B09

#pragma once

namespace CryptoLib
{
class CRndGenerator
{
public:
	void Init(void *pKey);

	void Generate(void *pBuff, ULONG buffSize);

	void Clear();

protected:
	void IncCounter()
	{
#if defined(AES_BLOCK_SIZE) && (AES_BLOCK_SIZE != 16)
#error	Bad counter size
#endif

#ifdef _WIN64
		UINT64* pCntr = reinterpret_cast<UINT64*>(m_counter);

		pCntr[0]++;
		if(pCntr[0] == 0)
		{
			pCntr[1]++;
		}
#else
		PVOID	ptr_cntr=(PVOID)m_counter;

		__asm
		{
			mov		eax, ptr_cntr
			mov		ecx, dword ptr [eax+0*4]
			mov		edx, dword ptr [eax+1*4]
			add		ecx, 1
			adc		edx, 0
			mov		dword ptr [eax+0*4], ecx
			mov		dword ptr [eax+1*4], edx

			mov		ecx, dword ptr [eax+2*4]
			mov		edx, dword ptr [eax+3*4]
			adc		ecx, 0
			adc		edx, 0
			mov		dword ptr [eax+2*4], ecx
			mov		dword ptr [eax+3*4], edx
		}
#endif
	}

	DNAES m_cipher;

	UCHAR m_counter[AES_BLOCK_SIZE];
};
};

#endif /* RNDGENERATOR_H_HEADER_INCLUDED_BAEC1B09 */
