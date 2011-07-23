/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RNDSAMPLER_H_HEADER_INCLUDED_BAEC28AF
#define RNDSAMPLER_H_HEADER_INCLUDED_BAEC28AF

#include "..\BaseCrypt\SHA256_HASH.h"

namespace CryptoLib
{
class RndSampler
{
public:
	RndSampler()
		:m_entropyEstimated(0)
	{}

	~RndSampler()
	{
		Clear();
	}

	void AddSample(const void *pSampleData, ULONG sampleSize, ULONG sampleEntropy)
	{
		m_hash.Update(pSampleData, sampleSize);
		m_entropyEstimated+=sampleEntropy;
	}

	void GenOutput(
		// Size must be SHA256_DIDGEST_SIZE
		void *pBuff)
	{
		m_hash.Final(pBuff);
		m_hash.Clear();
		m_hash.Init();
		m_entropyEstimated=0;
	}

	ULONG GetEstimatedEntropy()
	{
		return m_entropyEstimated;
	}

	void Clear()
	{
		m_hash.Clear();
		m_entropyEstimated=0;
	}

	void Init()
	{
		m_hash.Init();
		m_entropyEstimated=0;
	}

protected:
	SHA256_HASH m_hash;

	ULONG m_entropyEstimated;

};
};


#endif /* RNDSAMPLER_H_HEADER_INCLUDED_BAEC28AF */
