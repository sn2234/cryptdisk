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

class TwofishEngine 
{
public:
	__forceinline void SetupKey(void * UserKey)
	{
		TwofishSetKey(&Context,UserKey);
	}

	__forceinline void EncipherBlock(void * InputBlock, void * OutputBlock)
	{
		TwofishEncBlock1(&Context,InputBlock,OutputBlock);
	}

	__forceinline void EncipherBlock(void * Block)
	{
		TwofishEncBlock2(&Context,Block);
	}

	__forceinline void XorAndEncipher(void * XorData, void * InputBlock, void * OutputBlock)
	{
		TwofishXorEnc1(&Context,XorData,InputBlock,OutputBlock);
	}

	__forceinline void XorAndEncipher(void * XorData, void * Block)
	{
		TwofishXorEnc2(&Context,XorData,Block);
	}

	__forceinline void DecipherBlock(void * InputBlock, void * OutputBlock)
	{
		TwofishDecBlock1(&Context,InputBlock,OutputBlock);
	}

	__forceinline void DecipherBlock(void * Block)
	{
		TwofishDecBlock2(&Context,Block);
	}

	__forceinline void DecipherAndXor(void * XorData, void * InputBlock, void * OutputBlock)
	{
		TwofishDecXor1(&Context,XorData,InputBlock,OutputBlock);
	}

	__forceinline void DecipherAndXor(void * XorData, void * Block)
	{
		TwofishDecXor2(&Context,XorData,Block);
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


#endif	//_TWOFISH_ENGINE_H_INCLUDED_