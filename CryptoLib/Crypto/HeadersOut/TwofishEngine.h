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

#ifndef _TWOFISH_ENGINE_H_INCLUDED_
#define	_TWOFISH_ENGINE_H_INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "Twofish.h"

namespace CryptoLib
{
class TwofishEngine 
{
public:
	enum
	{
		BlockSize = TWOFISH_BLOCK_BYTES,
		KeySize = TWOFISH_KEY_BYTES
	};
public:
	__forceinline void SetupKey(const void *UserKey)
	{
		TwofishSetKey(&Context, const_cast<void*>(UserKey));
	}

	__forceinline void EncipherBlock(const void *InputBlock, void *OutputBlock) const
	{
		TwofishEncBlock1(&Context, const_cast<void*>(InputBlock), OutputBlock);
	}

	__forceinline void EncipherBlock(void *Block) const
	{
		TwofishEncBlock2(&Context, Block);
	}

	__forceinline void XorAndEncipher(const void *XorData, const void *InputBlock, void *OutputBlock) const
	{
		TwofishXorEnc1(&Context, const_cast<void*>(XorData),
			const_cast<void*>(InputBlock), OutputBlock);
	}

	__forceinline void XorAndEncipher(const void *XorData, void *Block) const
	{
		TwofishXorEnc2(&Context, const_cast<void*>(XorData), Block);
	}

	__forceinline void DecipherBlock(const void *InputBlock, void *OutputBlock) const
	{
		TwofishDecBlock1(&Context, const_cast<void*>(InputBlock), OutputBlock);
	}

	__forceinline void DecipherBlock(void *Block) const
	{
		TwofishDecBlock2(&Context, Block);
	}

	__forceinline void DecipherAndXor(const void *XorData, const void *InputBlock, void *OutputBlock) const
	{
		TwofishDecXor1(&Context, const_cast<void*>(XorData),
			const_cast<void*>(InputBlock), OutputBlock);
	}

	__forceinline void DecipherAndXor(const void *XorData, void *Block) const
	{
		TwofishDecXor2(&Context, const_cast<void*>(XorData), Block);
	}

	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&Context;
		int				i;

		i=sizeof(TWOFISH_KEY_CTX);
		while(i)
		{
			*ptr++=0;
			i--;
		}
	}

protected:
	TWOFISH_KEY_CTX Context;

};
};

#endif	//_TWOFISH_ENGINE_H_INCLUDED_