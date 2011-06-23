// PageMount1.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount1.h"
#include "afxdialogex.h"

#include "MountWizardModel.h"
#include "KeyFilesDialog.h"

// PageMount1 dialog

IMPLEMENT_DYNAMIC(PageMount1, CPropertyPage)

PageMount1::PageMount1(Document& doc)
	: CPropertyPage(PageMount1::IDD)
	, View<PageMount1>(doc)
	, m_bAddToDocuments(FALSE)
{

}

PageMount1::~PageMount1()
{
}

void PageMount1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_RECENT_DOC, m_bAddToDocuments);
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

	m_bAddToDocuments = m.UseRecentDocuments();
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
	if(m_bAddToDocuments)
		ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetOpenFileName(&ofn))
	{
		m.ImageFilePath(filePath);
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
