
#pragma once

#include "Tools.h"

#pragma	intrinsic(memset,memcpy)

template<class HashAlgorithm>
void CalcHmac(size_t keyLength, const void* key,
			  size_t messageLength, const void* message,
			  size_t outputLength, void* output)
{
	unsigned char k0[HashAlgorithm::blockSize];

	// Prepare internal key
	if(keyLength == HashAlgorithm::blockSize)
	{
		memcpy(k0, key, keyLength);
	}
	else if(keyLength < HashAlgorithm::blockSize)
	{
		memcpy(k0, key, keyLength);
		memset(k0 + keyLength, 0, HashAlgorithm::blockSize - keyLength);
	}
	else
	{
		HashAlgorithm hash;
		hash.Init();
		hash.Update(key, keyLength);
		hash.Final(k0);
		hash.Clear();

		memset(k0 + HashAlgorithm::didgestSize, 0,
			HashAlgorithm::blockSize - HashAlgorithm::didgestSize);
	}

	// Prepare keys xor'ed with IPAD and OPAD
	unsigned char k0opad[HashAlgorithm::blockSize];
	memcpy(k0opad, k0, sizeof(k0));

	const unsigned char IPAD = 0x36;
	const unsigned char OPAD = 0x5C;

	XorData(sizeof(k0), k0, IPAD);
	XorData(sizeof(k0opad), k0opad, OPAD);


	unsigned char tmp[HashAlgorithm::didgestSize];

	// Calculate H((K0^ipad)||text)
	{
		HashAlgorithm hash;
		hash.Init();
		hash.Update(k0, sizeof(k0));
		hash.Update(message, messageLength);
		hash.Final(tmp);
		hash.Clear();
	}

	// Calculate H((K0^opad)||H((K0^ipad)||text))
	{
		HashAlgorithm hash;
		hash.Init();
		hash.Update(k0opad, sizeof(k0opad));
		hash.Update(tmp, sizeof(tmp));
		hash.Final(tmp);
		hash.Clear();
	}

	// Produce MAC
	if(outputLength < HashAlgorithm::didgestSize)
	{
		memcpy(output, tmp, outputLength);
	}
	else
	{
		memcpy(output, tmp, HashAlgorithm::didgestSize);
		memset(static_cast<unsigned char*>(output) + HashAlgorithm::didgestSize, 0,
			outputLength - HashAlgorithm::didgestSize);
	}
}
