// PageMount1v.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount1v.h"
#include "afxdialogex.h"
#include "MountWizardModel.h"
#include "AppMemory.h"
#include "CommonTools.h"
#include "KeyFilesDialog.h"
#include "CryptDiskHelpers.h"
#include "DialogBackupHeader.h"

namespace fs = boost::filesystem;

namespace
{
	std::wstring FormatSize(const std::string& sizeStr)
	{
		if (sizeStr.empty())
		{
			return std::wstring(L"Unknown");
		}

		UINT64 size = boost::lexical_cast<UINT64>(sizeStr);

		return CommonTools::FormatSize(size);
	}
}
// PageMount1v dialog

IMPLEMENT_DYNAMIC(PageMount1v, CPropertyPage)

PageMount1v::PageMount1v(Document& doc)
	: CPropertyPage(PageMount1v::IDD)
	, View<PageMount1v>(doc)
	, m_editVolumeId(_T(""))
	, m_editSize(_T(""))
	, m_editDriveLetter(_T(""))
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageMount1v::~PageMount1v()
{
}

void PageMount1v::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VOLUME_ID, m_editVolumeId);
	DDX_Text(pDX, IDC_EDIT_VOLUME_SIZE, m_editSize);
	DDX_Text(pDX, IDC_EDIT_DRIVE_LETTER, m_editDriveLetter);
}


BEGIN_MESSAGE_MAP(PageMount1v, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PASSWORD, &PageMount1v::OnBnClickedButtonChangePassword)
	ON_BN_CLICKED(IDC_BUTTON_KEY_FILES, &PageMount1v::OnBnClickedButtonKeyFiles)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &PageMount1v::OnBnClickedButtonBackup)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &PageMount1v::OnBnClickedButtonRestore)
END_MESSAGE_MAP()



void PageMount1v::OnBnClickedButtonChangePassword()
{
	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	PropagateToModel();

	if (ValidateData())
	{
		m.ChangePassword();
	}
}


void PageMount1v::OnBnClickedButtonKeyFiles()
{
	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	KeyFilesDialog dlg(m.KeyFiles());

	if (dlg.DoModal() == IDOK)
	{
		m.KeyFiles(dlg.KeyFiles());
	}
}


void PageMount1v::OnBnClickedButtonBackup()
{
	UpdateData(TRUE);

	PropagateToModel();

	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	DialogBackupHeader dlg(m.VolumeDescriptor());

	dlg.DoModal();
}


void PageMount1v::OnBnClickedButtonRestore()
{
	try
	{
		UpdateData(TRUE);

		PropagateToModel();

		const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

 		if (AfxMessageBox(_T("Do you want to backup current header first?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
 		{
 			DialogBackupHeader dlg(m.VolumeDescriptor());
 
 			if (dlg.DoModal() == IDCANCEL)
 			{
 				return;
 			}
 		}

		OPENFILENAME	ofn;
		TCHAR			filePath[MAX_PATH];

		memset(&ofn, 0, sizeof(ofn));
		memset(&filePath, 0, sizeof(filePath));
		ofn.lStructSize = sizeof(ofn);
		ofn.hInstance = AfxGetApp()->m_hInstance;
		ofn.hwndOwner = GetSafeHwnd();
		ofn.lpstrFilter = _T("Backup files\0*.imghdr\0All files\0*.*\0\0");
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = sizeof(filePath);
		ofn.lpstrTitle = _T("Open image backup header");
		ofn.Flags |= OFN_DONTADDTORECENT;
		if (GetOpenFileName(&ofn))
		{
			if (fs::exists(filePath))
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
				CryptDiskHelpers::RestoreVolumeHeader(conv.from_bytes(m.VolumeDescriptor().deviceId), conv.to_bytes(filePath));

				AfxMessageBox(_T("Image header has been restored successfully"), MB_OK);
			}
		}
	}
	catch (const std::exception& ex)
	{
		AfxMessageBox(CString(ex.what()), MB_ICONERROR);
	}
}

void PageMount1v::OnDocumentUpdate()
{
	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	auto volumeDescriptor = m.VolumeDescriptor();

	m_editVolumeId = volumeDescriptor.deviceId.c_str();
	m_editSize = FormatSize(volumeDescriptor.capacity).c_str();
	m_editDriveLetter = volumeDescriptor.driveLetter.c_str();

	UpdateData(FALSE);

	SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, m.Password().c_str());
}

void PageMount1v::PropagateToModel()
{
	UpdateData(TRUE);

	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	UINT passLength = GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextLength();

	auto passwordBuffer = AllocPasswordBuffer(passLength + 1);
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, &passwordBuffer[0], passLength + 1);
	m.Password(&passwordBuffer[0]);
}

bool PageMount1v::ValidateData()
{
	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	try
	{
		// Check if all files exists
		if (!std::all_of(m.KeyFiles().cbegin(), m.KeyFiles().cend(), [](const std::wstring& f){
			return fs::exists(f);
		}))
		{
			AfxMessageBox(_T("Not all specified key files exists"), MB_ICONERROR);
			return false;
		}

		// Try to open image
		if (!m.TryOpenImage())
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


BOOL PageMount1v::OnSetActive()
{
	CPropertySheet* pWizard = static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	OnDocumentUpdate();

	return __super::OnSetActive();
} 


LRESULT PageMount1v::OnWizardNext()
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
