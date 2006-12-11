// DrivesCombo.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "DrivesCombo.h"


// CDrivesCombo

IMPLEMENT_DYNAMIC(CDrivesCombo, CComboBox)

CDrivesCombo::CDrivesCombo()
{

}

CDrivesCombo::~CDrivesCombo()
{
}


BEGIN_MESSAGE_MAP(CDrivesCombo, CComboBox)
END_MESSAGE_MAP()


void CDrivesCombo::FillList()
{
	DWORD	drives;
	TCHAR	str[2];

	drives=GetLogicalDrives();

	m_letters.RemoveAll();

	for(int i=0;i<='Z'-'A';i++)
	{
		if((drives&(1<<i)) == 0)
		{
			m_letters.Add(_T('A') +i);
		}
	}

	for(int i=0;i<m_letters.GetSize();i++)
	{
		str[0]=m_letters[i];
		str[1]=0;
		AddString(str);
	}

	SetCurSel(0);
}

TCHAR CDrivesCombo::GetCurDrive()
{
	if((GetCurSel() < 0) || (GetCurSel() > m_letters.GetSize()))
		return 0;
	return m_letters[GetCurSel()];
}

// CDrivesCombo message handlers


