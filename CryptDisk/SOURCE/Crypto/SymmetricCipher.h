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
#ifndef _INC_SYMMETRICCIPHER_41A9E0CD012A_INCLUDED
#define _INC_SYMMETRICCIPHER_41A9E0CD012A_INCLUDED

/*
	These defines must be declared

#define		ENGINE_BLOCK_SIZE
#define		ENGINE_KEY_SIZE	

*/

#pragma	intrinsic(memset,memcpy)

//##ModelId=41A9E0CD012A
template<class SymmetricCipherEngine>
class SymmetricCipher 
{
public:
	//##ModelId=41A9E1000368
	__forceinline void SetupKey(void * UserKey)
	{
		Engine.SetupKey(UserKey);
	}


	//##ModelId=41ACDC3C0292
	__forceinline void Clear()
	{
		Engine.Clear();
	}

	//##ModelId=41A9E13302FD
	void EncipherDataECB(long BlocksCount, void * InputData, void * OutputData)
	{
		unsigned char	*in,*out;

		in=(unsigned char*)InputData;
		out=(unsigned char*)OutputData;

		for(;BlocksCount;BlocksCount--)
		{
			Engine.EncipherBlock(in, out);
			in+=ENGINE_BLOCK_SIZE;
			out+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41A9E1A801F7
	void EncipherDataECB(long BlocksCount, void * Data)
	{
		unsigned char	*buff;

		buff=(unsigned char*)Data;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.EncipherBlock(buff);
			buff+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41A9E1BC0296
	void EncipherDataCBC(long BlocksCount, void * InitVector, void * InputData, void * OutputData)
	{
		unsigned char	*in,*out;

		if(!BlocksCount)
		{
			return;
		}

		in=(unsigned char*)InputData;
		out=(unsigned char*)OutputData;

		// First block special
		Engine.XorAndEncipher(InitVector,in,out);

		BlocksCount--;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.XorAndEncipher(out,
				in+ENGINE_BLOCK_SIZE,
				out+ENGINE_BLOCK_SIZE);
			in+=ENGINE_BLOCK_SIZE;
			out+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41A9E20A01B2
	void EncipherDataCBC(long BlocksCount, void * InitVector, void * Data)
	{
		unsigned char	*buff;

		if(!BlocksCount)
		{
			return;
		}

		// First block special
		Engine.XorAndEncipher(InitVector,Data);

		buff=(unsigned char*)Data;
		BlocksCount--;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.XorAndEncipher(buff, buff+ENGINE_BLOCK_SIZE);
			buff+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41B1D4450293
	void DecipherDataECB(long BlocksCount, void * InputData, void * OutputData)
	{
		unsigned char	*in,*out;

		in=(unsigned char*)InputData;
		out=(unsigned char*)OutputData;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.DecipherBlock(in,out);
			in+=ENGINE_BLOCK_SIZE;
			out+=ENGINE_BLOCK_SIZE;
		}

	}

	//##ModelId=41B1D44502BB
	void DecipherDataECB(long BlocksCount, void * Data)
	{
		unsigned char	*buff;

		buff=(unsigned char*)Data;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.DecipherBlock(buff);
			buff+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41B1D44502D9
	void DecipherDataCBC(long BlocksCount, void * InitVector, void * InputData, void * OutputData)
	{
		unsigned char	*in,*out;
		if(!BlocksCount)
		{
			return;
		}

		in=(unsigned char*)InputData;
		out=(unsigned char*)OutputData;
		// First block special
		Engine.DecipherAndXor(InitVector,in,out);

		BlocksCount--;
		for(;BlocksCount;BlocksCount--)
		{
			Engine.DecipherAndXor(in,
				in+ENGINE_BLOCK_SIZE,
				out+ENGINE_BLOCK_SIZE);
			in+=ENGINE_BLOCK_SIZE;
			out+=ENGINE_BLOCK_SIZE;
		}
	}

	//##ModelId=41B1D4450301
	void DecipherDataCBC(long BlocksCount, void * InitVector, void * Data)
	{
		unsigned char	Buff1[ENGINE_BLOCK_SIZE];
		unsigned char	Buff2[ENGINE_BLOCK_SIZE];
		unsigned char	*ptr1,*ptr2,*tmp,*data_ptr;

		if(!BlocksCount)
		{
			return;
		}

		ptr1=Buff1;
		ptr2=Buff2;

		memcpy(ptr1,Data,ENGINE_BLOCK_SIZE);
		Engine.DecipherAndXor(InitVector,Data);

		data_ptr=(unsigned char*)Data+ENGINE_BLOCK_SIZE;
		BlocksCount--;
		for(;BlocksCount;BlocksCount--)
		{
			memcpy(ptr2,data_ptr,ENGINE_BLOCK_SIZE);
			Engine.DecipherAndXor(ptr1,data_ptr);
			data_ptr+=ENGINE_BLOCK_SIZE;
			tmp=ptr1;
			ptr1=ptr2;
			ptr2=tmp;
		}
	}

protected:
	//##ModelId=41A9E5CB016D
	SymmetricCipherEngine Engine;

};

#endif /* _INC_SYMMETRICCIPHER_41A9E0CD012A_INCLUDED */
