///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2016  Washington State Department of Transportation
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

// GraphingAgentImp.cpp : Implementation of CGraphingAgentImp
#include "stdafx.h"
#include "GraphingAgent.h"
#include "GraphingAgentImp.h"
#include <IGraphManager.h>

#include "AnalysisResultsGraphBuilder.h"
#include "LiveLoadGraphBuilder.h"

#include <IFace\XBeamRateAgent.h>
#include <IFace\Views.h>
#include <EAF\EAFUIIntegration.h>

#include <XBeamRateExt\XBeamRateUtilities.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBRGraphingAgentImp
CGraphingAgentImp::CGraphingAgentImp()
{
   m_pBroker = 0;
}

CGraphingAgentImp::~CGraphingAgentImp()
{
}

HRESULT CGraphingAgentImp::FinalConstruct()
{
   return S_OK;
}

void CGraphingAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CGraphingAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CGraphingAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CGraphingAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   //pBrokerInit->RegInterface( IID_ILoadRating,    this );

   return S_OK;
};

STDMETHODIMP CGraphingAgentImp::Init()
{
   //EAF_AGENT_INIT;

   ////
   //// Attach to connection points for interfaces this agent depends on
   ////
   //CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   //CComPtr<IConnectionPoint> pCP;
   //HRESULT hr = S_OK;

   InitGraphBuilders();

   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CGraphingAgentImp::Init2()
{
   //
   // Attach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   // Connection point for the bridge description
   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CGraphingAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CGraphingAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_GraphingAgent;
   return S_OK;
}

STDMETHODIMP CGraphingAgentImp::ShutDown()
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

HRESULT CGraphingAgentImp::InitGraphBuilders()
{
   GET_IFACE(IGraphManager,pGraphMgr);

   CXBRAnalysisResultsGraphBuilder* pAnalysisResultsGraphBuilder = new CXBRAnalysisResultsGraphBuilder;
   
   if ( IsPGSExtension() )
   {
      // XBeam Rate is acting as an extension to PGSuper/PGSplice
      // Change the default graph name so it doesn't conflict with PGSuper/PGSplice
      pAnalysisResultsGraphBuilder->SetName(_T("Cross Beam Analysis Results"));
   }

   VERIFY(pGraphMgr->AddGraphBuilder( pAnalysisResultsGraphBuilder ));


   CXBRLiveLoadGraphBuilder* pLiveLoadGraphBuilder = new CXBRLiveLoadGraphBuilder;
   
   if ( IsPGSExtension() )
   {
      // XBeam Rate is acting as an extension to PGSuper/PGSplice
      // Change the default graph name so it doesn't conflict with PGSuper/PGSplice
      pLiveLoadGraphBuilder->SetName(_T("Cross Beam Live Load Results"));
   }

   VERIFY(pGraphMgr->AddGraphBuilder( pLiveLoadGraphBuilder ));

   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IProjectEventSink
HRESULT CGraphingAgentImp::OnProjectChanged()
{
   if ( IsStandAlone() )
   {
      // not extending PGSuper/PGSplice so we don't need to update PGSuper/PGSplice's graph view
      return S_OK;
   }

   GET_IFACE(IViews,pViews);
   long graphingViewKey = pViews->GetGraphingViewKey();

   if ( graphingViewKey < 0 )
   {
      // view doesn't exist
      return S_OK;
   }

   AFX_MANAGE_STATE(AfxGetAppModuleState());
   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
   pViewRegistrar->UpdateRegisteredView(graphingViewKey,NULL,0,NULL);

   return S_OK;
}
