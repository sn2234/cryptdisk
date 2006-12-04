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

#include "stdafx.h"


#include "TestAES.h"

typedef struct AES_TEST{
	BYTE key[AES_KEY_SIZE];
	BYTE plaintext[AES_BLOCK_SIZE];
	BYTE ciphertext[AES_BLOCK_SIZE];
} AES_TEST;

static const AES_TEST AesTests[]=
{
	{
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
		},
		{
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
			0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
		},
		{
			0x8E, 0xA2, 0xB7, 0xCA, 0x51, 0x67, 0x45, 0xBF, 
			0xEA, 0xFC, 0x49, 0x90, 0x4B, 0x49, 0x60, 0x89
		}
	}

};

BOOL TestAES()
{
	int		i;
	BYTE	buff_enc[AES_BLOCK_SIZE],buff_dec[AES_BLOCK_SIZE];
	BYTE	init_vector[AES_BLOCK_SIZE];
	DNAES	aes;

	memset(&init_vector,0xA5,sizeof(init_vector));
	for(i=0;i<sizeof(AesTests)/sizeof(AesTests[0]);i++)
	{
		memset(buff_enc,0,sizeof(buff_enc));
		memset(buff_dec,0,sizeof(buff_dec));

		aes.Clear();
		aes.SetupKey((void*)AesTests[i].key);
		aes.EncipherDataECB(1,(void*)AesTests[i].plaintext,buff_enc);
		aes.DecipherDataECB(1,buff_enc,buff_dec);

		if(memcmp(buff_dec,(void*)AesTests[i].plaintext,AES_BLOCK_SIZE))
		{
			return FALSE;
		}

		if(memcmp(buff_enc,(void*)AesTests[i].ciphertext,AES_BLOCK_SIZE))
		{
			return FALSE;
		}

		// Test CBC mode
		aes.EncipherDataCBC(1,&init_vector,buff_enc);
		aes.DecipherDataCBC(1,&init_vector,buff_enc);
		if(memcmp(buff_enc,(void*)AesTests[i].ciphertext,AES_BLOCK_SIZE))
		{
			return FALSE;
		}
	}

	// Test multi-block
	BYTE	*pPlain,*pCipher,*pDecipher;
	BOOL	rez=FALSE;

	pPlain=(BYTE*)GlobalAlloc(GPTR,1000*AES_BLOCK_SIZE);
	pCipher=(BYTE*)GlobalAlloc(GPTR,1000*AES_BLOCK_SIZE);
	pDecipher=(BYTE*)GlobalAlloc(GPTR,1000*AES_BLOCK_SIZE);

	memset(pPlain,0xA5,1000*AES_BLOCK_SIZE);
	memset(pCipher,0,1000*AES_BLOCK_SIZE);
	memset(pDecipher,0,1000*AES_BLOCK_SIZE);

	aes.EncipherDataCBC(1000,&init_vector,pPlain,pCipher);
	aes.DecipherDataCBC(1000,&init_vector,pCipher,pDecipher);
	if(memcmp(pDecipher,pPlain,1000*AES_BLOCK_SIZE))
	{
		goto exit;
	}

	memset(pCipher,0,1000*AES_BLOCK_SIZE);
	memset(pDecipher,0,1000*AES_BLOCK_SIZE);

	memcpy(pCipher,pPlain,1000*AES_BLOCK_SIZE);
	aes.EncipherDataCBC(1000,&init_vector,pCipher);
	aes.DecipherDataCBC(1000,&init_vector,pCipher);
	if(memcmp(pCipher,pPlain,1000*AES_BLOCK_SIZE))
	{
		goto exit;
	}

	rez=TRUE;
exit:
	GlobalFree(pDecipher);
	GlobalFree(pCipher);
	GlobalFree(pPlain);

	return rez;
}