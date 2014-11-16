
#pragma once
#include "CreateWizardModel.h"
#include "PageCreate1.h"
#include "PageCreate2.h"
#include "VolumeTools.h"

// CreateWizard

class CreateWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CreateWizard)

public:
	CreateWizard(const VolumeDesk* descriptor, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CreateWizard(const VolumeDesk* descriptor, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CreateWizard();

	void DoInit();
private:
	CreateWizardModel					m_model;
	std::unique_ptr<PageCreate1>		m_page1;
	std::unique_ptr<PageCreate2>		m_page2;
	bool								m_createVolume;
protected:
	DECLARE_MESSAGE_MAP()
};
