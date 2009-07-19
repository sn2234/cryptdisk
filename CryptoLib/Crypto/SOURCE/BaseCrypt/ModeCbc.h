
#pragma once

#pragma	intrinsic(memset,memcpy)

namespace CryptoLib
{

template<class Engine>
void EncipherDataCbc(const Engine& cipher, size_t BlocksCount, const void *InitVector, const void *InputData, void *OutputData)
{
	if(!BlocksCount)
	{
		return;
	}

	unsigned char *in = (unsigned char*)InputData;
	unsigned char *out = (unsigned char*)OutputData;

	// First block special
	cipher.XorAndEncipher(InitVector, in, out);

	BlocksCount--;
	for(; BlocksCount; BlocksCount--)
	{
		cipher.XorAndEncipher(out,
			in + Engine::BlockSize,
			out + Engine::BlockSize);
		in += Engine::BlockSize;
		out += Engine::BlockSize;
	}
}

template<class Engine>
void EncipherDataCbc(const Engine& cipher, size_t BlocksCount, const void *InitVector, void *Data)
{
	if(!BlocksCount)
	{
		return;
	}

	// First block special
	cipher.XorAndEncipher(InitVector, Data);

	unsigned char *buff = (unsigned char*)Data;

	BlocksCount--;
	for(; BlocksCount; BlocksCount--)
	{
		cipher.XorAndEncipher(buff, buff+Engine::BlockSize);
		buff += Engine::BlockSize;
	}
}

template<class Engine>
void DecipherDataCbc(const Engine& cipher, size_t BlocksCount, const void *InitVector, const void *InputData, void *OutputData)
{
	if(!BlocksCount)
	{
		return;
	}

	unsigned char *in = (unsigned char*)InputData;
	unsigned char *out = (unsigned char*)OutputData;

	// First block special
	cipher.DecipherAndXor(InitVector, in, out);

	BlocksCount--;
	for(;BlocksCount;BlocksCount--)
	{
		cipher.DecipherAndXor(in,
			in + Engine::BlockSize,
			out + Engine::BlockSize);
		in += Engine::BlockSize;
		out += Engine::BlockSize;
	}
}

template<class Engine>
void DecipherDataCbc(const Engine& cipher, size_t BlocksCount, const void *InitVector, void *Data)
{
	if(!BlocksCount)
	{
		return;
	}

	unsigned char buff1[Engine::BlockSize];
	unsigned char buff2[Engine::BlockSize];

	unsigned char *ptr1 = buff1;
	unsigned char *ptr2 = buff2;

	memcpy(ptr1, Data, Engine::BlockSize);
	cipher.DecipherAndXor(InitVector, Data);

	unsigned char *data_ptr = (unsigned char*)Data + Engine::BlockSize;
	BlocksCount--;
	for(; BlocksCount; BlocksCount--)
	{
		memcpy(ptr2, data_ptr, Engine::BlockSize);
		cipher.DecipherAndXor(ptr1, data_ptr);
		data_ptr += Engine::BlockSize;
		unsigned char *tmp = ptr1;
		ptr1 = ptr2;
		ptr2 = tmp;
	}
}

};
