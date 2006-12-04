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

#ifndef _KEY_FILE_COLLECTOR_H_INCLUDED_
#define	_KEY_FILE_COLLECTOR_H_INCLUDED_

#pragma once

class CKeyFileCollector
{
public:
	CKeyFileCollector(void)
	{
		Init();
	}

	~CKeyFileCollector(void)
	{
		Clear();
	}

	// Initialize hash state
	void Init()
	{
		m_hash.Init();
		m_filesCount=0;
	}
	// Destroy hash state
	void Clear()
	{
		m_hash.Clear();
		m_filesCount=0;
	}
	// Add data from file
	// dataLength - maximum size
	BOOL AddFile(LPCTSTR filePath, ULONG dataLength=0x1000);
	// Generate key and clear hash state
	// Size = SHA256_DIDGEST_SIZE
	void GetKey(void *pKeyBuff)
	{
		m_hash.Final(pKeyBuff);
		Clear();
	}
	int GetFilesCount()
	{
		return m_filesCount;
	}
protected:
	SHA256_HASH		m_hash;
	int				m_filesCount;
};

#endif	//_KEY_FILE_COLLECTOR_H_INCLUDED_