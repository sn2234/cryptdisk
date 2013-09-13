// ImagesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "ImagesView.h"
#include "afxdialogex.h"

#include "ImagesModel.h"

#include "MountWizard.h"
#include "CreateWizard.h"
#include "DriverTools.h"

std::wstring FormatSize(UINT64 size)
{
	const UINT64 szGiga = 1024*1024*1024;
	const UINT64 szMega = 1024*1024;
	const UINT64 szKilo = 1024;

	if (size >= szGiga) // GB
	{
		return boost::lexical_cast<std::wstring>(size / szGiga) + L"GB";
	}
	else if(size >= szMega) // MB
	{
		return boost::lexical_cast<std::wstring>(size / szMega) + L"MB";
	}
	else if(size >= szKilo) // KB
	{
		return boost::lexical_cast<std::wstring>(size / szKilo) + L"KB";
	}
	else
	{
		return boost::lexical_cast<std::wstring>(size) + L" Bytes";
	}
}

// ImagesView dialog

IMPLEMENT_DYNAMIC(ImagesView, CDialogEx)

ImagesView::ImagesView(Document& doc, CWnd* pParent)
	: CDialogEx(ImagesView::IDD, pParent)
	, View<ImagesView>(doc)
{

}

ImagesView::~ImagesView()
{
}

void ImagesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGES, m_listImages);
	DDX_Control(pDX, IDC_BUTTON_MOUNT, m_buttonMount);
	DDX_Control(pDX, IDC_BUTTON_DISMOUNT_IMAGE, m_buttonDismount);
	DDX_Control(pDX, IDC_BUTTON_DISMOUNTALL, m_buttonDismountAll);
}


BEGIN_MESSAGE_MAP(ImagesView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, &ImagesView::OnBnClickedButtonMount)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &ImagesView::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_DISMOUNT_IMAGE, &ImagesView::OnBnClickedButtonDismountImage)
	ON_BN_CLICKED(IDC_BUTTON_DISMOUNTALL, &ImagesView::OnBnClickedButtonDismountall)
END_MESSAGE_MAP()

void ImagesView::OnDocumentUpdate()
{
	auto images = static_cast<ImagesModel&>(m_document).MountedImages();

	m_listImages.DeleteAllItems();

	int nItem = 0;
	std::for_each(images.cbegin(), images.cend(), [&](const MountedImageInfo& info){
		m_listImages.InsertItem(nItem, info.driveLetter.c_str());
		m_listImages.SetItemText(nItem, 1, FormatSize(info.size).c_str());
		m_listImages.SetItemText(nItem, 2, info.imageFilePath.c_str());
		m_listImages.SetItemData(nItem, info.diskId);
		nItem++;
	});
}


BOOL ImagesView::OnInitDialog()
{
	__super::OnInitDialog();

	
	m_listImages.InsertColumn(0, _T("Letter"), 0, 50, 0);
	m_listImages.InsertColumn(1, _T("Size"), 0, 80, 1);
	m_listImages.InsertColumn(2, _T("Path"), 0, 400, 2);

	m_listImages.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);

	static_cast<ImagesModel&>(m_document).Refresh();

	if(!AppDriver::instance().getDriverControl())
	{
		m_buttonMount.EnableWindow(FALSE);
		m_buttonDismount.EnableWindow(FALSE);
		m_buttonDismountAll.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ImagesView::OnBnClickedButtonMount()
{
	MountWizard dlg(_T("Mount image"), this);

	if(dlg.DoModal() != IDCANCEL)
	{
		static_cast<ImagesModel&>(m_document).Refresh();
	}
}


void ImagesView::OnBnClickedButtonCreate()
{
	CreateWizard dlg(_T("Create image"), this);

	dlg.DoModal();
}


void ImagesView::OnBnClickedButtonDismountImage()
{
	if(m_listImages.GetSelectionMark() != -1)
	{
		ULONG diskId = static_cast<ULONG>(m_listImages.GetItemData(static_cast<ULONG>(m_listImages.GetSelectionMark())));

		try
		{
			static_cast<ImagesModel&>(m_document).DismountImage(diskId, false);
		}
		catch(std::exception&)
		{
			if(MessageBox(_T("This disk may be used by another applications. Do you want to force dismount it?"),
				_T("Warning"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				static_cast<ImagesModel&>(m_document).DismountImage(diskId, true);
			}
		}
		static_cast<ImagesModel&>(m_document).Refresh();
	}
}


void ImagesView::OnBnClickedButtonDismountall()
{
	auto images = static_cast<ImagesModel&>(m_document).MountedImages();

	int nItem = 0;
	std::for_each(images.cbegin(), images.cend(), [&](const MountedImageInfo& info){
		try
		{
			static_cast<ImagesModel&>(m_document).DismountImage(info.diskId, false);
		}
		catch(std::exception&)
		{
		}
	});

	static_cast<ImagesModel&>(m_document).Refresh();
}
