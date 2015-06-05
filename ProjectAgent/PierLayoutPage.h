
#pragma once

// PierLayoutPage.h : header file
//

#include "resource.h"
#include <PgsExt\ColumnData.h>

/////////////////////////////////////////////////////////////////////////////
// CPierLayoutPage dialog
class CPierLayoutPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPierLayoutPage)

// Construction
public:
	CPierLayoutPage();
	~CPierLayoutPage();

// Dialog Data
	//{{AFX_DATA(CPierLayoutPage)
	enum { IDD = IDD_PIER_LAYOUT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

   // Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPierLayoutPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   afx_msg void OnHelp();
   afx_msg void OnColumnShapeChanged();
   afx_msg void OnColumnCountChanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnConditionFactorTypeChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CMetaFileStatic m_LayoutPicture;
   CCacheEdit m_SpacingControl;

   void FillRefColumnComboBox();
   void FillHeightMeasureComboBox();
   void FillColumnShapeComboBox();
   void FillTransverseLocationComboBox();

   void UpdateColumnSpacingControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
