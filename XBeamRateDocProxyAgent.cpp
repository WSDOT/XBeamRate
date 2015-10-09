///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
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
#include "resource.h"
#include "XBeamRateDocProxyAgent.h"
#include "XBeamRateDoc.h"
#include "XBeamRateAppPlugin.h"

#include <IFace\Project.h>

#include "ReportViewChildFrame.h"
#include "XBeamRateReportView.h"

#include "GraphViewChildFrame.h"
#include "XBeamRateGraphView.h"

#include "XBeamRateChildFrame.h"
#include "XBeamRateView.h"

#include "XBeamRateHints.h"

#include "XBeamRateVersionInfoImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   CXBeamRateDocProxyAgent
****************************************************************************/

CXBeamRateDocProxyAgent::CXBeamRateDocProxyAgent()
{
   m_pMyDocument = NULL;
   m_StdToolBarID = -1;
   
   m_EventHoldCount = 0;
   m_bFiringEvents = false;
}

CXBeamRateDocProxyAgent::~CXBeamRateDocProxyAgent()
{
}

void CXBeamRateDocProxyAgent::SetDocument(CXBeamRateDoc* pDoc)
{
   m_pMyDocument = pDoc;
}

void CXBeamRateDocProxyAgent::AdviseEventSinks()
{
   //
   // Attach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRRatingSpecificationEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwRatingSpecCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   hr = pBrokerInit->FindConnectionPoint( IID_IEAFDisplayUnitsEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwDisplayUnitsCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
}

void CXBeamRateDocProxyAgent::UnadviseEventSinks()
{
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRRatingSpecificationEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwRatingSpecCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   hr = pBrokerInit->FindConnectionPoint( IID_IEAFDisplayUnitsEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwDisplayUnitsCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
}

//////////////////////////////////////////////////////////
// IAgentEx
STDMETHODIMP CXBeamRateDocProxyAgent::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CXBeamRateDocProxyAgent::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2> pBrokerInit(m_pBroker);
   pBrokerInit->RegInterface( IID_IXBeamRate,      this );
   pBrokerInit->RegInterface( IID_IXBRUIEvents,    this );
   pBrokerInit->RegInterface( IID_IXBRVersionInfo, this );
   return S_OK;
}

STDMETHODIMP CXBeamRateDocProxyAgent::Init()
{
   //EAF_AGENT_INIT;

   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CXBeamRateDocProxyAgent::Init2()
{
   AdviseEventSinks();
   return S_OK;
}

STDMETHODIMP CXBeamRateDocProxyAgent::Reset()
{
   txnTxnManager::GetInstance()->Clear();
   return S_OK;
}

STDMETHODIMP CXBeamRateDocProxyAgent::ShutDown()
{
   UnadviseEventSinks();

   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   //CLOSE_LOGFILE;

   return S_OK;
}

STDMETHODIMP CXBeamRateDocProxyAgent::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_XBeamRateDocProxyAgent;
   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IAgentUIIntegration
STDMETHODIMP CXBeamRateDocProxyAgent::IntegrateWithUI(BOOL bIntegrate)
{
   if ( bIntegrate )
   {
      RegisterViews();
      CreateToolBars();
//      CreateAcceleratorKeys();
//      CreateStatusBar();
   }
   else
   {
//      ResetStatusBar();
//      RemoveAcceleratorKeys();
      RemoveToolBars();
      UnregisterViews();
   }

   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IXBeamRate
void CXBeamRateDocProxyAgent::GetUnitServer(IUnitServer** ppUnitServer)
{
   CEAFDocTemplate* pTemplate = (CEAFDocTemplate*)(m_pMyDocument->GetDocTemplate());
   CComPtr<IEAFAppPlugin> pAppPlugin;
   pTemplate->GetPlugin(&pAppPlugin);
   CXBeamRateAppPlugin* pXBeamRate = dynamic_cast<CXBeamRateAppPlugin*>(pAppPlugin.p);

   CComPtr<IAppUnitSystem> appUnitSystem;
   pXBeamRate->GetAppUnitSystem(&appUnitSystem);

   appUnitSystem->get_UnitServer(ppUnitServer);
}

////////////////////////////////////////////////////////////////////
// IXBRProjectEventSink
HRESULT CXBeamRateDocProxyAgent::OnProjectChanged()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   m_pMyDocument->SetModifiedFlag();

   m_pMyDocument->UpdateAllViews(0,0,0);

   //boost::shared_ptr<CObject> pnull;
   //FireEvent( 0, HINT_PROJECTPROPERTIESCHANGED, pnull );
   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IXBRRatingSpecificationEventSink
HRESULT CXBeamRateDocProxyAgent::OnRatingSpecificationChanged()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   m_pMyDocument->SetModifiedFlag();

   m_pMyDocument->UpdateAllViews(0,0,0);

   //boost::shared_ptr<CObject> pnull;
   //FireEvent( 0, HINT_PROJECTPROPERTIESCHANGED, pnull );
   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IEAFDisplayUnitsEventSink
HRESULT CXBeamRateDocProxyAgent::OnUnitsChanged(eafTypes::UnitMode newUnitMode)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   m_pMyDocument->SetModifiedFlag();

   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);

   CComPtr<IDocUnitSystem> pDocUnitSystem;
   m_pMyDocument->GetDocUnitSystem(&pDocUnitSystem);
   pDocUnitSystem->put_UnitMode(UnitModeType(pDisplayUnits->GetUnitMode()));

   boost::shared_ptr<CObject> pnull;
   FireEvent( 0, HINT_UNITSCHANGED, pnull );

   return S_OK;
}

/////////////////////////////////////////////////////////////////////////
// IXBRUIEvents
void CXBeamRateDocProxyAgent::HoldEvents(bool bHold)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   if ( bHold )
   {
      m_EventHoldCount++;
   }
   else
   {
      m_EventHoldCount--;
   }

   if ( m_EventHoldCount <= 0 && !m_bFiringEvents )
   {
      m_EventHoldCount = 0;
      m_UIEvents.clear();
   }
}

void CXBeamRateDocProxyAgent::FirePendingEvents()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   if ( m_EventHoldCount == 0 )
   {
      return;
   }

   if ( 1 == m_EventHoldCount )
   {
      m_EventHoldCount--;

      m_bFiringEvents = true;

      std::vector<UIEvent>::iterator iter(m_UIEvents.begin());
      std::vector<UIEvent>::iterator iterEnd(m_UIEvents.end());
      for ( ; iter != iterEnd; iter++ )
      {
         UIEvent event = *iter;
         m_pMyDocument->UpdateAllViews(event.pSender,event.lHint,event.pHint.get());
      }

      m_bFiringEvents = false;
      m_UIEvents.clear();
   }
   else
   {
      m_EventHoldCount--;
   }
}

void CXBeamRateDocProxyAgent::CancelPendingEvents()
{
   m_EventHoldCount--;
   if ( m_EventHoldCount <= 0 && !m_bFiringEvents )
   {
      m_EventHoldCount = 0;
      m_UIEvents.clear();
   }
}

void CXBeamRateDocProxyAgent::FireEvent(CView* pSender,LPARAM lHint,boost::shared_ptr<CObject> pHint)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   if ( 0 < m_EventHoldCount )
   {
      UIEvent event;
      event.pSender = pSender;
      event.lHint = lHint;
      event.pHint = pHint;

      // skip all but one result hint - firing multiple result hints 
      // causes the UI to unnecessarilly update multiple times
      bool skip = false;
      std::vector<UIEvent>::iterator iter(m_UIEvents.begin());
      std::vector<UIEvent>::iterator iterEnd(m_UIEvents.end());
      for ( ; iter != iterEnd; iter++ )
      {
         UIEvent e = *iter;
         if ( MIN_RESULTS_HINT <= e.lHint && e.lHint <= MAX_RESULTS_HINT )
         {
            skip = true;
            break; // a result hint is already queued 
         }
      }

      if (!skip)
      {
         ATLASSERT( !m_bFiringEvents ); // don't add to the container while we are iterating through it
         m_UIEvents.push_back(event);
      }
   }
   else
   {
      m_pMyDocument->UpdateAllViews(pSender,lHint,pHint.get());
   }
}

///////////////////////////////////////////////////////////////////////////////////
// IXBRVersionInfo
CString CXBeamRateDocProxyAgent::GetVersionString(bool bIncludeBuildNumber)
{
   CXBeamRateVersionInfoImpl vi;
   return vi.GetVersionString(bIncludeBuildNumber);
}

CString CXBeamRateDocProxyAgent::GetVersion(bool bIncludeBuildNumber)
{
   CXBeamRateVersionInfoImpl vi;
   return vi.GetVersion(bIncludeBuildNumber);
}

/////////////////////////////////////////////////////////////////////
void CXBeamRateDocProxyAgent::CreateReportView(CollectionIndexType rptIdx,bool bPromptForSpec)
{
   CEAFReportViewCreationData data;
   data.m_RptIdx = rptIdx;
   data.m_bPromptForSpec = bPromptForSpec;

   GET_IFACE(IReportManager,pRptMgr);
   data.m_pRptMgr = pRptMgr;

   GET_IFACE(IEAFViewRegistrar,pViewReg);
   pViewReg->CreateView(m_ReportViewKey,(LPVOID)&data);
}

void CXBeamRateDocProxyAgent::CreateGraphView(CollectionIndexType graphIdx)
{
   CEAFGraphViewCreationData data;
   GET_IFACE(IGraphManager,pGraphMgr);
   data.m_pIGraphMgr = pGraphMgr;
   data.m_GraphIndex = graphIdx;

   GET_IFACE(IEAFViewRegistrar,pViewReg);
   pViewReg->CreateView(m_GraphingViewKey,(LPVOID)&data);
}

void CXBeamRateDocProxyAgent::CreatePierView()
{
   GET_IFACE(IEAFViewRegistrar,pViewReg);
   pViewReg->CreateView(m_PierViewKey);
}

/////////////////////////////////////////////////////////////////////
void CXBeamRateDocProxyAgent::RegisterViews()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CEAFDocTemplate* pTemplate = (CEAFDocTemplate*)(m_pMyDocument->GetDocTemplate());
   CComPtr<IEAFAppPlugin> pAppPlugin;
   pTemplate->GetPlugin(&pAppPlugin);

   HMENU hMenu = pAppPlugin->GetSharedMenuHandle();

   // Register all secondary views that are associated with our document type
   // TODO: After the menu and command extensions can be made, the agents that are responsble
   // for the views below will register them. For example, the analysis results view is the
   // responsiblity of the analysis results agent, so that view's implementation will move
   GET_IFACE(IEAFViewRegistrar,pViewReg);
   m_ReportViewKey    = pViewReg->RegisterView(IDR_REPORT,    NULL, RUNTIME_CLASS(CReportViewChildFrame), RUNTIME_CLASS(CXBeamRateReportView), hMenu, -1); // unlimited number of reports
   m_GraphingViewKey  = pViewReg->RegisterView(IDR_GRAPHING,  NULL, RUNTIME_CLASS(CGraphViewChildFrame),  RUNTIME_CLASS(CXBeamRateGraphView),  hMenu, -1); // unlimited number of reports
   m_PierViewKey      = pViewReg->RegisterView(IDR_XBEAMRATE, NULL, RUNTIME_CLASS(CXBeamRateChildFrame),  RUNTIME_CLASS(CXBeamRateView),        hMenu, 1);
}

void CXBeamRateDocProxyAgent::UnregisterViews()
{
   GET_IFACE(IEAFViewRegistrar,pViewReg);
   pViewReg->RemoveView(m_ReportViewKey);
   pViewReg->RemoveView(m_GraphingViewKey);
   pViewReg->RemoveView(m_PierViewKey);
}

void CXBeamRateDocProxyAgent::CreateToolBars()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IEAFToolbars,pToolBars);

   m_StdToolBarID = pToolBars->CreateToolBar(_T("Standard"));
   CEAFToolBar* pToolBar = pToolBars->GetToolBar(m_StdToolBarID);
   pToolBar->LoadToolBar(IDR_XBEAMRATE,NULL); // don't use a command callback because these commands are handled by 
                                               // the standard MFC message routing

   // Add a drop-down arrow to the Open and Report buttons
   pToolBar->CreateDropDownButton(ID_FILE_OPEN,   NULL,BTNS_DROPDOWN);
   pToolBar->CreateDropDownButton(ID_VIEW_GRAPHS, NULL,BTNS_WHOLEDROPDOWN);
   pToolBar->CreateDropDownButton(ID_VIEW_REPORTS,NULL,BTNS_WHOLEDROPDOWN);

   //OnStatusChanged(); // set the status items
}

void CXBeamRateDocProxyAgent::RemoveToolBars()
{
   GET_IFACE(IEAFToolbars,pToolBars);
   pToolBars->DestroyToolBar(m_StdToolBarID);
}

UINT CXBeamRateDocProxyAgent::GetStdToolBarID()
{
   return m_StdToolBarID;
}
