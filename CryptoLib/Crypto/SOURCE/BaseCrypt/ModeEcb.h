
#pragma once

namespace CryptoLib
{

template<class Engine>
void EncipherDataEcb(const Engine& cipher, size_t BlocksCount, const void *InputData, void *OutputData)
{
	unsigned char *in = (unsigned char*)InputData;
	unsigned char *out = (unsigned char*)OutputData;

	for(; BlocksCount; BlocksCount--)
	{
		cipher.EncipherBlock(in, out);
		in += Engine::BlockSize;
		out += Engine::BlockSize;
	}
}

template<class Engine>
void EncipherDataEcb(const Engine& cipher, size_t BlocksCount, void *Data)
{
	unsigned char *buff = (unsigned char*)Data;

	for(; BlocksCount; BlocksCount--)
	{
		cipher.EncipherBlock(buff);
		buff += Engine::BlockSize;
	}
}

template<class Engine>
void DecipherDataEcb(const Engine& cipher, size_t BlocksCount, const void *InputData, void *OutputData)
{
	unsigned char *in = (unsigned char*)InputData;
	unsigned char *out = (unsigned char*)OutputData;

	for(; BlocksCount; BlocksCount--)
	{
		cipher.DecipherBlock(in, out);
		in += Engine::BlockSize;
		out += Engine::BlockSize;
	}
}

template<class Engine>
void DecipherDataEcb(const Engine& cipher, size_t BlocksCount, void * Data)
{
	unsigned char *buff = (unsigned char*)Data;

	for(; BlocksCount; BlocksCount--)
	{
		cipher.DecipherBlock(buff);
		buff+=Engine::BlockSize;
	}
}

};
