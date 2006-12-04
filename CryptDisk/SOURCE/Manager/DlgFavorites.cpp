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

// DlgFavorites.cpp : implementation file
//

#include "stdafx.h"
#include "Common.h"
#include "Manager.h"
#include "DlgFavorites.h"
#include "DlgFavItem.h"
#include "DlgPassword.h"

// CDlgFavorites dialog

IMPLEMENT_DYNAMIC(CDlgFavorites, CDialog)

CDlgFavorites::CDlgFavorites(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFavorites::IDD, pParent)
{
	m_pFavManager=NULL;
}

CDlgFavorites::~CDlgFavorites()
{
}

void CDlgFavorites::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listFav);
}


BEGIN_MESSAGE_MAP(CDlgFavorites, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgFavorites::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgFavorites::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgFavorites::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgFavorites message handlers
BOOL CDlgFavorites::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	ASSERT(m_pFavManager);

	m_listFav.InsertColumn(0, _T("Path"), LVCFMT_LEFT, 150, 0);
	//m_listFav.InsertColumn(1, _T("Options"), LVCFMT_LEFT, 150, 1);
	m_listFav.InsertColumn(2, _T("Drive letter"), LVCFMT_LEFT, 67, 1);

	m_listFav.SetExtendedStyle(LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT);

	RefreshList();

	return TRUE;
}

// Mount new image
void CDlgFavorites::OnBnClickedButton3()
{
	CDlgFavItem	dlg;
	CFavNode	*pNode;
	int			nSel;

	nSel=m_listFav.GetSelectionMark();
	if(nSel != -1)
	{
		pNode=(CFavNode*)g_favorites.GetNode(nSel);
		if(pNode)
		{
			dlg.LoadData(pNode);
			dlg.m_bEditMode=FALSE;
			if(dlg.DoModal() == IDOK)
			{
				// Get password
				CDlgPassword	passDlg;

				passDlg.DoModal();

				if(passDlg.GetPassword() && passDlg.GetPasswordLength())
				{
					DiskImageNONE	image;

					if(image.Open(pNode->GetFilePath()+4, passDlg.GetPassword(),	 // Skip "\??\"
						passDlg.GetPasswordLength()))
					{
						DISK_ADD_INFO	info;

						info.DriveLetter=pNode->GetDriveLetter();
						wcscpy_s(info.FilePath, pNode->GetFilePath());
						info.MountOptions=pNode->GetMountOptions();
						memcpy(info.UserKey, image.GetHeader()->GetUserKey(),
							sizeof(info.UserKey));
						info.wAlgoId=image.GetHeader()->GetAlgoId();

						image.Close();
						if(!theApp.m_manager.InstallDisk(&info))
						{
							MessageBox(_T("Can't mount disk"), _T("Error"), MB_ICONERROR);
						}
					}
					else
					{
						MessageBox(_T("Unable to open image"), _T("Error"), MB_ICONERROR);
					}
				}
			}
		}
	}
}

// Edit
void CDlgFavorites::OnBnClickedButton2()
{
	CDlgFavItem	dlg;
	CFavNode	*pNode;
	int			nSel;

	nSel=m_listFav.GetSelectionMark();
	if(nSel != -1)
	{
		pNode=(CFavNode*)g_favorites.GetNode(nSel);
		if(pNode)
		{
			dlg.LoadData(pNode);
			dlg.m_bEditMode=TRUE;
			if(dlg.DoModal() == IDOK)
			{
				ULONG	mountOptions;

				mountOptions=0;
				if(dlg.m_bMountMgr)
					mountOptions|=MOUNT_VIA_MOUNTMGR;
				if(dlg.m_bReadOnly)
					mountOptions|=MOUNT_READ_ONLY;
				if(dlg.m_bRemovable)
					mountOptions|=MOUNT_AS_REMOVABLE;
				if(dlg.m_bPreserveTime)
					mountOptions|=MOUNT_SAVE_TIME;

				// Change node
				pNode->Init(dlg.m_strPath.GetBuffer(), mountOptions,
					dlg.m_driveLetter);

				RefreshList();
			}
		}
	}
}

// Remove
void CDlgFavorites::OnBnClickedButton1()
{
	int			nSel;
	nSel=m_listFav.GetSelectionMark();
	if(nSel != -1)
	{
		g_favorites.RemoveNode(nSel);
		RefreshList();
	}
}

void CDlgFavorites::RefreshList()
{
	m_listFav.DeleteAllItems();
	for(int i=0;i< m_pFavManager->GetNodeCount();i++)
	{
		CFavNode	*pNode;

		CString		strBuff;

		pNode=(CFavNode*)m_pFavManager->GetNode(i);

		m_listFav.InsertItem(i, pNode->GetFilePath());
		// 		strBuff.Empty();
		// 		if(pNode->GetMountOptions() & MOUNT_AS_REMOVABLE)
		// 			strBuff+=_T("removable");
		// 		if(pNode->GetMountOptions() & MOUNT_READ_ONLY)
		// 			strBuff+=_T(" readonly");
		// 		if(pNode->GetMountOptions() & MOUNT_VIA_MOUNTMGR)
		// 			strBuff+=_T(" MountMgr");
		// 		if(pNode->GetMountOptions() & MOUNT_SAVE_TIME)
		// 			strBuff+=_T(" Preserve file time");
		//		m_listFav.SetItemText(i, 1, strBuff);
		strBuff.Format(_T("%lc"), pNode->GetDriveLetter());
		m_listFav.SetItemText(i, 1, strBuff);
	}

	m_listFav.SetSelectionMark(0);
}
