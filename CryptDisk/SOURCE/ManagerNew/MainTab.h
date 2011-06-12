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
	
	// The order is important: the model should always be the first
	ImagesModel	m_imagesModel;
	ImagesView	m_imagesView;

protected:
	DECLARE_MESSAGE_MAP()
};
