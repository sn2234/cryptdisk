#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// DialogChangePassword dialog

class DialogChangePassword : public CDialogEx
{
	DECLARE_DYNAMIC(DialogChangePassword)

public:
	typedef std::function<void(const unsigned char*, size_t, const unsigned char*, size_t)> ProcessFunc;
	DialogChangePassword(ProcessFunc procFunc, const unsigned char* password, size_t passwordLength, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogChangePassword();

// Dialog Data
	enum { IDD = IDD_DIALOGCHANGEPASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool CheckPassword();

	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl				m_passwordStrengthBar;

	std::vector<std::wstring>	m_keyFiles;
	boost::shared_array<char>	m_password;
	size_t						m_passwordLength;

	ProcessFunc m_processFunc;
public:
	afx_msg void OnBnClickedButtonGenPass();
	afx_msg void OnBnClickedButtonKeyfiles();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditPassword();
private:
	CString m_strengthText;
};
