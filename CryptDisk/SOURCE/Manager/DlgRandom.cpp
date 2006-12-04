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

// DlgRandom.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "DlgRandom.h"


// CDlgRandom dialog

IMPLEMENT_DYNAMIC(CDlgRandom, CDialog)

CDlgRandom::CDlgRandom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRandom::IDD, pParent)
{
	m_nMaxPos=100;
}

CDlgRandom::~CDlgRandom()
{
}

void CDlgRandom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_RANDOM, m_progress);
}


BEGIN_MESSAGE_MAP(CDlgRandom, CDialog)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CDlgRandom::SetMaxPos(int nPos)
{
	m_nMaxPos=nPos;
}

void CDlgRandom::SetCurPos(int nPos)
{
	m_progress.SetPos(nPos);
}

BOOL CDlgRandom::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_progress.SetRange(0, m_nMaxPos);

	SendMessage(WM_GETDLGCODE, 0, 0);
	return TRUE;
}
// CDlgRandom message handlers
void CDlgRandom::OnOK()
{
	CDialog::OnOK();
}

void CDlgRandom::OnCancel()
{
	CDialog::OnCancel();
}
UINT CDlgRandom::OnGetDlgCode()
{
	return CDialog::OnGetDlgCode()|DLGC_WANTCHARS;
}
