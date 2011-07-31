#pragma once
#include "afxcmn.h"
#include "DiskFormat.h"


// DialogFormatProgress dialog

class DialogFormatProgress : public CDialogEx
{
	DECLARE_DYNAMIC(DialogFormatProgress)

public:
	DialogFormatProgress(const WCHAR* imagePath,
		INT64 imageSize, DISK_CIPHER cipherAlgorithm,
		const unsigned char* password, size_t passwordLength, bool quickFormat, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogFormatProgress();

// Dialog Data
	enum { IDD = IDD_DIALOGFORMATPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void WorkerTask();
	void WatcherTask();

private:
	boost::shared_array<char>	m_password;
	size_t						m_passwordLength;
	std::wstring				m_imagePath;
	INT64						m_imageSize;
	DISK_CIPHER					m_cipherAlgorithm;
	bool						m_bQuickFormat;
	volatile bool				m_cancel;

	HWND						m_progressHwnd;

	Concurrency::task_group					m_tasks;
	Concurrency::overwrite_buffer<double>	m_progressResult;

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
};
