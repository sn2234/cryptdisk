// PageCreateVolume.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreateVolume.h"
#include "afxdialogex.h"
#include "CreateWizardModel.h"
#include "CommonTools.h"


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
	DDX_Control(pDX, IDC_LIST_VOLUMES, m_listVolumes);
}


BEGIN_MESSAGE_MAP(PageCreateVolume, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &PageCreateVolume::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


void PageCreateVolume::OnDocumentUpdate()
{
	const CreateWizardModel& model = static_cast<const CreateWizardModel&>(m_document);

	m_listVolumes.DeleteAllItems();

	int nItem = 0;

	for(const auto& v : model.Volumes())
	{
		m_listVolumes.InsertItem(nItem, CString(v.deviceId.c_str()));
		m_listVolumes.SetItemText(nItem, 1, CString(CommonTools::FormatSize(boost::lexical_cast<UINT64>(v.capacity)).c_str()));
		m_listVolumes.SetItemText(nItem, 2, CString(v.fileSystem.c_str()));
		m_listVolumes.SetItemText(nItem, 3, CString(v.driveLetter.c_str()));
		m_listVolumes.SetItemText(nItem, 4, CString(boost::lexical_cast<std::string>(v.driveType).c_str()));

		nItem++;
	}
}


// PageCreateVolume message handlers


void PageCreateVolume::OnBnClickedButtonRefresh()
{
	static_cast<CreateWizardModel&>(m_document).RefreshVolumes();
}


BOOL PageCreateVolume::OnInitDialog()
{
	__super::OnInitDialog();

	// Fill ListView header
	m_listVolumes.InsertColumn(0, _T("Device ID"), 0, -1, 0);
	m_listVolumes.InsertColumn(0, _T("Capacity"), 0, -1, 1);
	m_listVolumes.InsertColumn(0, _T("File System"), 0, -1, 2);
	m_listVolumes.InsertColumn(0, _T("Drive letter"), 0, -1, 3);
	m_listVolumes.InsertColumn(0, _T("Type"), 0, -1, 4);

	static_cast<CreateWizardModel&>(m_document).RefreshVolumes();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
