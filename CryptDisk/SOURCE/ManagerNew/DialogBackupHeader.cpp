// DialogBackupHeader.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogBackupHeader.h"
#include "afxdialogex.h"
#include "KeyFilesDialog.h"
#include "CryptDiskHelpers.h"
#include "AppMemory.h"
#include "PasswordBuilder.h"
#include "DiskHeaderTools.h"

namespace fs = boost::filesystem;

// DialogBackupHeader dialog

IMPLEMENT_DYNAMIC(DialogBackupHeader, CDialogEx)

DialogBackupHeader::DialogBackupHeader(const std::wstring& imagePath, CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogBackupHeader::IDD, pParent)
	, m_imagePath(imagePath.c_str())
	, m_backupPath(_T(""))
	, m_imageOpenedSuccessfully(false)
{

}

DialogBackupHeader::~DialogBackupHeader()
{
}

void DialogBackupHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_imagePath);
	DDX_Text(pDX, IDC_EDIT_BACKUP_PATH, m_backupPath);
	DDX_Control(pDX, IDC_COMBO_VERSION, m_versionCombo);
	DDX_Control(pDX, IDC_COMBO_ALGORITHM, m_algorithmCombo);
}


BEGIN_MESSAGE_MAP(DialogBackupHeader, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &DialogBackupHeader::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &DialogBackupHeader::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_KEY_FILES, &DialogBackupHeader::OnBnClickedButtonKeyFiles)
	ON_BN_CLICKED(IDOK, &DialogBackupHeader::OnBnClickedOk)
END_MESSAGE_MAP()


// DialogBackupHeader message handlers


void DialogBackupHeader::OnBnClickedButtonBrowse()
{
	UpdateData(TRUE);

	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("Backup files\0*.imghdr\0All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Create image");
	ofn.Flags|=OFN_OVERWRITEPROMPT;
	ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetSaveFileName(&ofn))
	{
		// Check for extension
		fs::wpath p(filePath);

		if(!p.has_extension())
		{
			p.replace_extension(L".imghdr");
		}

		m_backupPath = p.wstring().c_str();
		UpdateData(FALSE);
	}
}


void DialogBackupHeader::OnBnClickedButtonOpen()
{
	size_t passwordLength = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
	boost::shared_array<char> passwordBuff(AllocPasswordBuffer(passwordLength + 1));
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, passwordBuff.get(), static_cast<int>(passwordLength+1));

	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(passwordBuff.get()), passwordLength);

	auto headerBuff = CryptDiskHelpers::ReadImageHeader(m_imagePath);

	DiskHeaderTools::CIPHER_INFO info;
	if(DiskHeaderTools::Decipher(&headerBuff[0], pb.Password(), static_cast<ULONG>(pb.PasswordLength()), &info))
	{
		// Set version and algorithm
		switch(info.versionInfo.formatVersion)
		{
		case DISK_VERSION::DISK_VERSION_3:
			m_versionCombo.SetCurSel(1);
			break;
		case DISK_VERSION::DISK_VERSION_4:
			m_versionCombo.SetCurSel(2);
			break;
		}

		switch(info.diskCipher)
		{
		case DISK_CIPHER::DISK_CIPHER_AES:
			m_algorithmCombo.SetCurSel(1);
			break;
		case DISK_CIPHER::DISK_CIPHER_TWOFISH:
			m_algorithmCombo.SetCurSel(2);
			break;
		}

		UpdateData(FALSE);
		m_imageOpenedSuccessfully = true;
	}
	else
	{
		AfxMessageBox(_T("Unable to open image"), MB_ICONERROR);
	}
}


void DialogBackupHeader::OnBnClickedButtonKeyFiles()
{
	KeyFilesDialog dlg(m_keyFiles);

	if(dlg.DoModal() == IDOK)
	{
		m_keyFiles = dlg.KeyFiles();
	}
}

void DialogBackupHeader::OnBnClickedOk()
{
	if(m_versionCombo.GetCurSel() == 0)
	{
		AfxMessageBox(_T("Open image first or select version manually"), MB_OK | MB_ICONWARNING);
		return;
	}

	size_t headerSize = 0;

	switch(m_versionCombo.GetCurSel())
	{
	case 1:
		headerSize = sizeof(DISK_HEADER_V3);
		break;
	case 2:
		headerSize = sizeof(DISK_HEADER_V4) * 2;
		break;
	}
	ASSERT(headerSize);
	
	std::vector<unsigned char> headerBuff(headerSize);

	
	HANDLE hImageFile(CreateFileW(m_imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	if (hImageFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Unable to open image file"), MB_ICONERROR);
		return;
	}

	SCOPE_EXIT{ CloseHandle(hImageFile); };

	{
		DWORD bytesRead;
		BOOL result = ReadFile(hImageFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesRead, NULL);
		if(!result || !(bytesRead == headerSize))
		{
			AfxMessageBox(_T("Unable to read data"), MB_ICONERROR);
			return;
		}
	}

	HANDLE hBackupFile(CreateFileW(m_backupPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL));
	if (hBackupFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Unable to open backup file"), MB_ICONERROR);
		return;
	}

	SCOPE_EXIT{ CloseHandle(hBackupFile); };

	{
		DWORD bytesWrite;
		BOOL result = WriteFile(hBackupFile, &headerBuff[0], static_cast<DWORD>(headerBuff.size()), &bytesWrite, NULL);
		if(!result || !(bytesWrite == headerSize))
		{
			AfxMessageBox(_T("Unable to write data"), MB_ICONERROR);
			return;
		}
	}

	CDialogEx::OnOK();
}


BOOL DialogBackupHeader::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_versionCombo.InsertString(0, _T(""));
	m_versionCombo.InsertString(1, _T("V3"));
	m_versionCombo.InsertString(2, _T("V4"));
	m_versionCombo.SetCurSel(0);

	m_algorithmCombo.InsertString(0, _T(""));
	m_algorithmCombo.InsertString(1, _T("AES"));
	m_algorithmCombo.InsertString(2, _T("Twofish"));
	m_algorithmCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
