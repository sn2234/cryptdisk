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

// DlgGenPass.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "DlgGenPass.h"


// CDlgGenPass dialog

IMPLEMENT_DYNAMIC(CDlgGenPass, CDialog)

CDlgGenPass::CDlgGenPass(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGenPass::IDD, pParent)
	, m_bUseUpper(TRUE)
	, m_bUseLower(TRUE)
	, m_bUseNumbers(TRUE)
	, m_bUseSymbols(FALSE)
	, m_bUseCustom(FALSE)
	, m_bEasyToRead(TRUE)
	, m_nPasswordLength(20)
	, m_pPasswordBuff(NULL)
{

}

CDlgGenPass::~CDlgGenPass()
{
	if(m_pPasswordBuff)
	{
		delete[] m_pPasswordBuff;
		m_pPasswordBuff=NULL;
	}
}

void CDlgGenPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bUseUpper);
	DDX_Check(pDX, IDC_CHECK2, m_bUseLower);
	DDX_Check(pDX, IDC_CHECK3, m_bEasyToRead);
	DDX_Check(pDX, IDC_CHECK4, m_bUseNumbers);
	DDX_Check(pDX, IDC_CHECK5, m_bUseSymbols);
	DDX_Check(pDX, IDC_CHECK6, m_bUseCustom);
	DDX_Text(pDX, IDC_EDIT3, m_nPasswordLength);
}

BEGIN_MESSAGE_MAP(CDlgGenPass, CDialog)
	ON_BN_CLICKED(IDC_CHECK6, &CDlgGenPass::OnBnClickedUseCustom)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgGenPass::OnBnGenerate)
END_MESSAGE_MAP()


// CDlgGenPass message handlers

void CDlgGenPass::OnBnClickedUseCustom()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_EDIT_CUSTOM)->EnableWindow(m_bUseCustom);
}

void CDlgGenPass::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CDlgGenPass::OnCancel()
{
	if(m_pPasswordBuff)
	{
		RtlSecureZeroMemory(m_pPasswordBuff, m_nPasswordLength);
		delete[] m_pPasswordBuff;
		m_pPasswordBuff=NULL;
	}

	CDialog::OnCancel();
}

void CDlgGenPass::OnBnGenerate()
{
	UpdateData(TRUE);

	if(m_pPasswordBuff)
	{
		RtlSecureZeroMemory(m_pPasswordBuff, m_passwordLength);
		delete[] m_pPasswordBuff;
		m_pPasswordBuff=NULL;
	}

	CPWTools::PASSWORD_PARAMS	params;
	char *pCustomSet=NULL;
	int customSetLen=0;

	memset(&params, 0, sizeof(params));

	// Get options
	if(m_bUseUpper)
		params.dwFlags|=CPWTools::USE_UPPER_ALPHA;
	if(m_bUseLower)
		params.dwFlags|=CPWTools::USE_LOWER_ALPHA;
	if(m_bUseNumbers)
		params.dwFlags|=CPWTools::USE_NUMERIC;
	if(m_bUseSymbols)
		params.dwFlags|=CPWTools::USE_SYMBOLS;
	if(m_bEasyToRead)
		params.dwFlags|=CPWTools::USE_EASY_TO_READ;
	if(m_bUseCustom)
	{
		customSetLen=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_CUSTOM)->GetSafeHwnd());
		if(customSetLen)
		{
			pCustomSet=new char[customSetLen+1];

			GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_CUSTOM, pCustomSet, customSetLen+1);

			params.dwFlags|=CPWTools::USE_CUSTOM_SET;
			params.pCustomSet=pCustomSet;
			params.customSetLength=customSetLen;
		}
	}

	// Alloc buffer
	m_pPasswordBuff=new char[m_nPasswordLength+1];
	m_passwordLength=m_nPasswordLength;

	memset(m_pPasswordBuff, 0, m_nPasswordLength+1);
	// Generate password
	if(!CPWTools::GenPassword(m_pPasswordBuff, m_passwordLength,
		&params, &g_randomSource))
	{
		RtlSecureZeroMemory(m_pPasswordBuff, m_passwordLength);
		delete[] m_pPasswordBuff;
		m_pPasswordBuff=NULL;
		m_passwordLength=0;

		MessageBox(_T("Can't generate password"), _T("Error"),
			MB_OK|MB_ICONERROR);
	}
	else
	{
		SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, m_pPasswordBuff);
	}

	if(pCustomSet)
	{
		delete[] pCustomSet;
	}
}
