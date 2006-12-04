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

// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "TestAES.h"
#include "TestRC4.h"
#include "TestTwofish.h"
#include "TestSHA.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("\nTesting symmetric algorithms:");

	// Test DNAES
	printf("\n Testing Rijndael-256... ");
	if(TestAES())
	{
		printf("OK");
	}
	else
	{
		printf("Error");
	}

	// Test	RC4
	printf("\n Testing RC4... ");
	if(TestRC4())
	{
		printf("OK");
	}
	else
	{
		printf("Error");
	}

	// Test Twofish
	printf("\n Testing Twofish-256... ");
	if(TestTwofish())
	{
		printf("OK");
	}
	else
	{
		printf("Error");
	}

	printf("\n\nTesting hash functions:");

	// Test hash
	printf("\n Testing SHA256... ");
	if(TestSHA())
	{
		printf("OK");
	}
	else
	{
		printf("Error");
	}

	printf("\n");
	return 0;
}

