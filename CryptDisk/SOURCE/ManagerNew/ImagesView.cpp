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
}


BEGIN_MESSAGE_MAP(ImagesView, CDialogEx)
END_MESSAGE_MAP()

void ImagesView::OnDocumentUpdate()
{

}
