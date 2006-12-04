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

// PageMount1.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "KeyFilesDlg.h"
#include "DlgChangePassword.h"
#include "PageMount1.h"

#include "WizMount.h"

// CPageMount1 dialog

IMPLEMENT_DYNAMIC(CPageMount1, CPropertyPage)

CPageMount1::CPageMount1()
	: CPropertyPage(CPageMount1::IDD)
	, m_bDontAddToRecent(TRUE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CPageMount1::~CPageMount1()
{
}

void CPageMount1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bDontAddToRecent);
}


BEGIN_MESSAGE_MAP(CPageMount1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CPageMount1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPageMount1::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPageMount1::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPageMount1 message handlers
BOOL CPageMount1::OnSetActive()
{
	CWizMount* pWizard= static_cast<CWizMount*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

BOOL CPageMount1::OnInitDialog()
{
	return CPropertyPage::OnInitDialog();
}

LRESULT CPageMount1::OnWizardNext()
{
	CWizMount* pWizard= static_cast<CWizMount*>(GetParent());

	DiskImageNONE		image;

	// Get file path
	int				n;

	n=GetDlgItemText(IDC_EDIT_PATH, m_filePath);
	if(n == 0)
	{
		return -1;
	}

	// Check if file has compressed or encrypted attribute
	DWORD			dwFileAttrib;
	dwFileAttrib=GetFileAttributes(m_filePath.GetBuffer());
	if(dwFileAttrib == INVALID_FILE_ATTRIBUTES)
	{
		// Error - can't get file attributes
		MessageBox(_T("Unable to open file"), _T("Error"), MB_ICONERROR);
		return -1;
	}
	else
	{
		if(dwFileAttrib & (FILE_ATTRIBUTE_COMPRESSED|FILE_ATTRIBUTE_ENCRYPTED))
		{
			// Warning - file is encrypted or compressed by FS
			if(MessageBox(_T("Image file is compressed or encrypted. Mounting such image may cause critical system error. Do you want to continue?"),
				_T("Warning"),
				MB_ICONWARNING|MB_YESNO) == IDNO)
			{
				return -1;
			}
		}
	}

	UCHAR			*pPassword=NULL;
	int				passLength=0;

	if(!CollectPassword(&pPassword, (ULONG*)&passLength))
	{
		return -1;
	}

	// Open image
	BOOL	bResult;
	bResult=image.Open(m_filePath.GetBuffer(), pPassword, passLength);

	RtlSecureZeroMemory(pPassword, passLength);
	delete[] pPassword;

	if(!bResult)
	{
		MessageBox(_T("Unable to open image"), _T("Error"), MB_ICONERROR);
		return -1;
	}
	else
	{
		memcpy(pWizard->m_userKey, image.GetHeader()->GetUserKey(),
			sizeof(pWizard->m_userKey));
		pWizard->m_algoId=image.GetHeader()->GetAlgoId();
		image.Close();
		return CPropertyPage::OnWizardNext();
	}
}

// Browse
void CPageMount1::OnBnClickedButton1()
{
	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("Images\0*.img\0All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Open image");
	if(m_bDontAddToRecent)
		ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetOpenFileName(&ofn))
	{
		SetDlgItemText(IDC_EDIT_PATH,filePath);
	}
}

// Change password
void CPageMount1::OnBnClickedButton2()
{
	CDlgChangePassword	dlg;
	DiskImageNONE		image;
	CString				filePath;

	// Try to open image
	// Get file path
	int				n;

	n=GetDlgItemText(IDC_EDIT_PATH, filePath);
	if(n == 0)
	{
		return ;
	}

	UCHAR			*pPassword=NULL;
	int				passLength=0;

	if(!CollectPassword(&pPassword, (ULONG*)&passLength))
	{
		return ;
	}

	if(!image.Open(filePath.GetBuffer(), pPassword, passLength))
	{
		MessageBox(_T("Unable to open image"), _T("Error"), MB_ICONERROR);
		goto _proc_exit;
	}

	// Get new password
	if((dlg.DoModal()==IDOK) && dlg.m_pPassword)
	{
		// Change password
		if(image.ChangePassword(dlg.m_pPassword, dlg.m_passwordLength))
		{
			m_keyFiles.RemoveAll();
			m_keyFiles.Append(dlg.m_keyFiles);

			SetDlgItemText(IDC_EDIT_PASSWORD, _T(""));

			MessageBox(_T("The password has been successfully changed"), _T("Success"), MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("Error when changing password"), _T("Error"), MB_ICONERROR);
		}
	}

	image.Close();


_proc_exit:
	RtlSecureZeroMemory(pPassword, passLength);
	delete[] pPassword;
}

// Manage keyfiles
void CPageMount1::OnBnClickedButton3()
{
	CKeyFilesDlg	dlg;

	dlg.m_pKeyFiles=&m_keyFiles;

	dlg.DoModal();
}

BOOL CPageMount1::CollectPassword(UCHAR **ppPassword, ULONG *pPaswordLength)
{
	BOOL				bKeyFilesPresent=FALSE;
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
				return FALSE;
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
		keyFilesCollector.Clear();

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

	if(pPassword)
	{
		*ppPassword=pPassword;
		*pPaswordLength=passLength;
		return TRUE;
	}

	return FALSE;
}

