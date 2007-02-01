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

#ifndef	_LRW_MATH_H_INCLUDED_
#define	_LRW_MATH_H_INCLUDED_

#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#pragma pack(push,16)
typedef __declspec(align(16)) struct LRW_CONTEXT
{
	UCHAR	m_gfTable[2048];	// table for optimized GF multiplication
	UCHAR	m_lrwTable[2048];	// table for optimized consecutive numbers multiplication
	UCHAR	m_key[16];			// tweak key
	UCHAR	m_currentTweak[16];	// current tweak value
	UCHAR	m_currentIndex[16];	// current index
}LRW_CONTEXT;
#pragma pack(pop)

void __stdcall GFMult(void *pDest, void *pC, void *pY);
void __stdcall GFPrepareTable(void *pTable, void *pConstant);
void __stdcall GFMultTable(void *pTable, void *pDest, void *pSrc);
void __stdcall LRWPrepareTable(void *pTable, void *pKeyTable);
void __stdcall LRWMult(void *pTable, void *pNumber, void *pBuff);

void __stdcall LRWInitContext(OUT LRW_CONTEXT *pCtx, IN void *tweakKey);
void __stdcall LRWStartSequence(IN OUT LRW_CONTEXT *pCtx, IN void *indexBegin);
void __stdcall LRWXorTweak(IN LRW_CONTEXT *pCtx, OUT void *pBuff);
void __stdcall LRWNextTweak(IN LRW_CONTEXT *pCtx);

#ifdef __cplusplus
};
#endif

#endif	//_LRW_MATH_H_INCLUDED_
