
#pragma once

namespace CryptoLib
{
class IRandomGenerator
{
public:
	virtual ~IRandomGenerator() {};
	virtual bool GenerateRandomBytes(void *pBuff, size_t size) = 0;
};
}
