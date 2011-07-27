#pragma once
#include "View.h"
#include "ColorProgress.h"


// PageCreate2 dialog

class PageCreate2 : public CPropertyPage, View<PageCreate2>
{
	DECLARE_DYNAMIC(PageCreate2)

public:
	PageCreate2(Document& doc);
	virtual ~PageCreate2();

	void OnDocumentUpdate();

// Dialog Data
	enum { IDD = IDD_PAGECREATE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PropagateToModel();
	bool ValidateData();

	bool CheckPassword();
	CComboBox		m_comboCipher;
	CString			m_staticBits;
	CColorProgress	m_barQuality;
	bool			m_bInitialized;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGenPass();
	afx_msg void OnBnClickedButtonKeyfiles();
	afx_msg void OnEnChangePassword();

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
};
