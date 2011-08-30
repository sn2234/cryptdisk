#pragma once
#include "afxwin.h"


// DialogBackupHeader dialog

class DialogBackupHeader : public CDialogEx
{
	DECLARE_DYNAMIC(DialogBackupHeader)

public:
	DialogBackupHeader(const std::wstring& imagePath, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogBackupHeader();

// Dialog Data
	enum { IDD = IDD_DIALOGBACKUPHEADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_imagePath;
	CString m_backupPath;
	CComboBox m_versionCombo;
	CComboBox m_algorithmCombo;

	bool m_imageOpenedSuccessfully;
	std::vector<std::wstring>	m_keyFiles;

	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonKeyFiles();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};