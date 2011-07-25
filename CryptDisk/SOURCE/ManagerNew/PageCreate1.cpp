// PageCreate1.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreate1.h"
#include "afxdialogex.h"

#include "CreateWizardModel.h"
#include "KeyFilesDialog.h"

namespace fs = boost::filesystem;

// PageCreate1 dialog

IMPLEMENT_DYNAMIC(PageCreate1, CPropertyPage)

PageCreate1::PageCreate1(Document& doc)
	: CPropertyPage(PageCreate1::IDD)
	, View<PageCreate1>(doc)
	, m_bNotAddToDocuments(TRUE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageCreate1::~PageCreate1()
{
}

void PageCreate1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_DOCUMENTS, m_bNotAddToDocuments);
	DDX_Text(pDX, IDC_EDIT_PATH, m_path);
}


BEGIN_MESSAGE_MAP(PageCreate1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &PageCreate1::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()

void PageCreate1::OnDocumentUpdate()
{
	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);

	m_bNotAddToDocuments = !m.UseRecentDocuments();
	m_path = m.ImageFilePath().c_str();

	UpdateData(FALSE);
}

// PageCreate1 message handlers


void PageCreate1::OnBnClickedButtonBrowse()
{
	UpdateData(TRUE);

	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);

	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	UpdateData(TRUE);

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("Images\0*.img\0All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Create image");
	ofn.Flags|=OFN_OVERWRITEPROMPT;
	if(m_bNotAddToDocuments)
		ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetSaveFileName(&ofn))
	{
		// Check for extension
		fs::wpath p(filePath);

		if(!p.has_extension())
		{
			p.replace_extension(L".img");
		}

		m.ImageFilePath(p.wstring());
		m.UpdateViews();
	}
}
