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
Float64 CPierAgentImp::GetArea(const xbrPointOfInterest& poi)
{
#pragma Reminder("UPDATE: need to compute and manage section properties")
   return 10.0;
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
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,LeftOH));
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,LeftOH+0.001));
      SpacingIndexType nSpaces = nColumns - 1;
      Float64 X = LeftOH;
      for ( SpacingIndexType spaceIdx = 0; spaceIdx < nSpaces; spaceIdx++ )
      {
         Float64 space = pProject->GetSpacing(spaceIdx);
         X += space;
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,X));
         m_XBeamPoi.push_back(xbrPointOfInterest(id++,X+0.001));
      }
   }

#pragma Reminder("UPDATE: need POIs at every bearing location")
   // need to pick up shear jumps at points of concentrated load

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
   std::sort(m_XBeamPoi.begin(),m_XBeamPoi.end());
   m_XBeamPoi.erase(std::unique(m_XBeamPoi.begin(),m_XBeamPoi.end()),m_XBeamPoi.end());
}

Float64 CPierAgentImp::GetCrownPointLocation()
{
   // returns the location of the crown point, measured from the left edge
   // of the cross beam

   GET_IFACE(IXBRProject,pProject);

   // Get location of first column from the alignment
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

   // at this point we know the location of the first column, measured from the alignment
   // X is the distance from the left edge of the cross beam to the crown point
   Float64 LOH = pProject->GetXBeamOverhang(pgsTypes::pstLeft);
   Float64 CPO = pProject->GetCrownPointOffset();
   Float64 skew = GetSkewAngle();

   Float64 X = refColumnOffset - LOH + CPO/cos(skew);
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
