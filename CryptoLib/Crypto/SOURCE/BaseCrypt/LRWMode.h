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

#ifndef	_LRW_MODE_H_INCLUDED_
#define	_LRW_MODE_H_INCLUDED_

#pragma once

#include "../Primitives/LRWMath.h"

namespace CryptoLib
{
class LRWMode
{
public:
	LRW_CONTEXT		m_ctx;

	__forceinline void Init(const void *tweakKey)
	{
		LRWInitContext(&m_ctx, tweakKey);
	}

	__forceinline void StartSequence(const void *indexBegin)
	{
		LRWStartSequence(&m_ctx, indexBegin);
	}

	__forceinline void XorTweak(void *pBuff)
	{
		LRWXorTweak(&m_ctx, pBuff);
	}

	__forceinline UCHAR * GetTweak()
	{
		return m_ctx.m_currentTweak;
	}

	__forceinline void NextTweak()
	{
		LRWNextTweak(&m_ctx);
	}

	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&m_ctx;
		int				i;

		i=sizeof(LRW_CONTEXT);
		while(i)
		{
			*ptr++=0;
			i--;
		}
	}
};
};

#endif	//_LRW_MODE_H_INCLUDED_