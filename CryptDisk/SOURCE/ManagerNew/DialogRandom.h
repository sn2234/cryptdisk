#pragma once
#include "ManagerNew.h"
#include "afxcmn.h"
#include "afxdialogex.h"

// DialogRandom dialog

class DialogRandom : public CDialogEx
{
	DECLARE_DYNAMIC(DialogRandom)

public:
	DialogRandom(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogRandom();

// Dialog Data
	enum { IDD = IDD_DIALOGRANDOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	static const int		m_progressMax = 1000;
	static const UINT_PTR	m_timerId = 1;
public:
	CProgressCtrl	m_progress;

	virtual BOOL OnInitDialog();
};
