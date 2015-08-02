// MountWizard.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "MountWizard.h"


// MountWizard

IMPLEMENT_DYNAMIC(MountWizard, CPropertySheet)

MountWizard::MountWizard(const VolumeDesk* descriptor, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_model(descriptor)
	, m_page1(descriptor ?
		static_cast<PPropertyPage>(std::make_shared<PageMount1v>(m_model)) :
		static_cast<PPropertyPage>(std::make_shared<PageMount1>(m_model)))
	, m_page2(std::make_shared<PageMount2>(m_model))
{
	DoInit();
}

MountWizard::MountWizard(const VolumeDesk* descriptor, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_model(descriptor)
	, m_page1(descriptor ?
		static_cast<PPropertyPage>(std::make_shared<PageMount1v>(m_model)) :
		static_cast<PPropertyPage>(std::make_shared<PageMount1>(m_model)))
	, m_page2(std::make_shared<PageMount2>(m_model))
{
	DoInit();
}

MountWizard::~MountWizard()
{
}


BEGIN_MESSAGE_MAP(MountWizard, CPropertySheet)
END_MESSAGE_MAP()

void MountWizard::DoInit()
{
	m_psh.dwFlags &= ~PSH_HASHELP;

	AddPage(m_page1.get());
	AddPage(m_page2.get());

	SetWizardMode();
}
