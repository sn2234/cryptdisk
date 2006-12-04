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

#ifndef	_SHA256_H_INCLUDED
#define	_SHA256_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

#define	SHA256_DIDGEST_SIZE		32
#define	SHA256_BLOCK_SIZE		64

typedef	struct	SHA256_CTX
{
	unsigned long State[8];
	unsigned long MessageLength[2];
	unsigned char Buffer[64];
	
}SHA256_CTX;

extern void __stdcall SHA256Init(SHA256_CTX *ctx);
extern void __stdcall SHA256Update(SHA256_CTX *ctx,void *Input,unsigned long InputLen);
extern void __stdcall SHA256Final(SHA256_CTX *ctx,void *Didgest);

// Internal
extern void __stdcall SHA256HashBlock(SHA256_CTX *ctx,void *Input);

#ifdef	__cplusplus
}
#endif

#endif	//_SHA256_H_INCLUDED
