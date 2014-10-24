#pragma once

#include "View.h"
#include "afxcmn.h"

// PageCreateVolume dialog

class PageCreateVolume : public CPropertyPage, View<PageCreateVolume>
{
	DECLARE_DYNAMIC(PageCreateVolume)

public:
	PageCreateVolume(Document& doc);
	virtual ~PageCreateVolume();

	void OnDocumentUpdate();

// Dialog Data
	enum { IDD = IDD_PAGECREATEVOLUME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_listVolumes;
public:
	afx_msg void OnBnClickedButtonRefresh();
	virtual BOOL OnInitDialog();
};
