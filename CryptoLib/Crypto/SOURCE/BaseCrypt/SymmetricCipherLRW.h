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

#ifndef	_SYMMETRIC_CIPHER_LRW_H_INCLUDED_
#define	_SYMMETRIC_CIPHER_LRW_H_INCLUDED_

/*
These defines must be declared

#define		ENGINE_BLOCK_SIZE
#define		ENGINE_KEY_SIZE	

*/

#pragma once
#pragma	intrinsic(memset,memcpy)

namespace CryptoLib
{
template<class SymmetricCipherEngine>
class SymmetricCipherLRW
{
protected:
	SymmetricCipherEngine	m_Engine;
	LRWMode					m_LRW;
public:
	__forceinline void SetupKey(const void *userKey, const void *tweakKey)
	{
		m_Engine.SetupKey(userKey);
		m_LRW.Init(tweakKey);
	}

	__forceinline void Clear()
	{
		m_Engine.Clear();
		m_LRW.Clear();
	}

	__forceinline void EncipherFirstBlock(const UCHAR *blockNumber, void *Data)
	{
		m_LRW.StartSequence(blockNumber);
		m_LRW.XorTweak(Data);
		m_Engine.EncipherBlock(Data);
		m_LRW.XorTweak(Data);
	}

	__forceinline void EncipherNextBlock(void *Data)
	{
		m_LRW.NextTweak();
		m_Engine.XorAndEncipher(m_LRW.GetTweak(), Data);
		m_LRW.XorTweak(Data);
	}

	__forceinline void EncipherFirstBlock(const UCHAR *blockNumber, const void *pPlain, void *pCipher)
	{
		m_LRW.StartSequence(blockNumber);
		m_Engine.XorAndEncipher(m_LRW.GetTweak(), pPlain, pCipher);
		m_LRW.XorTweak(pCipher);
	}

	__forceinline void EncipherNextBlock(const void *pPlain, void *pCipher)
	{
		m_LRW.NextTweak();
		m_Engine.XorAndEncipher(m_LRW.GetTweak(), pPlain, pCipher);
		m_LRW.XorTweak(pCipher);
	}

	__forceinline void DecipherFirstBlock(UCHAR *blockNumber, void *Data)
	{
		m_LRW.StartSequence(blockNumber);
		m_LRW.XorTweak(Data);
		m_Engine.DecipherBlock(Data);
		m_LRW.XorTweak(Data);
	}

	__forceinline void DecipherNextBlock(void *Data)
	{
		m_LRW.NextTweak();
		m_LRW.XorTweak(Data);
		m_Engine.DecipherAndXor(m_LRW.GetTweak(), Data);
	}

	__forceinline void DecipherFirstBlock(UCHAR *blockNumber, const void *pCipher, void *pPlain)
	{
		m_LRW.StartSequence(blockNumber);
		memcpy(pPlain, pCipher, SymmetricCipherEngine::BlockSize);
		m_LRW.XorTweak(pPlain);
		m_Engine.DecipherAndXor(m_LRW.GetTweak(), pPlain);
	}

	__forceinline void DecipherNextBlock(const void *pCipher, void *pPlane)
	{
		m_LRW.NextTweak();
		memcpy(pPlane, pCipher, SymmetricCipherEngine::BlockSize);
		m_LRW.XorTweak(pPlane);
		m_Engine.DecipherAndXor(m_LRW.GetTweak(), pPlane);
	}
};
};

#endif	//_SYMMETRIC_CIPHER_LRW_H_INCLUDED_