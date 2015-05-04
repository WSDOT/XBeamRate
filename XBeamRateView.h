// XBeamRateView.h : interface of the CXBeamRateView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_)
#define AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DManip\DManip.h>
#include <DManipTools\DManipTools.h>

class CXBeamRateView : public CDisplayView
{
protected: // create from serialization only
	CXBeamRateView();
	DECLARE_DYNCREATE(CXBeamRateView)

// Attributes
public:
	CXBeamRateDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_VIRTUAL

   virtual void OnInitialUpdate();

// Implementation
public:
	virtual ~CXBeamRateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);
   void UpdateDisplayObjects();

   void BuildDimensionLine(iDisplayList* pDL, IPoint2d* fromPoint,IPoint2d* toPoint,Float64 dimension,iDimensionLine** ppDimLine = NULL);

	virtual void HandleLButtonDblClk(UINT nFlags, CPoint logPoint);

   IDType m_DisplayObjectID; // used to generate display object IDs

// Generated message map functions
protected:
	//{{AFX_MSG(CXBeamRateView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in XBeamRateView.cpp
inline CXBeamRateDoc* CXBeamRateView::GetDocument()
   { return (CXBeamRateDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_)
