// ImagesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "ImagesView.h"
#include "afxdialogex.h"


// ImagesView dialog

IMPLEMENT_DYNAMIC(ImagesView, CDialogEx)

ImagesView::ImagesView(Document& doc, CWnd* pParent)
	: CDialogEx(ImagesView::IDD, pParent)
	, View<ImagesView>(doc)
{

}

ImagesView::~ImagesView()
{
}

void ImagesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGES, m_listImages);
}


BEGIN_MESSAGE_MAP(ImagesView, CDialogEx)
END_MESSAGE_MAP()

void ImagesView::OnDocumentUpdate()
{

}


BOOL ImagesView::OnInitDialog()
{
	__super::OnInitDialog();

	
	m_listImages.InsertColumn(0, _T("Letter"), 0, 50, 0);
	m_listImages.InsertColumn(1, _T("Size"), 0, 80, 1);
	m_listImages.InsertColumn(2, _T("Path"), 0, 200, 2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
