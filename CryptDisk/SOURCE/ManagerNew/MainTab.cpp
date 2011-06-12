// MainTab.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "MainTab.h"


// MainTab

IMPLEMENT_DYNAMIC(MainTab, CTabCtrl)

MainTab::MainTab()
	: m_imagesView(m_imagesModel, this)
{

}

MainTab::~MainTab()
{
}


BEGIN_MESSAGE_MAP(MainTab, CTabCtrl)
END_MESSAGE_MAP()

void MainTab::Init()
{
	InsertItem(0, _T("Images"));
	InsertItem(1, _T("Favorites"));

	m_imagesView.Create(ImagesView::IDD, this);
	m_imagesView.EnableWindow(TRUE);
	m_imagesView.ShowWindow(SW_SHOW);

	SetRectangle();
}

void MainTab::SetRectangle()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	m_imagesView.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
}

// MainTab message handlers
