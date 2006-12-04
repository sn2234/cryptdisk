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

// DlgPassword.cpp : implementation file
//

#include "stdafx.h"
#include "Common.h"
#include "Manager.h"
#include "DlgPassword.h"
#include "KeyFilesDlg.h"


// CDlgPassword dialog

IMPLEMENT_DYNAMIC(CDlgPassword, CDialog)

CDlgPassword::CDlgPassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPassword::IDD, pParent)
	, m_pPassword(NULL)
	, m_passwordLength(0)
{

}

CDlgPassword::~CDlgPassword()
{
	if(m_pPassword)
	{
		delete[] m_pPassword;
		m_pPassword=NULL;
	}
}

void CDlgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPassword, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_KEYFILES, &CDlgPassword::OnBnClickedButtonKeyfiles)
END_MESSAGE_MAP()


// CDlgPassword message handlers

void CDlgPassword::OnBnClickedButtonKeyfiles()
{
	CKeyFilesDlg	dlg;

	dlg.m_pKeyFiles=&m_keyFiles;

	dlg.DoModal();
}

void CDlgPassword::OnOK()
{
	BOOL	bKeyFilesPresent=FALSE;
	CKeyFileCollector	keyFilesCollector;

	// Open key files and add to collector
	if(!m_keyFiles.IsEmpty())
	{
		keyFilesCollector.Init();

		for(int i=0;i<m_keyFiles.GetCount();i++)
		{
			if(!keyFilesCollector.AddFile(m_keyFiles[i]))
			{
				MessageBox(m_keyFiles[i], _T("Unable to open file"), MB_ICONERROR);
				return;
			}
		}

		bKeyFilesPresent=TRUE;
	}

	UCHAR			*pPassword=NULL;
	int				passLength=0;

	// Check if key files present
	if(bKeyFilesPresent)
	{
		passLength=SHA256_DIDGEST_SIZE;
		passLength+=GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextLength();

		pPassword=new UCHAR[passLength+1];

		keyFilesCollector.GetKey(pPassword);

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, (char*)pPassword+SHA256_DIDGEST_SIZE,
			passLength-SHA256_DIDGEST_SIZE+1);
	}
	else
	{
		passLength=GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextLength();

		pPassword=new UCHAR[passLength+1];

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, (char*)pPassword,
			passLength+1);
	}

	keyFilesCollector.Clear();

	if(pPassword)
	{
		m_pPassword=pPassword;
		m_passwordLength=passLength;
		CDialog::OnOK();
	}
}

BOOL CDlgPassword::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

	return TRUE;
}
