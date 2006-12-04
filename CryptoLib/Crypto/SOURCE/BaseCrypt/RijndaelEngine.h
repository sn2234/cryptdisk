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

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RIJNDAELENGINE_41A9DE6C0304_INCLUDED
#define _INC_RIJNDAELENGINE_41A9DE6C0304_INCLUDED

#include "rj_opt.h"

#pragma intrinsic(memset)

//##ModelId=41A9DE6C0304
class RijndaelEngine 
{
public:
	//##ModelId=41A9DEB30220
	__forceinline void SetupKey(void * UserKey)
	{
		::SetupKey(&Context,UserKey);
	}

	//##ModelId=41A9DF0C021E
	__forceinline void EncipherBlock(void * InputBlock, void * OutputBlock)
	{
		EncipherBlock1(&Context,InputBlock,OutputBlock);
	}

	//##ModelId=41A9DF6F028F
	__forceinline void EncipherBlock(void * Block)
	{
		EncipherBlock2(&Context,Block);
	}

	//##ModelId=41A9DF8300FD
	__forceinline void XorAndEncipher(void * XorData, void * InputBlock, void * OutputBlock)
	{
		XorAndEncipher1(&Context,XorData,InputBlock,OutputBlock);
	}

	//##ModelId=41A9DFCD0149
	__forceinline void XorAndEncipher(void * XorData, void * Block)
	{
		XorAndEncipher2(&Context,XorData,Block);
	}

	//##ModelId=41A9DFE9012C
	__forceinline void DecipherBlock(void * InputBlock, void * OutputBlock)
	{
		DecipherBlock1(&Context,InputBlock,OutputBlock);
	}

	//##ModelId=41A9DFF202DD
	__forceinline void DecipherBlock(void * Block)
	{
		DecipherBlock2(&Context,Block);
	}

	//##ModelId=41A9E007030F
	__forceinline void DecipherAndXor(void * XorData, void * InputBlock, void * OutputBlock)
	{
		DecipherAndXor1(&Context,XorData,InputBlock,OutputBlock);
	}

	//##ModelId=41A9E0180206
	__forceinline void DecipherAndXor(void * XorData, void * Block)
	{
		DecipherAndXor2(&Context,XorData,Block);
	}

	//##ModelId=41A9E0540130
	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&Context;
		int				i;

		i=sizeof(RIJNDAEL_KEY_CTX);
		while(i)
		{
			*ptr++=0;
			i--;
		}
	}

protected:
	//##ModelId=41A9DE9A012A
	RIJNDAEL_KEY_CTX Context;

};

#endif /* _INC_RIJNDAELENGINE_41A9DE6C0304_INCLUDED */
