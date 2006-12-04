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

#pragma once
#include "colorprogress.h"
#include "afxwin.h"


// CPageCreate2 dialog

class CPageCreate2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageCreate2)

public:
	CPageCreate2();
	virtual ~CPageCreate2();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_CREATE2 };

	CStringArray		m_keyFiles;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();
	BOOL OnSetActive( );
	BOOL OnWizardFinish();

	void CheckPasswordQuality();

public:
	afx_msg void OnBnClickedButtonKeyfiles();
	afx_msg void OnEnChangePassword();
	afx_msg void OnBnClickedGenPass();
protected:
	CColorProgress m_barQuality;
	CString m_staticBits;
	CComboBox m_comboCipher;
public:
	char	*m_pPassword;
	int		m_passwordLength;

	DISK_CIPHER		m_cipher;
};
