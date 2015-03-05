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

// ReportAgentImp.cpp : Implementation of CReportAgentImp
#include "stdafx.h"
#include "ReportAgent.h"
#include "ReportAgentImp.h"

#include <IReportManager.h>

#include <EAF\EAFBrokerReportSpecificationBuilder.h>

#include "XBeamRateTitlePageBuilder.h"
#include "TestChapterBuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportAgentImp
CReportAgentImp::CReportAgentImp()
{
   m_pBroker = 0;
}

CReportAgentImp::~CReportAgentImp()
{
}

HRESULT CReportAgentImp::FinalConstruct()
{
   return S_OK;
}

void CReportAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CReportAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CReportAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CReportAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   //pBrokerInit->RegInterface( IID_???,    this );
   
   // this agent doesn't implement any interfaces... it just provies reports

   return S_OK;
};

STDMETHODIMP CReportAgentImp::Init()
{
   //EAF_AGENT_INIT;

   InitReportBuilders();

   return S_OK; // AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CReportAgentImp::Init2()
{
   ////
   //// Attach to connection points
   ////
   //CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   //CComPtr<IConnectionPoint> pCP;
   //HRESULT hr = S_OK;

   //// Connection point for the bridge description
   //hr = pBrokerInit->FindConnectionPoint( IID_IProjectEventSink, &pCP );
   //ATLASSERT( SUCCEEDED(hr) );
   //hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   //ATLASSERT( SUCCEEDED(hr) );
   //pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CReportAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CReportAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_ReportAgent;
   return S_OK;
}

STDMETHODIMP CReportAgentImp::ShutDown()
{
   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

/////////////////////////////////////////////////////////////
void CReportAgentImp::InitReportBuilders()
{
   GET_IFACE(IReportManager,pRptMgr);

   boost::shared_ptr<CReportSpecificationBuilder> pRptSpecBuilder( new CEAFBrokerReportSpecificationBuilder(m_pBroker) );


   CReportBuilder* pReportBuilder = new CReportBuilder(_T("XBeam Rate Test Report"));
   pReportBuilder->SetReportSpecificationBuilder( pRptSpecBuilder );
   pReportBuilder->AddTitlePageBuilder(boost::shared_ptr<CTitlePageBuilder>(new CXBeamRateTitlePageBuilder(m_pBroker,pReportBuilder->GetName())));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CTestChapterBuilder()));
   pRptMgr->AddReportBuilder(pReportBuilder);
}