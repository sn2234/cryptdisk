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

/*********************************************************************

Key derivation function based on SHA-256


*********************************************************************/

#include <string.h>

#include "SHA256_HASH.h"

#include "DeriveKey.h"

#pragma intrinsic(memset,memcpy)

namespace CryptoLib
{

static void __fastcall ProcessRound(const void *Password,unsigned long PasswordLength,
						const void *Salt,unsigned long SaltLength,
						void *Output,
						unsigned long Iterations,
						unsigned long BlockNumber);

void __stdcall DeriveKey(const void *Password,unsigned long PasswordLength,
			   const void *Salt,unsigned long SaltLength,
			   void *Key,unsigned long KeyLength,
			   unsigned long Iterations)
{
	unsigned long	l,r,i;
	unsigned char	temp[SHA256_DIDGEST_SIZE];

	l=KeyLength/SHA256_DIDGEST_SIZE;
	r=KeyLength%SHA256_DIDGEST_SIZE;
	
	memset(temp,0,SHA256_DIDGEST_SIZE);

	for(i=0;i<l;i++)
	{
			ProcessRound(Password,PasswordLength,Salt,SaltLength,
				((unsigned char*)Key+i*SHA256_DIDGEST_SIZE),
				Iterations,i);
	}

	if(r)
	{
		ProcessRound(Password,PasswordLength,Salt,SaltLength,
			temp,Iterations,i);
		memcpy(((unsigned char*)Key+i*SHA256_DIDGEST_SIZE),temp,r);
	}
	memset(temp,0,sizeof(temp));
}

void __fastcall ProcessRound(const void *Password,unsigned long PasswordLength,
				  const void *Salt,unsigned long SaltLength,
				  void *Output,
				  unsigned long Iterations,
				  unsigned long BlockNumber)
{
	int				i;
	unsigned char	temp[SHA256_DIDGEST_SIZE];
	SHA256_HASH		hash;
	
	hash.Init();
	hash.Update(Password,PasswordLength);
	hash.Update(Salt,SaltLength);
	hash.Update(&BlockNumber,sizeof(unsigned long));
	hash.Final(Output);

	for(;Iterations;Iterations--)
	{
		hash.Init();
		hash.Update(Password,PasswordLength);
		hash.Update(Output,SHA256_DIDGEST_SIZE);
		hash.Final(temp);
		for(i=0;i<SHA256_DIDGEST_SIZE;i++)
		{
			((unsigned char*)Output)[i]^=temp[i];
		}
	}
	hash.Clear();
	memset(temp,0,sizeof(temp));
}

};
