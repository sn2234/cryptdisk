
#pragma once

#include "Hmac.h"

#pragma	intrinsic(memset,memcpy)

template<class HashAlgorithm>
void Pkcs5DeriveKey(size_t passwordLength, const void *password,
					size_t saltLength, const void *salt,
					unsigned long iterationCount,
					size_t keyLength, void *key)
{
	unsigned long l = (keyLength / HashAlgorithm::didgestSize) + 
		(keyLength % HashAlgorithm::didgestSize) ? 1 : 0;
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
