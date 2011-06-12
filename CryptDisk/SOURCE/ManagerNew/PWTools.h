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

#ifndef _PW_TOLS_H_INCLUDED_
#define	_PW_TOLS_H_INCLUDED_

#pragma once

class CPWTools
{
public:
	enum
	{
		USE_CUSTOM_SET		=0x00000001,
		USE_UPPER_ALPHA		=0x00000002,
		USE_LOWER_ALPHA		=0x00000004,
		USE_NUMERIC			=0x00000008,
		USE_SYMBOLS			=0x00000010,
		USE_EASY_TO_READ	=0x00000020
	};

	struct PASSWORD_PARAMS
	{
		DWORD	dwFlags;
		char	*pCustomSet;
		int		customSetLength;
	};

	static int GetEntropy(char * pPassword, int passwordLength);
	static BOOL GenPassword(char *pBuff, int buffLen,
		PASSWORD_PARAMS *pParams, CryptoLib::CRandom *pRandom);
protected:
	static const char	m_alphaLower[];		// Alpha in lower case
	static const char	m_alphaUpper[];		// Alpha in upper case
	static const char	m_numeric[];		// Numbers
	static const char	m_symbols[];		// Symbols
	static const char	m_exclude[];		// Symbols to exclude when using USE_EASY_TO_READ flag
};

#endif	//_PW_TOLS_H_INCLUDED_