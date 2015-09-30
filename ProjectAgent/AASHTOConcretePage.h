///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

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
