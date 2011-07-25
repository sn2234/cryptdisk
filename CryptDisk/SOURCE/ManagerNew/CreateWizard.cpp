// CreateWizard.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "CreateWizard.h"


// CreateWizard

IMPLEMENT_DYNAMIC(CreateWizard, CPropertySheet)

CreateWizard::CreateWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_page1(m_model)
	, m_page2(m_model)
{
	DoInit();
}

CreateWizard::CreateWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_page1(m_model)
	, m_page2(m_model)
{
	DoInit();
}

CreateWizard::~CreateWizard()
{
}


BEGIN_MESSAGE_MAP(CreateWizard, CPropertySheet)
END_MESSAGE_MAP()

void CreateWizard::DoInit()
{
	m_psh.dwFlags &= ~PSH_HASHELP;

	AddPage(&m_page1);
	AddPage(&m_page2);

	SetWizardMode();
}

// CreateWizard message handlers
