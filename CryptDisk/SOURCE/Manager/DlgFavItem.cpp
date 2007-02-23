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

// DlgFavItem.cpp : implementation file
//

#include "stdafx.h"
#include "Common.h"
#include "Manager.h"
#include "DlgFavItem.h"


// CDlgFavItem dialog

IMPLEMENT_DYNAMIC(CDlgFavItem, CDialog)

CDlgFavItem::CDlgFavItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFavItem::IDD, pParent)
	, m_strPath(_T(""))
	, m_bReadOnly(FALSE)
	, m_bRemovable(FALSE)
	, m_bMountMgr(FALSE)
	, m_bPreserveTime(FALSE)
	, m_bEditMode(FALSE)
{

}

CDlgFavItem::~CDlgFavItem()
{
}

void CDlgFavItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPath);
	DDX_Check(pDX, IDC_CHECK1, m_bReadOnly);
	DDX_Check(pDX, IDC_CHECK2, m_bRemovable);
	DDX_Check(pDX, IDC_CHECK3, m_bMountMgr);
	DDX_Check(pDX, IDC_CHECK4, m_bPreserveTime);
}


BEGIN_MESSAGE_MAP(CDlgFavItem, CDialog)
END_MESSAGE_MAP()


// CDlgFavItem message handlers
void CDlgFavItem::OnOK()
{
	UpdateData(TRUE);

	TCHAR	str[20];

	GetDlgItemText(IDC_EDIT_LETTER, str, 19);

	m_driveLetter=str[0];

	CDialog::OnOK();
}

BOOL CDlgFavItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR	str[2];

	str[0]=m_driveLetter;
	str[1]=0;
	SetDlgItemText(IDC_EDIT_LETTER, str);

	if(m_bEditMode)
	{
		SetDlgItemText(IDOK, _T("&Save"));
	}
	else
	{
		SetDlgItemText(IDOK, _T("&Mount"));
	}

	return TRUE;
}

void CDlgFavItem::LoadData(const CFavNode *pNode)
{
	m_strPath=pNode->GetFilePath()+4;
	
	m_bMountMgr=(pNode->GetMountOptions()&MOUNT_VIA_MOUNTMGR) != 0;
	m_bRemovable=(pNode->GetMountOptions()&MOUNT_AS_REMOVABLE) != 0;
	m_bPreserveTime=(pNode->GetMountOptions()&MOUNT_SAVE_TIME) != 0;
	m_bReadOnly=(pNode->GetMountOptions()&MOUNT_READ_ONLY) != 0;

	m_driveLetter=pNode->GetDriveLetter();
}
