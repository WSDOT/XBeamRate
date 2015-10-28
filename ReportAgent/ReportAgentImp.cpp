///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
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

#include "XBeamRateReportSpecificationBuilder.h"

#include <XBeamRateExt\XBeamRateUtilities.h>

#include "XBeamRateReportBuilder.h"
#include "XBeamRateTitlePageBuilder.h"

#include "LoadRatingChapterBuilder.h"
#include "LoadRatingDetailsChapterBuilder.h"
#include "PierDescriptionDetailsChapterBuilder.h"
#include "LoadingDetailsChapterBuilder.h"
#include "MomentCapacityDetailsChapterBuilder.h"
#include "ShearCapacityDetailsChapterBuilder.h"

#include <\ARP\PGSuper\Include\IFace\Views.h>
#include <EAF\EAFUIIntegration.h>
#include <EAF\EAFReportView.h>

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
   EAF_AGENT_INIT;

   InitReportBuilders();

   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CReportAgentImp::Init2()
{
   //
   // Attach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   // Connection point for the user interface extension events
   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

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
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Unadvise( m_dwProjectCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

/////////////////////////////////////////////////////////////
void CReportAgentImp::InitReportBuilders()
{
   GET_IFACE(IReportManager,pRptMgr);

   boost::shared_ptr<CReportSpecificationBuilder> pRptSpecBuilder( new CXBeamRateReportSpecificationBuilder(m_pBroker) );

   CXBeamRateReportBuilder* pReportBuilder = new CXBeamRateReportBuilder(IsStandAlone() ? _T("Load Rating Report") : _T("Cross Beam Load Rating Report"));
   m_ReportNames.insert(pReportBuilder->GetName());
#if defined _DEBUG || defined _BETA_VERSION
   pReportBuilder->IncludeTimingChapter();
#endif
   pReportBuilder->SetReportSpecificationBuilder( pRptSpecBuilder );
   pReportBuilder->AddTitlePageBuilder(boost::shared_ptr<CTitlePageBuilder>(new CXBeamRateTitlePageBuilder(m_pBroker,pReportBuilder->GetName())));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CLoadRatingChapterBuilder()));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CLoadRatingDetailsChapterBuilder()));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CPierDescriptionDetailsChapterBuilder()));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CLoadingDetailsChapterBuilder()));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CMomentCapacityDetailsChapterBuilder()));
   pReportBuilder->AddChapterBuilder(boost::shared_ptr<CChapterBuilder>(new CShearCapacityDetailsChapterBuilder()));
   pRptMgr->AddReportBuilder(pReportBuilder);
}

/////////////////////////////////////////////////////////////////////////////////
// IXBRProjectEventSink
HRESULT CReportAgentImp::OnProjectChanged()
{
   if ( !IsStandAlone() )
   {
      // we are plugged into PGSuper/PGSplice
      // Something in our project changed
      // Look at the open report views, if any of them are displaying a cross beam load rating report
      // tell that view to update.
      GET_IFACE(IViews,pViews);
      GET_IFACE(IEAFViewRegistrar,pViewRegistrar);

      // get all the report views
      long rptViewKey = pViews->GetReportViewKey();
      std::vector<CView*> vViews = pViewRegistrar->GetRegisteredView(rptViewKey);
      BOOST_FOREACH(CView* pView,vViews)
      {
         // make sure it is the right type so we can cast it
         if ( pView->IsKindOf(RUNTIME_CLASS(CEAFReportView)) )
         {
            // Get the report spec so we can get the report name
            CEAFReportView* pReportView = (CEAFReportView*)pView;
            boost::shared_ptr<CReportSpecification> pReportSpec = pReportView->GetReportSpecification();
            
            // is it one of our reports?
            std::set<std::_tstring>::iterator found = m_ReportNames.find(pReportSpec->GetReportName());
            if ( found != m_ReportNames.end() )
            {
               // yes, it is one of our reports... update it.
               pReportView->UpdateNow(0);
               // OnUpdate is a protected method so we can't access it
            }
         }
         else
         {
            ATLASSERT(false); // did PGSuper/PGSplice change the base class of the report view?
         }
      }
   }

   return S_OK;
}
