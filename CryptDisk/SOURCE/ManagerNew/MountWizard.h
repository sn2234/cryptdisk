
#pragma once

#include "PageMount1.h"
#include "MountWizardModel.h"

// MountWizard

class MountWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(MountWizard)

public:
	MountWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	MountWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~MountWizard();

	void DoInit();
private:
	MountWizardModel	m_model;
	PageMount1			m_page1;
protected:
	DECLARE_MESSAGE_MAP()
};


