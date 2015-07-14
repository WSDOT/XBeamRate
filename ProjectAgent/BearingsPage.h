#pragma once

#include "BearingLayoutGrid.h"

// CBearingsPage dialog

class CBearingsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBearingsPage)

public:
	CBearingsPage();
	virtual ~CBearingsPage();

// Dialog Data
	enum { IDD = IDD_BEARINGS_PAGE };

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
   afx_msg void OnBackReactionTypeChanged();
   afx_msg void OnAheadReactionTypeChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void FillRefBearingComboBox(IndexType brgLineIdx,bool bInitialFill=false);
   void FillRefBearingDatumComboBox(IndexType brgLineIdx);
   void FillReactionTypeComboBox(IndexType brgLineIdx);
};
