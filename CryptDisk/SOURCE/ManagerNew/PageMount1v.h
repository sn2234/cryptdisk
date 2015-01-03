#pragma once


// PageMount1v dialog

class PageMount1v : public CPropertyPage
{
	DECLARE_DYNAMIC(PageMount1v)

public:
	PageMount1v();
	virtual ~PageMount1v();

// Dialog Data
	enum { IDD = IDD_PAGEMOUNT3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString m_editVolumeId;
public:
	CString m_editSize;
	CString m_editDriveLetter;
};
