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

#include "stdafx.h"

#include "Common.h"

#define		DEFAULT_BUFF_SIZE		(128*0x1000)

//##ModelId=43271066001F
BOOL DiskImage::Create(const TCHAR *filePath, IMAGE_CREATE_INFO *pCreateInfo, CRandom *pRandom, UCHAR *password, ULONG passwordLength, DISK_CIPHER cipherAlgo)
{
	DWORD			dwTemp;
	BOOL			bResult;
	LARGE_INTEGER	temp;

	// Validate parameters
	memcpy(&imageParams,pCreateInfo,sizeof(imageParams));
	// Align disk size to sector boundary
	if(imageParams.diskSize.QuadPart & BYTES_PER_SECTOR)
	{
		imageParams.diskSize.QuadPart=(imageParams.diskSize.QuadPart+(BYTES_PER_SECTOR-1))&
			~(BYTES_PER_SECTOR-1);
	}

	// Create file
	hFile=CreateFile(filePath,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	GetFileTime(hFile, &fileTimes.creation,
		&fileTimes.lastAccess, &fileTimes.lastWrite);

	// Create disk header
	diskHeader.Init(&headerBuff);
	if(!diskHeader.Create(pRandom))
	{
		RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
		CloseHandle(hFile);
		return FALSE;
	}
	
	diskCipher.Init(&headerBuff, cipherAlgo);
	
	diskHeader.Seal(password, passwordLength, cipherAlgo);

	// Write disk header
	bResult=WriteFile(hFile, &headerBuff, sizeof(DISK_HEADER), &dwTemp, NULL);
	if(!bResult)
	{
		diskHeader.Clear();
		diskCipher.Clear();
		RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
		CloseHandle(hFile);
		return FALSE;
	}

	// Set file size and calculate sectors count
	sectorsCount=(ULONG)(imageParams.diskSize.QuadPart/BYTES_PER_SECTOR);
	temp.QuadPart=imageParams.diskSize.QuadPart+sizeof(DISK_HEADER);
	if(SetFilePointerEx(hFile, temp,
		&temp, FILE_BEGIN))
	{
		if(SetEndOfFile(hFile))
		{
			Initialized=TRUE;
			return TRUE;
		}
	}

	diskHeader.Clear();
	diskCipher.Clear();
	RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
	CloseHandle(hFile);
	DeleteFile(filePath);
	return FALSE;
}

//##ModelId=432710E101D4
BOOL DiskImage::Open(const TCHAR *filePath, UCHAR *password, ULONG passwordLength)
{
	if(Initialized)
	{
		Close();
		Initialized=FALSE;
	}

	hFile=CreateFile(filePath, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	GetFileTime(hFile, &fileTimes.creation,
		&fileTimes.lastAccess, &fileTimes.lastWrite);

	DWORD	dwResult;

	if(!ReadFile(hFile, &headerBuff, sizeof(headerBuff), &dwResult, NULL))
	{
		SetFileTime(hFile, &fileTimes.creation,
			&fileTimes.lastAccess, &fileTimes.lastWrite);
		CloseHandle(hFile);
		return FALSE;
	}

	diskHeader.Init(&headerBuff);
	if(diskHeader.Open(password, passwordLength))
	{
		diskCipher.Init(&headerBuff, diskHeader.GetAlgoId());
		Initialized=TRUE;
		return TRUE;
	}
	else
	{
		diskHeader.Clear();
		RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
		SetFileTime(hFile, &fileTimes.creation,
			&fileTimes.lastAccess, &fileTimes.lastWrite);
		CloseHandle(hFile);
		return FALSE;
	}
}

//##ModelId=4327149D001F
void DiskImage::Close()
{
	diskHeader.Clear();
	diskCipher.Clear();
	RtlSecureZeroMemory(&headerBuff, sizeof(headerBuff));
	if(Initialized)
	{
		SetFileTime(hFile, &fileTimes.creation,
			&fileTimes.lastAccess, &fileTimes.lastWrite);
		CloseHandle(hFile);
	}
	Initialized=FALSE;
}

BOOL DiskImage::ReadSectors(ULONG sectorBegin, ULONG sectorCount, void *pBuff)
{
	ULONG				bytesToRead;
	DWORD				dwResult;
	LARGE_INTEGER		fileOffset,temp;

	bytesToRead=sectorCount*BYTES_PER_SECTOR;
	fileOffset.QuadPart=sectorBegin*BYTES_PER_SECTOR;

	if(fileOffset.QuadPart+bytesToRead > imageParams.diskSize.QuadPart)
	{
		return FALSE;
	}

	fileOffset.QuadPart+=sizeof(DISK_HEADER);

	if(!SetFilePointerEx(hFile, fileOffset, &temp, FILE_BEGIN))
	{
		return FALSE;
	}

	if(!ReadFile(hFile, pBuff, bytesToRead, &dwResult, NULL))
	{
		return FALSE;
	}

	diskCipher.DecipherSectors(sectorBegin, sectorCount, pBuff);
	
	return TRUE;
}

BOOL DiskImage::WriteSectors(ULONG sectorBegin, ULONG sectorCount, void *pBuff)
{
	ULONG				bytesToRead;
	DWORD				dwResult;
	LARGE_INTEGER		fileOffset,temp;

	bytesToRead=sectorCount*BYTES_PER_SECTOR;
	fileOffset.QuadPart=sectorBegin*BYTES_PER_SECTOR;
	if(fileOffset.QuadPart+bytesToRead > imageParams.diskSize.QuadPart)
	{
		return FALSE;
	}

	fileOffset.QuadPart+=sizeof(DISK_HEADER);

	if(!SetFilePointerEx(hFile, fileOffset, &temp, FILE_BEGIN))
	{
		return FALSE;
	}

	diskCipher.EncipherSectors(sectorBegin, sectorCount, pBuff);

	if(!WriteFile(hFile, pBuff, bytesToRead, &dwResult, NULL))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL DiskImage::ChangePassword(UCHAR *password, ULONG passwordLength)
{
	DISK_HEADER		newHeaderBuff;
	BOOL			bRez=FALSE;

	if(! Initialized)
		return FALSE;
	if((password == NULL) || (passwordLength == 0))
		return FALSE;

	diskHeader.Seal(password, passwordLength, diskHeader.GetAlgoId());

	memcpy(&newHeaderBuff, &headerBuff, sizeof(newHeaderBuff));

	if(diskHeader.Open(password, passwordLength))
	{
		DWORD	dwResult;

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		WriteFile(hFile, &newHeaderBuff, sizeof(newHeaderBuff), &dwResult, NULL);

		bRez=TRUE;
	}
	
	return bRez;
}
