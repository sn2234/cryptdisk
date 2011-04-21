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

#pragma once

#ifndef MAXIMUM_FILENAME_LENGTH
#define MAXIMUM_FILENAME_LENGTH 256
#endif

class CFavManager;

class CFavNode
{
	friend class CFavManager;
public:
	CFavNode(void);
	virtual ~CFavNode(void);

	void Init(LPCWSTR filePath, ULONG mountOptions, WCHAR driveLetter)
	{
		wcscpy_s(m_filepath, filePath);

		m_mountOptions=mountOptions;
		m_driveLetter=driveLetter;
	}

	LPCWSTR GetFilePath() const
	{
		return m_filepath;
	}

	WCHAR GetDriveLetter() const 
	{
		return m_driveLetter;
	}

	ULONG GetMountOptions()const 
	{
		return m_mountOptions;
	}


	int operator == (const CFavNode& other)
	{
		if(wcscmp(m_filepath, other.m_filepath))
			return 0;
		if(m_driveLetter != other.m_driveLetter)
			return 0;
		if(m_mountOptions != other.m_mountOptions)
			return 0;

		return 1;
	}

protected:
	WCHAR		m_filepath[MAXIMUM_FILENAME_LENGTH];
	WCHAR		m_driveLetter;
	ULONG		m_mountOptions;

	CFavNode*	m_pNext;
};

class CFavManager
{
public:
	CFavManager(void);
	virtual ~CFavManager(void);

	BOOL Load(LPCTSTR szFilePath);
	BOOL Save();
	void Close();

	int GetNodeCount()
	{
		return m_nodeCount;
	}
	const CFavNode* GetNode(int index);

	int AddNode(CFavNode *pNode);
	void RemoveNode(int index);
	void RemoveAll();

protected:
	int			m_nodeCount;
	CFavNode*	m_pFirstNode;

	BOOL		m_bOpened;
	HANDLE		m_hFile;
};
