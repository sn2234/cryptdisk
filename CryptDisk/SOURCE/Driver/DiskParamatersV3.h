///////////////////////////////////////////////////////////
//  DiskParamatersV3.h
//  Implementation of the Class DiskParamatersV3
//  Created on:      11-dec-2009 1:29:44
//  Original author: nobody
///////////////////////////////////////////////////////////

#if !defined(EA_CECB246A_968D_4542_A566_D25CA895D131__INCLUDED_)
#define EA_CECB246A_968D_4542_A566_D25CA895D131__INCLUDED_

#include <BaseCrypt\DNAES.h>
#include "IDiskParameters.h"

class DiskParamatersV3 : public IDiskParameters
{
public:
	DiskParamatersV3();
	virtual ~DiskParamatersV3(){}

	virtual const wchar_t* getTextVersion() const { return L"V3"; }
	virtual unsigned int getVersion() const { return 3; } 
	const void* getDiskKey() const { return m_diskKey; }
	unsigned int getDiskKeyLength() const { return sizeof(m_diskKey); }
	const void* getDiskTweakKey() const { return m_tweakKey; }
	unsigned int getDiskTweakKeyLength() const { return sizeof(m_tweakKey); }

private:
	unsigned char	m_diskKey[32];
	unsigned char	m_tweakKey[16];
};

#endif // !defined(EA_CECB246A_968D_4542_A566_D25CA895D131__INCLUDED_)
