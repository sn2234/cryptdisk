// PageMount1.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount1.h"
#include "afxdialogex.h"

#include "MountWizardModel.h"
#include "KeyFilesDialog.h"
#include "AppMemory.h"
#include "DialogBackupHeader.h"
#include "Manager\SRC\SafeHandle.h"

namespace fs = boost::filesystem;

// PageMount1 dialog

IMPLEMENT_DYNAMIC(PageMount1, CPropertyPage)

PageMount1::PageMount1(Document& doc)
	: CPropertyPage(PageMount1::IDD)
	, View<PageMount1>(doc)
	, m_bNotAddToDocuments(TRUE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageMount1::~PageMount1()
{
}

void PageMount1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_RECENT_DOC, m_bNotAddToDocuments);
	DDX_Text(pDX, IDC_EDIT_PATH, m_path);
}


BEGIN_MESSAGE_MAP(PageMount1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &PageMount1::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PASSWORD, &PageMount1::OnBnClickedButtonChangePassword)
	ON_BN_CLICKED(IDC_BUTTON_KEY_FILES, &PageMount1::OnBnClickedButtonKeyFiles)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &PageMount1::OnBnClickedButtonBackup)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &PageMount1::OnBnClickedButtonRestore)
END_MESSAGE_MAP()

void PageMount1::OnDocumentUpdate()
{
	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	m_bNotAddToDocuments = !m.UseRecentDocuments();
	m_path = m.ImageFilePath().c_str();

	UpdateData(FALSE);

	SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, m.Password().c_str());
}

// PageMount1 message handlers


void PageMount1::OnBnClickedButtonBrowse()
{
	UpdateData(TRUE);

	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("Images\0*.img\0All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Open image");
	if(!m_bNotAddToDocuments)
		ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetOpenFileName(&ofn))
	{
		m.ImageFilePath(filePath);
		m.UpdateViews();
	}
}


void PageMount1::OnBnClickedButtonChangePassword()
{
	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	PropagateToModel();

	if(ValidateData())
	{
		m.ChangePassword();
	}
}


void PageMount1::OnBnClickedButtonKeyFiles()
{
	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	KeyFilesDialog dlg(m.KeyFiles());

	if(dlg.DoModal() == IDOK)
	{
		m.KeyFiles(dlg.KeyFiles());
	}
}


void PageMount1::OnBnClickedButtonBackup()
{
	UpdateData(TRUE);

	if(!m_path.IsEmpty() && fs::exists((const wchar_t*)m_path))
	{
		DialogBackupHeader dlg((const wchar_t*)m_path);

		dlg.DoModal();
	}
}


void PageMount1::OnBnClickedButtonRestore()
{
	UpdateData(TRUE);

	if(!m_path.IsEmpty() && fs::exists((const wchar_t*)m_path))
	{
		if(AfxMessageBox(_T("Do you want to backup current header first?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DialogBackupHeader dlg((const wchar_t*)m_path);

			dlg.DoModal();
		}

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
		ofn.lpstrTitle=_T("Open image backup header");
		ofn.Flags|=OFN_DONTADDTORECENT;
		if(GetOpenFileName(&ofn))
		{
			if(fs::exists(filePath))
			{
				SafeHandle hBackupFile(CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
				if (hBackupFile == INVALID_HANDLE_VALUE)
				{
					AfxMessageBox(_T("Unable to open backup file"), MB_ICONERROR);
					return;
				}

				SafeHandle hImageFile(CreateFileW(m_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
				if (hImageFile == INVALID_HANDLE_VALUE)
				{
					AfxMessageBox(_T("Unable to open image file"), MB_ICONERROR);
					return;
				}

				size_t backupFileSize = GetFileSize(hBackupFile, NULL);

				std::vector<unsigned char> backupBuff(backupFileSize);
				{
					DWORD bytesRead;
					BOOL result = ReadFile(hBackupFile, &backupBuff[0], backupBuff.size(), &bytesRead, NULL);
					if(!result || !(bytesRead == backupFileSize))
					{
						AfxMessageBox(_T("Unable to read data"), MB_ICONERROR);
						return;
					}
				}

				{
					DWORD bytesWrite;
					BOOL result = WriteFile(hImageFile, &backupBuff[0], backupBuff.size(), &bytesWrite, NULL);
					if(!result || !(bytesWrite == backupFileSize))
					{
						AfxMessageBox(_T("Unable to write data"), MB_ICONERROR);
						return;
					}
				}

				AfxMessageBox(_T("Image header has been restored successfully"), MB_OK);
			}
		}
	}
}


BOOL PageMount1::OnSetActive()
{
	CPropertySheet* pWizard= static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	OnDocumentUpdate();

	return __super::OnSetActive();
}


LRESULT PageMount1::OnWizardNext()
{
	try
	{
		PropagateToModel();

		return ValidateData() ? __super::OnWizardNext() : -1;
	}
	catch (std::exception& e)
	{
		AfxMessageBox(CString(e.what()), MB_ICONERROR);
		return -1;
	}
}

void PageMount1::PropagateToModel()
{
	UpdateData(TRUE);

	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	m.UseRecentDocuments(m_bNotAddToDocuments == FALSE);
	m.ImageFilePath((LPCWSTR)m_path);

	UINT passLength = GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextLength();

	auto passwordBuffer = AllocPasswordBuffer(passLength+1);
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, &passwordBuffer[0], passLength+1);
	m.Password(&passwordBuffer[0]);
}

bool PageMount1::ValidateData()
{
	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	try
	{
		// Check if all files exists

		if(!fs::exists(m.ImageFilePath()))
		{
			AfxMessageBox(_T("Image file does not exists"), MB_ICONERROR);
			return false;
		}

		if(!std::all_of(m.KeyFiles().cbegin(), m.KeyFiles().cend(), [](const std::wstring& f){
			return fs::exists(f);
		}))
		{
			AfxMessageBox(_T("Not all specified key files exists"), MB_ICONERROR);
			return false;
		}

		// Try to open image
		if(!m.TryOpenImage())
		{
			AfxMessageBox(_T("Unable to open image"), MB_ICONERROR);
			return false;
		}
	}
	catch (std::exception& e)
	{
		AfxMessageBox(CString(e.what()), MB_ICONERROR);
		return false;
	}

	return true;
}
