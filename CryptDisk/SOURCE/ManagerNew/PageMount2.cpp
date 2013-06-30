// PageMount2.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageMount2.h"
#include "afxdialogex.h"
#include "MountWizardModel.h"


// PageMount2 dialog

IMPLEMENT_DYNAMIC(PageMount2, CPropertyPage)

	PageMount2::PageMount2(Document& doc)
	: CPropertyPage(PageMount2::IDD)
	, View<PageMount2>(doc)
	, m_dialogInitialized(false)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageMount2::~PageMount2()
{
}

void PageMount2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_READONLY, m_bReadOnly);
	DDX_Check(pDX, IDC_CHECK_REMOVABLE, m_bRemovable);
	DDX_Check(pDX, IDC_CHECK_MOUNTMGR, m_bUseMountManager);
	DDX_Check(pDX, IDC_CHECK_TIMESTAMP, m_bTime);
	DDX_Check(pDX, IDC_CHECK_FAVORITES, m_bAddToFav);
	DDX_Control(pDX, IDC_COMBO_DRIVE, m_driveCombo);

}


BEGIN_MESSAGE_MAP(PageMount2, CPropertyPage)
END_MESSAGE_MAP()


void PageMount2::OnDocumentUpdate()
{
	if(!m_dialogInitialized)
		return;

	const MountWizardModel& m = static_cast<const MountWizardModel&>(m_document);

	m_bReadOnly = m.MountAsReadOnly();
	m_bRemovable = m.MountAsRemovable();
	m_bUseMountManager = m.UseMountManager();
	m_bTime = m.PreserveImageTimestamp();
	m_driveLetter = m.DriveLetter();
	m_bAddToFav = m.AddToFavorites();

	if(!m_driveLetters.empty())
	{
		auto i = std::find(m_driveLetters.cbegin(), m_driveLetters.cend(), m_driveLetter);
		if(i != m_driveLetters.cend())
		{
			m_driveCombo.SetCurSel(static_cast<int>(i - m_driveLetters.cbegin()));
		}
		else
		{
			m_driveCombo.SetCurSel(0);
		}
	}

	UpdateData(FALSE);
}

// PageMount2 message handlers

BOOL PageMount2::OnInitDialog()
{
	__super::OnInitDialog();

	// Fill drives combo
	DWORD	drives;

	drives=GetLogicalDrives();

	for(int i=0;i<=L'Z'-L'A';i++)
	{
		if((drives&(1<<i)) == 0)
		{
			m_driveLetters.push_back(L'A' + i);
		}
	}

	std::for_each(m_driveLetters.cbegin(), m_driveLetters.cend(), [&] (WCHAR ch){
		m_driveCombo.AddString(CString(ch));
	});

	m_driveCombo.SetCurSel(0);

	m_dialogInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL PageMount2::OnSetActive()
{
	CPropertySheet* pWizard= static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	OnDocumentUpdate();

	return __super::OnSetActive();
}


BOOL PageMount2::OnWizardFinish()
{
	try
	{
		PropagateToModel();

		MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

		m.PerformMount();

		return __super::OnWizardFinish();
	}
	catch (std::exception& e)
	{
		AfxMessageBox(CString(e.what()), MB_ICONERROR);
		return -1;
	}
}


LRESULT PageMount2::OnWizardBack()
{
	PropagateToModel();

	return __super::OnWizardBack();
}

void PageMount2::PropagateToModel()
{
	UpdateData(TRUE);

	MountWizardModel& m = static_cast<MountWizardModel&>(m_document);

	m.MountAsReadOnly(m_bReadOnly == TRUE);
	m.MountAsRemovable(m_bRemovable == TRUE);
	m.UseMountManager(m_bUseMountManager == TRUE);
	m.PreserveImageTimestamp(m_bTime == TRUE);
	m.AddToFavorites(m_bAddToFav == TRUE);
	m.DriveLetter(m_driveLetters[m_driveCombo.GetCurSel()]);
}
