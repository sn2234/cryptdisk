/*
* Copyright (c) 2009, nobody
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

#include "Hmac.h"

#pragma	intrinsic(memset,memcpy)

//************************************
// Method:    Pkcs5DeriveKey
// FullName:  <HashAlgorithm>::Pkcs5DeriveKey
// Access:    public 
// Returns:   void
// Qualifier: Generates cryptographic key from user-supplied password.
//            For more details see PBKDF2 function from PKCS #5.
// Parameter: [in] size_t passwordLength
// Parameter: [in] const void * password
// Parameter: [in] size_t saltLength
// Parameter: [in] const void * salt
// Parameter: [in] unsigned long iterationCount
// Parameter: [in] size_t keyLength
// Parameter: [out] void * key
//************************************
template<class HashAlgorithm>
void Pkcs5DeriveKey(size_t passwordLength, const void *password,
					size_t saltLength, const void *salt,
					unsigned long iterationCount,
					size_t keyLength, void *key)
{
	unsigned long l = (keyLength / HashAlgorithm::didgestSize) + 
		((keyLength % HashAlgorithm::didgestSize) ? 1 : 0);
	unsigned long r = keyLength % HashAlgorithm::didgestSize;
	unsigned long blockNumber = 0;
	unsigned char result[HashAlgorithm::didgestSize];
	unsigned char *pKey = static_cast<unsigned char*>(key);

	while(l)
	{
		unsigned char u[HashAlgorithm::didgestSize];
		unsigned char u1[HashAlgorithm::didgestSize];
		const size_t buffSize = saltLength + sizeof(unsigned long);

		{
			unsigned char *tmp = new unsigned char[buffSize];

			memcpy(tmp, salt, saltLength);
			*((unsigned long*)(tmp + saltLength)) = _byteswap_ulong(blockNumber + 1);

			CalcHmac<HashAlgorithm>(passwordLength, password,
				buffSize, tmp, sizeof(u), u);
			delete[] tmp;
		}

		memcpy(result, u, HashAlgorithm::didgestSize);
		
		for (size_t c = 1; c < iterationCount; c++)
		{
			const unsigned char *from = (c & 0x01) ? u : u1;
			unsigned char *to = (c & 0x01) ? u1 : u;

			CalcHmac<HashAlgorithm>(passwordLength, password,
				HashAlgorithm::didgestSize, from, HashAlgorithm::didgestSize, to);
			XorData(HashAlgorithm::didgestSize, to, result);
		}

		if(blockNumber == (l - 1))
		{
			if(r)
			{
				memcpy(pKey, result, r);
			}
			else
			{
				memcpy(pKey, result, HashAlgorithm::didgestSize);
			}
			break;
		}
		memcpy(pKey, result, HashAlgorithm::didgestSize);
		blockNumber++;
		pKey += HashAlgorithm::didgestSize;
	}
}
