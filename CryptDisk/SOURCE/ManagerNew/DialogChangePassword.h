#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// DialogChangePassword dialog

class DialogChangePassword : public CDialogEx
{
	DECLARE_DYNAMIC(DialogChangePassword)

public:
	DialogChangePassword(const std::wstring& imagePath, const unsigned char* password, size_t passwordLength, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogChangePassword();

// Dialog Data
	enum { IDD = IDD_DIALOGCHANGEPASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool CheckPassword();

	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl				m_passwordStrengthBar;

	std::wstring				m_imagePath;
	std::vector<std::wstring>	m_keyFiles;
	boost::shared_array<char>	m_password;
	size_t						m_passwordLength;
public:
	afx_msg void OnBnClickedButtonGenPass();
	afx_msg void OnBnClickedButtonKeyfiles();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditPassword();
private:
	CString m_strengthText;
};
