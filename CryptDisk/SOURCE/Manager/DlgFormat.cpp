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

// DlgFormat.cpp : implementation file
//

#include "stdafx.h"

#include <mmintrin.h>

#include "Common.h"

#include "Manager.h"
#include "DlgFormat.h"

#define		WM_UPDATEPROGRESS	(WM_USER+1)

// CDlgFormat dialog

static DWORD __stdcall ThreadProc(LPVOID pParam);

IMPLEMENT_DYNAMIC(CDlgFormat, CDialog)

CDlgFormat::CDlgFormat(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFormat::IDD, pParent)
{

}

CDlgFormat::~CDlgFormat()
{
}

void CDlgFormat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(CDlgFormat, CDialog)
	ON_MESSAGE(WM_UPDATEPROGRESS, OnProgressUpdate)
END_MESSAGE_MAP()

BOOL CDlgFormat::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	if(!m_pImage)
		return FALSE;

	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);

	DWORD	dwThreadId;

	m_bStopThread=FALSE;
	m_error=0;

	m_hThread=CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadId);
	
	if(!m_hThread)
		return FALSE;

	return TRUE;
}

void CDlgFormat::OnOK()
{
	if(m_error)
	{
		MessageBox(_T("Error while formatting image"), _T("Error"),
			MB_OK|MB_ICONERROR);
		EndDialog(0);
	}
	EndDialog(1);
}

void CDlgFormat::OnCancel()
{
	m_bStopThread=TRUE;
}

// CDlgFormat message handlers

LRESULT CDlgFormat::OnProgressUpdate(WPARAM wParam, LPARAM lParam)
{
	CString		str;

	str.Format(_T("%.2f%%"), m_percentComplete);

	SetDlgItemText(IDC_STATIC_PERCENT, str.GetBuffer());

	m_progress.SetPos((int)m_percentComplete);

	if(m_bStopThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		SendMessage(WM_COMMAND, IDOK, 0);
	}

	return 0;
}

DWORD __stdcall ThreadProc(LPVOID pParam)
{
	CDlgFormat	*pParent=static_cast<CDlgFormat*>(pParam);
	DiskImageNONE	*pImage;

	RC4_CIPHER_ENGINE	rc4cipher;
	BYTE				rc4key[256];
	ULONG				sectors, reservedSectors;

	// Initialize RC4 as fast PRNG
	g_randomSource.GenRandom(rc4key, sizeof(rc4key));

	rc4cipher.SetupKey(rc4key, sizeof(rc4key));
	rc4cipher.Skip(1024);

	pImage=pParent->m_pImage;

	sectors=pImage->GetSectorsCount();
	reservedSectors=pImage->Format(NULL);


	if(!pParent->m_bQuickFormat)
	{
		ULONG	curSector;
		ULONG	sectorsLeft;
		ULONG	writeSectors;

		// Allocate 65k buffer. Windows write data to disk in 65k chunks.
		UCHAR *buff=(UCHAR*)VirtualAlloc(NULL, 0x10000, MEM_COMMIT, PAGE_READWRITE);

		if(buff)
		{
			sectorsLeft=sectors-reservedSectors;
			curSector=reservedSectors;
			writeSectors=0x10000/BYTES_PER_SECTOR;

			while(sectorsLeft >= writeSectors)
			{
				rc4cipher.Generate(buff, 0x10000);
				if(!pImage->WriteSectors(curSector, writeSectors, buff))
				{
					pParent->m_error=1;
					goto _loop_exit;
				}

				_mm_empty();

				curSector+=writeSectors;
				sectorsLeft-=writeSectors;

				pParent->m_percentComplete=((float)curSector/(float)sectors)*100.0f;
				
				if(pParent->m_bStopThread)
					goto _loop_exit;
				else
					SendMessage(pParent->GetSafeHwnd(), WM_UPDATEPROGRESS, 0, 0);
			}

			if(sectorsLeft)
			{
				rc4cipher.Generate(buff, 0x10000);
				if(!pImage->WriteSectors(curSector, sectorsLeft, buff))
				{
					pParent->m_error=1;
					goto _loop_exit;
				}

				_mm_empty();
				pParent->m_percentComplete=((float)curSector/(float)sectors)*100.0f;
				PostMessage(pParent->GetSafeHwnd(), WM_UPDATEPROGRESS, 0, 0);
			}
_loop_exit:
			RtlSecureZeroMemory(buff, 0x10000);
			VirtualFree(buff, 0, MEM_RELEASE);
		}
	}

	pParent->m_bStopThread=TRUE;
	pParent->m_percentComplete=100.0f;
	PostMessage(pParent->GetSafeHwnd(), WM_UPDATEPROGRESS, 0, 0);

	rc4cipher.Clear();
	RtlSecureZeroMemory(rc4key, sizeof(rc4key));
	
	return 0;
}
