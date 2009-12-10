
#pragma once

#pragma	intrinsic(memset,memcpy)

namespace CryptoLib
{

//************************************
// Method:    EncipherDataCbc
// FullName:  CryptoLib<Engine>::EncipherDataCbc
// Access:    public 
// Returns:   void
// Qualifier: Encipher data using CBC mode
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InitVector
// Parameter: [in] const void * InputData
// Parameter: [out] void * OutputData
//************************************
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

//************************************
// Method:    EncipherDataCbc
// FullName:  CryptoLib<Engine>::EncipherDataCbc
// Access:    public 
// Returns:   void
// Qualifier: Encipher data using CBC mode. In-place version.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InitVector
// Parameter: [in, out] void * Data
//************************************
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

//************************************
// Method:    DecipherDataCbc
// FullName:  CryptoLib<Engine>::DecipherDataCbc
// Access:    public 
// Returns:   void
// Qualifier: Decipher data using CBC mode.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InitVector
// Parameter: [in] const void * InputData
// Parameter: [in] void * OutputData
//************************************
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

//************************************
// Method:    DecipherDataCbc
// FullName:  CryptoLib<Engine>::DecipherDataCbc
// Access:    public 
// Returns:   void
// Qualifier: Decipher data using CBC mode. In-place version
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InitVector
// Parameter: [in, out] void * Data
//************************************
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
