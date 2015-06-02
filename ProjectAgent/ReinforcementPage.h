#pragma once

#include "LongitudinalRebarGrid.h"
#include "StirrupGrid.h"

// CReinforcementPage dialog

class CReinforcementPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CReinforcementPage)

public:
	CReinforcementPage();
	virtual ~CReinforcementPage();

// Dialog Data
	enum { IDD = IDD_REINFORCEMENT_PAGE };

   void GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade);

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
   afx_msg void OnAddLowerXBeam();
   afx_msg void OnRemoveLowerXBeam();
   afx_msg void OnAddFullDepth();
   afx_msg void OnRemoveFullDepth();
   afx_msg void OnChangeFc();
   afx_msg void OnLowerXBeamSymmetry();
   afx_msg void OnFullDepthSymmetry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CRebarMaterialComboBox m_cbRebar;
   CString m_strUserEc;

   CLongitudinalRebarGrid m_RebarGrid;

   CStirrupGrid m_LowerXBeamGrid;
   CStirrupGrid m_FullDepthGrid;

   void UpdateConcreteTypeLabel();
   void UpdateEc();
   void UpdateStirrupGrids();
public:
   virtual BOOL OnSetActive();
};
