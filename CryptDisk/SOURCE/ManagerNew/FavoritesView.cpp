// FavoritesView.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "FavoritesView.h"
#include "afxdialogex.h"
#include "AppFavorites.h"
#include "DialogMountFavorite.h"


// FavoritesView dialog

IMPLEMENT_DYNAMIC(FavoritesView, CDialogEx)

FavoritesView::FavoritesView(Document& doc, CWnd* pParent /*=NULL*/)
	: CDialogEx(FavoritesView::IDD, pParent)
	, View<FavoritesView>(doc)
	, m_dialogInitialized(false)
{

}

FavoritesView::~FavoritesView()
{
}

void FavoritesView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FAVORITES, m_favoritesList);
}


BEGIN_MESSAGE_MAP(FavoritesView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, &FavoritesView::OnBnClickedButtonMount)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &FavoritesView::OnBnClickedButtonRemove)
END_MESSAGE_MAP()


// FavoritesView message handlers


BOOL FavoritesView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_favoritesList.InsertColumn(0, _T("Letter"), 0, 50, 0);
	m_favoritesList.InsertColumn(2, _T("Path"), 0, 300, 1);

	m_favoritesList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);

	m_dialogInitialized = true;

	OnDocumentUpdate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void FavoritesView::OnDocumentUpdate()
{
	if(!m_dialogInitialized)
	{
		return;
	}

	m_favoritesList.DeleteAllItems();

	int nItem = 0;
	std::for_each(AppFavorites::instance().Favorites().cbegin(), AppFavorites::instance().Favorites().cend(), [this, &nItem](const FavoriteImage& i){
		m_favoritesList.InsertItem(nItem, CString(i.DriveLetter()));
		m_favoritesList.SetItemText(nItem, 1, i.ImagePath().c_str());
		nItem++;
	});
}


void FavoritesView::OnBnClickedButtonMount()
{
	const FavoriteImage& currentImage = AppFavorites::instance().Favorites()[m_favoritesList.GetSelectionMark()];
	DialogMountFavorite dlg(currentImage, this);

	dlg.DoModal();
}


void FavoritesView::OnBnClickedButtonRemove()
{
	if(m_favoritesList.GetSelectedCount() == 0)
	{
		return;
	}

	if(AfxMessageBox(_T("Do you want to remove selected favorite image?"), MB_ICONQUESTION|MB_YESNO) != IDYES)
	{
		return;
	}

	AppFavorites::instance().Favorites().erase(AppFavorites::instance().Favorites().begin() + m_favoritesList.GetSelectionMark());
	AppFavorites::instance().UpdateViews();
}
