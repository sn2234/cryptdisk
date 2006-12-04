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

#ifndef	_RC4_H_INCLUDED
#define	_RC4_H_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif 

typedef	struct RC4_KEY_CTX
{
	unsigned char State[256];
	unsigned long x;
	unsigned long y;
	
}RC4_KEY_CTX;

extern void __stdcall rc4KeySetup(RC4_KEY_CTX *pCtx, void *pUserKey, int KeyLengthBytes);
extern void __stdcall rc4Gen(RC4_KEY_CTX *pCtx, void *Buff, int BuffLen);
extern void __stdcall rc4Xor(RC4_KEY_CTX *pCtx, void *Buff, int BuffLen);
extern void __stdcall rc4Skip(RC4_KEY_CTX *pCtx,int SkipLen);

#ifdef __cplusplus
};
#endif 

#endif	//_RC4_H_INCLUDED