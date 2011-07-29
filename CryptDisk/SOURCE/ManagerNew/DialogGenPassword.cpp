// DialogGenPassword.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "DialogGenPassword.h"
#include "afxdialogex.h"
#include "PWTools.h"
#include "AppMemory.h"
#include "AppRandom.h"


// DialogGenPassword dialog

IMPLEMENT_DYNAMIC(DialogGenPassword, CDialogEx)

DialogGenPassword::DialogGenPassword(UINT passwordLength, CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogGenPassword::IDD, pParent)
	, m_bUseUpper(FALSE)
	, m_bUseLower(FALSE)
	, m_bUseNumbers(FALSE)
	, m_bUseSpecial(FALSE)
	, m_bUseEasyToRead(FALSE)
	, m_bUseCustomSet(FALSE)
	, m_customSymbols(_T(""))
	, m_passwordLength(passwordLength)
{

}

DialogGenPassword::~DialogGenPassword()
{
}

void DialogGenPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_UPPER, m_bUseUpper);
	DDX_Check(pDX, IDC_CHECK_LOWER, m_bUseLower);
	DDX_Check(pDX, IDC_CHECK_NUMBERS, m_bUseNumbers);
	DDX_Check(pDX, IDC_CHECK_SPECIAL, m_bUseSpecial);
	DDX_Check(pDX, IDC_CHECK_EASY_READ, m_bUseEasyToRead);
	DDX_Check(pDX, IDC_CHECK_CUSTOM, m_bUseCustomSet);
	DDX_Text(pDX, IDC_EDIT_CUSTOM, m_customSymbols);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_passwordLength);
}


BEGIN_MESSAGE_MAP(DialogGenPassword, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &DialogGenPassword::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_CHECK_CUSTOM, &DialogGenPassword::OnBnClickedCheckCustom)
END_MESSAGE_MAP()


// DialogGenPassword message handlers


void DialogGenPassword::OnBnClickedButtonGenerate()
{
	UpdateData(TRUE);

	DWORD dwFlags = 0;
	if(m_bUseUpper)
		dwFlags|=PWTools::USE_UPPER_ALPHA;
	if(m_bUseLower)
		dwFlags|=PWTools::USE_LOWER_ALPHA;
	if(m_bUseNumbers)
		dwFlags|=PWTools::USE_NUMERIC;
	if(m_bUseSpecial)
		dwFlags|=PWTools::USE_SYMBOLS;
	if(m_bUseEasyToRead)
		dwFlags|=PWTools::USE_EASY_TO_READ;

	boost::shared_array<char> passwordBuff(static_cast<char*>(AppMemory::instance().Alloc(m_passwordLength + 1)),
		boost::bind(&SecureHeap::Free, boost::ref(AppMemory::instance()), _1));

	std::fill_n(stdext::checked_array_iterator<char*>(passwordBuff.get(), m_passwordLength + 1), m_passwordLength + 1, 0);

	std::set<char> customSet;
	if(m_bUseCustomSet)
	{
		CStringA tmp(m_customSymbols);
		customSet.insert((const char*)tmp, (const char*)tmp + tmp.GetLength());
		dwFlags|=PWTools::USE_CUSTOM_SET;
	}

	PWTools::GenPassword(passwordBuff.get(), m_passwordLength, dwFlags, customSet, &AppRandom::instance());

	m_password = passwordBuff;

	SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, m_password.get());
}

void DialogGenPassword::OnBnClickedCheckCustom()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_EDIT_CUSTOM)->EnableWindow(m_bUseCustomSet);
}
