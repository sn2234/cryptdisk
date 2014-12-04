#pragma once
#include "afxcmn.h"
#include "DiskFormat.h"


// DialogFormatProgress dialog

class DialogFormatProgress : public CDialogEx
{
	DECLARE_DYNAMIC(DialogFormatProgress)

public:
	DialogFormatProgress(std::function<void(std::function<bool(double)>)> processfunc, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogFormatProgress();

// Dialog Data
	enum { IDD = IDD_DIALOGFORMATPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void WorkerTask();
	void WatcherTask();

private:
	std::function<void(std::function<bool(double)>)> m_processfunc;
	volatile bool				m_cancel;

	HWND						m_progressHwnd;

	Concurrency::task_group					m_tasks;
	std::atomic<double>						m_progressResult;
	Concurrency::event						m_progressSignal;

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
};
