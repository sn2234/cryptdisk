// CreateWizard.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "CreateWizard.h"
#include "AppRandom.h"


// CreateWizard

IMPLEMENT_DYNAMIC(CreateWizard, CPropertySheet)

CreateWizard::CreateWizard(const VolumeDesk* descriptor, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_model(descriptor)
	, m_createVolume(descriptor != nullptr)
	, m_page1(std::make_unique<PageCreate1>(m_model))
	, m_page1v(std::make_unique<PageCreate1v>(m_model))
	, m_page2(std::make_unique<PageCreate2>(m_model))
{
	DoInit();
}

CreateWizard::CreateWizard(const VolumeDesk* descriptor, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_model(descriptor)
	, m_createVolume(descriptor != nullptr)
	, m_page1(descriptor != nullptr ? nullptr : std::make_unique<PageCreate1>(m_model))
	, m_page1v(descriptor != nullptr ? std::make_unique<PageCreate1v>(m_model) : nullptr)
	, m_page2(std::make_unique<PageCreate2>(m_model))
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

	if (!m_createVolume)
	{
		AddPage(m_page1.get());
	}
	else
	{
		AddPage(m_page1v.get());
	}

	AddPage(m_page2.get());

	SetWizardMode();

	AppRandom::instance().InitRandomUI();
}

// CreateWizard message handlers
