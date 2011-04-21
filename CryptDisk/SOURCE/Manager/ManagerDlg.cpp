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

// ManagerDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Manager.h"
#include "ManagerDlg.h"

#include "WizMount.h"
#include "WizCreate.h"

#include "NewKeyFileDlg.h"

#include "DlgFavorites.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog();
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	TCHAR	moduleName[MAX_PATH];

	if(!CDialog::OnInitDialog())
		return FALSE;

	GetModuleFileName(theApp.m_hInstance, moduleName, _countof(moduleName));

	DWORD	dwSize, dwResult;

	dwSize=GetFileVersionInfoSize(moduleName, &dwResult);
	BYTE	*pBuff=new BYTE[dwSize];
	VS_FIXEDFILEINFO	*pVer;

	GetFileVersionInfo(moduleName, 0, dwSize, pBuff);
	VerQueryValue(pBuff, _T("\\"), (LPVOID*)&pVer, (PUINT)&dwResult);

	CString		strVersion;

	strVersion.Format(_T("CryptDisk.4h Manager %d.%d.%d.%d"),
		pVer->dwProductVersionMS >> 16,
		pVer->dwProductVersionMS & 0xFFFF,
		pVer->dwProductVersionLS >> 16,
		pVer->dwProductVersionLS & 0xFFFF);

	SetDlgItemText(IDC_STATIC_VER, strVersion.GetBuffer());

	delete[] pBuff;

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CManagerDlg dialog




CManagerDlg::CManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_imagesListView);
}

BEGIN_MESSAGE_MAP(CManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_ABOUT, &CManagerDlg::OnHelpAbout)
	ON_BN_CLICKED(IDC_BUTTON1, &CManagerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CManagerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CManagerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CManagerDlg::OnBnClickedButton4)
	ON_COMMAND(ID_FILE_OPENIMAGE, &CManagerDlg::OnFileOpenimage)
	ON_COMMAND(ID_FILE_NEWIMAGE, &CManagerDlg::OnFileNewimage)
	ON_COMMAND(ID_FILE_CREATEKEYFILE, &CManagerDlg::OnFileCreatekeyfile)
	ON_BN_CLICKED(IDC_BUTTON5, &CManagerDlg::OnBnClickedNewImage)
	ON_COMMAND(ID_HELP_USERMANUAL, &CManagerDlg::OnHelpUsermanual)
END_MESSAGE_MAP()


// CManagerDlg message handlers

BOOL CManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_imagesListView.SetExtendedStyle(LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT);

	// Init list
	LVCOLUMN	col;

	col.mask=LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	col.fmt=LVCFMT_LEFT;
	col.pszText=_T("Letter");
	col.iSubItem=0;
	col.cx=50;
	m_imagesListView.InsertColumn(0, &col);

	col.pszText=_T("Size");
	col.iSubItem=1;
	col.cx=80;
	m_imagesListView.InsertColumn(1, &col);

	col.pszText=_T("Path");
	col.iSubItem=3;
	col.cx=244;
	m_imagesListView.InsertColumn(2, &col);

	// Init interface
	if(!theApp.m_bDriverOk)
	{
 		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
 		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
 		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
 		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	}

	UpdateList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CManagerDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

// Mount image
void CManagerDlg::OnBnClickedButton1()
{
	CWizMount	wizard(_T("123"));

	wizard.DoModal();
	
	UpdateList();
}

// Favorites
void CManagerDlg::OnBnClickedButton2()
{
	CDlgFavorites	dlg;

	dlg.m_pFavManager=&g_favorites;
	dlg.DoModal();

	UpdateList();
}

// Dismount
void CManagerDlg::OnBnClickedButton3()
{
	DISK_DELETE_INFO	info;
	int					n;

	info.bForce=FALSE;

	n=m_imagesListView.GetSelectionMark();
	if(n != -1)
	{
		info.DriveLetter=LOWORD(m_imagesListView.GetItemData(n));

		if(!theApp.m_manager.UninstallDisk(&info))
		{
			if(MessageBox(_T("This disk may be used by another applications. Do you want to force dismount it?"),
				_T("Warning"),
				MB_ICONWARNING|MB_YESNO)==IDYES)
			{
				info.bForce=TRUE;
				if(!theApp.m_manager.UninstallDisk(&info))
				{
					MessageBox(_T("Can\'t dismount disk"), _T("Error"),
						MB_OK|MB_ICONERROR);
				}
			}
		}
	}

	UpdateList();
}

// Dismount all
void CManagerDlg::OnBnClickedButton4()
{
	DISK_DELETE_INFO	info;
	int					n;
	BOOL				bRez=TRUE;

	n=m_imagesListView.GetItemCount();
	for(int i=0;i<n;i++)
	{
		info.bForce=FALSE;
		info.DriveLetter=LOWORD(m_imagesListView.GetItemData(i));

		bRez&=theApp.m_manager.UninstallDisk(&info);
	}

	if(!bRez)
	{
		MessageBox(_T("Unable to dismount some disks"), _T("Warning"),
			MB_ICONWARNING);
	}

	UpdateList();
}

void CManagerDlg::UpdateList()
{
	m_imagesListView.DeleteAllItems();

	DISKS_COUNT_INFO	countInfo;

	if(theApp.m_manager.GetDisksCount(&countInfo))
	{
		DISK_BASIC_INFO		*pDiskInfo;

		pDiskInfo=new DISK_BASIC_INFO[countInfo.DiskCount];

		theApp.m_manager.GetDisksInfo(pDiskInfo,
			sizeof(DISK_BASIC_INFO)*countInfo.DiskCount);

		for(ULONG i=0;i<countInfo.DiskCount;i++)
		{
			CString		tmp;

			tmp.Format(_T("%wc:"), pDiskInfo[i].DriveLetter);
			m_imagesListView.InsertItem(i, tmp);

			tmp.Format(_T("%I64u"), pDiskInfo[i].FileSize.QuadPart);
			m_imagesListView.SetItemText(i, 1, tmp);

			tmp.Format(_T("%s"), pDiskInfo[i].FilePath+4);
			m_imagesListView.SetItemText(i, 2, tmp);

			m_imagesListView.SetItemData(i, pDiskInfo[i].DriveLetter);
		}

		delete[] pDiskInfo;
	}
}

void CManagerDlg::OnFileOpenimage()
{
	CWizMount	wizard(_T("123"));

	wizard.DoModal();

	UpdateList();
}

void CManagerDlg::OnFileNewimage()
{
	if(InitRandom(AfxGetApp()->m_hInstance, GetSafeHwnd()))
	{
		CWizCreate	wiz(_T("123"));

		wiz.DoModal();
	}
}

void CManagerDlg::OnFileCreatekeyfile()
{
	if(InitRandom(AfxGetApp()->m_hInstance, GetSafeHwnd()))
	{
		CNewKeyFileDlg	dlg;

		dlg.DoModal();
	}
}

void CManagerDlg::OnBnClickedNewImage()
{
	if(InitRandom(AfxGetApp()->m_hInstance, GetSafeHwnd()))
	{
		CWizCreate	wiz(_T("123"));

		wiz.DoModal();
	}
}

void CManagerDlg::OnHelpUsermanual()
{
	ShellExecute(GetSafeHwnd(), _T("open"), _T("manual.chm"), NULL,
		NULL, SW_SHOWMAXIMIZED);
}
