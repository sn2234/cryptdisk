
#pragma once

#include "PageMount1.h"
#include "PageMount1v.h"
#include "PageMount2.h"
#include "MountWizardModel.h"

// MountWizard

class MountWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(MountWizard)
	typedef std::shared_ptr<CPropertyPage> PPropertyPage;
public:
	MountWizard(const VolumeDesk* descriptor, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	MountWizard(const VolumeDesk* descriptor, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~MountWizard();

	void DoInit();
private:
	MountWizardModel	m_model;

	PPropertyPage	m_page1;
	PPropertyPage	m_page2;
protected:
	DECLARE_MESSAGE_MAP()
};
