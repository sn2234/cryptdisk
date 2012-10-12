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

#include "../Primitives/Tools.h"

namespace CryptoLib
{

//************************************
// Method:    XtsSectorEncipher
// FullName:  <Engine>::XtsSectorEncipher
// Access:    public 
// Returns:   void
// Qualifier: Encipher data unit (sector) using XTS algorithm.
//            For more details see IEEE 1619.
// Parameter: [in] const Engine& cipherK1 - disk key cipher
// Parameter: [in] const Engine& cipherK2 - tweak key cipher
// Parameter: [in] const unsigned char * tweak
// Parameter: [in] size_t length
// Parameter: [in] const void * source
// Parameter: [out] void * destination
//************************************
template<class Engine>
void XtsSectorEncipher(const Engine& cipherK1,
					   const Engine& cipherK2,
					   const unsigned char *tweak, // [Engine::BlockSize]
					   size_t length,
					   const void *source,
					   void *destination)
{
	unsigned char encTweak[Engine::BlockSize];

	cipherK2.EncipherBlock(tweak, encTweak);

	const unsigned char *pSrc = static_cast<const unsigned char*>(source);
	unsigned char *pDst = static_cast<unsigned char*>(destination);

	for (size_t i = 0; i < length/Engine::BlockSize; i++)
	{
		XorData(Engine::BlockSize, pSrc, encTweak, pDst);

		cipherK1.EncipherBlock(pDst);

		XorData(Engine::BlockSize, encTweak, pDst);

		XtsGfMult(Engine::BlockSize, encTweak);

		pSrc += Engine::BlockSize;
		pDst += Engine::BlockSize;
	}
}

//************************************
// Method:    XtsSectorEncipher
// FullName:  <Engine>::XtsSectorEncipher
// Access:    public 
// Returns:   void
// Qualifier: Encipher data unit (sector) using XTS algorithm.
//            For more details see IEEE 1619.
// Parameter: [in] const Engine& cipherK1 - disk key cipher
// Parameter: [in] const Engine& cipherK2 - tweak key cipher
// Parameter: [in] const unsigned char * tweak
// Parameter: [in] size_t length
// Parameter: [out] void * data
//************************************
template<class Engine>
void XtsSectorEncipher(const Engine& cipherK1,
					   const Engine& cipherK2,
					   const unsigned char *tweak, // [Engine::BlockSize]
					   size_t length,
					   void *data)
{
	unsigned char encTweak[Engine::BlockSize];

	cipherK2.EncipherBlock(tweak, encTweak);

	unsigned char *pDst = static_cast<unsigned char*>(data);

	for (size_t i = 0; i < length/Engine::BlockSize; i++)
	{
		XorData(Engine::BlockSize, encTweak, pDst);

		cipherK1.EncipherBlock(pDst);

		XorData(Engine::BlockSize, encTweak, pDst);

		XtsGfMult(Engine::BlockSize, encTweak);

		pDst += Engine::BlockSize;
	}
}

//************************************
// Method:    XtsSectorDecipher
// FullName:  <Engine>::XtsSectorDecipher
// Access:    public 
// Returns:   void
// Qualifier: Decipher data unit (sector) using XTS algorithm.
//            For more details see IEEE 1619.
// Parameter: [in] const Engine& cipherK1 - disk key cipher
// Parameter: [in] const Engine& cipherK2 - tweak key cipher
// Parameter: [in] const unsigned char * tweak
// Parameter: [in] size_t length
// Parameter: [in] const void * source
// Parameter: [in, out] void * destination
//************************************
template<class Engine>
void XtsSectorDecipher(const Engine& cipherK1,
					   const Engine& cipherK2,
					   const unsigned char *tweak, // [Engine::BlockSize]
					   size_t length,
					   const void *source,
					   void *destination)
{
	unsigned char encTweak[Engine::BlockSize];

	cipherK2.EncipherBlock(tweak, encTweak);

	const unsigned char *pSrc = static_cast<const unsigned char*>(source);
	unsigned char *pDst = static_cast<unsigned char*>(destination);

	for (size_t i = 0; i < length/Engine::BlockSize; i++)
	{
		XorData(Engine::BlockSize, pSrc, encTweak, pDst);

		cipherK1.DecipherBlock(pDst);

		XorData(Engine::BlockSize, encTweak, pDst);

		XtsGfMult(Engine::BlockSize, encTweak);

		pSrc += Engine::BlockSize;
		pDst += Engine::BlockSize;
	}
}

//************************************
// Method:    XtsSectorDecipher
// FullName:  <Engine>::XtsSectorDecipher
// Access:    public 
// Returns:   void
// Qualifier: Decipher data unit (sector) using XTS algorithm.
//            For more details see IEEE 1619.
// Parameter: [in] const Engine& cipherK1 - disk key cipher
// Parameter: [in] const Engine& cipherK2 - tweak key cipher
// Parameter: [in] const unsigned char * tweak
// Parameter: [in] size_t length
// Parameter: [in, out] void * data
//************************************
template<class Engine>
void XtsSectorDecipher(const Engine& cipherK1,
					   const Engine& cipherK2,
					   const unsigned char *tweak, // [Engine::BlockSize]
					   size_t length,
					   void *data)
{
	unsigned char encTweak[Engine::BlockSize];

	cipherK2.EncipherBlock(tweak, encTweak);

	unsigned char *pDst = static_cast<unsigned char*>(data);

	for (size_t i = 0; i < length/Engine::BlockSize; i++)
	{
		XorData(Engine::BlockSize, encTweak, pDst);

		cipherK1.DecipherBlock(pDst);

		XorData(Engine::BlockSize, encTweak, pDst);

		XtsGfMult(Engine::BlockSize, encTweak);

		pDst += Engine::BlockSize;
	}
}

}
