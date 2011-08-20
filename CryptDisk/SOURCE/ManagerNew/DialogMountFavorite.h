#pragma once
#include "Favorites.h"


// DialogMountFavorite dialog

class DialogMountFavorite : public CDialogEx
{
	DECLARE_DYNAMIC(DialogMountFavorite)

public:
	DialogMountFavorite(const FavoriteImage& favImage, CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogMountFavorite();

// Dialog Data
	enum { IDD = IDD_DIALOGMOUNTFAVORITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString						m_path;
	BOOL						m_bReadOnly;
	BOOL						m_bRemovable;
	BOOL						m_bUseMountManager;
	BOOL						m_bTime;
	CComboBox					m_driveCombo;
	std::vector<WCHAR>			m_driveLetters;

	const FavoriteImage&		m_favImage;
	std::vector<std::wstring>	m_keyFiles;

public:
	afx_msg void OnBnClickedButtonChangePassword();
	afx_msg void OnBnClickedButtonKeyFiles();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
