#pragma once
#include "afxcmn.h"
#include "View.h"


// FavoritesView dialog

class FavoritesView : public CDialogEx, View<FavoritesView>
{
	DECLARE_DYNAMIC(FavoritesView)

public:
	FavoritesView(Document& doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~FavoritesView();

// Dialog Data
	enum { IDD = IDD_FAVORITESVIEW };

	void OnDocumentUpdate();

protected:
	bool	m_dialogInitialized;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_favoritesList;
	afx_msg void OnBnClickedButtonMount();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnNMDblclkListFavorites(NMHDR *pNMHDR, LRESULT *pResult);
};
