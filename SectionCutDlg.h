
#pragma once

#include <XBeamRateExt\PointOfInterest.h>

/////////////////////////////////////////////////////////////////////////////
// CSectionCutDlg dialog

class CSectionCutDlg : public CDialog
{
// Construction
public:
   CSectionCutDlg(PierIDType pierID,const xbrPointOfInterest& initialPoi,CWnd* pParent=NULL);

   xbrPointOfInterest GetPOI();

// Dialog Data
	//{{AFX_DATA(CSectionCutDlg)
	enum { IDD = IDD_SECTION_CUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectionCutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSectionCutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
   PierIDType m_PierID;
   xbrPointOfInterest m_InitialPOI;

   std::vector<xbrPointOfInterest> m_vPOI;

   CSliderCtrl m_Slider;
   CStatic m_Label;
   int m_SliderPos;

   void UpdateSliderLabel();
   void UpdatePOI();
};
