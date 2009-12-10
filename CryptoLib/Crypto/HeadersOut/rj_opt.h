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

#ifndef	_RJ_OPT_H_INCLUDED
#define	_RJ_OPT_H_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif 


#define		Nk		8
#define		Nb		4
#define		Nr		14

#define	RJ_KEY_LEN_BITS			(256)
#define	RJ_KEY_LEN_BYTES		(RJ_KEY_LEN_BITS/8)

#define	RJ_BLOCK_LEN_BITS		(128)
#define	RJ_BLOCK_LEN_BYTES		(RJ_BLOCK_LEN_BITS/8)

#pragma pack(push,16)
typedef	__declspec(align(16)) struct	RIJNDAEL_KEY_CTX{
	
	unsigned char	EncKey[Nb*(Nr+1)*4];
	unsigned char	DecKey[Nb*(Nr+1)*4];
	
}RIJNDAEL_KEY_CTX;
#pragma pack(pop)

extern void __stdcall SetupKey(RIJNDAEL_KEY_CTX *pCTX, void *pUserKey);

extern void __stdcall EncipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, void *PlainText, void *CipherText);
extern void __stdcall EncipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff);

extern void __stdcall XorAndEncipher1(const RIJNDAEL_KEY_CTX *pCTX, void *XorData, void *PlainText, void *CipherText);
extern void __stdcall XorAndEncipher2(const RIJNDAEL_KEY_CTX *pCTX, void *XorData, void *Buff);

extern void __stdcall DecipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, void *CipherText, void *PlainText);
extern void __stdcall DecipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff);

extern void __stdcall DecipherAndXor1(const RIJNDAEL_KEY_CTX *pCTX, void *XorData, void *CipherText, void *PlainText);
extern void __stdcall DecipherAndXor2(const RIJNDAEL_KEY_CTX *pCTX, void *XorData, void *Buff);


#ifdef __cplusplus
};
#endif 

#endif	//_RJ_OPT_H_INCLUDED
