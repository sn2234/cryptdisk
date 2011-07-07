// PageMount1.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount1.h"
#include "afxdialogex.h"

#include "MountWizardModel.h"
#include "KeyFilesDialog.h"

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
	// TODO: Add your control notification handler code here
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
	// TODO: Add your control notification handler code here
}


void PageMount1::OnBnClickedButtonRestore()
{
	// TODO: Add your control notification handler code here
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
	PropagateToModel();

	return ValidateData() ? __super::OnWizardNext() : -1;
}

void PageMount1::PropagateToModel()
{
	UpdateData(TRUE);

	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	m.UseRecentDocuments(m_bNotAddToDocuments == FALSE);
	m.ImageFilePath((LPCWSTR)m_path);

	UINT passLength = GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowTextLength();

	std::vector<char> tmp(passLength + 1);
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, &tmp[0], tmp.size());
	m.Password(&tmp[0]);
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