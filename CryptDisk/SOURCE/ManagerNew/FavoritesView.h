#pragma once
#include "afxcmn.h"


// FavoritesView dialog

class FavoritesView : public CDialogEx
{
	DECLARE_DYNAMIC(FavoritesView)

public:
	FavoritesView(CWnd* pParent = NULL);   // standard constructor
	virtual ~FavoritesView();

// Dialog Data
	enum { IDD = IDD_FAVORITESVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_favoritesList;
	afx_msg void OnBnClickedButtonMount();
};
