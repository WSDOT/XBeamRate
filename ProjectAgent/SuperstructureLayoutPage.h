#pragma once

#include "BearingLayoutGrid.h"

// CSuperstructureLayoutPage dialog

class CSuperstructureLayoutPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSuperstructureLayoutPage)

public:
	CSuperstructureLayoutPage();
	virtual ~CSuperstructureLayoutPage();

// Dialog Data
	enum { IDD = IDD_SUPERSTRUCTURE_LAYOUT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   CBearingLayoutGrid m_Grid;

	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void FillTransverseMeasureComboBox();
};
