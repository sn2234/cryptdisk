#pragma once
#include "View.h"


// PageCreate1v dialog

class PageCreate1v : public CPropertyPage, View<PageCreate1v>
{
	DECLARE_DYNAMIC(PageCreate1v)

public:
	PageCreate1v(Document& doc);   // standard constructor
	virtual ~PageCreate1v();

	void OnDocumentUpdate();
// Dialog Data
	enum { IDD = IDD_PAGECREATE1V };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PropagateToModel();
	bool ValidateData();

	DECLARE_MESSAGE_MAP()
private:
	CString m_volumeId;
	CString m_size;
public:
	CString m_driveLetter;
private:
	BOOL m_quickFormat;
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
};
