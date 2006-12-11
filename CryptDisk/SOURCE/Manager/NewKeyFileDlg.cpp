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

// NewKeyFileDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Common.h"

#include "Manager.h"
#include "NewKeyFileDlg.h"


// CNewKeyFileDlg dialog

IMPLEMENT_DYNAMIC(CNewKeyFileDlg, CDialog)

CNewKeyFileDlg::CNewKeyFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewKeyFileDlg::IDD, pParent)
	, m_strPath(_T(""))
	, m_size(0)
{

}

CNewKeyFileDlg::~CNewKeyFileDlg()
{
}

void CNewKeyFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT2, m_size);
}


BEGIN_MESSAGE_MAP(CNewKeyFileDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CNewKeyFileDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CNewKeyFileDlg message handlers

void CNewKeyFileDlg::OnBnClickedButton1()
{
	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	UpdateData(TRUE);

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Create keyfile");
	ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetSaveFileName(&ofn))
	{
		m_strPath=filePath;
		UpdateData(FALSE);
	}
}

void CNewKeyFileDlg::OnOK()
{
	HANDLE	hFile, hMap;
	BYTE	*pData;

	BOOL	bResult=FALSE;

	UpdateData(TRUE);

	hFile=CreateFile(m_strPath, GENERIC_WRITE|GENERIC_READ, 0, NULL,
		CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		if(SetFilePointer(hFile, m_size, NULL, FILE_BEGIN))
		{
			SetEndOfFile(hFile);
			hMap=CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, m_size, NULL);
			if(hMap)
			{
				pData=(BYTE*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, m_size);
				if(pData)
				{
					if(!g_randomSource.GenRandom(pData, m_size))
					{
						MessageBox(_T("PRNG error"), _T("Error"), MB_ICONERROR);
					}
					else
					{
						bResult=TRUE;
					}
					UnmapViewOfFile(pData);
				}
				else
				{
					MessageBox(_T("Can't map file to memory"), _T("Error"), MB_ICONERROR);
				}
				CloseHandle(hMap);
			}
			else
			{
				MessageBox(_T("Can't create file mapping"), _T("Error"), MB_ICONERROR);
			}
		}
		else
		{
			MessageBox(_T("Can't set file size"), _T("Error"), MB_ICONERROR);
		}
		CloseHandle(hFile);
	}
	else
	{
		MessageBox(_T("Can't create file"), _T("Error"), MB_ICONERROR);
	}

	if(bResult)
	{
		CDialog::OnOK();
	}
}

BOOL CNewKeyFileDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	DWORD	dwBuff;

	m_size=0;

	do
	{
		g_randomSource.GenRandom(&dwBuff, sizeof(dwBuff));
		dwBuff&=0xFFF;
		m_size+=dwBuff;
	}while(m_size< 32);		// Restrict minimum size to 256 bits

	UpdateData(FALSE);

	return TRUE;
}
