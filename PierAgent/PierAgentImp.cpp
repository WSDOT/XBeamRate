///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
// Copyright © 1999-2015  Washington State Department of Transportation
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

// PierAgentImp.cpp : Implementation of CPierAgentImp
#include "stdafx.h"
#include "PierAgent.h"
#include "PierAgentImp.h"

#include <IFace\Project.h>
#include <algorithm>
#include <Math\Math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPierAgentImp
CPierAgentImp::CPierAgentImp()
{
   m_pBroker = 0;
   m_NextPoiID = 0;
}

CPierAgentImp::~CPierAgentImp()
{
}

HRESULT CPierAgentImp::FinalConstruct()
{
   return S_OK;
}

void CPierAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CPierAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CPierAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CPierAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface(IID_IXBRPier,              this);
   pBrokerInit->RegInterface(IID_IXBRSectionProperties, this);
   pBrokerInit->RegInterface(IID_IXBRMaterial,          this);
   pBrokerInit->RegInterface(IID_IXBRRebar,             this);
   pBrokerInit->RegInterface(IID_IXBRStirrups,          this);
   pBrokerInit->RegInterface(IID_IXBRPointOfInterest,   this);

   return S_OK;
};

STDMETHODIMP CPierAgentImp::Init()
{
   //EAF_AGENT_INIT;

   //
   // Attach to connection points for interfaces this agent depends on
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CPierAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CPierAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CPierAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_PierAgent;
   return S_OK;
}

STDMETHODIMP CPierAgentImp::ShutDown()
{
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint(IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the connection point

   EAF_AGENT_CLEAR_INTERFACE_CACHE;

   return S_OK;
}

//////////////////////////////////////////
// IXBRPier
IndexType CPierAgentImp::GetBearingLineCount(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetBearingLineCount(pierID);
}

IndexType CPierAgentImp::GetBearingCount(PierIDType pierID,IndexType brgLineIdx)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetBearingCount(pierID,brgLineIdx);
}

Float64 CPierAgentImp::GetBearingLocation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 leftBrgOffset    = GetLeftBearingOffset(pierID,brgLineIdx); // offset of left-most bearing from alignment
   Float64 leftColumnOffset = GetLeftColumnOffset(pierID);  // offset of left-most column from alignment
   Float64 leftColumnOverhang = pProject->GetXBeamLeftOverhang(pierID); // overhang from left-most column to left edge of cross beam
   Float64 leftBrgLocation  = leftColumnOverhang - leftColumnOffset + leftBrgOffset; // dist from left edge of cross beam to left most bearing

   for ( IndexType idx = 0; idx < brgIdx && 0 < brgIdx; idx++ )
   {
      Float64 spacing = pProject->GetBearingSpacing(pierID,brgLineIdx,idx);
      leftBrgLocation += spacing;
   }

   return leftBrgLocation;
}

IndexType CPierAgentImp::GetColumnCount(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetColumnCount(pierID);
}

Float64 CPierAgentImp::GetColumnLocation(PierIDType pierID,IndexType colIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 columnLocation = pProject->GetXBeamLeftOverhang(pierID); // overhang from left-most column to left edge of cross beam

   for ( IndexType idx = 0; idx < colIdx && 0 < colIdx; idx++ )
   {
      Float64 spacing = pProject->GetColumnSpacing(pierID);
      columnLocation += spacing;
   }

   return columnLocation;
}

Float64 CPierAgentImp::GetColumnHeight(PierIDType pierID,IndexType colIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 h = pProject->GetColumnHeight(pierID);
   CColumnData::ColumnHeightMeasurementType heightType = pProject->GetColumnHeightMeasurementType(pierID);
   if ( heightType == CColumnData::chtBottomElevation )
   {
      Float64 elevBot = h;
      Float64 elevTop = GetTopColumnElevation(pierID,colIdx);
      h = elevTop - elevBot;
   }

   return h;
}

Float64 CPierAgentImp::GetTopColumnElevation(PierIDType pierID,IndexType colIdx)
{
   Float64 Xcol = GetColumnLocation(pierID,colIdx);
   Float64 elev = GetElevation(pierID,Xcol);
   Float64 H = GetDepth(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Xcol));
   elev -= H;
   return elev;
}

Float64 CPierAgentImp::GetBottomColumnElevation(PierIDType pierID,IndexType colIdx)
{
   Float64 Ytop = GetTopColumnElevation(pierID,colIdx);
   Float64 Hcol = GetColumnHeight(pierID,colIdx);
   return Ytop - Hcol;
}

Float64 CPierAgentImp::GetMaxColumnHeight(PierIDType pierID)
{
   IndexType nColumns = GetColumnCount(pierID);
   Float64 Hmax = 0;
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 h = GetColumnHeight(pierID,colIdx);
      Hmax = Max(Hmax,h);
   }

   return Hmax;
}

void CPierAgentImp::GetUpperXBeamPoints(PierIDType pierID,IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBC,IPoint2d** ppBR)
{
   GET_IFACE(IXBRProject,pProject);

   // get the key dimensions
   Float64 Ydeck = pProject->GetDeckElevation(pierID);
   Float64 CPO    = pProject->GetCrownPointOffset(pierID); // distance from Alignment to Crown Point
   Float64 Xcrown = GetCrownPointLocation(pierID);
   Float64 L = pProject->GetXBeamLength(pierID);
   Float64 SL, SR;
   pProject->GetCrownSlopes(pierID,&SL,&SR);

   Float64 H, W;
   pProject->GetDiaphragmDimensions(pierID,&H,&W);

   // Create the points
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppTL);
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppTC);
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppTR);
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppBL);
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppBC);
   ::CoCreateInstance(CLSID_Point2d,NULL,CLSCTX_ALL,IID_IPoint2d,(LPVOID*)ppBR);

   if ( 0 <= CPO )
   {
      (*ppTC)->Move(CPO,-CPO*SL+Ydeck);
   }
   else
   {
      (*ppTC)->Move(CPO,CPO*SR+Ydeck);
   }

   // work CCW around the upper cross beam
   (*ppTR)->Move(CPO + L-Xcrown,SR*(L-Xcrown)+Ydeck);   // top-right corner
   (*ppBR)->Move(CPO + L-Xcrown,SR*(L-Xcrown)+Ydeck-H); // bottom-right corner
   (*ppBC)->Move(CPO,Ydeck-H);                          // bottom below crown point
   (*ppBL)->Move(CPO-Xcrown,SL*Xcrown+Ydeck-H);         // bottom-left corner
   (*ppTL)->Move(CPO-Xcrown,SL*Xcrown+Ydeck);           // top-left corner

   Float64 XL, XC, XR;
   (*ppTL)->get_X(&XL);
   (*ppTC)->get_X(&XC);
   (*ppTR)->get_X(&XR);

   // make sure crown point is on the XBeam (it could actually be off the beam.. if it is, just put it at the edge)
   if ( XC < XL )
   {
      Float64 Y;
      (*ppTL)->get_Y(&Y);
      (*ppTC)->Move(XL,Y);

      Float64 X;
      (*ppBL)->Location(&X,&Y);
      (*ppBC)->Move(X,Y);
   }
   else if ( XR < XC )
   {
      Float64 Y;
      (*ppTR)->get_Y(&Y);
      (*ppTC)->Move(XR,Y);

      Float64 X;
      (*ppBR)->Location(&X,&Y);
      (*ppBC)->Move(X,Y);
   }
}

void CPierAgentImp::GetLowerXBeamPoints(PierIDType pierID,IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBL2,IPoint2d** ppBR2,IPoint2d** ppBR)
{
   GET_IFACE(IXBRProject,pProject);

   Float64 H1, H2, X1;
   Float64 H3, H4, X2;
   Float64 W;
   pProject->GetLowerXBeamDimensions(pierID,&H1,&H2,&H3,&H4,&X1,&X2,&W);

   CComPtr<IPoint2d> uxbTL,uxbTC,uxbTR,uxbBL,uxbBC,uxbBR;
   GetUpperXBeamPoints(pierID,&uxbTL,&uxbTC,&uxbTR,&uxbBL,&uxbBC,&uxbBR);

   uxbBL.CopyTo(ppTL);
   uxbBC.CopyTo(ppTC);
   uxbBR.CopyTo(ppTR);

   uxbBL->Clone(ppBL);
   uxbBL->Clone(ppBL2);
   uxbBR->Clone(ppBR);
   uxbBR->Clone(ppBR2);
   (*ppBL)->Offset(0,-H1);
   (*ppBL2)->Offset(X1,-(H1+H2));
   (*ppBR2)->Offset(-X2,-(H3+H4));
   (*ppBR)->Offset(0,-H3);
}

void CPierAgentImp::GetUpperXBeamProfile(PierIDType pierID,IShape** ppShape)
{
   CComPtr<IPoint2d> pntTL, pntTC, pntTR;
   CComPtr<IPoint2d> pntBL, pntBC, pntBR;
   GetUpperXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);

   CComPtr<IPolyShape> shape;
   shape.CoCreateInstance(CLSID_PolyShape);
   shape->AddPointEx(pntTL);
   shape->AddPointEx(pntTC);
   shape->AddPointEx(pntTR);
   shape->AddPointEx(pntBR);
   shape->AddPointEx(pntBC);
   shape->AddPointEx(pntBL);

   shape.QueryInterface(ppShape);
}

void CPierAgentImp::GetLowerXBeamProfile(PierIDType pierID,IShape** ppShape)
{
   CComPtr<IPoint2d> pntTL,pntTC,pntTR,pntBL,pntBL2,pntBR2,pntBR;
   GetLowerXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);

   CComPtr<IPolyShape> shape;
   shape.CoCreateInstance(CLSID_PolyShape);
   shape->AddPointEx(pntTL);
   shape->AddPointEx(pntTC);
   shape->AddPointEx(pntTR);
   shape->AddPointEx(pntBR);
   shape->AddPointEx(pntBR2);
   shape->AddPointEx(pntBL2);
   shape->AddPointEx(pntBL);

   shape.QueryInterface(ppShape);
}

Float64 CPierAgentImp::GetElevation(PierIDType pierID,Float64 distFromLeftEdge)
{
   CComPtr<IPoint2d> pntTL, pntTC, pntTR;
   CComPtr<IPoint2d> pntBL, pntBC, pntBR;
   GetUpperXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);

   Float64 x1,y1;
   pntTL->Location(&x1,&y1);

   Float64 x2,y2;
   pntTC->Location(&x2,&y2);

   Float64 x3,y3;
   pntTR->Location(&x3,&y3);

   Float64 X = distFromLeftEdge + x1;

   Float64 y;
   if ( ::InRange(x1,X,x2) )
   {
      y = ::LinInterp(X-x1,y1,y2,x2-x1);
   }
   else
   {
      ATLASSERT(::InRange(x2,X,x3));
      y = ::LinInterp(X-x2,y2,y3,x3-x2);
   }
   return y;
}

Float64 CPierAgentImp::GetPierCoordinate(PierIDType pierID,Float64 distFromLeftEdge)
{
   CComPtr<IPoint2d> pntTL, pntTC, pntTR;
   CComPtr<IPoint2d> pntBL, pntBC, pntBR;
   GetUpperXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);

   Float64 x1,y1;
   pntTL->Location(&x1,&y1);

   Float64 X = distFromLeftEdge + x1;
   return X;
}

//////////////////////////////////////////
// IXBRSectionProperties
Float64 CPierAgentImp::GetDepth(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;
      Float64 H;

      GET_IFACE(IXBRProject,pProject);
      pProject->GetColumnProperties(pierID,&shapeType,&D1,&D2,&columnHeightType,&H);

      return D1;
   }
   else
   {
      // Create a function that represents the top of the lower cross beam
      mathPwLinearFunction2dUsingPoints fnTop;

      CComPtr<IPoint2d> pntTL,pntTC,pntTR,pntBL,pntBL2,pntBR2,pntBR;
      GetLowerXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);

      Float64 Xoffset;
      Float64 x,y;
      pntTL->Location(&x,&y);
      Xoffset = x;
      fnTop.AddPoint(x-Xoffset,y);

      pntTC->Location(&x,&y);
      fnTop.AddPoint(x-Xoffset,y);

      pntTR->Location(&x,&y);
      fnTop.AddPoint(x-Xoffset,y);

      mathPwLinearFunction2dUsingPoints fnBottom;
      pntBL->Location(&x,&y);
      fnBottom.AddPoint(x-Xoffset,y);

      pntBL2->Location(&x,&y);
      fnBottom.AddPoint(x-Xoffset,y);

      pntBR2->Location(&x,&y);
      fnBottom.AddPoint(x-Xoffset,y);

      pntBR->Location(&x,&y);
      fnBottom.AddPoint(x-Xoffset,y);

      Float64 Y1 = fnTop.Evaluate(poi.GetDistFromStart());
      Float64 Y2 = fnBottom.Evaluate(poi.GetDistFromStart());

      Float64 H = Y1 - Y2;

      if ( stage == xbrTypes::Stage1 )
      {
         return H;
      }

      Float64 D,W;
      GET_IFACE(IXBRProject,pProject);
      pProject->GetDiaphragmDimensions(pierID,&D,&W);
      H += D;

      return H;
   }
}

Float64 CPierAgentImp::GetArea(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      Float64 H;
      pProject->GetColumnProperties(pierID,&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1/4;
      }
      else
      {
         return D1*D2;
      }
   }
   else
   {
      Float64 W = pProject->GetXBeamWidth(pierID);
      Float64 H = GetDepth(pierID,stage,poi);

      return W*H;
   }
}

Float64 CPierAgentImp::GetIxx(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      Float64 H;
      pProject->GetColumnProperties(pierID,&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1*D1*D1/64;
      }
      else
      {
         return D1*D2*D2*D2/12;
      }
   }
   else
   {
      Float64 W = pProject->GetXBeamWidth(pierID);
      Float64 H = GetDepth(pierID,stage,poi);

      return W*H*H*H/12;
   }
}

Float64 CPierAgentImp::GetIyy(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      Float64 H;
      pProject->GetColumnProperties(pierID,&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1*D1*D1/64;
      }
      else
      {
         return D1*D1*D1*D2/12;
      }
   }
   else
   {
      Float64 W = pProject->GetXBeamWidth(pierID);
      Float64 H = GetDepth(pierID,stage,poi);

      return W*W*W*H/12;
   }
}

void CPierAgentImp::GetUpperXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape)
{
   Float64 Y = GetElevation(pierID,poi.GetDistFromStart());

   Float64 D,W;
   GET_IFACE(IXBRProject,pProject);
   pProject->GetDiaphragmDimensions(pierID,&D,&W);

   CComPtr<IRectangle> rect;
   rect.CoCreateInstance(CLSID_Rect);
   rect->put_Height(D);
   rect->put_Width(W);

   CComQIPtr<IXYPosition> position(rect);
   CComPtr<IPoint2d> pnt;
   position->get_LocatorPoint(lpTopCenter,&pnt);
   pnt->Move(0,Y);
   position->put_LocatorPoint(lpTopCenter,pnt);

   rect->get_Shape(ppShape);
}

void CPierAgentImp::GetLowerXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape)
{
   Float64 Y = GetElevation(pierID,poi.GetDistFromStart());

   GET_IFACE(IXBRProject,pProject);
   Float64 W = pProject->GetXBeamWidth(pierID);
   Float64 D = GetDepth(pierID,xbrTypes::Stage1,poi);

   CComPtr<IRectangle> rect;
   rect.CoCreateInstance(CLSID_Rect);
   rect->put_Height(D);
   rect->put_Width(W);

   pProject->GetDiaphragmDimensions(pierID,&D,&W);

   CComQIPtr<IXYPosition> position(rect);
   CComPtr<IPoint2d> pnt;
   position->get_LocatorPoint(lpTopCenter,&pnt);
   pnt->Move(0,Y-D);
   position->put_LocatorPoint(lpTopCenter,pnt);

   rect->get_Shape(ppShape);
}

//////////////////////////////////////////
// IXBRMaterial
Float64 CPierAgentImp::GetXBeamDensity(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   return concrete.WeightDensity;
}

Float64 CPierAgentImp::GetXBeamEc(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   Float64 Ec;
   if ( concrete.bUserEc )
   {
      Ec = concrete.Ec;
   }
   else
   {
      Ec = lrfdConcreteUtil::ModE(concrete.Fc,concrete.StrengthDensity,false);
      if ( lrfdVersionMgr::ThirdEditionWith2005Interims <= lrfdVersionMgr::GetVersion() )
      {
         Ec *= (concrete.EcK1*concrete.EcK2);
      }
   }
   return Ec;
}

Float64 CPierAgentImp::GetColumnEc(PierIDType pierID,IndexType colIdx)
{
   // right now, everything uses the same material
   return GetXBeamEc(pierID);
}


//////////////////////////////////////////
// IXBRRebar
IndexType CPierAgentImp::GetRebarRowCount(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetRebarRowCount(pierID);
}

IndexType CPierAgentImp::GetRebarCount(PierIDType pierID,IndexType rowIdx)
{
   GET_IFACE(IXBRProject,pProject);
   xbrTypes::LongitudinalRebarDatumType datum;
   Float64 cover;
   matRebar::Size barSize;
   IndexType nBars;
   Float64 spacing;
   pProject->GetRebarRow(pierID,rowIdx,&datum,&cover,&barSize,&nBars,&spacing);
   return nBars;
}

void CPierAgentImp::GetRebarProfile(PierIDType pierID,IndexType rowIdx,IPoint2dCollection** ppPoints)
{
   GET_IFACE(IXBRProject,pProject);
   xbrTypes::LongitudinalRebarDatumType datum;
   matRebar::Size barSize;
   IndexType nBars;
   Float64 cover;
   Float64 spacing;
   pProject->GetRebarRow(pierID,rowIdx,&datum,&cover,&barSize,&nBars,&spacing);

   lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();
   matRebar::Type barType = matRebar::A615;
   matRebar::Grade barGrade = matRebar::Grade60;
   const matRebar* pRebar = pRebarPool->GetRebar(barType,barGrade,barSize);
   Float64 db = pRebar->GetNominalDimension();

   Float64 offset = cover + db;

   CComPtr<IPoint2dCollection> points;
   points.CoCreateInstance(CLSID_Point2dCollection);

   if ( datum == xbrTypes::Bottom )
   {
      CComPtr<IPoint2d> pntTL, pntTC, pntTR, pntBL, pntBL2, pntBR2, pntBR;
      GetLowerXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);
      pntBL->Offset(0,offset);
      pntBL2->Offset(0,offset);
      pntBR2->Offset(0,offset);
      pntBR->Offset(0,offset);
      points->Add(pntBL);
      points->Add(pntBL2);
      points->Add(pntBR2);
      points->Add(pntBR);
   }
   else if ( datum == xbrTypes::TopLowerXBeam )
   {
      CComPtr<IPoint2d> pntTL, pntTC, pntTR, pntBL, pntBL2, pntBR2, pntBR;
      GetLowerXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);
      pntTL->Offset(0,-offset);
      pntTC->Offset(0,-offset);
      pntTR->Offset(0,-offset);
      points->Add(pntTL);
      points->Add(pntTC);
      points->Add(pntTR);
   }
   else 
   {
      CComPtr<IPoint2d> pntTL, pntTC, pntTR, pntBL, pntBC, pntBR;
      GetUpperXBeamPoints(pierID,&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);
      pntTL->Offset(0,-offset);
      pntTC->Offset(0,-offset);
      pntTR->Offset(0,-offset);
      points->Add(pntTL);
      points->Add(pntTC);
      points->Add(pntTR);
   }

   points.CopyTo(ppPoints);
}

Float64 CPierAgentImp::GetRebarRowLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx)
{
   GET_IFACE(IXBRProject,pProject);
   xbrTypes::LongitudinalRebarDatumType datum;
   matRebar::Size barSize;
   IndexType nBars;
   Float64 cover;
   Float64 spacing;
   pProject->GetRebarRow(pierID,rowIdx,&datum,&cover,&barSize,&nBars,&spacing);

   lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();
   matRebar::Type barType;
   matRebar::Grade barGrade;
   pProject->GetRebarMaterial(pierID,&barType,&barGrade);
   const matRebar* pRebar = pRebarPool->GetRebar(barType,barGrade,barSize);
   Float64 db = pRebar->GetNominalDimension();

   Float64 offset = cover + db;
   Float64 Ybar;
   if ( datum == xbrTypes::Bottom )
   {
      Float64 H = GetDepth(pierID,xbrTypes::Stage2,poi);
      Ybar = H - offset;
   }
   else if ( datum == xbrTypes::TopLowerXBeam )
   {
      Float64 H = GetDepth(pierID,xbrTypes::Stage2,poi);
      Float64 Hlower = GetDepth(pierID,xbrTypes::Stage1,poi);
      Ybar = H - Hlower + offset;
   }
   else 
   {
      Ybar = offset;
   }

   return Ybar;
}

void CPierAgentImp::GetRebarLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx,IndexType barIdx,IPoint2d** ppPoint)
{
   Float64 Ybar = GetRebarRowLocation(pierID,poi,rowIdx);

   GET_IFACE(IXBRProject,pProject);
   xbrTypes::LongitudinalRebarDatumType datum;
   matRebar::Size barSize;
   IndexType nBars;
   Float64 cover;
   Float64 spacing;
   pProject->GetRebarRow(pierID,rowIdx,&datum,&cover,&barSize,&nBars,&spacing);

   // horizontal position of bar, assuming bar row is centered on cross beam
   Float64 barSpacingWidth = spacing*(nBars - 1);
   Float64 Xbar = -barSpacingWidth/2 + barIdx*spacing;

   CComPtr<IPoint2d> pnt;
   pnt.CoCreateInstance(CLSID_Point2d);
   pnt->Move(Xbar,Ybar);
   pnt.CopyTo(ppPoint);
}

//////////////////////////////////////////
// IXBRStirrups
ZoneIndexType CPierAgentImp::GetStirrupZoneCount(PierIDType pierID,xbrTypes::Stage stage)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   return vStirrupZones.size();
}

void CPierAgentImp::GetStirrupZoneBoundary(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx,Float64* pXstart,Float64* pXend)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   *pXstart = zone.Xstart;
   *pXend   = zone.Xend;
}

Float64 CPierAgentImp::GetStirrupZoneSpacing(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.S;
}

Float64 CPierAgentImp::GetStirrupZoneReinforcement(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.Av_over_S;
}

IndexType CPierAgentImp::GetStirrupCount(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.nStirrups;
}

//////////////////////////////////////////
// IXBRointOfInterest
std::vector<xbrPointOfInterest> CPierAgentImp::GetXBeamPointsOfInterest(PierIDType pierID,PoiAttributeType attrib)
{
   std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   if ( attrib == 0 )
   {
      return vPoi;
   }

   std::vector<xbrPointOfInterest> vFilteredPoi;
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      if ( poi.HasAttribute(attrib) )
      {
         vFilteredPoi.push_back(poi);
      }
   }

   return vFilteredPoi;
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetColumnPointsOfInterest(PierIDType pierID,ColumnIndexType colIdx)
{
   ATLASSERT(false); // not really using column POI just yet
   return std::vector<xbrPointOfInterest>();
}

//////////////////////////////////////////
// IXBRProjectEventSink
HRESULT CPierAgentImp::OnProjectChanged()
{
   Invalidate();
   return S_OK;
}

//////////////////////////////////////////
void CPierAgentImp::Invalidate()
{
   m_NextPoiID = 0;
   m_XBeamPoi.clear();
   m_StirrupZones[xbrTypes::Stage1].clear();
   m_StirrupZones[xbrTypes::Stage2].clear();
}

std::vector<CPierAgentImp::StirrupZone>& CPierAgentImp::GetStirrupZones(PierIDType pierID,xbrTypes::Stage stage)
{
   std::map<PierIDType,std::vector<StirrupZone>>::iterator found(m_StirrupZones[stage].find(pierID));
   if ( found == m_StirrupZones[stage].end() )
   {
      ValidateStirrupZones(pierID,stage);
      found = m_StirrupZones[stage].find(pierID);
   }

   return found->second;
}

void CPierAgentImp::ValidateStirrupZones(PierIDType pierID,xbrTypes::Stage stage)
{
   GET_IFACE(IXBRProject,pProject);
   const xbrPierData& pierData = pProject->GetPierData(pierID);
   std::vector<StirrupZone> vStirrupZones;
   if ( stage == xbrTypes::Stage1 )
   {
      ValidateStirrupZones(pierID,pierData.GetLowerXBeamStirrups(),&vStirrupZones);
   }
   else
   {
      if ( pProject->GetPierType(pierID) == xbrTypes::pctIntegral )
      {
         // there are only full depth stirrups for integral piers
         ValidateStirrupZones(pierID,pierData.GetFullDepthStirrups(), &vStirrupZones);
      }
   }

   m_StirrupZones[stage].insert(std::make_pair(pierID,vStirrupZones));
}

void CPierAgentImp::ValidateStirrupZones(PierIDType pierID,const xbrStirrupData& stirrupData,std::vector<StirrupZone>* pvStirrupZones)
{
   // Map the input stirrup zones into actual stirrup zones
   pvStirrupZones->clear();

   GET_IFACE(IXBRProject,pProject);
   Float64 L = pProject->GetXBeamLength(pierID);

   matRebar::Type type;
   matRebar::Grade grade;
   pProject->GetRebarMaterial(pierID,&type,&grade);
   lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();

   if ( stirrupData.Symmetric )
   {
      Float64 Xstart = 0;
      std::vector<xbrStirrupData::StirrupZone>::const_iterator iter(stirrupData.Zones.begin());
      std::vector<xbrStirrupData::StirrupZone>::const_iterator end(stirrupData.Zones.end());
      for ( ; iter != end; iter++ )
      {
         const xbrStirrupData::StirrupZone& zone(*iter);
         const matRebar* pRebar = pRebarPool->GetRebar(type,grade,zone.BarSize);
         Float64 av = pRebar->GetNominalArea();
         Float64 Av = av*zone.nBars;
         
         ATLASSERT(!IsZero(zone.BarSpacing));// UI should have prevented this
         Float64 Av_over_S = Av/zone.BarSpacing;

         StirrupZone myZone;
         myZone.Av_over_S = Av_over_S;
         myZone.BarSize = zone.BarSize;
         myZone.S = zone.BarSpacing;
         myZone.nLegs = zone.nBars;

         Float64 Xend = (iter+1 == end ? Xstart + L/2 : Xstart + zone.Length);
         bool bDone = false;
         if ( L/2 < Xend )
         {
            Xend = L/2;
            bDone = true;
         }
         myZone.Xstart = Xstart;
         myZone.Xend = Xend;
         ATLASSERT(myZone.Xstart < myZone.Xend);
         myZone.Length = Xend - Xstart;

         pvStirrupZones->push_back(myZone);
         Xstart = Xend;
         if ( bDone )
         {
            break;
         }
      }

      // make sure last zone goes to centerline of xbeam
      if ( pvStirrupZones->back().Xend < L/2 )
      {
         pvStirrupZones->back().Xend = L/2;
         pvStirrupZones->back().Length = pvStirrupZones->back().Xend - pvStirrupZones->back().Xstart;
      }

      // we've gone half-way... double the size of the last zone saved
      // This is the zone that is symmetric about the centerline of the XBeam
      pvStirrupZones->back().Length *= 2;
      pvStirrupZones->back().Xend = pvStirrupZones->back().Xstart + pvStirrupZones->back().Length;

      // fill this vector with all the previously define stirrup zones. fill in reverse
      // order and skip the center zone
      std::vector<StirrupZone> myZones;
      myZones.insert(myZones.begin(),pvStirrupZones->rbegin()+1,pvStirrupZones->rend());
      // Adjust the Start/End of the zone
      BOOST_FOREACH(StirrupZone& myZone,myZones)
      {
         Float64 Xstart = myZone.Xstart;
         Float64 Xend   = myZone.Xend;

         myZone.Xstart = L - Xend;
         myZone.Xend   = L - Xstart;
         ATLASSERT(myZone.Xstart < myZone.Xend);
      }

      // put the mirror zones into the target vector
      pvStirrupZones->insert(pvStirrupZones->end(),myZones.begin(),myZones.end());
   }
   else
   {
      Float64 Xstart = 0;
      std::vector<xbrStirrupData::StirrupZone>::const_iterator iter(stirrupData.Zones.begin());
      std::vector<xbrStirrupData::StirrupZone>::const_iterator end(stirrupData.Zones.end());
      for ( ; iter != end; iter++ )
      {
         const xbrStirrupData::StirrupZone& zone(*iter);
         const matRebar* pRebar = pRebarPool->GetRebar(type,grade,zone.BarSize);
         Float64 av = pRebar->GetNominalArea();
         Float64 Av = av*zone.nBars;
         
         ATLASSERT(!IsZero(zone.BarSpacing));// UI should have prevented this
         Float64 Av_over_S = Av/zone.BarSpacing;

         StirrupZone myZone;
         myZone.Av_over_S = Av_over_S;
         myZone.BarSize = zone.BarSize;
         myZone.S = zone.BarSpacing;
         myZone.nLegs = zone.nBars;

         Float64 Xend = (iter+1 == end ? Xstart + L : Xstart + zone.Length);
         bool bDone = false;
         if ( L < Xend )
         {
            Xend = L;
            bDone = true;
         }
         myZone.Xstart = Xstart;
         myZone.Xend = Xend;
         ATLASSERT(myZone.Xstart < myZone.Xend);
         myZone.Length = Xend - Xstart;

         pvStirrupZones->push_back(myZone);
         Xstart = Xend;
         if ( bDone )
         {
            break;
         }
      }

      // make sure the last zone ends at L
      pvStirrupZones->back().Xend = L;
      pvStirrupZones->back().Length = pvStirrupZones->back().Xstart - pvStirrupZones->back().Xend;
   }

   // Compute the number of stirrups in each zone
   // This is a little inefficient because we are looping through all the zones again, but it is easier and more clear
   BOOST_FOREACH(StirrupZone& zone,*pvStirrupZones)
   {
      ATLASSERT(0 <= zone.Xstart && zone.Xstart <= L);
      ATLASSERT(0 <= zone.Xend   && zone.Xend   <= L);
      ATLASSERT(zone.Xstart < zone.Xend);

      zone.nStirrups = IndexType(zone.Length/zone.S);
   }
}

std::vector<xbrPointOfInterest>& CPierAgentImp::GetPointsOfInterest(PierIDType pierID)
{
   std::map<PierIDType,std::vector<xbrPointOfInterest>>::iterator found(m_XBeamPoi.find(pierID));
   if ( found == m_XBeamPoi.end() )
   {
      ValidatePointsOfInterest(pierID);
      found = m_XBeamPoi.find(pierID);
      ATLASSERT(found != m_XBeamPoi.end());
   }

   return found->second;
}

void CPierAgentImp::ValidatePointsOfInterest(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);

   Float64 H1, H2, X1;
   Float64 H3, H4, X2;
   Float64 W;
   pProject->GetLowerXBeamDimensions(pierID,&H1,&H2,&H3,&H4,&X1,&X2,&W);

   Float64 L = pProject->GetXBeamLength(pierID);

   std::vector<xbrPointOfInterest> vPoi;

   // Put a POI at every location the section changes depth
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,0.0,POI_SECTIONCHANGE));

   if ( !IsZero(X1) )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X1,POI_SECTIONCHANGE));
   }

   Float64 crownPoint = GetCrownPointLocation(pierID);
   if ( 0 < crownPoint && crownPoint < L )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,crownPoint,POI_SECTIONCHANGE));
   }

   if ( !IsZero(X2) )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L-X2,POI_SECTIONCHANGE));
   }

   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L,POI_SECTIONCHANGE));

   // Put POI at each side of a column so we pick up jumps in the moment and shear diagrams
   Float64 LeftOH = pProject->GetXBeamLeftOverhang(pierID); 
   ColumnIndexType nColumns = pProject->GetColumnCount(pierID);
   if ( 1 < nColumns )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH,POI_COLUMN));
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH+0.001));
      SpacingIndexType nSpaces = nColumns - 1;
      Float64 X = LeftOH;
      for ( SpacingIndexType spaceIdx = 0; spaceIdx < nSpaces; spaceIdx++ )
      {
         Float64 space = pProject->GetColumnSpacing(pierID);
         X += space;
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X,POI_COLUMN));
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X+0.001));
      }
   }

   // need to pick up shear jumps at points of concentrated load
   // put POI at at bearing locations
   IndexType nBrgLines = pProject->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBrgLines; brgLineIdx++ )
   {
      IndexType nBearings = pProject->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = GetBearingLocation(pierID,brgLineIdx,brgIdx);
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg,POI_BRG));
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg+0.001));
      }
   }

   // Need POI on a one-foot grid
   Float64 step = ::ConvertToSysUnits(1.0,unitMeasure::Feet);
   Float64 Xpoi = 0;
   while ( Xpoi < L/2 - step)
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xpoi));
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L-Xpoi));

      Xpoi += step;
   }
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L/2));

   // put POI in left-to-right sorted order
   std::sort(vPoi.begin(),vPoi.end());

   // remove any duplicates
   vPoi.erase(std::unique(vPoi.begin(),vPoi.end()),vPoi.end());

   m_XBeamPoi.insert(std::make_pair(pierID,vPoi));
}

Float64 CPierAgentImp::GetLeftBearingOffset(PierIDType pierID,IndexType brgLineIdx)
{
   // returns the offset from the alignment to the left-most bearing
   // values less than zero indicate the left-most bearing is to the
   // left ofthe alignment

   GET_IFACE(IXBRProject,pProject);
   ATLASSERT(brgLineIdx < pProject->GetBearingLineCount(pierID));

   IndexType refBrgIdx;
   Float64 refBearingOffset;
   pgsTypes::OffsetMeasurementType refBearingDatum;
   pProject->GetReferenceBearing(pierID,brgLineIdx,&refBrgIdx,&refBearingOffset,&refBearingDatum);

   if ( refBearingDatum == pgsTypes::omtBridge )
   {
      // reference bearing is located from the bridge line... adjust its location
      // so that it is measured from the alignment
      Float64 blo = pProject->GetBridgeLineOffset(pierID);
      refBearingOffset += blo;
   }

   if ( 0 < refBrgIdx )
   {
      // make the reference bearing the first bearing
      for ( IndexType brgIdx = refBrgIdx-1; 0 <= brgIdx && brgIdx != INVALID_INDEX; brgIdx-- )
      {
         Float64 space = pProject->GetBearingSpacing(pierID,brgLineIdx,brgIdx);
         refBearingOffset -= space;
      }
   }

   return refBearingOffset;
}

Float64 CPierAgentImp::GetLeftColumnOffset(PierIDType pierID)
{
   // returns the offset from the alignment to the left-most column
   // values less than zero indicate the left-most column is to the 
   // left of the alignment

   GET_IFACE(IXBRProject,pProject);
   IndexType nColumns;
   ColumnIndexType refColIdx;
   Float64 refColumnOffset;
   pgsTypes::OffsetMeasurementType refColumnDatum;
   Float64 X3, X4, S;
   pProject->GetColumnLayout(pierID,&nColumns,&refColumnDatum,&refColIdx,&refColumnOffset,&X3,&X4,&S);

   if ( refColumnDatum == pgsTypes::omtBridge )
   {
      // reference column is located from the bridge line... adjust its location
      // so that it is measured from the alignment
      Float64 blo = pProject->GetBridgeLineOffset(pierID);
      refColumnOffset += blo;
   }

   if ( 0 < refColIdx )
   {
      // make the reference column the first column
      for ( SpacingIndexType spaceIdx = refColIdx-1; 0 <= spaceIdx && spaceIdx != INVALID_INDEX; spaceIdx-- )
      {
         refColumnOffset -= S;
      }
   }

   return refColumnOffset;
}

Float64 CPierAgentImp::GetLeftEdgeLocation(PierIDType pierID)
{
   // returns the X coordinate of the left edge of the cross beam
   GET_IFACE(IXBRProject,pProject);
   Float64 CPO    = pProject->GetCrownPointOffset(pierID); // distance from Alignment to Crown Point
   Float64 Xcrown = GetCrownPointLocation(pierID);
   return CPO - Xcrown;
}

Float64 CPierAgentImp::GetCrownPointLocation(PierIDType pierID)
{
   // returns the location of the crown point, measured from the left edge
   // of the cross beam

   Float64 refColumnOffset = GetLeftColumnOffset(pierID);

   // X is the distance from the left edge of the cross beam to the crown point
   GET_IFACE(IXBRProject,pProject);
   Float64 LOH = pProject->GetXBeamLeftOverhang(pierID);
   Float64 CPO = pProject->GetCrownPointOffset(pierID);
   Float64 skew = GetSkewAngle(pierID);

   Float64 X = LOH - refColumnOffset + CPO/cos(skew); // negative because of refColumnOffset sign convension
   return X;
}

Float64 CPierAgentImp::GetSkewAngle(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   LPCTSTR lpszSkew = pProject->GetOrientation(pierID);
   CComPtr<IAngle> objSkew;
   objSkew.CoCreateInstance(CLSID_Angle);
   HRESULT hr = objSkew->FromString(CComBSTR(lpszSkew));
   ATLASSERT(SUCCEEDED(hr));

   Float64 skew;
   objSkew->get_Value(&skew);
   return skew;
}
