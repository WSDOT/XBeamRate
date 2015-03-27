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

   CBearingLayoutGrid m_Grid[2];

	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   afx_msg void OnHelp();
   afx_msg void OnAddBack();
   afx_msg void OnRemoveBack();
   afx_msg void OnAddAhead();
   afx_msg void OnRemoveAhead();
   afx_msg void OnCopyAhead();
   afx_msg void OnCopyBack();
   afx_msg void OnBearingLineCountChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void FillTransverseMeasureComboBox();
   void FillRefBearingComboBox(IndexType brgLineIdx);
   void FillRefBearingDatumComboBox(IndexType brgLineIdx);
};
