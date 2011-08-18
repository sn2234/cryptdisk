// FavoritesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "FavoritesView.h"
#include "afxdialogex.h"


// FavoritesView dialog

IMPLEMENT_DYNAMIC(FavoritesView, CDialogEx)

FavoritesView::FavoritesView(CWnd* pParent /*=NULL*/)
	: CDialogEx(FavoritesView::IDD, pParent)
{

}

FavoritesView::~FavoritesView()
{
}

void FavoritesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FAVORITES, m_favoritesList);
}


BEGIN_MESSAGE_MAP(FavoritesView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, &FavoritesView::OnBnClickedButtonMount)
END_MESSAGE_MAP()


// FavoritesView message handlers


void FavoritesView::OnBnClickedButtonMount()
{
	// TODO: Add your control notification handler code here
}
