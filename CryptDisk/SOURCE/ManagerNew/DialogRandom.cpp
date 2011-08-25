// DialogRandom.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogRandom.h"



// DialogRandom dialog

IMPLEMENT_DYNAMIC(DialogRandom, CDialogEx)

DialogRandom::DialogRandom(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogRandom::IDD, pParent)
{

}

DialogRandom::~DialogRandom()
{
}

void DialogRandom::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(DialogRandom, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void DialogRandom::OnTimer( UINT_PTR nIDEvent )
{
	if(nIDEvent == m_timerId)
	{
		m_progress.OffsetPos(1);

		if(m_progress.GetPos() >= m_progressMax)
		{
			OnOK();
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

// DialogRandom message handlers


BOOL DialogRandom::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_progress.SetRange32(0, m_progressMax);
	m_progress.SetStep(1);

	SetTimer(m_timerId, 20, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
