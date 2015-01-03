// PageMount1v.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount1v.h"
#include "afxdialogex.h"


// PageMount1v dialog

IMPLEMENT_DYNAMIC(PageMount1v, CPropertyPage)

PageMount1v::PageMount1v()
	: CPropertyPage(PageMount1v::IDD)
	, m_editVolumeId(_T(""))
	, m_editSize(_T(""))
	, m_editDriveLetter(_T(""))
{

}

PageMount1v::~PageMount1v()
{
}

void PageMount1v::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VOLUME_ID, m_editVolumeId);
	DDX_Text(pDX, IDC_EDIT_VOLUME_SIZE, m_editSize);
	DDX_Text(pDX, IDC_EDIT_DRIVE_LETTER, m_editDriveLetter);
}


BEGIN_MESSAGE_MAP(PageMount1v, CPropertyPage)
END_MESSAGE_MAP()


// PageMount1v message handlers
