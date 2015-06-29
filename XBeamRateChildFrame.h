#pragma once

#include <EAF\EAFChildFrame.h>
#include "SectionCut.h"

class CXBeamRateChildFrame : public CEAFChildFrame, public iCutLocation
{
	DECLARE_DYNCREATE(CXBeamRateChildFrame)
protected:
	CXBeamRateChildFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
   PierIDType GetPierID();
   PierIndexType GetPierIndex();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateChildFrame)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
   virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
   // iCutLocation
   virtual Float64 GetCurrentCutLocation();
   virtual void CutAt(Float64 Xgl);
   virtual void CutAtNext();
   virtual void CutAtPrev();
   virtual void ShowCutDlg();
   virtual Float64 GetMinCutLocation();
   virtual Float64 GetMaxCutLocation();

   void UpdateSectionCutExtents();

// Implementation
protected:
	virtual ~CXBeamRateChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   CDialogBar m_ControlBar;

   Float64 m_CutLocation;
   Float64 m_Xmin;
   Float64 m_Xmax;

	// Generated message map functions
	//{{AFX_MSG(CXBeamRateChildFrame)
   afx_msg void OnPierChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};
