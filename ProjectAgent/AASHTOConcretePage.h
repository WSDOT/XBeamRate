#pragma once

// AASHTOConcretePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAASHTOConcretePage dialog

class CAASHTOConcretePage : public CPropertyPage
{
// Construction
public:
	CAASHTOConcretePage();

// Dialog Data
	//{{AFX_DATA(CAASHTOConcretePage)
	enum { IDD = IDD_AASHTO_CONCRETE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAASHTOConcretePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CEdit	m_ctrlK1;
	CEdit	m_ctrlK2;

   CString m_strFct;

public:
   Float64 m_EccK1;
   Float64 m_EccK2;
   Float64 m_CreepK1;
   Float64 m_CreepK2;
   Float64 m_ShrinkageK1;
   Float64 m_ShrinkageK2;
   bool m_bHasFct;
   Float64 m_Fct;

   // Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAASHTOConcretePage)
	virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
	afx_msg void OnHelp();
   afx_msg void OnAggSplittingStrengthClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
