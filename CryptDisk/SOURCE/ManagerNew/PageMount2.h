#pragma once
#include "View.h"


// PageMount2 dialog

class PageMount2 : public CPropertyPage, View<PageMount2>
{
	DECLARE_DYNAMIC(PageMount2)

public:
	PageMount2(Document& doc);
	virtual ~PageMount2();

	void OnDocumentUpdate();
// Dialog Data
	enum { IDD = IDD_PAGEMOUNT2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PropagateToModel();

	DECLARE_MESSAGE_MAP()

	WCHAR		m_driveLetter;
	BOOL		m_bReadOnly;
	BOOL		m_bRemovable;
	BOOL		m_bUseMountManager;
	BOOL		m_bTime;
	BOOL		m_bAddToFav;
	CComboBox	m_driveCombo;

	bool		m_dialogInitialized;

	std::vector<WCHAR> m_driveLetters;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
};
