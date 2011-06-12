#pragma once

#include "ImagesView.h"
#include "ImagesModel.h"
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

	ImagesModel	m_imagesModel;
	ImagesView	m_imagesView;

protected:
	DECLARE_MESSAGE_MAP()
};
