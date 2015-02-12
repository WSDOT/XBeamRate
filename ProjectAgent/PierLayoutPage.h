#pragma once


// CPierLayoutPage dialog

class CPierLayoutPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPierLayoutPage)

public:
	CPierLayoutPage();
	virtual ~CPierLayoutPage();

   Float64 m_LeftOverhang;
   Float64 m_RightOverhang;
   IndexType m_nColumns;
   Float64 m_ColumnHeight;
   Float64 m_ColumnSpacing;

// Dialog Data
	enum { IDD = IDD_PIER_LAYOUT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
