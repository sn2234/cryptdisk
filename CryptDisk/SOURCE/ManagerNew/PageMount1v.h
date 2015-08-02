#pragma once

#include "View.h"

// PageMount1v dialog

class PageMount1v : public CPropertyPage, View<PageMount1v>
{
	DECLARE_DYNAMIC(PageMount1v)

public:
	PageMount1v(Document& doc);
	virtual ~PageMount1v();

	void OnDocumentUpdate();
// Dialog Data
	enum { IDD = IDD_PAGEMOUNT3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedButtonChangePassword();
	afx_msg void OnBnClickedButtonKeyFiles();
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnBnClickedButtonRestore();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

	void PropagateToModel();
	bool ValidateData();

	DECLARE_MESSAGE_MAP()
private:
	CString m_editVolumeId;
	CString m_editSize;
	CString m_editDriveLetter;
};
