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

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateChildFrame)
	public:
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

	// Generated message map functions
	//{{AFX_MSG(CXBeamRateChildFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   //virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};
