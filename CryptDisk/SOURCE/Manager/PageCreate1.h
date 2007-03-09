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
#include "afxwin.h"


// CPageCreate1 dialog

class CPageCreate1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageCreate1)

public:
	CPageCreate1();
	virtual ~CPageCreate1();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_CREATE1 };

	void CheckFileExtension(LPTSTR szPath, LPCTSTR szExt, int nMaxChars);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	BOOL OnSetActive( );
	BOOL OnInitDialog();
	LRESULT OnWizardNext();

	BOOL CheckNextConditions();

	afx_msg void OnBnBrowse();
	afx_msg void OnCbnSelchangeComboSize();
	afx_msg void OnEnChangeEditPath();
	afx_msg void OnEnChangeEditSize();
protected:
	enum	DISK_SIZE_UNITS
	{
		UNIT_MEGABYTE=0,
		UNIT_GIGABYTE=1
	};

	CComboBox m_comboSize;
	DISK_SIZE_UNITS		m_units;
	DWORD m_dwSize;
	BOOL m_bDontAddToRecent;
public:
	BOOL m_bQuick;
	CString m_strPath;
	UINT64 GetDiskSize()
	{
		switch(m_units)
		{
		case UNIT_MEGABYTE:
			return (UINT64)m_dwSize*1024*1024;
			break;
		case UNIT_GIGABYTE:
			return (UINT64)m_dwSize*1024*1024*1024;
			break;
		}

		return 0;
	}
};
