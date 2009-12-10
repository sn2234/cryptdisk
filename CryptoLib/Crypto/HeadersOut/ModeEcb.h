
#pragma once

namespace CryptoLib
{

//************************************
// Method:    EncipherDataEcb
// FullName:  CryptoLib<Engine>::EncipherDataEcb
// Access:    public 
// Returns:   void
// Qualifier: Encipher data in ECB mode.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InputData
// Parameter: [out] void * OutputData
//************************************
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

//************************************
// Method:    EncipherDataEcb
// FullName:  CryptoLib<Engine>::EncipherDataEcb
// Access:    public 
// Returns:   void
// Qualifier: Encipher data in ECB mode. In-place version.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in, out] void * Data
//************************************
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

//************************************
// Method:    DecipherDataEcb
// FullName:  CryptoLib<Engine>::DecipherDataEcb
// Access:    public 
// Returns:   void
// Qualifier: Decipher data in ECB mode.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in] const void * InputData
// Parameter: [out] void * OutputData
//************************************
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

//************************************
// Method:    DecipherDataEcb
// FullName:  CryptoLib<Engine>::DecipherDataEcb
// Access:    public 
// Returns:   void
// Qualifier: Decipher data in ECB mode. In-place version.
// Parameter: [in] const Engine & cipher
// Parameter: [in] size_t BlocksCount
// Parameter: [in, out] void * Data
//************************************
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
