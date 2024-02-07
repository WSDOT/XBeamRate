///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2024  Washington State Department of Transportation
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

#include "stdafx.h"
#include "SectionCut.h"
#include <MathEx.h>
#include <PGSuperColors.h>
#include "XBeamRateChildFrame.h"
#include <IFace\Pier.h>
#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>

#include <DManip/DisplayMgr.h>
#include <DManip/TaskFactory.h>

// height of section cut above/below x-beam
static const Uint32 SSIZE = 1440 * 3/8; // (twips)

UINT CSectionCutDisplayImpl::ms_Format = ::RegisterClipboardFormat(_T("XBRSectionCutData"));

CSectionCutDisplayImpl::CSectionCutDisplayImpl():
m_pCutLocation(nullptr),
m_Color(CUT_COLOR)
{
}

CSectionCutDisplayImpl::~CSectionCutDisplayImpl()
{
}

void CSectionCutDisplayImpl::Init(CXBeamRateChildFrame* pFrame,std::shared_ptr<WBFL::DManip::iPointDisplayObject> pDO, iCutLocation* pCutLoc)
{
   m_pFrame = pFrame;
   m_pCutLocation = pCutLoc;
   m_MinCutLocation = pCutLoc->GetMinCutLocation();
   m_MaxCutLocation = pCutLoc->GetMaxCutLocation();

   Float64 Xgl = m_pCutLocation->GetCurrentCutLocation();

   WBFL::Geometry::Point2d pnt(Xgl,0.0);
   pDO->SetPosition(pnt, false, false);
}

void CSectionCutDisplayImpl::SetColor(COLORREF color)
{
   m_Color = color;
}

xbrPointOfInterest CSectionCutDisplayImpl::GetCutPOI(Float64 Xp)
{
   PierIDType pierID = m_pFrame->GetPierID();
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker, IXBRPointOfInterest, pPoi);
   return pPoi->ConvertPierCoordinateToPoi(pierID, Xp);
}

void CSectionCutDisplayImpl::Draw(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO,CDC* pDC) const
{
   COLORREF color;

   if ( pDO->IsSelected() )
   {
      auto pDL = pDO->GetDisplayList();
      auto pDispMgr = pDL->GetDisplayMgr();
      color = pDispMgr->GetSelectionLineColor();
   }
   else
   {
      color = m_Color;
   }

   auto pos = pDO->GetPosition();
   Draw(pDO,pDC,color,pos);
}

void CSectionCutDisplayImpl::DrawHighlight(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO,CDC* pDC,bool bHighlight) const
{
   Draw(pDO,pDC);
}

void CSectionCutDisplayImpl::DrawDragImage(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO,CDC* pDC, std::shared_ptr<const WBFL::DManip::iCoordinateMap> map, const POINT& dragStart, const POINT& dragPoint) const
{
   Float64 wx, wy;
   map->LPtoWP(dragPoint.x, dragPoint.y, &wx, &wy);
   m_CachePoint.Move(wx, wy);

   Draw(pDO,pDC,SELECTED_OBJECT_LINE_COLOR,m_CachePoint);
}

WBFL::Geometry::Rect2d CSectionCutDisplayImpl::GetBoundingBox(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO) const
{
   auto pnt = pDO->GetPosition();
   
   Float64 Xgl = pnt.X();;

   Float64 top, left, right, bottom;
   GetBoundingBox(pDO, Xgl, &top, &left, &right, &bottom);

   return { left,bottom,right,top };
}

void CSectionCutDisplayImpl::GetBoundingBox(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO, Float64 Xpier, 
                                            Float64* top, Float64* left, Float64* right, Float64* bottom) const
{
   auto pMap = pDO->GetDisplayList()->GetDisplayMgr()->GetCoordinateMap();

   // height of cut above/below girder
   Float64 xo,yo;
   pMap->TPtoWP(0,0,&xo,&yo);
   Float64 x2,y2;
   pMap->TPtoWP(SSIZE,SSIZE,&x2,&y2);

   Float64 width = (x2-xo)/2.0;  // width is half of height
   Float64 height = y2-yo;

   PierIDType pierID = m_pFrame->GetPierID();
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   
   GET_IFACE2(pBroker,IXBRPier,pPier);
   Float64 Xxb = pPier->ConvertPierToCrossBeamCoordinate(pierID,Xpier);
   Float64 Xcl = pPier->ConvertPierToCurbLineCoordinate(pierID,Xpier);

   Float64 Ytop = pPier->GetElevation(pierID,Xcl);

   GET_IFACE2(pBroker,IXBRSectionProperties,pSectProp);
   Float64 H = pSectProp->GetDepth(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Xxb));

   GET_IFACE2(pBroker,IXBRProject,pProject);
   if ( pProject->GetPierType(pierID) != xbrTypes::pctIntegral )
   {
      Float64 d,w;
      pProject->GetDiaphragmDimensions(pierID,&d,&w);
      H += d;
   }

   *top    = Ytop + height;
   *bottom = Ytop -(H + height);
   *left   = Xpier;
   *right  = *left + width;
}

void CSectionCutDisplayImpl::Draw(std::shared_ptr<const WBFL::DManip::iPointDisplayObject> pDO,CDC* pDC,COLORREF color,const WBFL::Geometry::Point2d& userLoc) const
{
   Float64 Xp = userLoc.X();// in Pier Coordinates
   Xp = ::ForceIntoRange(m_MinCutLocation,Xp,m_MaxCutLocation);

   Float64 wtop, wleft, wright, wbottom;
   GetBoundingBox(pDO, Xp, &wtop, &wleft, &wright, &wbottom);

   auto pMap = pDO->GetDisplayList()->GetDisplayMgr()->GetCoordinateMap();

   long ltop, lleft, lright, lbottom;
   pMap->WPtoLP(wleft, wtop, &lleft, &ltop);
   pMap->WPtoLP(wright, wbottom, &lright,  &lbottom);

   CRect rect(lleft, ltop, lright, lbottom);
   rect.NormalizeRect();
   rect.DeflateRect(1,1); // deflate slightly to prevent artifacts when dragging

   CPen   pen(PS_SOLID,2,color);
   CPen* old_pen = pDC->SelectObject(&pen);
   CBrush brush(color);
   CBrush* old_brush = pDC->SelectObject(&brush);

   LONG xs = rect.Width() /2;
   LONG dy = xs/2;
   rect.DeflateRect(0,dy);

   pDC->MoveTo(rect.right, rect.top);
   pDC->LineTo(rect.left, rect.top);
   pDC->LineTo(rect.left, rect.bottom);
   pDC->LineTo(rect.right, rect.bottom);

   // arrows
   POINT points[3];
   points[0].x = rect.right - xs;
   points[0].y = rect.top + dy;
   points[1].x = rect.right;
   points[1].y = rect.top;
   points[2].x = rect.right - xs;
   points[2].y = rect.top - dy;
   pDC->Polygon(points, 3);

   points[0].x = rect.right - xs;
   points[0].y = rect.bottom + dy;
   points[1].x = rect.right;
   points[1].y = rect.bottom;
   points[2].x = rect.right - xs;
   points[2].y = rect.bottom - dy;
   pDC->Polygon(points, 3);

   pDC->SelectObject(old_pen);
   pDC->SelectObject(old_brush);
}

void CSectionCutDisplayImpl::OnChanged(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO)
{
   auto pPointDO = std::dynamic_pointer_cast<WBFL::DManip::iPointDisplayObject>(pDO);
   ATLASSERT(pPointDO);

   if (pPointDO)
   {
      Float64 Xp = m_pCutLocation->GetCurrentCutLocation();
   
      WBFL::Geometry::Point2d pnt(Xp, 0.0);
      pPointDO->SetPosition(pnt, true, false);
   }
}

void CSectionCutDisplayImpl::OnDragMoved(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,const WBFL::Geometry::Size2d& offset)
{
   Float64 Xp =  m_pCutLocation->GetCurrentCutLocation();

   Float64 xOffset = offset.Dx();

   Xp += xOffset;

   PutPosition(Xp);
}

void CSectionCutDisplayImpl::OnMoved(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO)
{
   CHECK(false);
}

void CSectionCutDisplayImpl::OnCopied(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO)
{
   CHECK(false);
}

bool CSectionCutDisplayImpl::OnLButtonDblClk(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   m_pCutLocation->ShowCutDlg();
   return true;
}

bool CSectionCutDisplayImpl::OnLButtonDown(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   auto dispMgr = pDO->GetDisplayList()->GetDisplayMgr();

   // If control key is pressed, don't clear current selection
   // (i.e. we want multi-select)
   BOOL bMultiSelect = nFlags & MK_CONTROL ? TRUE : FALSE;

   dispMgr->SelectObject(pDO,!bMultiSelect);

   // d&d task
   auto factory = dispMgr->GetTaskFactory();
   auto task = factory->CreateLocalDragDropTask(dispMgr, point);
   dispMgr->SetTask(task);

   return true;
}

bool CSectionCutDisplayImpl::OnRButtonUp(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   return false;
}

bool CSectionCutDisplayImpl::OnLButtonUp(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   return false;
}


bool CSectionCutDisplayImpl::OnRButtonDblClk(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   return false;
}

bool CSectionCutDisplayImpl::OnRButtonDown(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   return false;
  
}

bool CSectionCutDisplayImpl::OnMouseMove(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,const POINT& point)
{
   return false;
}

bool CSectionCutDisplayImpl::OnMouseWheel(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nFlags,short zDelta,const POINT& point)
{
   if ( zDelta < 0 )
   {
      m_pCutLocation->CutAtPrev();
   }
   else
   {
      m_pCutLocation->CutAtNext();
   }
   return true;
}

bool CSectionCutDisplayImpl::OnKeyDown(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_RIGHT:
      m_pCutLocation->CutAtNext();
      break;

   case VK_LEFT:
      m_pCutLocation->CutAtPrev();
      break;

   case VK_RETURN:
      m_pCutLocation->ShowCutDlg();
      break;
   }

   return true;
}

bool CSectionCutDisplayImpl::OnContextMenu(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,CWnd* pWnd,const POINT& point)
{
   return false;
}

void CSectionCutDisplayImpl::PutPosition(Float64 Xp)
{
   Xp = ::ForceIntoRange(m_MinCutLocation,Xp,m_MaxCutLocation);
   m_pCutLocation->CutAt(Xp);
}


void CSectionCutDisplayImpl::OnSelect(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO)
{

}

void CSectionCutDisplayImpl::OnUnselect(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO)
{
}

UINT CSectionCutDisplayImpl::Format()
{
   return ms_Format;
}

bool CSectionCutDisplayImpl::PrepareForDrag(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,std::shared_ptr<WBFL::DManip::iDragDataSink> pSink)
{
   // Create a place to store the drag data for this object
   pSink->CreateFormat(ms_Format);

   CWinThread* thread = ::AfxGetThread( );
   DWORD threadid = thread->m_nThreadID;

   pSink->Write(ms_Format,&threadid,sizeof(DWORD));
   pSink->Write(ms_Format,&m_Color,sizeof(COLORREF));
   pSink->Write(ms_Format,&m_MinCutLocation,sizeof(Float64));
   pSink->Write(ms_Format,&m_MaxCutLocation,sizeof(Float64));
   pSink->Write(ms_Format,&m_pCutLocation,sizeof(iCutLocation*));

   return TRUE;
}

void CSectionCutDisplayImpl::OnDrop(std::shared_ptr<WBFL::DManip::iDisplayObject> pDO,std::shared_ptr<WBFL::DManip::iDragDataSource> pSource)
{
   // Tell the source we are about to read from our format
   pSource->PrepareFormat(ms_Format);

   CWinThread* thread = ::AfxGetThread( );
   DWORD threadid = thread->m_nThreadID;

   DWORD threadl;
   pSource->Read(ms_Format,&threadl,sizeof(DWORD));

   ATLASSERT(threadid == threadl);

   pSource->Read(ms_Format,&m_Color,sizeof(COLORREF));
   pSource->Read(ms_Format,&m_MinCutLocation,sizeof(Float64));
   pSource->Read(ms_Format,&m_MaxCutLocation,sizeof(Float64));
   pSource->Read(ms_Format,&m_pCutLocation,sizeof(iCutLocation*));
}

