
#pragma once

#include "Tools.h"

template<class Engine>
void XtsSectorEncipher(Engine cipherK1,
					   Engine cipherK2,
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

template<class Engine>
void XtsSectorDecipher(Engine cipherK1,
					   Engine cipherK2,
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
