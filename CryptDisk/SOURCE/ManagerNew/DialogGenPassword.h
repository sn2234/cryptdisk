#pragma once


// DialogGenPassword dialog

class DialogGenPassword : public CDialogEx
{
	DECLARE_DYNAMIC(DialogGenPassword)

public:
	DialogGenPassword(UINT passwordLength, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogGenPassword();

	UINT PasswordLength() const { return m_passwordLength; }
	const char* Password() const { return m_password.get(); }

// Dialog Data
	enum { IDD = IDD_DIALOG_GENPASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bUseUpper;
	BOOL m_bUseLower;
	BOOL m_bUseNumbers;
	BOOL m_bUseSpecial;
	BOOL m_bUseEasyToRead;
	BOOL m_bUseCustomSet;
	CString m_customSymbols;
	UINT m_passwordLength;
	boost::shared_array<char> m_password;
public:
	afx_msg void OnBnClickedButtonGenerate();
	afx_msg void OnBnClickedCheckCustom();
};
