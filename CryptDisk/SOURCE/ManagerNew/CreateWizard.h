
#pragma once
#include "CreateWizardModel.h"
#include "PageCreate1.h"
#include "PageCreate2.h"

// CreateWizard

class CreateWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CreateWizard)

public:
	CreateWizard(bool createVolume, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CreateWizard(bool createVolume, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CreateWizard();

	void DoInit();
private:
	CreateWizardModel	m_model;
	PageCreate1			m_page1;
	PageCreate2			m_page2;
	bool				m_createVolume;
protected:
	DECLARE_MESSAGE_MAP()
};
