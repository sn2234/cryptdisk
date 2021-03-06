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

#pragma once

#ifdef __cplusplus
extern "C"{
#endif 

#define	TWOFISH_KEY_BITS		(256)
#define	TWOFISH_KEY_BYTES		(TWOFISH_KEY_BITS/8)

#define	TWOFISH_BLOCK_BITS		(128)
#define	TWOFISH_BLOCK_BYTES		(TWOFISH_BLOCK_BITS/8)

#ifdef _WIN64
	// 64-bit stuff
#include "x64/twofish_ltc.h"
#else
	// 32-bit stuff
#include "x86/twofish_asm.h"
#endif

extern void __stdcall TwofishSetKey(TWOFISH_KEY_CTX *pCtx, const void *pUserKey);

extern void __stdcall TwofishEncBlock1(const TWOFISH_KEY_CTX *pCtx, const void *PlainText, void *CipherText);
extern void __stdcall TwofishEncBlock2(const TWOFISH_KEY_CTX *pCtx, void *Buff);

extern void __stdcall TwofishXorEnc1(const TWOFISH_KEY_CTX *pCtx, void *XorData, const void *PlainText, void *CipherText);
extern void __stdcall TwofishXorEnc2(const TWOFISH_KEY_CTX *pCtx, void *XorData, void *Buff);

extern void __stdcall TwofishDecBlock1(const TWOFISH_KEY_CTX *pCtx, const void *CipherText, void *PlainText);
extern void __stdcall TwofishDecBlock2(const TWOFISH_KEY_CTX *pCtx, void *Buff);

extern void __stdcall TwofishDecXor1(const TWOFISH_KEY_CTX *pCtx, void *XorData, const void *CipherText, void *PlainText);
extern void __stdcall TwofishDecXor2(const TWOFISH_KEY_CTX *pCtx, void *XorData, void *Buff);

#ifdef __cplusplus
};
#endif 
