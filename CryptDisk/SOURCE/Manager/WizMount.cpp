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

// WizMount.cpp : implementation file
//

#include "stdafx.h"

#include "Manager.h"
#include "WizMount.h"


// CWizMount

IMPLEMENT_DYNAMIC(CWizMount, CPropertySheet)

CWizMount::CWizMount(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	InternalInit();
	AddPages();
}

CWizMount::CWizMount(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InternalInit();
	AddPages();
}

CWizMount::~CWizMount()
{
	RtlSecureZeroMemory(m_userKey, sizeof(m_userKey));
}

void CWizMount::AddPages()
{
	m_psh.dwFlags &= ~PSH_HASHELP;

	AddPage(&m_page1);
	AddPage(&m_page2);

	SetWizardMode();
}

void CWizMount::InternalInit()
{
}

BEGIN_MESSAGE_MAP(CWizMount, CPropertySheet)
END_MESSAGE_MAP()


// CWizMount message handlers
BOOL CWizMount::OnFinish()
{
	DISK_ADD_INFO	info;

	memset(&info, 0, sizeof(info));

	wcscpy_s(info.FilePath, L"\\??\\");
	wcscat_s(info.FilePath, m_page1.m_filePath.GetBuffer());

	memcpy(info.UserKey, m_userKey, sizeof(info.UserKey));
	info.wAlgoId=m_algoId;

	info.DriveLetter=m_page2.m_driveLetter;
	info.MountOptions=0;
	if(m_page2.m_bReadOnly)
		info.MountOptions|=MOUNT_READ_ONLY;
	if(m_page2.m_bRemovable)
		info.MountOptions|=MOUNT_AS_REMOVABLE;
	if(m_page2.m_bUseMountManager)
		info.MountOptions|=MOUNT_VIA_MOUNTMGR;
	if(m_page2.m_bTime)
		info.MountOptions|=MOUNT_SAVE_TIME;

	if(!theApp.m_manager.InstallDisk(&info))
	{
		MessageBox(_T("Can't mount disk"), _T("Error"), MB_ICONERROR);
		return FALSE;
	}

	if(m_page2.m_bAddToFav)
	{
		// Add this image to favorites
		CFavNode	*pNewNode;

		pNewNode=new CFavNode;

		pNewNode->Init(info.FilePath, info.MountOptions, info.DriveLetter);

		g_favorites.AddNode(pNewNode);
	}

	return TRUE;
}
