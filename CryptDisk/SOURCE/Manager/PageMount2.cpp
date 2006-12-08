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

// PageMount2.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "PageMount2.h"

#include "WizMount.h"

// CPageMount2 dialog

IMPLEMENT_DYNAMIC(CPageMount2, CPropertyPage)

CPageMount2::CPageMount2()
	: CPropertyPage(CPageMount2::IDD)
	, m_bReadOnly(FALSE)
	, m_bRemovable(FALSE)
	, m_bUseMountManager(TRUE)
	, m_bTime(TRUE)
	, m_bAddToFav(FALSE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CPageMount2::~CPageMount2()
{
}

void CPageMount2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bReadOnly);
	DDX_Check(pDX, IDC_CHECK2, m_bRemovable);
	DDX_Check(pDX, IDC_CHECK3, m_bUseMountManager);
	DDX_Check(pDX, IDC_CHECK4, m_bTime);
	DDX_Check(pDX, IDC_CHECK5, m_bAddToFav);
	DDX_Control(pDX, IDC_COMBO_DRIVE, m_driveCombo);
}


BEGIN_MESSAGE_MAP(CPageMount2, CPropertyPage)
END_MESSAGE_MAP()


// CPageMount2 message handlers
BOOL CPageMount2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_driveCombo.FillList();

	return TRUE;
}

BOOL CPageMount2::OnSetActive()
{
	CWizMount* pWizard= static_cast<CWizMount*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}

BOOL CPageMount2::OnWizardFinish()
{
	CWizMount* pWizard= static_cast<CWizMount*>(GetParent());

	UpdateData(TRUE);

	m_driveLetter=m_driveCombo.GetCurDrive();

	if((m_driveLetter < _T('A')) || m_driveLetter > _T('Z'))
	{
		MessageBox(_T("Bad drive letter"),
			_T("Error"), MB_ICONERROR);
		return FALSE;
	}

	if(pWizard->OnFinish())
	{
		return CPropertyPage::OnWizardFinish();
	}
	else
	{
		return FALSE;
	}
}
