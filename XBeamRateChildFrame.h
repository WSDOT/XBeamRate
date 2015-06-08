#pragma once

#include <EAF\EAFChildFrame.h>

class CXBeamRateChildFrame : public CEAFChildFrame
{
	DECLARE_DYNCREATE(CXBeamRateChildFrame)
protected:
	CXBeamRateChildFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
   PierIDType GetPierID();

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

// Implementation
protected:
	virtual ~CXBeamRateChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   CDialogBar m_ControlBar;

	// Generated message map functions
	//{{AFX_MSG(CXBeamRateChildFrame)
   afx_msg void OnPierChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   //virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};
