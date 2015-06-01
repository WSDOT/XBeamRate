
#pragma once

#include "ConcreteGeneralPage.h"
#include "AASHTOConcretePage.h"

#include <XBeamRateExt\ConcreteMaterial.h>

// ConcreteDetailsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConcreteDetailsDlg dialog

class CConcreteDetailsDlg : public CPropertySheet
{
// Construction
public:
	CConcreteDetailsDlg(CWnd* pParent = NULL,UINT iSelectPage=0);

   // text strings to in in display units... Ec comes out in display units
   static CString UpdateEc(const CString& strFc,const CString& strDensity,const CString& strK1,const CString& strK2);

// Dialog Data
	//{{AFX_DATA(CConcreteDetailsDlg)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConcreteDetailsDlg)
	//}}AFX_VIRTUAL

   // Common to all property pages
   xbrConcreteMaterial Concrete;

   CConcreteGeneralPage m_General;
   CAASHTOConcretePage m_AASHTO;

   // Implementation
protected:
   void Init();

	// Generated message map functions
	//{{AFX_MSG(CConcreteDetailsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
