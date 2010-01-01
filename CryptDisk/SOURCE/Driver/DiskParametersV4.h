///////////////////////////////////////////////////////////
//  DiskParametersV4.h
//  Implementation of the Class DiskParametersV4
//  Created on:      11-dec-2009 1:29:49
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_33657598_F79C_4886_B133_AFC5482FF432__INCLUDED_)
#define EA_33657598_F79C_4886_B133_AFC5482FF432__INCLUDED_

#include "IDiskParameters.h"

class DiskParametersV4 : public IDiskParameters
{
public:
	DiskParametersV4();
	virtual ~DiskParametersV4(){}
	DiskParametersV4(const DiskParametersV4& theDiskParametersV4);

	virtual const wchar_t* getTextVersion() const { return L"V4"; }
	virtual unsigned int getVersion() const { return 4; }
	const void* getDiskKey() const { return m_diskKey; }
	unsigned int getDiskKeyLength() const { return sizeof(m_diskKey); }
	const void* getDiskTweakKey() const { return m_tweakKey; }
	unsigned int getDiskTweakKeyLength() const { return sizeof(m_tweakKey); }
	const void* getDiskTweakNumber() const { return m_tweakNumber; }
	unsigned int getDiskTweakNumberLength() const { return sizeof(m_tweakNumber); }
	unsigned int getDiskSectorSize() const { return m_diskSectorSize; }
private:
	unsigned char	m_diskKey[32];
	unsigned char	m_tweakKey[32];
	unsigned char	m_tweakNumber[16];
	unsigned int	m_diskSectorSize;
};

#endif // !defined(EA_33657598_F79C_4886_B133_AFC5482FF432__INCLUDED_)
