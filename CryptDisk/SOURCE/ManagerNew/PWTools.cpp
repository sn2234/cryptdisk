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

#include "PWTools.h"


const char	PWTools::m_alphaLower[]=
{
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
};
const char	PWTools::m_alphaUpper[]=
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};
const char	PWTools::m_numeric[]=
{
	'0','1','2','3','4','5','6','7','8','9'
};
const char	PWTools::m_symbols[]=
{
	' ','`','~','!','@','#','$','%','^','&','*','(',')','_','+','-','=','{','}','[',']','\"','\'',':',';','<','>',',','.','?','/','|','\\'
};
const char	PWTools::m_exclude[]=
{
	'0','O','l','I','1','!','|','`','\''
};

static BOOL IsCharInSet(char ch, const char *set, int len)
{
	for(int i=0;i<len;i++)
	{
		if(set[i] == ch)
			return TRUE;
	}

	return FALSE;
}

static int CopyCharsEx(char *dest, const char *src, int len,
					   const char *set, int setLen)
{
	int		n;

	n=0;
	for(int i=0;i<len;i++)
	{
		if(! IsCharInSet(src[i], set, setLen))
		{
			dest[n]=src[i];
			n++;
		}
	}

	return n;
}

int PWTools::GetEntropy(char * pPassword, int passwordLength)
{
	int		alphaLen;
	BOOL	bAlphaUPresent=FALSE;
	BOOL	bAlphaLPresent=FALSE;
	BOOL	bNumberPresent=FALSE;
	BOOL	bSpecialPresent=FALSE;
	BOOL	bOtherPresent=FALSE;

	ASSERT(pPassword);

	if(passwordLength == 0)
	{
		return 0;
	}

	// Estimate what symbol groups present in password
	for(int i=0;i<passwordLength;i++)
	{
		BOOL	bFound=FALSE;

		if(IsCharInSet(pPassword[i], m_alphaUpper, _countof(m_alphaUpper)))
		{
			bAlphaUPresent=TRUE;
			bFound=TRUE;
		}
		if(IsCharInSet(pPassword[i], m_alphaLower, _countof(m_alphaLower)))
		{
			bAlphaLPresent=TRUE;
			bFound=TRUE;
		}
		if(IsCharInSet(pPassword[i], m_numeric, _countof(m_numeric)))
		{
			bNumberPresent=TRUE;
			bFound=TRUE;
		}
		if(IsCharInSet(pPassword[i], m_symbols, _countof(m_symbols)))
		{
			bSpecialPresent=TRUE;
			bFound=TRUE;
		}
		if(!bFound)
			bOtherPresent=TRUE;
	}

	// Compute password alphabet length
	alphaLen=0;
	if(bAlphaUPresent)
		alphaLen+=_countof(m_alphaUpper);
	if(bAlphaLPresent)
		alphaLen+=_countof(m_alphaLower);
	if(bNumberPresent)
		alphaLen+=_countof(m_numeric);
	if(bSpecialPresent)
		alphaLen+=_countof(m_symbols);
	if(bOtherPresent)
		alphaLen+=256-(_countof(m_alphaUpper)+_countof(m_alphaLower)+
			_countof(m_numeric)+_countof(m_symbols));

	// Compute entropy
	double	bitsPerChar;

	bitsPerChar=log((double)alphaLen)/log(2.0);
	return (int)ceil(bitsPerChar*(double)passwordLength);
}

BOOL PWTools::GenPassword(char *pBuff, int buffLen,
						PASSWORD_PARAMS *pParams, CryptoLib::IRandomGenerator *pRandom)
{
	char	*pAlphabet;
	int		alphaBuffLen;
	int		alphaLen;
	BOOL	bRez=FALSE;

	ASSERT(pBuff);
	ASSERT(pParams);
	ASSERT(pRandom);

	if(!buffLen)
	{
		return TRUE;
	}

	// Compute alphabet length
	alphaBuffLen=0;
	alphaLen=0;
	if(pParams->dwFlags & USE_UPPER_ALPHA)
		alphaBuffLen+=_countof(m_alphaUpper);
	if(pParams->dwFlags & USE_LOWER_ALPHA)
		alphaBuffLen+=_countof(m_alphaLower);
	if(pParams->dwFlags & USE_NUMERIC)
		alphaBuffLen+=_countof(m_numeric);
	if(pParams->dwFlags & USE_SYMBOLS)
		alphaBuffLen+=_countof(m_symbols);
	if(pParams->dwFlags & USE_CUSTOM_SET)
		alphaBuffLen+=pParams->customSetLength;

	if(alphaBuffLen <= 0)
	{
		return FALSE;
	}

	// Allocate buffer
	pAlphabet=new char[alphaBuffLen];

	char	*ptr;
	ptr=pAlphabet;

	if(! (pParams->dwFlags & USE_EASY_TO_READ))
	{
		if(pParams->dwFlags & USE_UPPER_ALPHA)
		{
			memcpy(ptr, m_alphaUpper, sizeof(m_alphaUpper));
			ptr+=sizeof(m_alphaUpper);
			alphaLen+=sizeof(m_alphaUpper);
		}
		if(pParams->dwFlags & USE_LOWER_ALPHA)
		{
			memcpy(ptr, m_alphaLower, sizeof(m_alphaLower));
			ptr+=sizeof(m_alphaLower);
			alphaLen+=sizeof(m_alphaLower);
		}
		if(pParams->dwFlags & USE_NUMERIC)
		{
			memcpy(ptr, m_numeric, sizeof(m_numeric));
			ptr+=sizeof(m_numeric);
			alphaLen+=sizeof(m_numeric);
		}
		if(pParams->dwFlags & USE_SYMBOLS)
		{
			memcpy(ptr, m_symbols, sizeof(m_symbols));
			ptr+=sizeof(m_symbols);
			alphaLen+=sizeof(m_symbols);
		}
		if(pParams->dwFlags & USE_CUSTOM_SET)
		{
			memcpy(ptr, pParams->pCustomSet, pParams->customSetLength);
			ptr+=pParams->customSetLength;
			alphaLen+=pParams->customSetLength;
		}

		ASSERT(alphaLen == alphaBuffLen);
	}
	else
	{
		if(pParams->dwFlags & USE_UPPER_ALPHA)
		{
			int n=CopyCharsEx(ptr, m_alphaUpper, sizeof(m_alphaUpper),
				m_exclude, sizeof(m_exclude));
			ptr+=n;
			alphaLen+=n;
		}
		if(pParams->dwFlags & USE_LOWER_ALPHA)
		{
			int n=CopyCharsEx(ptr, m_alphaLower, sizeof(m_alphaLower),
				m_exclude, sizeof(m_exclude));
			ptr+=n;
			alphaLen+=n;
		}
		if(pParams->dwFlags & USE_NUMERIC)
		{
			int n=CopyCharsEx(ptr, m_numeric, sizeof(m_numeric),
				m_exclude, sizeof(m_exclude));
			ptr+=n;
			alphaLen+=n;
		}
		if(pParams->dwFlags & USE_SYMBOLS)
		{
			int n=CopyCharsEx(ptr, m_symbols, sizeof(m_symbols),
				m_exclude, sizeof(m_exclude));
			ptr+=n;
			alphaLen+=n;
		}
		if(pParams->dwFlags & USE_CUSTOM_SET)
		{
			int n=CopyCharsEx(ptr, pParams->pCustomSet, pParams->customSetLength,
				m_exclude, sizeof(m_exclude));
			ptr+=n;
			alphaLen+=n;
		}
	}

	ASSERT((ptr-pAlphabet) == alphaLen);

	if(alphaLen)
	{
		// Generate password
		int i;
		for(i=0;i<buffLen;i++)
		{
			BYTE	idx;

			if(!pRandom->GenerateRandomBytes(&idx, sizeof(BYTE)))
			{
				break;
			}

			pBuff[i]=pAlphabet[idx%alphaLen];
		}

		if(i == buffLen)
		{
			bRez=TRUE;
		}
	}

	RtlSecureZeroMemory(pAlphabet, alphaLen);
	delete[] pAlphabet;
	return bRez;
}
