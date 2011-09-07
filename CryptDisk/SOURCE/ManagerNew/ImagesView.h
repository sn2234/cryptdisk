#pragma once

#include "View.h"
#include "afxcmn.h"
#include "afxwin.h"

// ImagesView dialog

class ImagesView : public CDialogEx, View<ImagesView>
{
	DECLARE_DYNAMIC(ImagesView)

public:
	ImagesView(Document& doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~ImagesView();

// Dialog Data
	enum { IDD = IDD_IMAGESVIEW };

	void OnDocumentUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listImages;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMount();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonDismountImage();
	afx_msg void OnBnClickedButtonDismountall();
	CButton m_buttonMount;
	CButton m_buttonDismount;
	CButton m_buttonDismountAll;
};
