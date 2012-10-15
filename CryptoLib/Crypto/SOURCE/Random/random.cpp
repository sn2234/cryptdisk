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

#include <tchar.h>
#include <Windows.h>
#include <string.h>

#include "DNAES.h"
#include "SHA256_HASH.h"

#include "EncryptionModes.h"

#include "RndGenerator.h"
#include "RndSampler.h"
#include "RndPool.h"

#include "Random.h"

namespace CryptoLib
{
void CRandom::AddMouseEvent(RND_MOUSE_EVENT *pEvent)
{
	LARGE_INTEGER	timestamp;

	AddSample(pEvent, sizeof(RND_MOUSE_EVENT), 6);

	if(QueryPerformanceCounter(&timestamp))
	{
		AddSample(&timestamp, sizeof(timestamp), 2);
	}
}

void CRandom::AddKeyboardEvent(RND_KEYBOARD_EVENT *pEvent)
{
	LARGE_INTEGER	timestamp;

	AddSample(pEvent, sizeof(RND_KEYBOARD_EVENT), 7);

	if(QueryPerformanceCounter(&timestamp))
	{
		AddSample(&timestamp, sizeof(timestamp), 2);
	}
}

void CRandom::AddSystemRandom()
{
#pragma region "System random"
	// Add system parameters
	DWORD_PTR	dwTemp;

	dwTemp=(DWORD_PTR)GetActiveWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCapture();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetClipboardOwner();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetClipboardViewer();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCurrentProcessId();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCurrentThreadId();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetDesktopWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetFocus();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetInputState();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetMessagePos();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetMessageTime();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetOpenClipboardWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetProcessHeap();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetProcessWindowStation();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetTickCount();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);

	POINT	pt;

	GetCaretPos(&pt);
	AddSample(&pt, sizeof(pt), 4);
	GetCursorPos(&pt);
	AddSample(&pt, sizeof(pt), 4);

	MEMORYSTATUSEX	memstat;

	GlobalMemoryStatusEx(&memstat);
	AddSample(&memstat, sizeof(memstat), 6);

	HANDLE	hndl;
	struct
	{
		FILETIME	creation;
		FILETIME	exit;
		FILETIME	kernel;
		FILETIME	user;
	}times_info;

	hndl=GetCurrentThread();
	GetThreadTimes(hndl, &times_info.creation, &times_info.exit, &times_info.kernel,
		&times_info.user);
	AddSample(&times_info, sizeof(times_info), 12);
	hndl=GetCurrentProcess();
	GetProcessTimes(hndl, &times_info.creation, &times_info.exit, &times_info.kernel,
		&times_info.user);
	AddSample(&times_info, sizeof(times_info), 12);

	SIZE_T	minSize,maxSize;

	GetProcessWorkingSetSize(hndl, &minSize, &maxSize);
	AddSample(&minSize, sizeof(minSize), 4);
	AddSample(&maxSize, sizeof(maxSize), 4);

#pragma endregion 

#pragma region "MS CryptoAPI"
	// Add data from MS CryptoAPI
	HCRYPTPROV		hProv=0;
	BYTE			rnd_buff[256];

	if((!CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_SILENT))&&
		(!CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_SILENT|CRYPT_NEWKEYSET)))
	{
		hProv=0;
	}
	if(hProv)
	{
		CryptGenRandom(hProv,sizeof(rnd_buff),rnd_buff);
		CryptReleaseContext(hProv,0);

		AddSample(rnd_buff, sizeof(rnd_buff), sizeof(rnd_buff)*8);
	}
	RtlSecureZeroMemory(rnd_buff,sizeof(rnd_buff));
#pragma endregion 
}

void CRandom::AddSample(void *pData, ULONG dataSize, ULONG randomBits)
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

void CRandom::LoadSeedFile(TCHAR *fileName)
{
	HANDLE	hFile, hMapping;
	LPVOID	pData;

	Lock();

	hFile=CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		hMapping=CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,
			SEED_FILE_SIZE, NULL);
		if(hMapping)
		{
			pData=MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, SEED_FILE_SIZE);
			if(pData)
			{
				AddSample(pData, SEED_FILE_SIZE, SEED_FILE_SIZE*8);

				UnmapViewOfFile(pData);
			}
			CloseHandle(hMapping);
		}
		CloseHandle(hFile);
	}

	Unlock();
}

void CRandom::SaveSeedFile(TCHAR *fileName)
{
	HANDLE	hFile, hMapping;
	LPVOID	pData;

	Lock();

	hFile=CreateFile(fileName, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		hMapping=CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0,
			SEED_FILE_SIZE, NULL);
		if(hMapping)
		{
			pData=MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, SEED_FILE_SIZE);
			if(pData)
			{
				BYTE	key[AES_KEY_SIZE];

				m_pool.GenKey(key);
				m_gen.Init(key);
				m_gen.Generate(pData, SEED_FILE_SIZE);
				m_gen.Clear();

				UnmapViewOfFile(pData);
			}
			CloseHandle(hMapping);
		}
		CloseHandle(hFile);
	}

	Unlock();
}

void CRandom::Init(TCHAR *seedFileName)
{
	LoadSeedFile(seedFileName);
	_tcscpy_s(m_seedFileName, seedFileName);
	AddSystemRandom();
}

void CRandom::Cleanup()
{
	SaveSeedFile(m_seedFileName);
}

BOOL CRandom::GenRandom(void *pBuff, ULONG size)
{
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
			return FALSE;
		}
	}
	
	BYTE key[AES_KEY_SIZE];

	m_pool.GenKey(key);
	m_gen.Init(key);
	RtlSecureZeroMemory(key, sizeof(key));

	m_gen.Generate(pBuff, size);
	m_gen.Clear();

	return TRUE;
}
};
