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


#include "TestTwofish.h"

typedef struct TWOFISH_TEST{
	BYTE key[TWOFISH_KEY_SIZE];
	BYTE plaintext[TWOFISH_BLOCK_SIZE];
	BYTE ciphertext[TWOFISH_BLOCK_SIZE];
} TWOFISH_TEST;

static const TWOFISH_TEST TwofishTests[]=
{
	{
		{00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00},		// Key
		{00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00},		// Plaintext
		{0x57,0xFF,0x73,0x9D,0x4D,0xC9,0x2C,0x1B,0xD7,0xFC,0x01,0x70,0x0C,0xC8,0x21,0x6F}		// Ciphertext
	},
	{
		{0x24,0x8A,0x7F,0x35,0x28,0xB1,0x68,0xAC,0xFD,0xD1,0x38,0x6E,0x3F,0x51,0xE3,0x0C,0x2E,0x21,0x58,0xBC,0x3E,0x5F,0xC7,0x14,0xC1,0xEE,0xEC,0xA0,0xEA,0x69,0x6D,0x48},		// Key
		{0x43,0x10,0x58,0xF4,0xDB,0xC7,0xF7,0x34,0xDA,0x4F,0x02,0xF0,0x4C,0xC4,0xF4,0x59},		// Plaintext
		{0x37,0xFE,0x26,0xFF,0x1C,0xF6,0x61,0x75,0xF5,0xDD,0xF4,0xC3,0x3B,0x97,0xA2,0x05}		// Ciphertext
	}
};

BOOL TestTwofish()
{
	int			i;
	BYTE		buff_enc[TWOFISH_BLOCK_SIZE],buff_dec[TWOFISH_BLOCK_SIZE];
	BYTE		init_vector[TWOFISH_BLOCK_SIZE];
	DNTwofish	twofish;

	memset(&init_vector,0xA5,sizeof(init_vector));
	for(i=0;i<sizeof(TwofishTests)/sizeof(TwofishTests[0]);i++)
	{
		memset(buff_enc,0,sizeof(buff_enc));
		memset(buff_dec,0,sizeof(buff_dec));

		twofish.Clear();
		twofish.SetupKey((void*)TwofishTests[i].key);
		twofish.EncipherDataECB(1,(void*)TwofishTests[i].plaintext,buff_enc);
		twofish.DecipherDataECB(1,buff_enc,buff_dec);

		if(memcmp(buff_dec,(void*)TwofishTests[i].plaintext,TWOFISH_BLOCK_SIZE))
		{
			return FALSE;
		}

		if(memcmp(buff_enc,(void*)TwofishTests[i].ciphertext,TWOFISH_BLOCK_SIZE))
		{
			return FALSE;
		}

		// Test CBC mode
		twofish.EncipherDataCBC(1,&init_vector,buff_enc);
		twofish.DecipherDataCBC(1,&init_vector,buff_enc);
		if(memcmp(buff_enc,(void*)TwofishTests[i].ciphertext,TWOFISH_BLOCK_SIZE))
		{
			return FALSE;
		}
	}

	// Test multi-block
	BYTE	*pPlain,*pCipher,*pDecipher;
	BOOL	rez=FALSE;

	pPlain=(BYTE*)GlobalAlloc(GPTR,1000*TWOFISH_BLOCK_SIZE);
	pCipher=(BYTE*)GlobalAlloc(GPTR,1000*TWOFISH_BLOCK_SIZE);
	pDecipher=(BYTE*)GlobalAlloc(GPTR,1000*TWOFISH_BLOCK_SIZE);

	memset(pPlain,0xA5,1000*TWOFISH_BLOCK_SIZE);
	memset(pCipher,0,1000*TWOFISH_BLOCK_SIZE);
	memset(pDecipher,0,1000*TWOFISH_BLOCK_SIZE);

	twofish.EncipherDataCBC(1000,&init_vector,pPlain,pCipher);
	twofish.DecipherDataCBC(1000,&init_vector,pCipher,pDecipher);
	if(memcmp(pDecipher,pPlain,1000*TWOFISH_BLOCK_SIZE))
	{
		goto exit;
	}

	memset(pCipher,0,1000*TWOFISH_BLOCK_SIZE);
	memset(pDecipher,0,1000*TWOFISH_BLOCK_SIZE);

	memcpy(pCipher,pPlain,1000*TWOFISH_BLOCK_SIZE);
	twofish.EncipherDataCBC(1000,&init_vector,pCipher);
	twofish.DecipherDataCBC(1000,&init_vector,pCipher);
	if(memcmp(pCipher,pPlain,1000*TWOFISH_BLOCK_SIZE))
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
