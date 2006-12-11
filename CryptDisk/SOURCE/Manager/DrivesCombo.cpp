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


