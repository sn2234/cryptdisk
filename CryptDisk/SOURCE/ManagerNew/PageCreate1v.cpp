// PageCreate1v.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreate1v.h"
#include "afxdialogex.h"
#include "CreateWizardModel.h"


// PageCreate1v dialog

IMPLEMENT_DYNAMIC(PageCreate1v, CPropertyPage)

PageCreate1v::PageCreate1v(Document& doc)
	: CPropertyPage(PageCreate1v::IDD)
	, View<PageCreate1v>(doc)
	, m_volumeId(_T(""))
	, m_size(_T(""))
	, m_driveLetter(_T(""))
	, m_quickFormat(FALSE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageCreate1v::~PageCreate1v()
{
}

void PageCreate1v::OnDocumentUpdate()
{
	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);

	m_quickFormat = m.QuickFormat();

	m_volumeId = m.VolumeDescriptor().deviceId.c_str();
	m_driveLetter = m.VolumeDescriptor().driveLetter.c_str();
	m_size = m.VolumeDescriptor().capacity.c_str();

	UpdateData(FALSE);
}

void PageCreate1v::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VOLUME_ID, m_volumeId);
	DDX_Text(pDX, IDC_EDIT_VOLUME_SIZE, m_size);
	DDX_Text(pDX, IDC_EDIT_DRIVE_LETTER, m_driveLetter);
	DDX_Check(pDX, IDC_CHECK_QUICK2, m_quickFormat);
}


void PageCreate1v::PropagateToModel()
{
	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);

	UpdateData(TRUE);

	m.QuickFormat(m_quickFormat == TRUE);
}

bool PageCreate1v::ValidateData()
{
	return true;
}

BEGIN_MESSAGE_MAP(PageCreate1v, CPropertyPage)
END_MESSAGE_MAP()


// PageCreate1v message handlers


BOOL PageCreate1v::OnSetActive()
{
	CPropertySheet* pWizard = static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_NEXT);

	OnDocumentUpdate();

	return __super::OnSetActive();
}


LRESULT PageCreate1v::OnWizardNext()
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
