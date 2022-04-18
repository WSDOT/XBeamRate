///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2022  Washington State Department of Transportation
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

#include <DManip\DManip.h>
#include <XBeamRateExt\PointOfInterest.h>

class CXBeamRateChildFrame;

class iCutLocation
{
public:
   // Cut locations are in the Girderline Coordinate System
   virtual Float64 GetCurrentCutLocation() = 0;
   virtual void CutAt(Float64 Xgl) = 0;
   virtual void CutAtNext() = 0;
   virtual void CutAtPrev() = 0;
   virtual void ShowCutDlg() = 0;
   virtual Float64 GetMinCutLocation() = 0;
   virtual Float64 GetMaxCutLocation() = 0;
};


// {C5245B3C-7D15-41bd-AE85-969F85F442F3}
DEFINE_GUID(IID_iSectionCutDrawStrategy, 
0xc5245b3c, 0x7d15, 0x41bd, 0xae, 0x85, 0x96, 0x9f, 0x85, 0xf4, 0x42, 0xf3);
interface iSectionCutDrawStrategy : public IUnknown
{
	STDMETHOD_(void,Init)(CXBeamRateChildFrame* pFrame,iPointDisplayObject* pDO, iCutLocation* pCutLoc) PURE;
	STDMETHOD_(void,SetColor)(COLORREF color) PURE;
   STDMETHOD_(xbrPointOfInterest,GetCutPOI)(Float64 Xp) PURE;
};


class CSectionCutDisplayImpl : public CCmdTarget
{
public:
   CSectionCutDisplayImpl();
   ~CSectionCutDisplayImpl();

   xbrPointOfInterest GetCutPOI(Float64 Xp);

   DECLARE_INTERFACE_MAP()

   BEGIN_INTERFACE_PART(Strategy,iSectionCutDrawStrategy)
	   STDMETHOD_(void,Init)(CXBeamRateChildFrame* pFrame,iPointDisplayObject* pDO, iCutLocation* pCutLoc) override;
      STDMETHOD_(void,SetColor)(COLORREF color) override;
      STDMETHOD_(xbrPointOfInterest,GetCutPOI)(Float64 Xp) override;
   END_INTERFACE_PART(Strategy)

//   BEGIN_INTERFACE_PART(Events,iSectionCutEvents)
//   END_INTERFACE_PART(Events)

   BEGIN_INTERFACE_PART(DrawPointStrategy,iDrawPointStrategy)
      STDMETHOD_(void,Draw)(iPointDisplayObject* pDO,CDC* pDC) override;
      STDMETHOD_(void,DrawDragImage)(iPointDisplayObject* pDO,CDC* pDC, iCoordinateMap* map, const CPoint& dragStart, const CPoint& dragPoint) override;
      STDMETHOD_(void,DrawHighlite)(iPointDisplayObject* pDO,CDC* pDC,BOOL bHighlite) override;
      STDMETHOD_(void,GetBoundingBox)(iPointDisplayObject* pDO, IRect2d** rect) override;
   END_INTERFACE_PART(DrawPointStrategy)

   BEGIN_INTERFACE_PART(DisplayObjectEvents,iDisplayObjectEvents)
      STDMETHOD_(void,OnChanged)(iDisplayObject* pDO) override;
      STDMETHOD_(void,OnDragMoved)(iDisplayObject* pDO,ISize2d* offset) override;
      STDMETHOD_(void,OnMoved)(iDisplayObject* pDO) override;
      STDMETHOD_(void,OnCopied)(iDisplayObject* pDO) override;
      STDMETHOD_(bool,OnLButtonDblClk)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnLButtonDown)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnRButtonDblClk)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnRButtonDown)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnRButtonUp)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnLButtonUp)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnMouseMove)(iDisplayObject* pDO,UINT nFlags,CPoint point) override;
      STDMETHOD_(bool,OnMouseWheel)(iDisplayObject* pDO,UINT nFlags,short zDelta,CPoint point) override;
      STDMETHOD_(bool,OnKeyDown)(iDisplayObject* pDO,UINT nChar, UINT nRepCnt, UINT nFlags) override;
      STDMETHOD_(bool,OnContextMenu)(iDisplayObject* pDO,CWnd* pWnd,CPoint point) override;
      STDMETHOD_(void,OnSelect)(iDisplayObject* pDO) override;
      STDMETHOD_(void,OnUnselect)(iDisplayObject* pDO) override;
   END_INTERFACE_PART(DisplayObjectEvents)
 
   // iDragData Implementation
   BEGIN_INTERFACE_PART(DragData,iDragData)
      STDMETHOD_(UINT,Format)() override;
      STDMETHOD_(BOOL,PrepareForDrag)(iDisplayObject* pDO,iDragDataSink* pSink) override;
      STDMETHOD_(void,OnDrop)(iDisplayObject* pDO,iDragDataSource* pSource) override;
   END_INTERFACE_PART(DragData)

	DECLARE_MESSAGE_MAP()

   DECLARE_DISPATCH_MAP()

    // Note from George Shepherd: ClassWizard looks for these comments:
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(AClassWithAutomation)
    //}}AFX_DISPATCH
public: 
   static UINT ms_Format;

private:
   void Draw(iPointDisplayObject* pDO,CDC* pDC,COLORREF color, IPoint2d* loc);
   void GetBoundingBox(iPointDisplayObject* pDO, Float64 Xgl, 
                       Float64* top, Float64* left, Float64* right, Float64* bottom);
   //Float64 GetGirderHeight(Float64 Xgl);

   CXBeamRateChildFrame* m_pFrame;

   COLORREF           m_Color;
   //CGirderKey         m_GirderKey;
   //IBroker*           m_pBroker;
   Float64            m_MinCutLocation;  // in Pier coordinates
   Float64            m_MaxCutLocation;  // in Pier coordinates
   iCutLocation*      m_pCutLocation;
   
   CComPtr<IPoint2d> m_CachePoint;

   void PutPosition(Float64 Xp);
};


/////////////////////////////////////////////////////////////////////////////
