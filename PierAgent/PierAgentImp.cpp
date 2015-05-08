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

   pBrokerInit->RegInterface(IID_IXBRPier, this);
   pBrokerInit->RegInterface(IID_IXBRSectionProperties, this);
   pBrokerInit->RegInterface(IID_IXBRMaterial, this);
   pBrokerInit->RegInterface(IID_IXBRPointOfInterest, this);

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
IndexType CPierAgentImp::GetBearingLineCount()
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetBearingLineCount();
}

IndexType CPierAgentImp::GetBearingCount(IndexType brgLineIdx)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetBearingCount(brgLineIdx);
}

Float64 CPierAgentImp::GetBearingLocation(IndexType brgLineIdx,IndexType brgIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 leftBrgOffset    = GetLeftBearingOffset(brgLineIdx); // offset of left-most bearing from alignment
   Float64 leftColumnOffset = GetLeftColumnOffset();  // offset of left-most column from alignment
   Float64 leftColumnOverhang = pProject->GetXBeamOverhang(pgsTypes::pstLeft); // overhang from left-most column to left edge of cross beam
   Float64 leftBrgLocation  = leftColumnOverhang - leftColumnOffset + leftBrgOffset; // dist from left edge of cross beam to left most bearing

   for ( IndexType idx = 0; idx < brgIdx && 0 < brgIdx; idx++ )
   {
      Float64 spacing = pProject->GetBearingSpacing(brgLineIdx,idx);
      leftBrgLocation += spacing;
   }

   return leftBrgLocation;
}

IndexType CPierAgentImp::GetColumnCount()
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetColumnCount();
}

Float64 CPierAgentImp::GetColumnLocation(IndexType colIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 columnLocation = pProject->GetXBeamOverhang(pgsTypes::pstLeft); // overhang from left-most column to left edge of cross beam

   for ( IndexType idx = 0; idx < colIdx && 0 < colIdx; idx++ )
   {
      Float64 spacing = pProject->GetSpacing(idx);
      columnLocation += spacing;
   }

   return columnLocation;
}

Float64 CPierAgentImp::GetColumnHeight(IndexType colIdx)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 h = pProject->GetColumnHeight(colIdx);
   CColumnData::ColumnHeightMeasurementType heightType = pProject->GetColumnHeightMeasurementType();
   if ( heightType == CColumnData::chtBottomElevation )
   {
#pragma Reminder("IMPLEMENT: compute column height from elevation")
      // Also need methods to get top and bottom column elevations
      ATLASSERT(false); // not implemented yet... need to compute height based on bottom elevation
   }

   return h;
}

Float64 CPierAgentImp::GetTopColumnElevation(IndexType colIdx)
{
   Float64 Xcol = GetColumnLocation(colIdx);
   Float64 elev = GetElevation(Xcol);
   Float64 H = GetDepth(xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Xcol));
   elev -= H;
   return elev;
}

Float64 CPierAgentImp::GetBottomColumnElevation(IndexType colIdx)
{
   Float64 Ytop = GetTopColumnElevation(colIdx);
   Float64 Hcol = GetColumnHeight(colIdx);
   return Ytop - Hcol;
}

Float64 CPierAgentImp::GetMaxColumnHeight()
{
   IndexType nColumns = GetColumnCount();
   Float64 Hmax = 0;
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 h = GetColumnHeight(colIdx);
      Hmax = Max(Hmax,h);
   }

   return Hmax;
}

void CPierAgentImp::GetUpperXBeamPoints(IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBC,IPoint2d** ppBR)
{
   GET_IFACE(IXBRProject,pProject);

   // get the key dimensions
   Float64 Ydeck = pProject->GetDeckElevation();
   Float64 CPO    = pProject->GetCrownPointOffset(); // distance from Alignment to Crown Point
   Float64 Xcrown = GetCrownPointLocation();
   Float64 L = pProject->GetXBeamLength();
   Float64 SL, SR;
   pProject->GetCrownSlopes(&SL,&SR);

   Float64 H, W;
   pProject->GetDiaphragmDimensions(&H,&W);

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

void CPierAgentImp::GetLowerXBeamPoints(IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBL2,IPoint2d** ppBR2,IPoint2d** ppBR)
{
   GET_IFACE(IXBRProject,pProject);

   Float64 H1, H2, X1;
   pProject->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1);

   Float64 H3, H4, X2;
   pProject->GetXBeamDimensions(pgsTypes::pstRight,&H3,&H4,&X2);

   CComPtr<IPoint2d> uxbTL,uxbTC,uxbTR,uxbBL,uxbBC,uxbBR;
   GetUpperXBeamPoints(&uxbTL,&uxbTC,&uxbTR,&uxbBL,&uxbBC,&uxbBR);

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

void CPierAgentImp::GetUpperXBeamProfile(IShape** ppShape)
{
   CComPtr<IPoint2d> pntTL, pntTC, pntTR;
   CComPtr<IPoint2d> pntBL, pntBC, pntBR;
   GetUpperXBeamPoints(&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);

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

void CPierAgentImp::GetLowerXBeamProfile(IShape** ppShape)
{
   CComPtr<IPoint2d> pntTL,pntTC,pntTR,pntBL,pntBL2,pntBR2,pntBR;
   GetLowerXBeamPoints(&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);

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

Float64 CPierAgentImp::GetElevation(Float64 X)
{
   CComPtr<IPoint2d> pntTL, pntTC, pntTR;
   CComPtr<IPoint2d> pntBL, pntBC, pntBR;
   GetUpperXBeamPoints(&pntTL,&pntTC,&pntTR,&pntBL,&pntBC,&pntBR);

   Float64 x1,y1;
   pntTL->Location(&x1,&y1);

   Float64 x2,y2;
   pntTC->Location(&x2,&y2);

   Float64 x3,y3;
   pntTR->Location(&x3,&y3);

   X += x1;

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

//////////////////////////////////////////
// IXBRSectionProperties
Float64 CPierAgentImp::GetDepth(xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);

   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      pProject->GetColumnShape(&shapeType,&D1,&D2);
      return D1;
   }
   else
   {
      // Create a function that represents the top of the lower cross beam
      mathPwLinearFunction2dUsingPoints fnTop;

      CComPtr<IPoint2d> pntTL,pntTC,pntTR,pntBL,pntBL2,pntBR2,pntBR;
      GetLowerXBeamPoints(&pntTL,&pntTC,&pntTR,&pntBL,&pntBL2,&pntBR2,&pntBR);

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
      pProject->GetDiaphragmDimensions(&D,&W);
      H += D;

      return H;
   }
}

Float64 CPierAgentImp::GetArea(xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      pProject->GetColumnShape(&shapeType,&D1,&D2);
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
      Float64 W = pProject->GetXBeamWidth();
      Float64 H = GetDepth(stage,poi);

      return W*H;
   }
}

//////////////////////////////////////////
// IXBRMaterial
Float64 CPierAgentImp::GetXBeamDensity()
{
#pragma Reminder("UPDATE: need material model")
   return ::ConvertToSysUnits(150.,unitMeasure::PCF);
}

//////////////////////////////////////////
// IXBRointOfInterest
std::vector<xbrPointOfInterest> CPierAgentImp::GetXBeamPointsOfInterest(PoiAttributeType attrib)
{
   Validate();
   if ( attrib == 0 )
   {
      return m_XBeamPoi;
   }

   std::vector<xbrPointOfInterest> vPoi;
   BOOST_FOREACH(xbrPointOfInterest& poi,m_XBeamPoi)
   {
      if ( poi.HasAttribute(attrib) )
      {
         vPoi.push_back(poi);
      }
   }

   return vPoi;
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetColumnPointsOfInterest(ColumnIndexType colIdx)
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
void CPierAgentImp::Validate()
{
   if ( m_Pier == NULL )
   {
      m_Pier.CoCreateInstance(CLSID_TransversePierDescription);
      // Build Pier Description using raw data from the project agent
   }

   ValidatePointsOfInterest();
}

void CPierAgentImp::Invalidate()
{
   m_Pier.Release();

   m_XBeamPoi.clear();
}

void CPierAgentImp::ValidatePointsOfInterest()
{
   if ( 0 < m_XBeamPoi.size() )
   {
      return;
   }

   GET_IFACE(IXBRProject,pProject);

   PoiIDType id = 0;

   Float64 H1, H2, H3, H4, X1, X2;
   pProject->GetXBeamDimensions(pgsTypes::pstLeft, &H1,&H2,&X1);
   pProject->GetXBeamDimensions(pgsTypes::pstRight,&H3,&H4,&X2);

   Float64 L = pProject->GetXBeamLength();

   // Put a POI at every location the section changes depth
   m_XBeamPoi.push_back(xbrPointOfInterest(id++,0.0,POI_SECTCHANGE));

   if ( !IsZero(X1) )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,X1,POI_SECTCHANGE));
   }

   Float64 crownPoint = GetCrownPointLocation();
   if ( 0 < crownPoint && crownPoint < L )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,crownPoint,POI_SECTCHANGE));
   }

   if ( !IsZero(X2) )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,L-X2,POI_SECTCHANGE));
   }

   m_XBeamPoi.push_back(xbrPointOfInterest(id++,L,POI_SECTCHANGE));

   // Put POI at each side of a column so we pick up jumps in the moment and shear diagrams
   Float64 LeftOH = pProject->GetXBeamOverhang(pgsTypes::pstLeft); 
   ColumnIndexType nColumns = pProject->GetColumnCount();
   if ( 1 < nColumns )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,LeftOH,POI_COLUMN));
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,LeftOH+0.001));
      SpacingIndexType nSpaces = nColumns - 1;
      Float64 X = LeftOH;
      for ( SpacingIndexType spaceIdx = 0; spaceIdx < nSpaces; spaceIdx++ )
      {
         Float64 space = pProject->GetSpacing(spaceIdx);
         X += space;
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,X,POI_COLUMN));
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,X+0.001));
      }
   }

   // need to pick up shear jumps at points of concentrated load
   // put POI at at bearing locations
   IndexType nBrgLines = pProject->GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBrgLines; brgLineIdx++ )
   {
      IndexType nBearings = pProject->GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = GetBearingLocation(brgLineIdx,brgIdx);
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,Xbrg,POI_BRG));
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,Xbrg+0.001));
      }
   }

   // Need POI on a one-foot grid
   Float64 step = ::ConvertToSysUnits(1.0,unitMeasure::Feet);
   Float64 Xpoi = 0;
   while ( Xpoi < L/2 - step)
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,Xpoi));
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,L-Xpoi));

      Xpoi += step;
   }
   m_XBeamPoi.push_back(xbrPointOfInterest(id++,L/2));

   // put POI in left-to-right sorted order
   std::sort(m_XBeamPoi.begin(),m_XBeamPoi.end());

   // remove any duplicates
   m_XBeamPoi.erase(std::unique(m_XBeamPoi.begin(),m_XBeamPoi.end()),m_XBeamPoi.end());
}

Float64 CPierAgentImp::GetLeftBearingOffset(IndexType brgLineIdx)
{
   // returns the offset from the alignment to the left-most bearing
   // values less than zero indicate the left-most bearing is to the
   // left ofthe alignment

   GET_IFACE(IXBRProject,pProject);
   ATLASSERT(brgLineIdx < pProject->GetBearingLineCount());

   IndexType refBrgIdx;
   Float64 refBearingOffset;
   pgsTypes::OffsetMeasurementType refBearingDatum;
   pProject->GetReferenceBearing(brgLineIdx,&refBrgIdx,&refBearingOffset,&refBearingDatum);

   if ( refBearingDatum == pgsTypes::omtBridge )
   {
      // reference bearing is located from the bridge line... adjust its location
      // so that it is measured from the alignment
      Float64 blo = pProject->GetBridgeLineOffset();
      refBearingOffset += blo;
   }

   if ( 0 < refBrgIdx )
   {
      // make the reference bearing the first bearing
      for ( IndexType brgIdx = refBrgIdx-1; 0 <= brgIdx && brgIdx != INVALID_INDEX; brgIdx-- )
      {
         Float64 space = pProject->GetBearingSpacing(brgLineIdx,brgIdx);
         refBearingOffset -= space;
      }
   }

   return refBearingOffset;
}

Float64 CPierAgentImp::GetLeftColumnOffset()
{
   // returns the offset from the alignment to the left-most column
   // values less than zero indicate the left-most column is to the 
   // left of the alignment

   GET_IFACE(IXBRProject,pProject);
   ColumnIndexType refColIdx;
   Float64 refColumnOffset;
   pgsTypes::OffsetMeasurementType refColumnDatum;
   pProject->GetTransverseLocation(&refColIdx,&refColumnOffset,&refColumnDatum);

   if ( refColumnDatum == pgsTypes::omtBridge )
   {
      // reference column is located from the bridge line... adjust its location
      // so that it is measured from the alignment
      Float64 blo = pProject->GetBridgeLineOffset();
      refColumnOffset += blo;
   }

   if ( 0 < refColIdx )
   {
      // make the reference column the first column
      for ( SpacingIndexType spaceIdx = refColIdx-1; 0 <= spaceIdx && spaceIdx != INVALID_INDEX; spaceIdx-- )
      {
         Float64 space = pProject->GetSpacing(spaceIdx);
         refColumnOffset -= space;
      }
   }

   return refColumnOffset;
}

Float64 CPierAgentImp::GetCrownPointLocation()
{
   // returns the location of the crown point, measured from the left edge
   // of the cross beam

   Float64 refColumnOffset = GetLeftColumnOffset();

   // X is the distance from the left edge of the cross beam to the crown point
   GET_IFACE(IXBRProject,pProject);
   Float64 LOH = pProject->GetXBeamOverhang(pgsTypes::pstLeft);
   Float64 CPO = pProject->GetCrownPointOffset();
   Float64 skew = GetSkewAngle();

   Float64 X = LOH - refColumnOffset + CPO/cos(skew); // negative because of refColumnOffset sign convension
   return X;
}

Float64 CPierAgentImp::GetSkewAngle()
{
   GET_IFACE(IXBRProject,pProject);
   LPCTSTR lpszSkew = pProject->GetOrientation();
   CComPtr<IAngle> objSkew;
   objSkew.CoCreateInstance(CLSID_Angle);
   HRESULT hr = objSkew->FromString(CComBSTR(lpszSkew));
   ATLASSERT(SUCCEEDED(hr));

   Float64 skew;
   objSkew->get_Value(&skew);
   return skew;
}
