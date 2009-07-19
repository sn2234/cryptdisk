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

#ifndef RANDOM_H_HEADER_INCLUDED_BAEC765F
#define RANDOM_H_HEADER_INCLUDED_BAEC765F

#define	SEED_FILE_SIZE	4096

namespace CryptoLib
{
struct RND_MOUSE_EVENT
{
	DWORD		position;	// 4
	DWORD		time;		// 2
};

struct RND_KEYBOARD_EVENT
{
	DWORD		vkCode;		// 2
	DWORD		scanCode;	// 2
	DWORD		flags;		// 1
	DWORD		time;		// 2
};

class CRandom
{
public:
	CRandom()
	{
		InitializeCriticalSection(&m_lock);

		m_sampler.Init();
		m_pool.Init();

		m_seedFileName[0]=0;
	}

	~CRandom()
	{
		m_sampler.Clear();
		m_pool.Clear();
		m_gen.Clear();

		DeleteCriticalSection(&m_lock);
	}


	void AddMouseEvent(RND_MOUSE_EVENT *pEvent);
	void AddKeyboardEvent(RND_KEYBOARD_EVENT *pEvent);
	void AddSystemRandom();

	void AddSample(void *pData, ULONG dataSize, ULONG randomBits);

	void LoadSeedFile(TCHAR *fileName);
	void SaveSeedFile(TCHAR *fileName);

	void Init(TCHAR *seedFileName);
	void Cleanup();

	BOOL GenRandom(void *pBuff, ULONG size);

protected:
	void Lock()
	{
		EnterCriticalSection(&m_lock);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_lock);
	}

	CRITICAL_SECTION	m_lock;
	CRndSampler			m_sampler;
	CRndPool			m_pool;
	CRndGenerator		m_gen;
	TCHAR				m_seedFileName[MAX_PATH];
};
};

#endif /* RANDOM_H_HEADER_INCLUDED_BAEC765F */
