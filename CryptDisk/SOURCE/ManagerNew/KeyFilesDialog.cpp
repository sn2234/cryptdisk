// KeyFilesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "KeyFilesDialog.h"
#include "afxdialogex.h"


// KeyFilesDialog dialog

IMPLEMENT_DYNAMIC(KeyFilesDialog, CDialogEx)

KeyFilesDialog::KeyFilesDialog(const std::vector<std::wstring>& keyFiles, CWnd* pParent /*=NULL*/)
	: CDialogEx(KeyFilesDialog::IDD, pParent)
	, m_keyFiles(keyFiles)
{

}

KeyFilesDialog::~KeyFilesDialog()
{
}

void KeyFilesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_keyFilesList);
}


BEGIN_MESSAGE_MAP(KeyFilesDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &KeyFilesDialog::OnBnClickedButtonAddfile)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &KeyFilesDialog::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON1, &KeyFilesDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &KeyFilesDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// KeyFilesDialog message handlers


void KeyFilesDialog::OnBnClickedButtonAddfile()
{
	OPENFILENAME	ofn;
	TCHAR			filePath[MAX_PATH];

	memset(&ofn,0,sizeof(ofn));
	memset(&filePath,0,sizeof(filePath));
	ofn.lStructSize=sizeof(ofn);
	ofn.hInstance=AfxGetApp()->m_hInstance;
	ofn.hwndOwner=GetSafeHwnd();
	ofn.lpstrFilter=_T("All files\0*.*\0\0");
	ofn.lpstrFile=filePath;
	ofn.nMaxFile=sizeof(filePath);
	ofn.lpstrTitle=_T("Open keyfile");
	ofn.Flags|=OFN_DONTADDTORECENT;
	if(GetOpenFileName(&ofn))
	{
		m_keyFilesList.InsertItem(m_keyFilesList.GetItemCount(), filePath);
	}
}


void KeyFilesDialog::OnBnClickedButtonRemove()
{
	int	n=m_keyFilesList.GetSelectionMark();

	if(n != -1)
	{
		m_keyFilesList.DeleteItem(n);
	}
}

void KeyFilesDialog::OnBnClickedButton1()
{
	m_keyFilesList.DeleteAllItems();
}


BOOL KeyFilesDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDialog::OnInitDialog();

	LVCOLUMN	col;

	col.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("File path");
	col.iSubItem = 0;
	col.cx = 200;
	m_keyFilesList.InsertColumn(0, &col);

	std::for_each(m_keyFiles.cbegin(), m_keyFiles.cend(), [&](const std::wstring& x){
		m_keyFilesList.InsertItem(m_keyFilesList.GetItemCount(), x.c_str());
	});

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void KeyFilesDialog::OnBnClickedOk()
{
	m_keyFiles.clear();

	for (int i = 0; i < m_keyFilesList.GetItemCount(); i++)
	{
		m_keyFiles.push_back((LPCWSTR)m_keyFilesList.GetItemText(i, 0));
	}

	CDialogEx::OnOK();
}
