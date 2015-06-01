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
	CEdit	m_ctrlEc;
	CButton m_ctrlEcCheck;
	CEdit	m_ctrlFc;

	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   afx_msg void OnMoreProperties();
   afx_msg void OnBnClickedEc();
   afx_msg void OnAddRebar();
   afx_msg void OnRemoveRebar();
   afx_msg void OnChangeFc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CRebarMaterialComboBox m_cbRebar;
   CLongitudinalRebarGrid m_RebarGrid;
   CString m_strUserEc;

   void UpdateConcreteTypeLabel();
   void UpdateEc();
};
