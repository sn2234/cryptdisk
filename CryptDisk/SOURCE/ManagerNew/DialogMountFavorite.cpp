// DialogMountFavorite.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogMountFavorite.h"
#include "afxdialogex.h"
#include "KeyFilesDialog.h"
#include "PasswordBuilder.h"
#include "CryptDiskHelpers.h"
#include "DialogChangePassword.h"
#include "AppMemory.h"
#include "DriverTools.h"
#include "DriverProtocol.h"


// DialogMountFavorite dialog

IMPLEMENT_DYNAMIC(DialogMountFavorite, CDialogEx)

DialogMountFavorite::DialogMountFavorite(const FavoriteImage& favImage, CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogMountFavorite::IDD, pParent)
	, m_favImage(favImage)
	, m_path(favImage.ImagePath().c_str())
	, m_bReadOnly(favImage.ReadOnly())
	, m_bRemovable(favImage.Removable())
	, m_bUseMountManager(favImage.MountManager())
	, m_bTime(favImage.PreserveTimestamp())
{

}

DialogMountFavorite::~DialogMountFavorite()
{
}

void DialogMountFavorite::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_path);
	DDX_Check(pDX, IDC_CHECK_READONLY, m_bReadOnly);
	DDX_Check(pDX, IDC_CHECK_REMOVABLE, m_bRemovable);
	DDX_Check(pDX, IDC_CHECK_MOUNTMGR, m_bUseMountManager);
	DDX_Check(pDX, IDC_CHECK_TIMESTAMP, m_bTime);
	DDX_Control(pDX, IDC_COMBO_DRIVE, m_driveCombo);
}


BEGIN_MESSAGE_MAP(DialogMountFavorite, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PASSWORD, &DialogMountFavorite::OnBnClickedButtonChangePassword)
	ON_BN_CLICKED(IDC_BUTTON_KEY_FILES, &DialogMountFavorite::OnBnClickedButtonKeyFiles)
END_MESSAGE_MAP()


// DialogMountFavorite message handlers


void DialogMountFavorite::OnBnClickedButtonChangePassword()
{
	int passwordLength = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
	auto passwordBuffer = AllocPasswordBuffer(passwordLength+1);

	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, &passwordBuffer[0], passwordLength+1);

	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(&passwordBuffer[0]), passwordLength);

	if(!CryptDiskHelpers::CheckImage(m_path, pb.Password(), pb.PasswordLength()))
	{
		AfxMessageBox(_T("Unable to open image"), MB_ICONERROR);
		return;
	}

	DialogChangePassword dlg((const WCHAR*)m_path, pb.Password(), pb.PasswordLength());

	dlg.DoModal();
}


void DialogMountFavorite::OnBnClickedButtonKeyFiles()
{
	KeyFilesDialog dlg(m_keyFiles);

	if(dlg.DoModal() == IDOK)
	{
		m_keyFiles = dlg.KeyFiles();
	}
}


void DialogMountFavorite::OnOK()
{
	try
	{

		int passwordLength = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
		auto passwordBuffer = AllocPasswordBuffer(passwordLength+1);

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, &passwordBuffer[0], passwordLength+1);

		PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(&passwordBuffer[0]), passwordLength);

		if(CryptDiskHelpers::CheckImage(m_path, pb.Password(), pb.PasswordLength()))
		{
			ULONG mountOptions = 0;

			if(m_bUseMountManager) mountOptions |= MOUNT_VIA_MOUNTMGR;
			if(m_bTime) mountOptions |= MOUNT_SAVE_TIME;
			if(m_bReadOnly) mountOptions |= MOUNT_READ_ONLY;
			if(m_bRemovable) mountOptions |= MOUNT_AS_REMOVABLE;

			CryptDiskHelpers::MountImage(
				*AppDriver::instance().getDriverControl(),
				m_path,
				m_driveLetters[m_driveCombo.GetCurSel()],
				pb.Password(),
				pb.PasswordLength(),
				mountOptions);

			CDialogEx::OnOK();
		}
		else
		{
			AfxMessageBox(_T("Unable to open image"), MB_ICONERROR);
		}
	}
	catch (const std::exception& e)
	{
		AfxMessageBox(CString(e.what()), MB_ICONERROR);
	}
}


BOOL DialogMountFavorite::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Fill drives combo
	DWORD	drives;

	drives=GetLogicalDrives();

	for(int i=0;i<=L'Z'-L'A';i++)
	{
		if((drives&(1<<i)) == 0)
		{
			m_driveLetters.push_back(L'A' + i);
		}
	}

	std::for_each(m_driveLetters.cbegin(), m_driveLetters.cend(), [&] (WCHAR ch){
		m_driveCombo.AddString(CString(ch));
	});

	auto i = std::find(m_driveLetters.cbegin(), m_driveLetters.cend(), m_favImage.DriveLetter());
	if(i != m_driveLetters.cend())
	{
		m_driveCombo.SetCurSel(static_cast<int>(i - m_driveLetters.cbegin()));
	}
	else
	{
		m_driveCombo.SetCurSel(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
