#pragma once

#include "ImagesView.h"
#include "ImagesModel.h"

#include "FavoritesView.h"
#include "FavoritesModel.h"

#include "VolumesView.h"
#include "VolumesModel.h"

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

	VolumesModel	m_volumesModel;
	VolumesView		m_volumesView;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
};
