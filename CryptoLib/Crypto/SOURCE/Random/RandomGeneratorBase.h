
#pragma once

#include "IRandomGenerator.h"
#include <Windows.h>
#include "RndSampler.h"
#include "RndPool.h"
#include "RndGenerator.h"

namespace CryptoLib
{
class RandomGeneratorBase : public IRandomGenerator
{
public:
	RandomGeneratorBase(void);
	virtual ~RandomGeneratorBase(void);

	void AddSample(const void *pData, ULONG dataSize, ULONG randomBits);

	virtual bool GenerateRandomBytes( void *pBuff, size_t size );

protected:
	_Acquires_lock_(this->m_lock) void Lock()	{ EnterCriticalSection(&m_lock); }

	_Releases_lock_(this->m_lock) void Unlock() {	LeaveCriticalSection(&m_lock); }

	CRITICAL_SECTION	m_lock;
	RndSampler			m_sampler;
	RndPool				m_pool;
	RndGenerator		m_gen;
};
}
