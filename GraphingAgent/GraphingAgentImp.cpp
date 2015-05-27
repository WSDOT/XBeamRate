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

// GraphingAgentImp.cpp : Implementation of CGraphingAgentImp
#include "stdafx.h"
#include "GraphingAgent.h"
#include "GraphingAgentImp.h"
#include <IGraphManager.h>

#include "GraphBuilder.h"

#include <IFace\XBeamRateAgent.h>

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

   return S_OK;
}

STDMETHODIMP CGraphingAgentImp::Init2()
{
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
   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

HRESULT CGraphingAgentImp::InitGraphBuilders()
{
   GET_IFACE(IGraphManager,pGraphMgr);

   CXBRGraphBuilder* pGraphBuilder = new CXBRGraphBuilder;
   
   CComPtr<IXBeamRateAgent> pXBR;
   HRESULT hr = m_pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pXBR);
   if ( SUCCEEDED(hr) )
   {
      // XBeam Rate is acting as an extension to PGSuper/PGSplice
      // Change the default graph name so it doesn't conflict with PGSuper/PGSplice
      pGraphBuilder->SetName(_T("Cross Beam Rating"));
   }

   VERIFY(pGraphMgr->AddGraphBuilder( pGraphBuilder ));

   return S_OK;
}