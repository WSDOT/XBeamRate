// XBeamRateView.h : interface of the CXBeamRateView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_)
#define AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXBeamRateView : public CView
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
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXBeamRateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

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
