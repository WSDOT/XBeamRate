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

   pBrokerInit->RegInterface(IID_IPointOfInterest, this);

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

   hr = pBrokerInit->FindConnectionPoint( IID_IProjectEventSink, &pCP );
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

   hr = pBrokerInit->FindConnectionPoint(IID_IProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the connection point

   EAF_AGENT_CLEAR_INTERFACE_CACHE;

   return S_OK;
}

//////////////////////////////////////////
// IPointOfInterest
std::vector<xbrPointOfInterest> CPierAgentImp::GetXBeamPointsOfInterest()
{
   Validate();
   return m_XBeamPoi;
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetColumnPointsOfInterest(ColumnIndexType colIdx)
{
   ATLASSERT(false); // not really using column POI just yet
   return std::vector<xbrPointOfInterest>();
}

//////////////////////////////////////////
// IProjectEventSink
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

   GET_IFACE_(XBR,IProject,pProject);

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

#pragma Reminder("UPDATE: need a section change POI at the crown point")

   if ( !IsZero(X2) )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,L-X2,POI_SECTCHANGE));
   }

   m_XBeamPoi.push_back(xbrPointOfInterest(id++,L,POI_SECTCHANGE));

   // Need POI on a one-foot grid
   Float64 step = ::ConvertToSysUnits(1.0,unitMeasure::Feet);
   Float64 Xpoi = 0;
   while ( Xpoi < L/2 )
   {
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,Xpoi));
      m_XBeamPoi.push_back(xbrPointOfInterest(id++,L-Xpoi));

      Xpoi += step;
   }
   std::sort(m_XBeamPoi.begin(),m_XBeamPoi.end());
   m_XBeamPoi.erase(std::unique(m_XBeamPoi.begin(),m_XBeamPoi.end()),m_XBeamPoi.end());
}
