
#include "RandomGeneratorBase.h"

namespace CryptoLib
{
RandomGeneratorBase::RandomGeneratorBase(void)
{
	InitializeCriticalSection(&m_lock);
}


RandomGeneratorBase::~RandomGeneratorBase(void)
{
}

bool RandomGeneratorBase::GenerateRandomBytes( void *pBuff, size_t size )
{
	Lock();

	if(m_pool.GetDataCount() == 0)
	{
		if(m_sampler.GetEstimatedEntropy() != 0)
		{
			BYTE	buff[SHA256_DIDGEST_SIZE];

			m_sampler.GenOutput(buff);
			m_pool.AddData(buff);

			RtlSecureZeroMemory(buff, sizeof(buff));
		}
		else
		{
			Unlock();
			return false;
		}
	}

	BYTE key[AES_KEY_SIZE];

	m_pool.GenKey(key);
	m_gen.Init(key);
	RtlSecureZeroMemory(key, sizeof(key));

	m_gen.Generate(pBuff, size);
	m_gen.Clear();

	Unlock();
	return true;
}

void RandomGeneratorBase::AddSample( const void *pData, ULONG dataSize, ULONG randomBits )
{
	Lock();

	m_sampler.AddSample(pData, dataSize, randomBits);

	if(m_sampler.GetEstimatedEntropy() >= SHA256_DIDGEST_SIZE*8)
	{
		BYTE	buff[SHA256_DIDGEST_SIZE];

		m_sampler.GenOutput(buff);
		m_pool.AddData(buff);
	}

	Unlock();
}
}