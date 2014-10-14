// PageCreateVolume.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreateVolume.h"
#include "afxdialogex.h"


// PageCreateVolume dialog

IMPLEMENT_DYNAMIC(PageCreateVolume, CPropertyPage)

PageCreateVolume::PageCreateVolume(Document& doc)
	: CPropertyPage(PageCreateVolume::IDD)
	, View<PageCreateVolume>(doc)
{

}

PageCreateVolume::~PageCreateVolume()
{
}

void PageCreateVolume::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PageCreateVolume, CPropertyPage)
END_MESSAGE_MAP()


void PageCreateVolume::OnDocumentUpdate()
{

}


// PageCreateVolume message handlers
