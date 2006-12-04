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

// KeyFilesDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "KeyFilesDlg.h"


// CKeyFilesDlg dialog

IMPLEMENT_DYNAMIC(CKeyFilesDlg, CDialog)

CKeyFilesDlg::CKeyFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyFilesDlg::IDD, pParent)
{
	m_pKeyFiles=NULL;
}

CKeyFilesDlg::~CKeyFilesDlg()
{
}

void CKeyFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_listFiles);
}


BEGIN_MESSAGE_MAP(CKeyFilesDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &CKeyFilesDlg::OnAddFile)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CKeyFilesDlg::OnRemoveFile)
	ON_BN_CLICKED(IDC_BUTTON1, &CKeyFilesDlg::OnRemoveAll)
END_MESSAGE_MAP()

BOOL CKeyFilesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LVCOLUMN	col;

	ASSERT(m_pKeyFiles);

	col.mask=LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	col.fmt=LVCFMT_LEFT;
	col.pszText=_T("File path");
	col.iSubItem=0;
	col.cx=200;
	m_listFiles.InsertColumn(0, &col);

	for(int i=0;i< m_pKeyFiles->GetCount();i++)
	{
		m_listFiles.InsertItem(m_listFiles.GetItemCount(), m_pKeyFiles->GetAt(i));
	}

	return TRUE;
}

// CKeyFilesDlg message handlers

void CKeyFilesDlg::OnAddFile()
{
	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Open keyfile");
	ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetOpenFileName(&ofn))
	{
		m_listFiles.InsertItem(m_listFiles.GetItemCount(), filePath);
	}
}

void CKeyFilesDlg::OnRemoveFile()
{
	int	n=m_listFiles.GetSelectionMark();

	if(n != -1)
	{
		m_listFiles.DeleteItem(n);
	}
}

void CKeyFilesDlg::OnRemoveAll()
{
	m_listFiles.DeleteAllItems();
}

void CKeyFilesDlg::OnOK()
{
	CString		strFilePath;
	int			n;
	BOOL		bResult=TRUE;

	m_pKeyFiles->RemoveAll();

	n=m_listFiles.GetItemCount();

	for(int i=0;i<n;i++)
	{
		strFilePath=m_listFiles.GetItemText(i, 0);
		m_pKeyFiles->Add(strFilePath);
	}

	if(bResult)
	{
		CDialog::OnOK();
	}
}
