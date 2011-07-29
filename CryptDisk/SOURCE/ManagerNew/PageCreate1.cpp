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
	DDX_Text(pDX, IDC_EDIT_SIZE, m_imageSize);
	DDX_Control(pDX, IDC_COMBO_SIZE, m_comboSize);
}


BEGIN_MESSAGE_MAP(PageCreate1, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &PageCreate1::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()

void PageCreate1::OnDocumentUpdate()
{
	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);

	m_bNotAddToDocuments = !m.UseRecentDocuments();
	m_path = m.ImageFilePath().c_str();
	m_bQuickFormat = m.QuickFormat();

	if(m.ImageSize() == 0)
	{
		m_imageSize = _T("");
	}
	else if(m.ImageSize() > 1024ULL * 1024 * 1024)
	{
		m_comboSize.SetCurSel(1);
		m_imageSize = boost::lexical_cast<std::wstring>(m.ImageSize()/(1024ULL * 1024 * 1024)).c_str();
	}
	else
	{
		m_comboSize.SetCurSel(0);
		m_imageSize = boost::lexical_cast<std::wstring>(m.ImageSize()/(1024ULL * 1024)).c_str();
	}

	UpdateData(FALSE);
}

// PageCreate1 message handlers


void PageCreate1::OnBnClickedButtonBrowse()
{
	UpdateData(TRUE);

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

		m_path = p.wstring().c_str();
		UpdateData(FALSE);
	}
}

BOOL PageCreate1::OnInitDialog()
{
	__super::OnInitDialog();

	m_comboSize.InsertString(0, _T("MB"));
	m_comboSize.InsertString(1, _T("GB"));
	m_comboSize.SetCurSel(0);

	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);
	m.UpdateViews();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PageCreate1::PropagateToModel()
{
	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);
	
	UpdateData(TRUE);

	m.UseRecentDocuments(!m_bNotAddToDocuments);
	m.ImageFilePath((LPCWSTR)m_path);
	m.QuickFormat(m_bQuickFormat == TRUE);
	if(m_imageSize.IsEmpty())
	{
		m.ImageSize(0);
	}
	else
	{
		m.ImageSize(boost::lexical_cast<unsigned __int64>((LPCWSTR)m_imageSize) * (m_comboSize.GetCurSel() == 0 ? 1024ULL * 1024 : 1024ULL * 1024 * 1024));
	}
}

bool PageCreate1::ValidateData()
{
	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);
	try
	{
		if(fs::path(m.ImageFilePath()).empty())
		{
			AfxMessageBox(_T("Image path can't be empty"), MB_ICONERROR);
			return false;
		}

		if(!fs::path(m.ImageFilePath()).has_root_path())
		{
			AfxMessageBox(_T("Image path should be absolute"), MB_ICONERROR);
			return false;
		}

		if(m.ImageSize() == 0)
		{
			AfxMessageBox(_T("Image size can't be zero"), MB_ICONERROR);
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

BOOL PageCreate1::OnSetActive()
{
	CPropertySheet* pWizard= static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	OnDocumentUpdate();

	return __super::OnSetActive();
}

LRESULT PageCreate1::OnWizardNext()
{
	UpdateData(TRUE);

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
