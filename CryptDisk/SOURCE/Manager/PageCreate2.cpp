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

// PageCreate2.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "ColorProgress.h"
#include "PageCreate2.h"
#include "KeyFilesDlg.h"
#include "DlgGenPass.h"
#include "WizCreate.h"


// CPageCreate2 dialog

IMPLEMENT_DYNAMIC(CPageCreate2, CPropertyPage)

CPageCreate2::CPageCreate2()
	: CPropertyPage(CPageCreate2::IDD)
	, m_staticBits(_T(""))
	, m_pPassword(NULL)
	, m_passwordLength(0)
	, m_cipher(DISK_CIPHER_AES)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CPageCreate2::~CPageCreate2()
{
	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		g_heap.Free(m_pPassword);
		m_pPassword=NULL;
	}
}

void CPageCreate2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_QUALITY, m_barQuality);
	DDX_Text(pDX, IDC_STATIC_ENTROPY, m_staticBits);
	DDX_Control(pDX, IDC_COMBO_CIPHER, m_comboCipher);
}


BEGIN_MESSAGE_MAP(CPageCreate2, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_KEYFILES, &CPageCreate2::OnBnClickedButtonKeyfiles)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CPageCreate2::OnEnChangePassword)
	ON_BN_CLICKED(IDC_BUTTON_GEN_PASS, &CPageCreate2::OnBnClickedGenPass)
END_MESSAGE_MAP()


// CPageCreate2 message handlers

BOOL CPageCreate2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_barQuality.SetRange(0, 128);

	m_barQuality.SetPos(0);

	m_staticBits.Format(_T("%d bits"), 0);

	m_comboCipher.Clear();
	m_comboCipher.InsertString(0, _T("AES-256"));
	m_comboCipher.InsertString(1, _T("Twofish-256"));
	m_comboCipher.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;
}

BOOL CPageCreate2::OnSetActive( )
{
	CWizCreate* pWizard= static_cast<CWizCreate*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}

void CPageCreate2::OnBnClickedButtonKeyfiles()
{
	CKeyFilesDlg	dlg;

	dlg.m_pKeyFiles=&m_keyFiles;

	dlg.DoModal();
}

void CPageCreate2::OnEnChangePassword()
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

void CPageCreate2::OnBnClickedGenPass()
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

BOOL CPageCreate2::OnWizardFinish()
{
	char	*pwBuff1, *pwBuff2;
	char	pwLen, confLen;

	UpdateData(TRUE);

	CWizCreate* pWizard= static_cast<CWizCreate*>(GetParent());

	if(m_pPassword)
	{
		g_heap.Free(m_pPassword);
		m_pPassword=NULL;
	}

	// Get password
	pwLen=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
	if(pwLen)
	{
		if(pwLen < 12)
		{
			if(MessageBox(_T("Your password is very short. Recommended length is not less then 12 symbols. Do you want to continue with this password?"),
				_T("Warning"), MB_YESNO|MB_ICONWARNING) != IDYES)
			{
				return FALSE;
			}
		}
		// Confirm password
		confLen=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_CONFIRM)->GetSafeHwnd());
		if(pwLen != confLen)
		{
			MessageBox(_T("Confirmation failed"),
				_T("Error"), MB_OK|MB_ICONERROR);

			return FALSE;
		}
		pwBuff1=(char*)g_heap.Alloc(pwLen+1);
		pwBuff2=(char*)g_heap.Alloc(pwLen+1);
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, pwBuff1, pwLen+1);
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_CONFIRM, pwBuff2, pwLen+1);

		if(memcmp(pwBuff1, pwBuff2, pwLen))
		{
			MessageBox(_T("Confirmation failed"),
				_T("Error"), MB_OK|MB_ICONERROR);

			RtlSecureZeroMemory(pwBuff1, pwLen);
			RtlSecureZeroMemory(pwBuff2, pwLen);
			g_heap.Free(pwBuff1);
			g_heap.Free(pwBuff2);

			return FALSE;
		}

		// Get key files
		if(m_keyFiles.GetCount())
		{
			CKeyFileCollector	keyFilesCollector;

			for(int i=0;i<m_keyFiles.GetCount();i++)
			{
				if(!keyFilesCollector.AddFile(m_keyFiles[i]))
				{
					MessageBox(m_keyFiles[i], _T("Unable to open key file"), MB_ICONERROR);

					RtlSecureZeroMemory(pwBuff1, pwLen);
					RtlSecureZeroMemory(pwBuff2, pwLen);
					g_heap.Free(pwBuff1);
					g_heap.Free(pwBuff2);

					return FALSE;
				}
			}

			m_passwordLength=pwLen+SHA256_DIDGEST_SIZE;
			m_pPassword=(char*)g_heap.Alloc(m_passwordLength);

			keyFilesCollector.GetKey(m_pPassword);
			keyFilesCollector.Clear();
			memcpy(m_pPassword+SHA256_DIDGEST_SIZE, pwBuff1, pwLen);
		}
		else
		{
			m_passwordLength=pwLen;
			m_pPassword=(char*)g_heap.Alloc(m_passwordLength);
			memcpy(m_pPassword, pwBuff1, pwLen);
		}

		RtlSecureZeroMemory(pwBuff1, pwLen);
		RtlSecureZeroMemory(pwBuff2, pwLen);
		g_heap.Free(pwBuff1);
		g_heap.Free(pwBuff2);
	}
	else
	{
		if(m_keyFiles.GetCount() ==0)
		{
			MessageBox(_T("Blank password is not allowed without key files"),
				_T("Error"), MB_OK|MB_ICONERROR);
			return FALSE;
		}

		// Create password from key files only
		CKeyFileCollector	keyFilesCollector;

		for(int i=0;i<m_keyFiles.GetCount();i++)
		{
			if(!keyFilesCollector.AddFile(m_keyFiles[i]))
			{
				MessageBox(m_keyFiles[i], _T("Unable to open file"), MB_ICONERROR);

				return FALSE;
			}
		}

		m_passwordLength=pwLen+SHA256_DIDGEST_SIZE;
		m_pPassword=(char*)g_heap.Alloc(m_passwordLength);
		keyFilesCollector.GetKey(m_pPassword);
		keyFilesCollector.Clear();
	}

	m_cipher=DISK_CIPHER_AES;
	switch(m_comboCipher.GetCurSel())
	{
	case 0:
		m_cipher=DISK_CIPHER_AES;
		break;
	case 1:
		m_cipher=DISK_CIPHER_TWOFISH;
		break;
	}

	if(pWizard->OnFinish())
	{
		return CPropertyPage::OnWizardFinish();
	}
	else
	{
		return FALSE;
	}

	if(m_pPassword)
	{
		RtlSecureZeroMemory(m_pPassword, m_passwordLength);
		g_heap.Free(m_pPassword);
		m_pPassword=NULL;
	}
}
