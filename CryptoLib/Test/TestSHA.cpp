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


#include "TestSHA.h"

typedef struct SHA_TEST{
	char *data;							/* Data to hash */
	int length;							/* Length of data */
	BYTE dig256[ SHA256_DIDGEST_SIZE ];	/* Digest of data */
}SHA_TEST;

const SHA_TEST ShaTests[]=
{
	{
		"abc",3,
		{
			0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
				0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
				0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
				0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
		},
	},
	{
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56,
		{
			0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
				0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
				0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
				0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
		},
			},

			{
				"aaaaa...", 1000000L,
				{
					0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92,
						0x81, 0xa1, 0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67,
						0xf1, 0x80, 0x9a, 0x48, 0xa4, 0x97, 0x20, 0x0e,
						0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, 0x2c, 0xd0
				},
			}
};

BOOL TestSHA()
{
	int				i;
	SHA256_HASH		sha;
	BYTE			buff[SHA256_DIDGEST_SIZE];
	char			*a10000;

	for(i=0;i<2;i++)
	{
		sha.Init();
		sha.Update((void*)ShaTests[i].data,ShaTests[i].length);
		sha.Final((void*)buff);

		if(memcmp(buff,ShaTests[i].dig256,SHA256_DIDGEST_SIZE))
		{
			return FALSE;
		}
	}

	if(a10000=(char*)GlobalAlloc(GPTR,1000000))
	{
		memset((void*)a10000,'a',1000000);
		sha.Init();
		sha.Update((void*)a10000,1000000);
		sha.Final((void*)buff);

		GlobalFree(a10000);
		if(memcmp(buff,ShaTests[2].dig256,SHA256_DIDGEST_SIZE))
		{
			return FALSE;
		}
	}

	return TRUE;
}
