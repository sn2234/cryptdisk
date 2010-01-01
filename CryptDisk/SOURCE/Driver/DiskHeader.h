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

#ifndef	_DISK_HEADER_H_INCLUDED_
#define	_DISK_HEADER_H_INCLUDED_

#pragma once

#include "DiskCipher.h"

class CDiskHeader
{
public:
	// For Kernel-Mode
	void Init(DISK_HEADER *pHeader)
	{
		m_pHeader=pHeader;
	}

	BOOL Open(UCHAR *pUserKey, DISK_CIPHER algoId);		// Ignore salt in header
	void Clear()
	{
		RtlSecureZeroMemory(m_userKey, sizeof(m_userKey));
	}

	// For User-Mode
#ifdef	_USER_MODE_
	CDiskHeader()
	{
		m_pHeader=NULL;
	}

	~CDiskHeader()
	{
		Clear();
	}

	BOOL Create(CRandom *pRndGen);
	void Seal(UCHAR *passwordData, ULONG passwordLength, DISK_CIPHER algoId);

	BOOL Open(UCHAR *passwordData, ULONG passwordLength);
#endif	//_USER_MODE_

	UCHAR* GetUserKey()
	{
		return m_userKey;
	}

	DISK_CIPHER GetAlgoId()
	{
		return m_algoId;
	}

protected:
	DISK_CIPHER		m_algoId;
	DISK_HEADER		*m_pHeader;
	UCHAR			m_userKey[AES_KEY_SIZE];
};

#endif	// _DISK_HEADER_H_INCLUDED_
