// PageCreate2.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreate2.h"
#include "afxdialogex.h"


// PageCreate2 dialog

IMPLEMENT_DYNAMIC(PageCreate2, CPropertyPage)

PageCreate2::PageCreate2(Document& doc)
	: CPropertyPage(PageCreate2::IDD)
	, View<PageCreate2>(doc)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageCreate2::~PageCreate2()
{
}

void PageCreate2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PageCreate2, CPropertyPage)
END_MESSAGE_MAP()

void PageCreate2::OnDocumentUpdate()
{

}

// PageCreate2 message handlers
