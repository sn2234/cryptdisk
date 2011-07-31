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

DialogFormatProgress::DialogFormatProgress( const WCHAR* imagePath, INT64 imageSize,
	DISK_CIPHER cipherAlgorithm, const unsigned char* password, size_t passwordLength, bool quickFormat, CWnd* pParent /*= NULL*/ )
	: CDialogEx(DialogFormatProgress::IDD, pParent)
	, m_imagePath(imagePath)
	, m_passwordLength(passwordLength)
	, m_imageSize(imageSize)
	, m_cipherAlgorithm(cipherAlgorithm)
	, m_bQuickFormat(quickFormat)
	, m_password(static_cast<char*>(AppMemory::instance().Alloc(passwordLength + 1)),
		boost::bind(&SecureHeap::Free, boost::ref(AppMemory::instance()), _1))
	, m_cancel(false)
	, m_progressHwnd(NULL)
{
	std::copy_n(password, passwordLength, stdext::checked_array_iterator<char*>(m_password.get(), passwordLength + 1));

	m_tasks.run(std::bind(&DialogFormatProgress::WorkerTask, this));
	m_tasks.run(std::bind(&DialogFormatProgress::WatcherTask, this));
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
	CryptDiskHelpers::CreateImage(&AppRandom::instance(), m_imagePath.c_str(), m_imageSize, m_cipherAlgorithm,
		reinterpret_cast<const unsigned char*>(m_password.get()), m_passwordLength,	!m_bQuickFormat,
		[this](double x) -> bool{
			Concurrency::asend(m_progressResult, x);
			return !Concurrency::is_current_task_group_canceling();
	});

	PostMessage(WM_COMMAND, IDOK, 0);
	m_tasks.cancel();
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
