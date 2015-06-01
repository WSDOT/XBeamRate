
#pragma once

// ConcreteGeneralPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConcreteGeneralPage dialog

class CConcreteGeneralPage : public CPropertyPage
{
// Construction
public:
	CConcreteGeneralPage();

// Dialog Data
	//{{AFX_DATA(CConcreteGeneralPage)
	enum { IDD = IDD_CONCRETE_DETAILS };
	CStatic	m_ctrlStrengthDensityUnit;
	CStatic	m_ctrlStrengthDensityTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConcreteGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CEdit	m_ctrlEc;
	CButton m_ctrlEcCheck;
	CEdit	m_ctrlFc;
	CEdit	m_ctrlStrengthDensity;

   Float64 m_MinNWCDensity;
   Float64 m_MaxLWCDensity;
   bool m_bErrorInDDX;

   CString m_strFct;

public:
   CString m_strUserEc;

   // Implementation
protected:
   void UpdateEc();
   void ShowStrengthDensity(bool enable);

	// Generated message map functions
	//{{AFX_MSG(CConcreteGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
   afx_msg void OnUserEc();
	afx_msg void OnChangeFc();
	afx_msg void OnChangeDs();
   afx_msg void OnConcreteType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
   virtual void OnOK();
   pgsTypes::ConcreteType GetConreteType();
   bool IsDensityInRange(Float64 density,pgsTypes::ConcreteType type);
};
