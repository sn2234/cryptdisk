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

BOOL CKeyFileCollector::AddFile(LPCTSTR filePath, ULONG dataLength)
{
	HANDLE	hFile,hMapping;
	BOOL	bResult=FALSE;
	void	*pData;
	ULONG	fileSize;

#pragma pack(push, 1)
	struct
	{
		FILETIME			creation;
		FILETIME			lastAccess;
		FILETIME			lastWrite;
	}fileTimes;
#pragma pack(pop)

	hFile=CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(GetFileTime(hFile, &fileTimes.creation,
		&fileTimes.lastAccess, &fileTimes.lastWrite))
	{

		fileSize=GetFileSize(hFile, NULL);

		if(dataLength > fileSize)
		{
			dataLength=fileSize;
		}

		hMapping=CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,
			dataLength, NULL);
		if(hMapping)
		{
			pData=MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dataLength);
			if(pData)
			{
				m_hash.Update(pData, dataLength);
				m_filesCount++;
				bResult=TRUE;

				UnmapViewOfFile(pData);
			}
			CloseHandle(hMapping);
		}

		SetFileTime(hFile, &fileTimes.creation,
			&fileTimes.lastAccess, &fileTimes.lastWrite);
	}
	CloseHandle(hFile);
	return bResult;
}
