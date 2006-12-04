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

#include "StdAfx.h"

#include "Common.h"

#include "FavManager.h"

#pragma pack(push, 1)
typedef struct CONFIG_FILE_BLOCK
{
	WCHAR		m_filepath[MAXIMUM_FILENAME_LENGTH];
	WCHAR		m_driveLetter;
	ULONG		m_mountOptions;
}CONFIG_FILE_BLOCK;
#pragma pack(pop)

CFavNode::CFavNode(void)
{
	m_pNext=NULL;
}

CFavNode::~CFavNode(void)
{
}

CFavManager::CFavManager(void)
{
	m_nodeCount=0;
	m_pFirstNode=NULL;
	m_bOpened=FALSE;
}

CFavManager::~CFavManager(void)
{
}

BOOL CFavManager::Load(LPCTSTR szFilePath)
{
	if(m_bOpened)
	{
		Close();
	}

	m_bOpened=FALSE;
	m_hFile=CreateFile(szFilePath, GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	m_bOpened=TRUE;

	DWORD	dwFileSize;
	dwFileSize=GetFileSize(m_hFile, NULL);

	if(dwFileSize < sizeof(CONFIG_FILE_BLOCK))
	{
		// Empty favorites file
		return TRUE;
	}

	DWORD numBlocks;

	numBlocks=dwFileSize/sizeof(CONFIG_FILE_BLOCK);

	for(DWORD i=0;i<numBlocks;i++)
	{
		CONFIG_FILE_BLOCK	cfg;
		DWORD				dwResult;

		if(!ReadFile(m_hFile, &cfg, sizeof(CONFIG_FILE_BLOCK), &dwResult, 0))
			break;

		CFavNode *pNewNode;

		pNewNode=new CFavNode;

		pNewNode->Init(cfg.m_filepath, cfg.m_mountOptions, cfg.m_driveLetter);
		AddNode(pNewNode);
	}

	return TRUE;
}

BOOL CFavManager::Save()
{
	if(!m_bOpened)
		return FALSE;

	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
	SetEndOfFile(m_hFile);

	for(int i=0;i<GetNodeCount();i++)
	{
		CONFIG_FILE_BLOCK	cfg;

		CFavNode	*pNode;

		pNode=(CFavNode*)GetNode(i);
		if(pNode)
		{
			memset(&cfg, 0, sizeof(cfg));
			wcscpy_s(cfg.m_filepath, pNode->GetFilePath());
			cfg.m_driveLetter=pNode->GetDriveLetter();
			cfg.m_mountOptions=pNode->GetMountOptions();

			DWORD	dwResult;
			WriteFile(m_hFile, &cfg, sizeof(cfg), &dwResult, NULL);
		}
	}

	return TRUE;
}

void CFavManager::Close()
{
	RemoveAll();

	if(m_bOpened)
	{
		CloseHandle(m_hFile);
		m_bOpened=FALSE;
	}
}

const CFavNode* CFavManager::GetNode(int index)
{
	CFavNode *pNode=m_pFirstNode;

	if(!pNode)
		return NULL;

	for(int i=0;i<index;i++)
	{
		pNode=pNode->m_pNext;
		ASSERT(pNode);
		if(!pNode)
			return NULL;
	}

	return pNode;
}

int CFavManager::AddNode(CFavNode *pNode)
{
	CFavNode *pNodeTmp=m_pFirstNode;
	int i;

	if(!pNodeTmp)
	{
		m_pFirstNode=pNode;
		m_nodeCount=1;
		return 0;
	}

	if(*m_pFirstNode == *pNode)
		return 0;

	i=1;
	for(;pNodeTmp->m_pNext;pNodeTmp=pNodeTmp->m_pNext)
	{
		if(*pNodeTmp == *pNode)
			return i;
		i++;
	}

	pNodeTmp->m_pNext=pNode;
	m_nodeCount++;
	return i;
}

void CFavManager::RemoveNode(int index)
{
	CFavNode *pNode=m_pFirstNode;
	CFavNode *pTmp;

	if(!pNode)
		return;

	for(int i=0;i<index-1;i++)
	{
		pNode=pNode->m_pNext;
		ASSERT(pNode);
		if(!pNode)
			return ;
	}

	if(!index)
	{
		delete m_pFirstNode;
		m_pFirstNode=NULL;
	}
	else
	{
		pTmp=pNode->m_pNext;
		pNode->m_pNext=pTmp->m_pNext;
		delete pTmp;
	}
	m_nodeCount--;
}

void CFavManager::RemoveAll()
{
	CFavNode *pTmp, *pTmp1;

	for(pTmp=m_pFirstNode;pTmp;pTmp=pTmp1)
	{
		pTmp1=pTmp->m_pNext;
		delete pTmp;
	}
	m_nodeCount=0;
}
