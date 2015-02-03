#pragma once


// CPierLayoutPage dialog

class CPierLayoutPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPierLayoutPage)

public:
	CPierLayoutPage();
	virtual ~CPierLayoutPage();

// Dialog Data
	enum { IDD = IDD_PIER_LAYOUT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
