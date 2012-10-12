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
#ifndef _INC_RC4_CIPHER_ENGINE_41ACDCE40261_INCLUDED
#define _INC_RC4_CIPHER_ENGINE_41ACDCE40261_INCLUDED

#include "../Primitives/rc4.h"

#pragma intrinsic(memset)

namespace CryptoLib
{
//##ModelId=41ACDCE40261
class RC4_CIPHER_ENGINE 
{
public:
	~RC4_CIPHER_ENGINE()
	{
		Clear();
	}

	//##ModelId=41ACDD2A032A
	__forceinline void SetupKey(const void *UserKey, int KeyLengthBytes)
	{
		rc4KeySetup(&Context, const_cast<void*>(UserKey), KeyLengthBytes);
	}

	//##ModelId=41ACDD7E0281
	__forceinline void Generate(void *Buff, int BuffLen)
	{
		rc4Gen(&Context,Buff,BuffLen);
	}

	//##ModelId=41ACDDF7031B
	__forceinline void Xor(void *Buff, int BuffLen)
	{
		rc4Xor(&Context,Buff,BuffLen);
	}

	//##ModelId=41ACDE200220
	__forceinline void Skip(int SkipLen)
	{
		rc4Skip(&Context,SkipLen);
	}

	//##ModelId=41ACDE580284
	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&Context;
		int				i;

		i=sizeof(RC4_KEY_CTX);
		while(i)
		{
			*ptr++=0;
			i--;
		}
	}

protected:
	//##ModelId=41ACDD0E0302
	RC4_KEY_CTX Context;

};
};

#endif /* _INC_RC4_CIPHER_ENGINE_41ACDCE40261_INCLUDED */
