// DialogFormatProgress.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogFormatProgress.h"
#include "afxdialogex.h"
#include "AppMemory.h"
#include "CryptDiskHelpers.h"
#include "AppRandom.h"


// DialogFormatProgress dialog

IMPLEMENT_DYNAMIC(DialogFormatProgress, CDialogEx)

//, const WCHAR* imagePath, INT64 imageSize,
// DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool quickFormat,

DialogFormatProgress::DialogFormatProgress(std::function<void(std::function<bool(double)>)> processfunc, CWnd* pParent /*= NULL*/ )
	: CDialogEx(DialogFormatProgress::IDD, pParent)
	, m_processfunc(processfunc)
	, m_cancel(false)
	, m_progressHwnd(NULL)
{
	m_tasks.run(static_cast<const std::function<void(void)>>(std::bind(&DialogFormatProgress::WorkerTask, this)));
	m_tasks.run(static_cast<const std::function<void(void)>>(std::bind(&DialogFormatProgress::WatcherTask, this)));
}

DialogFormatProgress::~DialogFormatProgress()
{
	m_tasks.cancel();
	m_tasks.wait();
}

void DialogFormatProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(DialogFormatProgress, CDialogEx)
END_MESSAGE_MAP()


// DialogFormatProgress message handlers


void DialogFormatProgress::OnCancel()
{
	m_tasks.cancel();

	CDialogEx::OnCancel();
}


BOOL DialogFormatProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_progressHwnd = m_progress.GetSafeHwnd();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DialogFormatProgress::WorkerTask()
{
	try
	{
		Concurrency::asend(m_progressResult, 0.0);

		m_processfunc([this](double x) -> bool{
			Concurrency::asend(m_progressResult, x);
			return !Concurrency::is_current_task_group_canceling();
		});

		::PostMessage(GetSafeHwnd(), WM_COMMAND, IDOK, 0);
		m_tasks.cancel();
	}
	catch (...)
	{
		::PostMessage(GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
		throw;
	}
}

void DialogFormatProgress::WatcherTask()
{
	while (!Concurrency::is_current_task_group_canceling())
	{
		double p = Concurrency::receive(m_progressResult);

		if(m_progressHwnd)
		{
			::SendMessage(m_progressHwnd, PBM_SETPOS, static_cast<WPARAM>(std::floor(100 * p)), 0);
		}
	}
}
