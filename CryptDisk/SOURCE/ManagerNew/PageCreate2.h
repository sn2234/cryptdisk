#pragma once
#include "View.h"


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

	DECLARE_MESSAGE_MAP()
};
