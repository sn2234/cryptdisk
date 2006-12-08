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

// PageCreate1.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "PageCreate1.h"

#include "WizCreate.h"

// CPageCreate1 dialog

IMPLEMENT_DYNAMIC(CPageCreate1, CPropertyPage)

CPageCreate1::CPageCreate1()
	: CPropertyPage(CPageCreate1::IDD)
	, m_dwSize(0)
	, m_bQuick(FALSE)
	, m_strPath(_T(""))
	, m_units(UNIT_MEGABYTE)
	, m_bDontAddToRecent(TRUE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CPageCreate1::~CPageCreate1()
{
}

void CPageCreate1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_dwSize);
	DDX_Control(pDX, IDC_COMBO_SIZE, m_comboSize);
	DDX_Check(pDX, IDC_CHECK_QUICK, m_bQuick);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Check(pDX, IDC_CHECK1, m_bDontAddToRecent);
}


BEGIN_MESSAGE_MAP(CPageCreate1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CPageCreate1::OnBnBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, &CPageCreate1::OnCbnSelchangeComboSize)
END_MESSAGE_MAP()


// CPageCreate1 message handlers

BOOL CPageCreate1::OnSetActive( )
{
	CWizCreate* pWizard= static_cast<CWizCreate*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

BOOL CPageCreate1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_comboSize.InsertString(0, _T("MB"));
	m_comboSize.InsertString(1, _T("GB"));
	m_comboSize.SetCurSel(0);
	m_units=UNIT_MEGABYTE;

	return TRUE;
}

LRESULT CPageCreate1::OnWizardNext()
{
	UpdateData(TRUE);

	return CPropertyPage::OnWizardNext();
}


void CPageCreate1::OnBnBrowse()
{
	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	UpdateData(TRUE);

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("Images\0*.img\0All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Create image");
	if(m_bDontAddToRecent)
		ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetSaveFileName(&ofn))
	{
		// Check for extension
		CheckFileExtension(filePath, _T(".img"), _countof(filePath)-1);
		SetDlgItemText(IDC_EDIT_PATH,filePath);
	}
}

void CPageCreate1::CheckFileExtension(LPTSTR szPath, LPCTSTR szExt, int nMaxChars)
{
	int		i;
	size_t	pathLen,extLen;

	pathLen=_tcslen(szPath);
	extLen=_tcslen(szExt);

	for(i=0;szPath[i]&& (i<nMaxChars);i++)
	{
		if(szPath[i] == _T('.'))
		{
			break;
		}
	}
	if(szPath[i]!=_T('.'))
	{
		_tcscat_s(szPath, nMaxChars, szExt);
	}
}

void CPageCreate1::OnCbnSelchangeComboSize()
{
	UpdateData(TRUE);

	switch(m_comboSize.GetCurSel())
	{
	case 0:
		m_units=UNIT_MEGABYTE;
		break;
	case 1:
		m_units=UNIT_GIGABYTE;
		break;
	}
}
