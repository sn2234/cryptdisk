#pragma once

#include "View.h"
#include "afxcmn.h"

// VolumesView dialog

class VolumesView : public CDialogEx, View<VolumesView>
{
	DECLARE_DYNAMIC(VolumesView)

public:
	VolumesView(Document& doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~VolumesView();

// Dialog Data
	enum { IDD = IDD_VOLUMESVIEW };

	void OnDocumentUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CListCtrl m_listVolumes;
};
