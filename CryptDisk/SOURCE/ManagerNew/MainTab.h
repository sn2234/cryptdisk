#pragma once

#include "ImagesView.h"
#include "ImagesModel.h"

#include "FavoritesView.h"
#include "FavoritesModel.h"
// MainTab

class MainTab : public CTabCtrl
{
	DECLARE_DYNAMIC(MainTab)

public:
	MainTab();
	virtual ~MainTab();

	void Init();

private:
	void SetRectangle();
	
	// The order is important: the model should always be the first
	ImagesModel		m_imagesModel;
	ImagesView		m_imagesView;

	FavoritesModel	m_favoritesModel;
	FavoritesView	m_favoritesView;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
};
