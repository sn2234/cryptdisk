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

// DlgChangePassword.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"
#include "ColorProgress.h"
#include "Manager.h"
#include "KeyFilesDlg.h"
#include "DlgGenPass.h"
#include "DlgChangePassword.h"


// CDlgChangePassword dialog

IMPLEMENT_DYNAMIC(CDlgChangePassword, CDialog)

CDlgChangePassword::CDlgChangePassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangePassword::IDD, pParent)
	, m_staticBits(_T(""))
{
	m_pPassword=NULL;
	m_passwordLength=0;
}

CDlgChangePassword::~CDlgChangePassword()
{
	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		m_pPassword=NULL;
		m_passwordLength=0;
	}
}

void CDlgChangePassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_QUALITY, m_barQuality);
	DDX_Text(pDX, IDC_STATIC_ENTROPY, m_staticBits);
}


BEGIN_MESSAGE_MAP(CDlgChangePassword, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_KEYFILES, &CDlgChangePassword::OnBnClickedButtonKeyfiles)
	ON_BN_CLICKED(IDC_BUTTON_GEN_PASS, &CDlgChangePassword::OnBnClickedButtonGenPass)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CDlgChangePassword::OnEnChangeEditPassword)
END_MESSAGE_MAP()


BOOL CDlgChangePassword::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_barQuality.SetRange(0, 128);
	m_barQuality.SetPos(0);

	m_staticBits.Format(_T("%d bits"), 0);

	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		m_pPassword=NULL;
		m_passwordLength=0;
	}

	UpdateData(FALSE);
	return TRUE;
}

// CDlgChangePassword message handlers

void CDlgChangePassword::OnBnClickedButtonKeyfiles()
{
	CKeyFilesDlg	dlg;

	dlg.m_pKeyFiles=&m_keyFiles;

	dlg.DoModal();
}

void CDlgChangePassword::OnBnClickedButtonGenPass()
{
	CDlgGenPass	dlg;

	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_pPasswordBuff)
		{
			SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, dlg.m_pPasswordBuff);
//			SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_CONFIRM, dlg.m_pPasswordBuff);
		}
	}
}

void CDlgChangePassword::OnEnChangeEditPassword()
{
	char	*buff;
	int		len;
	int		bits=0;

	len=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());

	if(len)
	{
		buff=new char[len+1];
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, buff, len+1);

		bits=CPWTools::GetEntropy(buff, len);

		RtlSecureZeroMemory(buff, len+1);
		delete[] buff;
	}

	m_barQuality.SetPos(bits);
	m_staticBits.Format(_T("%d bits"), bits);

	UpdateData(FALSE);
}

void CDlgChangePassword::OnOK()
{
	UCHAR	*pConfirm;
	ULONG	confirmLen;
	
	UCHAR	*pPassword;
	ULONG	passwordLength;

	BOOL				bKeyFilesPresent=FALSE;
	CKeyFileCollector	keyFilesCollector;

	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		delete[] m_pPassword;
		m_pPassword=NULL;
		m_passwordLength=0;
	}

	passwordLength=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
	confirmLen=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_CONFIRM)->GetSafeHwnd());

	if(passwordLength != confirmLen)
	{
		MessageBox(_T("Confirmation failed"),
			_T("Error"), MB_OK|MB_ICONERROR);
		return;
	}

	if(passwordLength < 12)
	{
		if(MessageBox(_T("Your password is very short. Recommended length is not less then 12 symbols. Do you want to continue with this password?"),
			_T("Warning"), MB_YESNO|MB_ICONWARNING) != IDYES)
		{
			return;
		}
	}

	if(passwordLength)
	{
		pPassword=new UCHAR[passwordLength+1];
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, (LPSTR)pPassword, passwordLength+1);

		pConfirm=new UCHAR[confirmLen+1];
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_CONFIRM, (LPSTR)pConfirm, confirmLen+1);

		if(memcmp(pConfirm, pPassword, passwordLength))
		{
			RtlSecureZeroMemory(pPassword, passwordLength);
			delete[] pPassword;

			RtlSecureZeroMemory(pConfirm, confirmLen);
			delete[] pConfirm;

			MessageBox(_T("Confirmation failed"),
				_T("Error"), MB_OK|MB_ICONERROR);
			return;
		}

		RtlSecureZeroMemory(pConfirm, confirmLen);
		delete[] pConfirm;
	}
	else
	{
		if(m_keyFiles.GetCount() == 0)
		{
			MessageBox(_T("Blank password is not allowed without key files"),
				_T("Error"), MB_OK|MB_ICONERROR);
			return;
		}
	}
	
	// Open key files and add to collector
	if(!m_keyFiles.IsEmpty())
	{
		keyFilesCollector.Init();

		for(int i=0;i<m_keyFiles.GetCount();i++)
		{
			if(!keyFilesCollector.AddFile(m_keyFiles[i]))
			{
				MessageBox(m_keyFiles[i], _T("Unable to open file"), MB_ICONERROR);

				RtlSecureZeroMemory(pPassword, passwordLength);
				delete[] pPassword;

				return;
			}
		}

		bKeyFilesPresent=TRUE;
	}

	if(bKeyFilesPresent)
	{
		m_passwordLength=passwordLength+SHA256_DIDGEST_SIZE;
		m_pPassword=new UCHAR[m_passwordLength+1];

		keyFilesCollector.GetKey(m_pPassword);
		keyFilesCollector.Clear();

		memcpy(m_pPassword+SHA256_DIDGEST_SIZE, pPassword, passwordLength);

		RtlSecureZeroMemory(pPassword, passwordLength);
		delete[] pPassword;
	}
	else
	{
		m_passwordLength=passwordLength;
		m_pPassword=pPassword;
	}

	CDialog::OnOK();
}

void CDlgChangePassword::OnCancel()
{
	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		m_pPassword=NULL;
		m_passwordLength=0;
	}

	CDialog::OnCancel();
}
