#pragma once
#include "View.h"


// PageCreate1 dialog

class PageCreate1 : public CPropertyPage, View<PageCreate1>
{
	DECLARE_DYNAMIC(PageCreate1)

public:
	PageCreate1(Document& doc);
	virtual ~PageCreate1();

	void OnDocumentUpdate();
// Dialog Data
	enum { IDD = IDD_PAGECREATE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PropagateToModel();
	bool ValidateData();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();

	BOOL		m_bNotAddToDocuments;
	CString		m_path;
	CString		m_imageSize;
	CComboBox	m_comboSize;
	BOOL		m_bQuickFormat;

	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

	virtual BOOL OnInitDialog();
};
