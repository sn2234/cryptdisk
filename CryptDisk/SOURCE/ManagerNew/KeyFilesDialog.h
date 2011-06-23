#pragma once
#include "afxcmn.h"


// KeyFilesDialog dialog

class KeyFilesDialog : public CDialogEx
{
	DECLARE_DYNAMIC(KeyFilesDialog)

public:
	KeyFilesDialog(const std::vector<std::wstring>& keyFiles, CWnd* pParent = NULL);   // standard constructor
	virtual ~KeyFilesDialog();

	const std::vector<std::wstring>& KeyFiles() const { return m_keyFiles; }

// Dialog Data
	enum { IDD = IDD_KEYFILESDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl					m_keyFilesList;
	std::vector<std::wstring>	m_keyFiles;
public:
	afx_msg void OnBnClickedButtonAddfile();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
