
#pragma once

#include "View.h"

// PageMount1 dialog

class PageMount1 : public CPropertyPage, View<PageMount1>
{
	DECLARE_DYNAMIC(PageMount1)

public:
	PageMount1(Document& doc);
	virtual ~PageMount1();

	void OnDocumentUpdate();
// Dialog Data
	enum { IDD = IDD_PAGEMOUNT1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PropagateToModel();
	bool ValidateData();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonChangePassword();
	afx_msg void OnBnClickedButtonKeyFiles();
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnBnClickedButtonRestore();
	BOOL		m_bNotAddToDocuments;
	CString		m_path;

	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
};
