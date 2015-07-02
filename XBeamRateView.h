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
#include <XBeamRateExt\PointOfInterest.h>

class CXBeamRateDoc;
class CXBeamRateChildFrame;

#define HINT_SECTION_CUT_MOVED 1

class CXBeamRateView : public CDisplayView
{
   friend CXBeamRateChildFrame;

protected: // create from serialization only
	CXBeamRateView();
	DECLARE_DYNCREATE(CXBeamRateView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_VIRTUAL

   virtual void OnInitialUpdate();

   virtual DROPEFFECT CanDrop(COleDataObject* pDataObject,DWORD dwKeyState,IPoint2d* point);
   virtual void OnDropped(COleDataObject* pDataObject,DROPEFFECT dropEffect,IPoint2d* point);

   PierIDType GetPierID();
   PierIndexType GetPierIndex();

   xbrPointOfInterest GetCutLocation();

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
   void UpdateXBeamDisplayObjects();
   void UpdateColumnDisplayObjects();
   void UpdateBearingDisplayObjects();
   void UpdateGirderDisplayObjects();
   void UpdateRebarDisplayObjects();
   void UpdateStirrupDisplayObjects();
   void UpdateDimensionsDisplayObjects();
   void UpdateSectionCutDisplayObjects();

   void CreateLineDisplayObject(IPoint2d* pntStart,IPoint2d* pntEnd,iLineDisplayObject** ppLineDO);
   void BuildDimensionLine(iDisplayList* pDL, IPoint2d* fromPoint,IPoint2d* toPoint,iDimensionLine** ppDimLine = 0);
   void BuildDimensionLine(iDisplayList* pDL, IPoint2d* fromPoint,IPoint2d* toPoint,Float64 dimension,iDimensionLine** ppDimLine = NULL);

	virtual void HandleLButtonDblClk(UINT nFlags, CPoint logPoint);

   IDType m_DisplayObjectID; // used to generate display object IDs

   bool m_bIsIdealized;

   CXBeamRateChildFrame* m_pFrame;

// Generated message map functions
protected:
	//{{AFX_MSG(CXBeamRateView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBeamRateView_H__536DF63B_D04E_4BD4_A347_53ADDDDA2863__INCLUDED_)
