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

BOOL PWTools::GenPassword( char *pBuff, int buffLen, DWORD dwFlags, std::set<char> customCharsSet, CryptoLib::IRandomGenerator *pRandom )
{
	ASSERT(pBuff);
	ASSERT(pRandom);

	std::set<char> alphabetSet;

	if(dwFlags & USE_UPPER_ALPHA)
	{
		alphabetSet.insert(std::begin(m_alphaUpper), std::end(m_alphaUpper));
	}
	if(dwFlags & USE_LOWER_ALPHA)
	{
		alphabetSet.insert(std::begin(m_alphaLower), std::end(m_alphaLower));
	}
	if(dwFlags & USE_NUMERIC)
	{
		alphabetSet.insert(std::begin(m_numeric), std::end(m_numeric));
	}
	if(dwFlags & USE_SYMBOLS)
	{
		alphabetSet.insert(std::begin(m_symbols), std::end(m_symbols));
	}
	if(dwFlags & USE_CUSTOM_SET)
	{
		alphabetSet.insert(std::begin(customCharsSet), std::end(customCharsSet));
	}

	if(dwFlags & USE_EASY_TO_READ)
	{
		std::set<char> tmp;
		std::set<char> excluded(std::begin(m_exclude), std::end(m_exclude));

		tmp.swap(alphabetSet);
		std::set_difference(tmp.cbegin(), tmp.cend(), excluded.cbegin(), excluded.cend(), std::inserter(alphabetSet, alphabetSet.end()));
	}

	if(!alphabetSet.empty())
	{
		std::vector<char> alphabet(alphabetSet.cbegin(), alphabetSet.cend());
		UINT32	idx;

		for (int i = 0; i < buffLen; i++)
		{
			if(!pRandom->GenerateRandomBytes(&idx, sizeof(idx)))
			{
				break;
			}

			pBuff[i]=alphabet[idx % alphabet.size()];
		}

		RtlSecureZeroMemory(&idx, sizeof(idx));
	}

	return FALSE;
}
