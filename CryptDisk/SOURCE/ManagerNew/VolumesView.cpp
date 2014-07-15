// VolumesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "VolumesView.h"
#include "afxdialogex.h"


// VolumesView dialog

IMPLEMENT_DYNAMIC(VolumesView, CDialogEx)

VolumesView::VolumesView(Document& doc, CWnd* pParent /*=NULL*/)
	: CDialogEx(VolumesView::IDD, pParent)
	, View<VolumesView>(doc)
{

}

VolumesView::~VolumesView()
{
}

void VolumesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VolumesView, CDialogEx)
END_MESSAGE_MAP()

void VolumesView::OnDocumentUpdate()
{

}


// VolumesView message handlers
