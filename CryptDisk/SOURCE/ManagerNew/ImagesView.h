#pragma once

#include "View.h"

// ImagesView dialog

class ImagesView : public CDialogEx, View<ImagesView>
{
	DECLARE_DYNAMIC(ImagesView)

public:
	ImagesView(Document& doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~ImagesView();

// Dialog Data
	enum { IDD = IDD_IMAGESVIEW };

	void OnDocumentUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
