// PageCreate2.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "PageCreate2.h"
#include "afxdialogex.h"
#include "CreateWizardModel.h"
#include "KeyFilesDialog.h"
#include "PWTools.h"
#include "AppMemory.h"
#include "DialogGenPassword.h"

namespace fs = boost::filesystem;
// PageCreate2 dialog

IMPLEMENT_DYNAMIC(PageCreate2, CPropertyPage)

PageCreate2::PageCreate2(Document& doc)
	: CPropertyPage(PageCreate2::IDD)
	, View<PageCreate2>(doc)
	, m_bInitialized(false)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

PageCreate2::~PageCreate2()
{
}

void PageCreate2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ENTROPY, m_staticBits);
	DDX_Control(pDX, IDC_COMBO_CIPHER, m_comboCipher);
	DDX_Control(pDX, IDC_PROGRESS_QUALITY, m_barQuality);
}


BEGIN_MESSAGE_MAP(PageCreate2, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GEN_PASS, &PageCreate2::OnBnClickedButtonGenPass)
	ON_BN_CLICKED(IDC_BUTTON_KEYFILES, &PageCreate2::OnBnClickedButtonKeyfiles)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &PageCreate2::OnEnChangePassword)
END_MESSAGE_MAP()

void PageCreate2::OnDocumentUpdate()
{
	if(!m_bInitialized)
	{
		return;
	}

	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);

	switch(m.CipherAlgorithm())
	{
	case DISK_CIPHER::DISK_CIPHER_AES:
		m_comboCipher.SetCurSel(0);
		break;
	case DISK_CIPHER::DISK_CIPHER_TWOFISH:
		m_comboCipher.SetCurSel(1);
		break;
	}

	UpdateData(FALSE);
}

// PageCreate2 message handlers


void PageCreate2::OnBnClickedButtonGenPass()
{
	DialogGenPassword dlg(20);

	dlg.DoModal();
}


void PageCreate2::OnBnClickedButtonKeyfiles()
{
	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);

	KeyFilesDialog dlg(m.KeyFiles());

	if(dlg.DoModal() == IDOK)
	{
		m.KeyFiles(dlg.KeyFiles());
	}
}


BOOL PageCreate2::OnInitDialog()
{
	__super::OnInitDialog();

	m_comboCipher.Clear();
	m_comboCipher.InsertString(0, _T("AES-256"));
	m_comboCipher.InsertString(1, _T("Twofish-256"));
	m_comboCipher.SetCurSel(0);

	m_bInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PageCreate2::OnEnChangePassword()
{
	char	*buff;
	int		len;
	int		bits=0;

	len=GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());

	if(len)
	{
		buff = static_cast<char*>(AppMemory::instance().Alloc(len+1));

		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, buff, len+1);

		bits=PWTools::GetEntropy(buff, len);

		RtlSecureZeroMemory(buff, len+1);
		
		AppMemory::instance().Free(buff);
	}

	m_barQuality.SetPos(bits);
	m_staticBits.Format(_T("%d bits"), bits);

	UpdateData(FALSE);
}

BOOL PageCreate2::OnSetActive()
{
	CPropertySheet* pWizard= static_cast<CPropertySheet*>(GetParent());

	pWizard->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	OnDocumentUpdate();

	return __super::OnSetActive();
}

BOOL PageCreate2::OnWizardFinish()
{
	try
	{
		if(!CheckPassword())
		{
			return FALSE;
		}

		PropagateToModel();

		if(!ValidateData())
		{
			return FALSE;
		}

		CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);

		m.DoCreate();

		return __super::OnWizardFinish();
	}
	catch (std::exception& e)
	{
		AfxMessageBox(CString(e.what()), MB_ICONERROR);
		return FALSE;
	}
}

void PageCreate2::PropagateToModel()
{
	CreateWizardModel& m = static_cast<CreateWizardModel&>(m_document);

	UpdateData(TRUE);

	{
		int passwordLength1 = GetWindowTextLengthA(GetDlgItem(IDC_EDIT_PASSWORD)->GetSafeHwnd());
		char* password1 = static_cast<char*>(AppMemory::instance().Alloc(passwordLength1+1));
		GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_PASSWORD, password1, passwordLength1+1);

		m.Password(password1);

		RtlSecureZeroMemory(password1, passwordLength1+1);
		AppMemory::instance().Free(password1);
	}

	switch(m_comboCipher.GetCurSel())
	{
	case 0:
		m.CipherAlgorithm(DISK_CIPHER::DISK_CIPHER_AES);
		break;
	case 1:
		m.CipherAlgorithm(DISK_CIPHER::DISK_CIPHER_TWOFISH);
		break;
	}
}

bool PageCreate2::ValidateData()
{
	const CreateWizardModel& m = static_cast<const CreateWizardModel&>(m_document);

	if(m.Password().empty() && m.KeyFiles().empty())
	{
		AfxMessageBox(_T("Password can't be empty, unless some key files specified"), MB_ICONERROR);
		return false;
	}

	if(!std::all_of(m.KeyFiles().cbegin(), m.KeyFiles().cend(), [](const std::wstring& f){
		return fs::exists(f);
	}))
	{
		AfxMessageBox(_T("Not all specified key files exists"), MB_ICONERROR);
		return false;
	}

	if(m.Password().size() < 12)
	{
		if(AfxMessageBox(_T("Your password is very short. Recommended length is not less then 12 symbols. Do you want to continue with this password?"),
			MB_YESNO|MB_ICONWARNING) != IDYES)
		{
			return false;
		}
	}

	return true;
}

bool PageCreate2::CheckPassword()
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

LRESULT PageCreate2::OnWizardBack()
{
	if(!CheckPassword())
	{
		return -1;
	}

	PropagateToModel();

	return __super::OnWizardBack();
}
