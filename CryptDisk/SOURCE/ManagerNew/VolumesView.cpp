// VolumesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "VolumesView.h"
#include "afxdialogex.h"
#include "VolumesModel.h"

namespace
{
	std::wstring FormatSize(const std::string& sizeStr)
	{
		if (sizeStr.empty())
		{
			return std::wstring(L"Unknown");
		}

		UINT64 size = boost::lexical_cast<UINT64>(sizeStr);

		const UINT64 szGiga = 1024 * 1024 * 1024;
		const UINT64 szMega = 1024 * 1024;
		const UINT64 szKilo = 1024;

		if (size >= szGiga) // GB
		{
			return boost::lexical_cast<std::wstring>(size / szGiga) + L"GB";
		}
		else if (size >= szMega) // MB
		{
			return boost::lexical_cast<std::wstring>(size / szMega) + L"MB";
		}
		else if (size >= szKilo) // KB
		{
			return boost::lexical_cast<std::wstring>(size / szKilo) + L"KB";
		}
		else
		{
			return boost::lexical_cast<std::wstring>(size) +L" Bytes";
		}
	}
}

// VolumesView dialog

IMPLEMENT_DYNAMIC(VolumesView, CDialogEx)

VolumesView::VolumesView(Document& doc, CWnd* pParent /*=NULL*/)
	: CDialogEx(VolumesView::IDD, pParent)
	, View<VolumesView>(doc)
{

}

VolumesView::~VolumesView()
{
}

void VolumesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VOLUMES, m_listVolumes);
}


BEGIN_MESSAGE_MAP(VolumesView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, &VolumesView::OnBnClickedButtonMount)
	ON_BN_CLICKED(IDC_BUTTON_ENCIPHER, &VolumesView::OnBnClickedButtonEncipher)
END_MESSAGE_MAP()

void VolumesView::OnDocumentUpdate()
{
	m_listVolumes.DeleteAllItems();

	auto volumes = static_cast<VolumesModel&>(m_document).getVolumes();

	int nItem = 0;
	for (auto v : volumes)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		m_listVolumes.InsertItem(nItem, conv.from_bytes(v.deviceId).c_str());
		m_listVolumes.SetItemText(nItem, 1, FormatSize(v.capacity).c_str());
		m_listVolumes.SetItemText(nItem, 2, conv.from_bytes(v.driveLetter).c_str());
		m_listVolumes.SetItemText(nItem, 3, conv.from_bytes(v.fileSystem).c_str());
		m_listVolumes.SetItemText(nItem, 4, conv.from_bytes(v.driveType).c_str());
	}
}


// VolumesView message handlers


BOOL VolumesView::OnInitDialog()
{
	__super::OnInitDialog();

	m_listVolumes.InsertColumn(0, _T("Volume name"), 0, 400, 0);
	m_listVolumes.InsertColumn(1, _T("Size"), 0, 80, 1);
	m_listVolumes.InsertColumn(2, _T("Letter"), 0, 80, 2);
	m_listVolumes.InsertColumn(3, _T("File System"), 0, 80, 2);
	m_listVolumes.InsertColumn(4, _T("Drive Type"), 0, 80, 2);

	m_listVolumes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);

	static_cast<VolumesModel&>(m_document).Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void VolumesView::OnBnClickedButtonMount()
{
	// TODO: Add your control notification handler code here
}


void VolumesView::OnBnClickedButtonEncipher()
{
	// TODO: Add your control notification handler code here
}
