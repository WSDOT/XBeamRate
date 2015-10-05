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

// XBeamRateDoc.cpp : implementation of the CXBeamRateDoc class
//

#include "stdafx.h"
#include "resource.h"
#include "XBeamRateAppPlugin.h"
#include "XBeamRatePluginApp.h"
#include "XBeamRateDoc.h"
#include "XBeamRateDocProxyAgent.h"

#include "AboutDlg.h"

#include <XBeamRateCatCom.h>

#include <EAF\EAFMainFrame.h>
#include <EAF\EAFUnits.h>

#include <WBFLReportManagerAgent.h>
#include <WBFLGraphManagerAgent.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc

IMPLEMENT_DYNCREATE(CXBeamRateDoc, CEAFBrokerDocument)

BEGIN_MESSAGE_MAP(CXBeamRateDoc, CEAFBrokerDocument)
	//{{AFX_MSG_MAP(CXBeamRateDoc)
   ON_COMMAND(ID_HELP_ABOUT, OnAbout)
   ON_UPDATE_COMMAND_UI(ID_VIEW_GRAPHS,OnUpdateViewGraphs)
   ON_UPDATE_COMMAND_UI(ID_VIEW_REPORTS,OnUpdateViewReports)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc construction/destruction

#define XBR_PLUGIN_COMMAND_COUNT 256

CXBeamRateDoc::CXBeamRateDoc()
{
	// TODO: add one-time construction code here
   GetPluginCommandManager()->ReserveCommandIDRange(XBR_PLUGIN_COMMAND_COUNT);

   m_pMyDocProxyAgent = NULL;
   m_bAutoCalcEnabled = true;

   CEAFAutoCalcDocMixin::SetDocument(this);
}

CXBeamRateDoc::~CXBeamRateDoc()
{
}

// CEAFAutoCalcDocMixin overrides
bool CXBeamRateDoc::IsAutoCalcEnabled() const
{
   return m_bAutoCalcEnabled;
}

void CXBeamRateDoc::EnableAutoCalc(bool bEnable)
{
   if ( m_bAutoCalcEnabled != bEnable )
   {
      bool bWasDisabled = !IsAutoCalcEnabled();
      m_bAutoCalcEnabled = bEnable;

#pragma Reminder("UPDATE: need our own status subclass to do the autocalc indicator")
      //CPGSuperStatusBar* pStatusBar = ((CPGSuperStatusBar*)EAFGetMainFrame()->GetStatusBar());
      //pStatusBar->AutoCalcEnabled( m_bAutoCalcEnabled );

      // If AutoCalc was off and now it is on,
      // Update the views.
      if ( bWasDisabled && IsAutoCalcEnabled() )
      {
        OnUpdateNow();
      }
   }
}

void CXBeamRateDoc::GetDocUnitSystem(IDocUnitSystem** ppDocUnitSystem)
{
   (*ppDocUnitSystem) = m_DocUnitSystem;
   (*ppDocUnitSystem)->AddRef();
}

BOOL CXBeamRateDoc::Init()
{
   CEAFDocTemplate* pTemplate = (CEAFDocTemplate*)GetDocTemplate();
   CComPtr<IEAFAppPlugin> pAppPlugin;
   pTemplate->GetPlugin(&pAppPlugin);
   CXBeamRateAppPlugin* pXBeamRate = dynamic_cast<CXBeamRateAppPlugin*>(pAppPlugin.p);

   if ( !CEAFBrokerDocument::Init() )
   {
      return FALSE;
   }

   // Set up the document unit system
   CComPtr<IAppUnitSystem> appUnitSystem;
   pXBeamRate->GetAppUnitSystem(&appUnitSystem);
   EAFCreateDocUnitSystem(appUnitSystem,&m_DocUnitSystem);

   //// Transfer report favorites and custom reports data from CPGSuperBaseAppPlugin to CEAFBrokerDocument (this)
   //bool doDisplayFavorites = pPGSuper->GetDoDisplayFavoriteReports();
   //std::vector<std::_tstring> Favorites = pPGSuper->GetFavoriteReports();

   //SetDoDisplayFavoriteReports(doDisplayFavorites);
   //SetFavoriteReports(Favorites);

   //CEAFCustomReports customs = pPGSuper->GetCustomReports();
   //SetCustomReports(customs);

   return TRUE;
}

BOOL CXBeamRateDoc::LoadSpecialAgents(IBrokerInitEx2* pBrokerInit)
{
   if ( !CEAFBrokerDocument::LoadSpecialAgents(pBrokerInit) )
   {
      return FALSE;
   }

   CComObject<CXBeamRateDocProxyAgent>* pDocProxyAgent;
   CComObject<CXBeamRateDocProxyAgent>::CreateInstance(&pDocProxyAgent);
   m_pMyDocProxyAgent = dynamic_cast<CXBeamRateDocProxyAgent*>(pDocProxyAgent);
   m_pMyDocProxyAgent->SetDocument( this );

   CComPtr<IAgentEx> pAgent(m_pMyDocProxyAgent);
   
   HRESULT hr = pBrokerInit->AddAgent( pAgent );
   if ( FAILED(hr) )
   {
      return FALSE;
   }

   // we want to use some special agents
   CLSID clsid[] = {CLSID_ReportManagerAgent,CLSID_GraphManagerAgent};
   if ( !CEAFBrokerDocument::LoadAgents(pBrokerInit, clsid, sizeof(clsid)/sizeof(CLSID) ) )
   {
      return FALSE;
   }

   return TRUE;
}

CATID CXBeamRateDoc::GetAgentCategoryID()
{
   return CATID_XBeamRateAgent;
}

HINSTANCE CXBeamRateDoc::GetResourceInstance()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return AfxGetInstanceHandle();
}

HRESULT CXBeamRateDoc::LoadTheDocument(IStructuredLoad* pStrLoad)
{
   Float64 version;
   HRESULT hr = pStrLoad->get_Version(&version);
   if ( FAILED(hr) )
   {
      return hr;
   }

   CComVariant var;
   var.vt = VT_BSTR;
   hr = pStrLoad->get_Property(_T("Version"),&var);
   if ( FAILED(hr) )
   {
      return hr;
   }

#if defined _DEBUG
   TRACE(_T("Loading data saved with XBeamRate Version %s\n"),CComBSTR(var.bstrVal));
#endif

   hr = CEAFBrokerDocument::LoadTheDocument(pStrLoad);
   if ( FAILED(hr) )
   {
      return hr;
   }

   return S_OK;
}

HRESULT CXBeamRateDoc::WriteTheDocument(IStructuredSave* pStrSave)
{
   // before the standard broker document persistence, write out the version
   // number of the application that created this document
   CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();
   HRESULT hr = pStrSave->put_Property(_T("Version"),CComVariant(pApp->GetVersion(true)));
   if ( FAILED(hr) )
   {
      return hr;
   }

   hr = CEAFBrokerDocument::WriteTheDocument(pStrSave);
   if ( FAILED(hr) )
   {
      return hr;
   }


   return S_OK;
}

void CXBeamRateDoc::CreateReportView(CollectionIndexType rptIdx,bool bPrompt)
{
   m_pMyDocProxyAgent->CreateReportView(rptIdx,bPrompt);
}

void CXBeamRateDoc::CreateGraphView(CollectionIndexType graphIdx)
{
   m_pMyDocProxyAgent->CreateGraphView(graphIdx);
}

CString CXBeamRateDoc::GetRootNodeName()
{
   return _T("XBeamRate");
}

Float64 CXBeamRateDoc::GetRootNodeVersion()
{
   return 1.0;
}

BOOL CXBeamRateDoc::OnNewDocument()
{
	if (!CEAFBrokerDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL CXBeamRateDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   // document classes can't process ON_NOTIFY
   // see http://www.codeproject.com/KB/docview/NotifierApp.aspx for details
   if ( HIWORD(nCode) == WM_NOTIFY )
   {
    // verify that this is a WM_NOTIFY message
     WORD wCode = LOWORD(nCode) ;

     AFX_NOTIFY * notify = reinterpret_cast<AFX_NOTIFY*>(pExtra) ;
     if ( notify->pNMHDR->code == TBN_DROPDOWN )
     {
        if ( notify->pNMHDR->idFrom == m_pMyDocProxyAgent->GetStdToolBarID() && ((NMTOOLBAR*)(notify->pNMHDR))->iItem == ID_VIEW_REPORTS )
        {
           return OnViewReports(notify->pNMHDR,notify->pResult); 
        }

        if ( notify->pNMHDR->idFrom == m_pMyDocProxyAgent->GetStdToolBarID() && ((NMTOOLBAR*)(notify->pNMHDR))->iItem == ID_VIEW_GRAPHS )
        {
           return OnViewGraphs(notify->pNMHDR,notify->pResult); 
        }
     }
   }

   return CEAFBrokerDocument::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}

void CXBeamRateDoc::LoadToolbarState()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CEAFBrokerDocument::LoadToolbarState();
}

void CXBeamRateDoc::SaveToolbarState()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CEAFBrokerDocument::SaveToolbarState();
}

CString CXBeamRateDoc::GetToolbarSectionName()
{
   return _T("XBeamRate");
}

BOOL CXBeamRateDoc::GetStatusBarMessageString(UINT nID,CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return CEAFBrokerDocument::GetStatusBarMessageString(nID,rMessage);
}

BOOL CXBeamRateDoc::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return CEAFBrokerDocument::GetToolTipMessageString(nID,rMessage);
}

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc diagnostics

#ifdef _DEBUG
void CXBeamRateDoc::AssertValid() const
{
	CEAFBrokerDocument::AssertValid();
}

void CXBeamRateDoc::Dump(CDumpContext& dc) const
{
	CEAFBrokerDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc commands
BOOL CXBeamRateDoc::OpenTheDocument(LPCTSTR lpszPathName)
{
   // don't fire UI events as the UI isn't completely built when the document is created
   // (view classes haven't been initialized)
   m_pMyDocProxyAgent->HoldEvents();
   // Events are released in OnCreateFinalize()

   if ( !CEAFBrokerDocument::OpenTheDocument(lpszPathName) )
   {
      return FALSE;
   }

   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   m_DocUnitSystem->put_UnitMode( IS_US_UNITS(pDisplayUnits) ? umUS : umSI );
  
   return TRUE;
}

void CXBeamRateDoc::OnCreateFinalize()
{
   CEAFBrokerDocument::OnCreateFinalize();
   PopulateReportMenu();
   PopulateGraphMenu();

   // views have been initilized so fire any pending events
   GET_IFACE(IXBREvents,pEvents);
   GET_IFACE(IXBRUIEvents,pUIEvents);
   pEvents->FirePendingEvents(); 
   pUIEvents->HoldEvents(false);
}

void CXBeamRateDoc::BrokerShutDown()
{
   CEAFBrokerDocument::BrokerShutDown();

   m_pMyDocProxyAgent = NULL;
}

void CXBeamRateDoc::PopulateReportMenu()
{
   CEAFMenu* pMainMenu = GetMainMenu();

   UINT viewPos = pMainMenu->FindMenuItem(_T("&View"));
   ASSERT( 0 <= viewPos );

   CEAFMenu* pViewMenu = pMainMenu->GetSubMenu(viewPos);
   ASSERT( pViewMenu != NULL );

   UINT reportsPos = pViewMenu->FindMenuItem(_T("&Reports"));
   ASSERT( 0 <= reportsPos );

   // Get the reports menu
   CEAFMenu* pReportsMenu = pViewMenu->GetSubMenu(reportsPos);
   ASSERT(pReportsMenu != NULL);

   CEAFBrokerDocument::PopulateReportMenu(pReportsMenu);
}

void CXBeamRateDoc::PopulateGraphMenu()
{
   CEAFMenu* pMainMenu = GetMainMenu();

   UINT viewPos = pMainMenu->FindMenuItem(_T("&View"));
   ASSERT( 0 <= viewPos );

   CEAFMenu* pViewMenu = pMainMenu->GetSubMenu(viewPos);
   ASSERT( pViewMenu != NULL );

   UINT graphsPos = pViewMenu->FindMenuItem(_T("&Graphs"));
   ASSERT( 0 <= graphsPos );

   // Get the graphs menu
   CEAFMenu* pGraphMenu = pViewMenu->GetSubMenu(graphsPos);
   ASSERT(pGraphMenu != NULL);

   CEAFBrokerDocument::PopulateGraphMenu(pGraphMenu);
}

void CXBeamRateDoc::OnAbout()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CEAFDocTemplate* pTemplate = (CEAFDocTemplate*)GetDocTemplate();
   UINT resourceID = pTemplate->GetResourceID();

   CAboutDlg dlg(resourceID);
   dlg.DoModal();
}

void CXBeamRateDoc::OnUpdateViewGraphs(CCmdUI* pCmdUI)
{
   GET_IFACE(IGraphManager,pGraphMgr);
   pCmdUI->Enable( 0 < pGraphMgr->GetGraphBuilderCount() );
}

BOOL CXBeamRateDoc::OnViewGraphs(NMHDR* pnmhdr,LRESULT* plr) 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // This method gets called when the down arrow toolbar button is used
   // It creates the drop down menu with the report names on it
   NMTOOLBAR* pnmtb = (NMTOOLBAR*)(pnmhdr);
   if ( pnmtb->iItem != ID_VIEW_GRAPHS )
   {
      return FALSE; // not our button
   }

   CMenu menu;
   VERIFY( menu.LoadMenu(IDR_GRAPHS) );
   CMenu* pMenu = menu.GetSubMenu(0);
   pMenu->RemoveMenu(0,MF_BYPOSITION); // remove the placeholder

   CEAFMenu contextMenu(pMenu->Detach(),GetPluginCommandManager());


   BuildGraphMenu(&contextMenu);

   GET_IFACE(IEAFToolbars,pToolBars);
   CEAFToolBar* pToolBar = pToolBars->GetToolBar( m_pMyDocProxyAgent->GetStdToolBarID() );
   int idx = pToolBar->CommandToIndex(ID_VIEW_GRAPHS,NULL);
   CRect rect;
   pToolBar->GetItemRect(idx,&rect);

   CPoint point(rect.left,rect.bottom);
   pToolBar->ClientToScreen(&point);
   contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x,point.y, EAFGetMainFrame() );

   return TRUE;
}

void CXBeamRateDoc::OnUpdateViewReports(CCmdUI* pCmdUI)
{
   GET_IFACE(IReportManager,pReportMgr);
   pCmdUI->Enable( 0 < pReportMgr->GetReportBuilderCount() );
}

BOOL CXBeamRateDoc::OnViewReports(NMHDR* pnmhdr,LRESULT* plr) 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // This method gets called when the down arrow toolbar button is used
   // It creates the drop down menu with the report names on it
   NMTOOLBAR* pnmtb = (NMTOOLBAR*)(pnmhdr);
   if ( pnmtb->iItem != ID_VIEW_REPORTS )
   {
      return FALSE; // not our button
   }

   CMenu menu;
   VERIFY( menu.LoadMenu(IDR_REPORTS) );
   CMenu* pMenu = menu.GetSubMenu(0);
   pMenu->RemoveMenu(0,MF_BYPOSITION); // remove the placeholder

   CEAFMenu contextMenu(pMenu->Detach(),GetPluginCommandManager());

   CEAFBrokerDocument::PopulateReportMenu(&contextMenu);

   GET_IFACE(IEAFToolbars,pToolBars);
   CEAFToolBar* pToolBar = pToolBars->GetToolBar( m_pMyDocProxyAgent->GetStdToolBarID() );
   int idx = pToolBar->CommandToIndex(ID_VIEW_REPORTS,NULL);
   CRect rect;
   pToolBar->GetItemRect(idx,&rect);

   CPoint point(rect.left,rect.bottom);
   pToolBar->ClientToScreen(&point);
   contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x,point.y, EAFGetMainFrame() );

   return TRUE;
}
