// DialogChangePassword.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogChangePassword.h"
#include "afxdialogex.h"
#include "DialogGenPassword.h"
#include "KeyFilesDialog.h"
#include "AppMemory.h"
#include "PasswordBuilder.h"
#include "CryptDiskHelpers.h"
#include "AppRandom.h"
#include "PWTools.h"

namespace fs = boost::filesystem;
// DialogChangePassword dialog

IMPLEMENT_DYNAMIC(DialogChangePassword, CDialogEx)

DialogChangePassword::DialogChangePassword(const std::wstring& imagePath, const unsigned char* password, size_t passwordLength, CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogChangePassword::IDD, pParent)
	, m_imagePath(imagePath)
	, m_passwordLength(passwordLength)
	, m_password(AllocPasswordBuffer(passwordLength + 1))
	, m_strengthText(_T(""))
{
	if(!fs::exists(imagePath))
	{
		throw std::invalid_argument("The image file does not exists");
	}
	std::copy_n(password, passwordLength, stdext::checked_array_iterator<char*>(m_password.get(), passwordLength + 1));

	AppRandom::instance().InitRandomUI();
}

DialogChangePassword::~DialogChangePassword()
{
}

void DialogChangePassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_QUALITY, m_passwordStrengthBar);
	DDX_Text(pDX, IDC_STATIC_ENTROPY, m_strengthText);
}


BEGIN_MESSAGE_MAP(DialogChangePassword, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GEN_PASS, &DialogChangePassword::OnBnClickedButtonGenPass)
	ON_BN_CLICKED(IDC_BUTTON_KEYFILES, &DialogChangePassword::OnBnClickedButtonKeyfiles)
	ON_BN_CLICKED(IDOK, &DialogChangePassword::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &DialogChangePassword::OnEnChangeEditPassword)
END_MESSAGE_MAP()


// DialogChangePassword message handlers


void DialogChangePassword::OnBnClickedButtonGenPass()
{
	DialogGenPassword dlg(20);

	dlg.DoModal();
}


void DialogChangePassword::OnBnClickedButtonKeyfiles()
{
	KeyFilesDialog dlg(m_keyFiles);

	if(dlg.DoModal() == IDOK)
	{
		m_keyFiles = dlg.KeyFiles();
	}
}


void DialogChangePassword::OnBnClickedOk()
{
	if(!CheckPassword())
	{
		return;
	}

	if(!std::all_of(m_keyFiles.cbegin(), m_keyFiles.cend(), [](const std::wstring& f){
		return fs::exists(f);
	}))
	{
		AfxMessageBox(_T("Not all specified key files exists"), MB_ICONERROR);
		return;
	}

	int passwordLength = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());

	if((passwordLength == 0) && m_keyFiles.empty())
	{
		AfxMessageBox(_T("Password can't be empty, unless some key files specified"), MB_ICONERROR);
		return ;
	}

	if(passwordLength < 12)
	{
		if(AfxMessageBox(_T("Your password is very short. Recommended length is not less then 12 symbols. Do you want to continue with this password?"),
			MB_YESNO|MB_ICONWARNING) != IDYES)
		{
			return ;
		}
	}

	boost::shared_array<char> passwordBuff(AllocPasswordBuffer(passwordLength + 1));

	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, passwordBuff.get(), passwordLength+1);

	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(passwordBuff.get()), passwordLength);

	CryptDiskHelpers::ChangePassword(&AppRandom::instance(), m_imagePath.c_str(),
		reinterpret_cast<unsigned char*>(m_password.get()), m_passwordLength, pb.Password(), pb.PasswordLength());

	CDialogEx::OnOK();
}

bool DialogChangePassword::CheckPassword()
{
	bool passwordIsGood = false;
	int passwordLength1 = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
	int passwordLength2 = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_CONFIRM)->GetSafeHwnd());

	if(passwordLength1 == passwordLength2)
	{
		char* password1 = static_cast<char*>(AppMemory::instance().Alloc(passwordLength1+1));
		char* password2 = static_cast<char*>(AppMemory::instance().Alloc(passwordLength2+1));

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, password1, passwordLength1+1);
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_CONFIRM, password2, passwordLength2+1);

		passwordIsGood = strcmp(password1, password2) == 0;

		RtlSecureZeroMemory(password1, passwordLength1+1);
		RtlSecureZeroMemory(password2, passwordLength2+1);
		AppMemory::instance().Free(password1);
		AppMemory::instance().Free(password2);
	}

	if(!passwordIsGood)
	{
		AfxMessageBox(_T("Passwords does not match"), MB_ICONERROR);
	}

	return passwordIsGood;
}


void DialogChangePassword::OnEnChangeEditPassword()
{
	int		bits=0;

	size_t len = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());

	if(len)
	{
		boost::shared_array<char> buff = AllocPasswordBuffer(len+1);

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, buff.get(), len+1);

		bits=PWTools::GetEntropy(buff.get(), len);

		RtlSecureZeroMemory(buff.get(), len+1);
	}

	m_passwordStrengthBar.SetPos(bits);
	m_strengthText.Format(_T("%d bits"), bits);

	UpdateData(FALSE);
}
