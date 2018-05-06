///////////////////////////////////////////////////////////
//  DiskParametersV4.h
//  Implementation of the Class DiskParametersV4
//  Created on:      11-dec-2009 1:29:49
//  Original author: nobody
///////////////////////////////////////////////////////////

#pragma once

#include "IDiskParameters.h"

class DiskParametersV4 : public IDiskParameters
{
public:
	static const size_t DiskKeyLength = 32;
	static const size_t TweakKeyLength = 32;
	static const size_t TweakNumberLength = 16;

	DiskParametersV4(const unsigned char* pDiskKey, const unsigned char* pTweakKey, const unsigned char* pTweakNumber, unsigned int sectorSize)
	{
		memcpy(m_diskKey, pDiskKey, sizeof(m_diskKey));
		memcpy(m_tweakKey, pTweakKey, sizeof(m_tweakKey));
		memcpy(m_tweakNumber, pTweakNumber, sizeof(m_tweakNumber));
		m_diskSectorSize = sectorSize;
	}

	virtual ~DiskParametersV4(){}

	virtual const wchar_t* getTextVersion() const { return L"V4"; }
	virtual unsigned int getVersion() const { return 4; }
	const void* getDiskKey() const { return m_diskKey; }
	size_t getDiskKeyLength() const { return sizeof(m_diskKey); }
	const void* getDiskTweakKey() const { return m_tweakKey; }
	size_t getDiskTweakKeyLength() const { return sizeof(m_tweakKey); }
	const void* getDiskTweakNumber() const { return m_tweakNumber; }
	size_t getDiskTweakNumberLength() const { return sizeof(m_tweakNumber); }
	unsigned int getDiskSectorSize() const { return m_diskSectorSize; }
private:
	unsigned char	m_diskKey[DiskKeyLength];
	unsigned char	m_tweakKey[TweakKeyLength];
	unsigned char	m_tweakNumber[TweakNumberLength];
	unsigned int	m_diskSectorSize;
};
