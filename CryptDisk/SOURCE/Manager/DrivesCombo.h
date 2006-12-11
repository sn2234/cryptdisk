#pragma once


// CDrivesCombo

class CDrivesCombo : public CComboBox
{
	DECLARE_DYNAMIC(CDrivesCombo)

public:
	CDrivesCombo();
	virtual ~CDrivesCombo();

	void FillList();
	TCHAR GetCurDrive();
protected:
	DECLARE_MESSAGE_MAP()
protected:
	CArray<TCHAR>	m_letters;
};
