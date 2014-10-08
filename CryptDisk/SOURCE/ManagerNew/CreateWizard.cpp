// CreateWizard.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "CreateWizard.h"
#include "AppRandom.h"


// CreateWizard

IMPLEMENT_DYNAMIC(CreateWizard, CPropertySheet)

CreateWizard::CreateWizard(bool createVolume, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_createVolume(createVolume)
	, m_page1(m_model)
	, m_page2(m_model)
{
	DoInit();
}

CreateWizard::CreateWizard(bool createVolume, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_createVolume(createVolume)
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

	if (m_createVolume)
	{
		// TODO: add another page
	}
	else
	{
		AddPage(&m_page1);
	}

	AddPage(&m_page2);

	SetWizardMode();

	AppRandom::instance().InitRandomUI();
}

// CreateWizard message handlers
