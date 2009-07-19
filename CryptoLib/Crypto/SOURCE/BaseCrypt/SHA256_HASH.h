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
#ifndef _INC_SHA256_HASH_41A9DCB4038E_INCLUDED
#define _INC_SHA256_HASH_41A9DCB4038E_INCLUDED

#include "sha256.h"
#pragma	intrinsic(memset)

namespace CryptoLib
{
//##ModelId=41A9DCB4038E
class SHA256_HASH 
{
public:
	enum
	{
		blockSize = SHA256_BLOCK_SIZE,
		didgestSize = SHA256_DIDGEST_SIZE
	};
public:
	//##ModelId=41A9DD6D00A5
	__forceinline void Init()
	{
		SHA256Init(&Context);
	}

	//##ModelId=41A9DD6D0362
	__forceinline void Update(const void *Data, unsigned long DataLength)
	{
		SHA256Update(&Context, const_cast<void*>(Data), DataLength);
	}

	//##ModelId=41A9DD6E01E7
	__forceinline void Final(void *Didgest)
	{
		SHA256Final(&Context, Didgest);
	}

	//##ModelId=41A9DDE5030A
	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&Context;
		int				i;

		i=sizeof(SHA256_CTX);
		while(i)
		{
			*ptr++=0;
			i--;
		}
	}

protected:
	//##ModelId=41A9DDF70003
	SHA256_CTX Context;

};
};

#endif /* _INC_SHA256_HASH_41A9DCB4038E_INCLUDED */
