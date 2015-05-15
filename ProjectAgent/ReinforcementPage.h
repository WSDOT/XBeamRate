#pragma once

#include "LongitudinalRebarGrid.h"

// CReinforcementPage dialog

class CReinforcementPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CReinforcementPage)

public:
	CReinforcementPage();
	virtual ~CReinforcementPage();

// Dialog Data
	enum { IDD = IDD_REINFORCEMENT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   afx_msg void OnAddRebar();
   afx_msg void OnRemoveRebar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CLongitudinalRebarGrid m_RebarGrid;
};