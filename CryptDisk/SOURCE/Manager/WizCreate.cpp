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

// WizCreate.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "DlgFormat.h"
#include "WizCreate.h"


// CWizCreate

IMPLEMENT_DYNAMIC(CWizCreate, CPropertySheet)

CWizCreate::CWizCreate(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	InternalInit();
	AddPages();
}

CWizCreate::CWizCreate(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InternalInit();
	AddPages();
}

CWizCreate::~CWizCreate()
{
}

void CWizCreate::AddPages()
{
	m_psh.dwFlags &= ~PSH_HASHELP;

	AddPage(&m_page1);
	AddPage(&m_page2);

	SetWizardMode();
}

void CWizCreate::InternalInit()
{
}

BEGIN_MESSAGE_MAP(CWizCreate, CPropertySheet)
END_MESSAGE_MAP()


// CWizCreate message handlers

BOOL CWizCreate::OnFinish()
{
	BOOL	bRez=FALSE;

	// Parameters validation
	if(m_page1.m_strPath.GetLength() == 0)
		return FALSE;

	if(! (m_page2.m_pPassword && m_page2.m_passwordLength))
		return FALSE;

	if((m_page2.m_cipher != DISK_CIPHER_AES) &&
		(m_page2.m_cipher != DISK_CIPHER_TWOFISH))
		return FALSE;

	DiskImageNONE	image;
	DiskImageNONE::IMAGE_CREATE_INFO	info;

	memset(&info, 0, sizeof(info));

	info.diskSize.QuadPart=m_page1.GetDiskSize();

	if(image.Create(m_page1.m_strPath.GetBuffer(),
		&info, &g_randomSource, (UCHAR*)m_page2.m_pPassword,
		m_page2.m_passwordLength, m_page2.m_cipher))
	{
		// Format disk if needed
		
		CDlgFormat	dlgFormat;
		INT_PTR		dlgResult;

		dlgFormat.m_pImage=&image;
		dlgFormat.m_bQuickFormat=m_page1.m_bQuick;

		dlgResult=dlgFormat.DoModal();
		image.Close();

		if(!dlgResult)	// An error occured
		{
			DeleteFile(m_page1.m_strPath.GetBuffer());
			bRez=FALSE;
		}
		else
		{
			bRez=TRUE;
		}
	}
	else
	{
		// Error
		MessageBox(_T("Can't create disk"), _T("Error"), MB_OK|MB_ICONERROR);
	}

	return bRez;
}
