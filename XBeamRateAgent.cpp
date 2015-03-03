///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
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

// ExampleExtensionAgent.cpp : Implementation of CXBeamRateAgent

#include "stdafx.h"
#include "XBeamRateAgent.h"
#include <XBeamRateCatCom.h>

#include <IFace\Project.h>
#include <PgsExt\PierData2.h>

#include <IFace\DocumentType.h>

//#include <EAF\EAFOutputChildFrame.h>
//#include "MyView.h"
//
//#include <IFace\Tools.h>
//#include <IFace\EditByUI.h>
//#include <EAF\EAFStatusCenter.h>
//#include <EAF\EAFMenu.h>
//#include <EAF\EAFDisplayUnits.h>
//
//// Includes for reporting
//#include <IReportManager.h>
//#include "MyReportSpecification.h"
//#include "MyReportSpecificationBuilder.h"
//#include "MyChapterBuilder.h"
//#include <Reporting\PGSuperTitlePageBuilder.h>
//
//// Includes for graphing
//#include <IGraphManager.h>
//#include "TestGraphBuilder.h"
//
//#include <MFCTools\Prompts.h>

// CXBeamRateAgent

HRESULT CXBeamRateAgent::FinalConstruct()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   CComQIPtr<IBrokerInitEx2> pBrokerInit(pBroker);

   CComPtr<ICatRegister> pICatReg;
   HRESULT hr;
   hr = ::CoCreateInstance( CLSID_StdComponentCategoriesMgr,
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            IID_ICatRegister,
                            (void**)&pICatReg );

   // deal with failure
   if ( FAILED(hr) )
   {
      return hr;
   }

   // get the necessary interfaces from the component category manager
   CComQIPtr<ICatInformation> pICatInfo(pICatReg);

   CComPtr<IEnumCLSID> pIEnumCLSID;

   // get the agent category identifier
   const int nID = 1;
   CATID ID[nID];
   ID[0] = CATID_XBeamRateAgent;

   // enum agents
   pICatInfo->EnumClassesOfCategories(nID,ID,0,NULL,&pIEnumCLSID);

   // load up to 10 agents at a time
   const int nMaxAgents = 10;
   CLSID clsid[nMaxAgents];

   ULONG nAgentsLoaded = 0;
   while (SUCCEEDED(pIEnumCLSID->Next(nMaxAgents,clsid,&nAgentsLoaded)) && 0 < nAgentsLoaded )
   {
      // load the agents
      CComPtr<IIndexArray> lErrArray;
      hr = pBrokerInit->LoadAgents( clsid, nAgentsLoaded, &lErrArray );
      if ( FAILED(hr) )
      {
         return E_FAIL;
      }
   }

	return S_OK;
}

//void CXBeamRateAgent::RegisterViews()
//{
//   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
//   int maxViewCount = 2; // only allow 2 instances of this view to be created at a time
//   m_MyViewKey = pViewRegistrar->RegisterView(IDR_MENU,this,RUNTIME_CLASS(CEAFOutputChildFrame),RUNTIME_CLASS(CMyView),NULL,maxViewCount);
//}
//
//void CXBeamRateAgent::UnregisterViews()
//{
//   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
//   pViewRegistrar->RemoveView(m_MyViewKey);
//}
//
//void CXBeamRateAgent::CreateMyView()
//{
//   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
//   pViewRegistrar->CreateView(m_MyViewKey,this);
//}
//
//void CXBeamRateAgent::RegisterGraphs()
//{
//   GET_IFACE(IGraphManager,pGraphMgr);
//
//   CTestGraphBuilder* pTestGraphBuilder = new CTestGraphBuilder;
//   pTestGraphBuilder->SetMenuBitmap(&m_bmpMenu);
//   pGraphMgr->AddGraphBuilder(pTestGraphBuilder);
//
//   CTestGraphBuilder2* pTestGraphBuilder2 = new CTestGraphBuilder2;
//   pTestGraphBuilder2->SetMenuBitmap(&m_bmpMenu);
//   pGraphMgr->AddGraphBuilder(pTestGraphBuilder2);
//
//   CTestGraphBuilder3* pTestGraphBuilder3 = new CTestGraphBuilder3;
//   pTestGraphBuilder3->SetMenuBitmap(&m_bmpMenu);
//   pGraphMgr->AddGraphBuilder(pTestGraphBuilder3);
//}
//
//void CXBeamRateAgent::CreateMenus()
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//   GET_IFACE(IEAFMainMenu,pMainMenu);
//   CEAFMenu* pMenu = pMainMenu->GetMainMenu();
//
//   INT nMenus = pMenu->GetMenuItemCount();
//   if ( nMenus == 0 )
//      return;
//
//   m_pMyMenu = pMenu->CreatePopupMenu(nMenus-1,_T("MyExtension")); // put the menu before the last menu (Help)
//   m_pMyMenu->LoadMenu(IDR_MENU,this);
//}
//
//void CXBeamRateAgent::RemoveMenus()
//{
//   GET_IFACE(IEAFMainMenu,pMainMenu);
//   CEAFMenu* pMenu = pMainMenu->GetMainMenu();
//   pMenu->DestroyMenu(m_pMyMenu);
//}
//
//void CXBeamRateAgent::CreateToolBar()
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   GET_IFACE(IEAFToolbars,pToolBars);
//   m_ToolBarID = pToolBars->CreateToolBar(_T("Extension Agent Toolbar"));
//#if defined _EAF_USING_MFC_FEATURE_PACK
//   CEAFToolBar* pToolBar = pToolBars->GetToolBarByID(m_ToolBarID);
//#else
//   CEAFToolBar* pToolBar = pToolBars->GetToolBar(m_ToolBarID);
//#endif
//   pToolBar->LoadToolBar(IDR_TOOLBAR,this);
//
//   //GET_IFACE(IEditByUI,pEditUI);
//   //UINT stdID = pEditUI->GetStdToolBarID();
//   //CEAFToolBar* pStdToolBar = pToolBars->GetToolBar(stdID);
//   //UINT cmdID = ID_COMMAND1;
//   //pStdToolBar->InsertButton(-1,cmdID,-1,_T("Extension Command"),this);
//}
//
//void CXBeamRateAgent::RemoveToolBar()
//{
//   GET_IFACE(IEAFToolbars,pToolBars);
//   pToolBars->DestroyToolBar(m_ToolBarID);
//
//   //GET_IFACE(IEditByUI,pEditUI);
//   //UINT stdID = pEditUI->GetStdToolBarID();
//   //CEAFToolBar* pStdToolBar = pToolBars->GetToolBar(stdID);
//   //pStdToolBar->RemoveButtons(this);
//}
//
//void CXBeamRateAgent::RegisterReports()
//{
//   // Register our reports
//   GET_IFACE(IReportManager,pRptMgr);
//
//   //
//   // Create report spec builders
//   //
//
//   boost::shared_ptr<CReportSpecificationBuilder> pMyRptSpecBuilder( new CMyReportSpecificationBuilder(m_pBroker) );
//
//   // My report
//   CReportBuilder* pRptBuilder = new CReportBuilder(_T("Extension Agent Report"));
//   pRptBuilder->SetMenuBitmap(&m_bmpMenu);
//   pRptBuilder->AddTitlePageBuilder( boost::shared_ptr<CTitlePageBuilder>(new CPGSuperTitlePageBuilder(m_pBroker,pRptBuilder->GetName(),false)) );
//   pRptBuilder->SetReportSpecificationBuilder( pMyRptSpecBuilder );
//   pRptBuilder->AddChapterBuilder( boost::shared_ptr<CChapterBuilder>(new CMyChapterBuilder) );
//   pRptMgr->AddReportBuilder( pRptBuilder );
//}

void CXBeamRateAgent::RegisterUIExtensions()
{
   GET_IFACE(IExtendPGSuperUI,pExtendPGSuperUI);
   //m_EditBridgeCallbackID = pExtendPGSuperUI->RegisterEditBridgeCallback(this);
   m_EditPierCallbackID = pExtendPGSuperUI->RegisterEditPierCallback(this);
   //m_EditSpanCallbackID = pExtendPGSuperUI->RegisterEditSpanCallback(this);
   //m_EditGirderCallbackID = pExtendPGSuperUI->RegisterEditGirderCallback(this);

   //GET_IFACE(IExtendPGSpliceUI,pExtendPGSpliceUI);
   //m_EditTemporarySupportCallbackID = pExtendPGSpliceUI->RegisterEditTemporarySupportCallback(this);
   //m_EditSplicedGirderCallbackID = pExtendPGSpliceUI->RegisterEditSplicedGirderCallback(this);
   //m_EditSegmentCallbackID = pExtendPGSpliceUI->RegisterEditSegmentCallback(this);
   //m_EditClosureJointCallbackID = pExtendPGSpliceUI->RegisterEditClosureJointCallback(this);
}

void CXBeamRateAgent::UnregisterUIExtensions()
{
   GET_IFACE(IExtendPGSuperUI,pExtendPGSuperUI);
   //pExtendPGSuperUI->UnregisterEditBridgeCallback(m_EditBridgeCallbackID);
   pExtendPGSuperUI->UnregisterEditPierCallback(m_EditPierCallbackID);
   //pExtendPGSuperUI->UnregisterEditSpanCallback(m_EditSpanCallbackID);
   //pExtendPGSuperUI->UnregisterEditGirderCallback(m_EditGirderCallbackID);

   //GET_IFACE(IExtendPGSpliceUI,pExtendPGSpliceUI);
   //pExtendPGSpliceUI->UnregisterEditTemporarySupportCallback(m_EditTemporarySupportCallbackID);
   //pExtendPGSpliceUI->UnregisterEditSplicedGirderCallback(m_EditSplicedGirderCallbackID);
   //pExtendPGSpliceUI->UnregisterEditSegmentCallback(m_EditSegmentCallbackID);
   //pExtendPGSpliceUI->UnregisterEditClosureJointCallback(m_EditClosureJointCallbackID);
}

/////////////////////////////////////////////////////////////////////////
// IAgentEx

STDMETHODIMP CXBeamRateAgent::SetBroker(IBroker *pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   
   // Register interfaces here
   pBrokerInit->RegInterface( IID_IXBeamRateAgent, this);
   pBrokerInit->RegInterface( IID_IXBeamRate,      this);

   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::Init()
{
   //CREATE_LOGFILE(_T("XBeamRateAgent"));
   EAF_AGENT_INIT;

   //AFX_MANAGE_STATE(AfxGetStaticModuleState());
   //VERIFY(m_bmpMenu.LoadBitmap(IDB_LOGO));

   //
   // Attach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   // Connection point for the user interface extension events
   hr = pBrokerInit->FindConnectionPoint( IID_IProjectPropertiesEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Advise( GetUnknown(), &m_dwProjectPropertiesCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::Init2()
{
   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::Reset()
{
   //m_bmpMenu.DeleteObject();
   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::ShutDown()
{
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint(IID_IProjectPropertiesEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Unadvise( m_dwProjectPropertiesCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the connection point
   }

   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_XBeamRateAgent;
   return S_OK;
}


////////////////////////////////////////////////////////////////////
// IAgentPersist
//STDMETHODIMP CXBeamRateAgent::Load(IStructuredLoad* pStrLoad)
//{
//   USES_CONVERSION;
//   CComVariant var;
//   //var.vt = VT_BSTR;
//   
//   HRESULT hr = pStrLoad->BeginUnit(_T("XBeamRateAgent"));
//   if ( FAILED(hr) )
//      return hr;
//
//   //var.vt = VT_BSTR;
//   //hr = pStrLoad->get_Property(_T("SampleData"),&var);
//   //if ( FAILED(hr) )
//   //   return hr;
//
//   //m_Answer = OLE2T(var.bstrVal);
//
//   hr = pStrLoad->EndUnit();
//   if ( FAILED(hr) )
//      return hr;
//
//   return S_OK;
//}
//
//STDMETHODIMP CXBeamRateAgent::Save(IStructuredSave* pStrSave)
//{
//   pStrSave->BeginUnit(_T("XBeamRateAgent"),1.0);
//   //pStrSave->put_Property(_T("SampleData"),CComVariant(m_Answer));
//   pStrSave->EndUnit();
//   return S_OK;
//}

////////////////////////////////////////////////////////////////////
// IXBeamRateAgent
bool CXBeamRateAgent::IsExtendingPGSuper()
{
   return true;
}

////////////////////////////////////////////////////////////////////
// IXBeamRate
void CXBeamRateAgent::GetUnitServer(IUnitServer** ppUnitServer)
{
   GET_IFACE(IDocumentUnitSystem,pDocUnits);
   pDocUnits->GetUnitServer(ppUnitServer);
}

void CXBeamRateAgent::GetUnitConverter(IUnitConvert2** ppUnitConvert)
{
   GET_IFACE(IDocumentUnitSystem,pDocUnits);
   CComPtr<IUnitServer> unitServer;
   pDocUnits->GetUnitServer(&unitServer);
   unitServer.QueryInterface(ppUnitConvert);
}

////////////////////////////////////////////////////////////////////
// IAgentUIIntegration
STDMETHODIMP CXBeamRateAgent::IntegrateWithUI(BOOL bIntegrate)
{
   if ( bIntegrate )
   {
   //   CreateMenus();
   //   CreateToolBar();
   //   RegisterViews();
      RegisterUIExtensions();
   }
   else
   {
   //   RemoveMenus();
   //   RemoveToolBar();
   //   UnregisterViews();
      UnregisterUIExtensions();
   }

   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IAgentReportingIntegration
STDMETHODIMP CXBeamRateAgent::IntegrateWithReporting(BOOL bIntegrate)
{
   //if ( bIntegrate )
   //{
   //   RegisterReports();
   //}
   //else
   //{
   //}

   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IAgentGraphingIntegration
STDMETHODIMP CXBeamRateAgent::IntegrateWithGraphing(BOOL bIntegrate)
{
   //if ( bIntegrate )
   //{
   //   RegisterGraphs();
   //}
   //else
   //{
   //}

   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
////// IEAFCommandCallback
////BOOL CXBeamRateAgent::OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
////{
////   return m_MyCommandTarget.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
////}
////
////BOOL CXBeamRateAgent::GetStatusBarMessageString(UINT nID, CString& rMessage) const
////{
////   AFX_MANAGE_STATE(AfxGetStaticModuleState());
////
////   // load appropriate string
////	if ( rMessage.LoadString(nID) )
////	{
////		// first newline terminates actual string
////      rMessage.Replace('\n','\0');
////	}
////	else
////	{
////		// not found
////		TRACE1("Warning (CXBeamRateAgent): no message line prompt for ID 0x%04X.\n", nID);
////	}
////
////   return TRUE;
////}
////
////BOOL CXBeamRateAgent::GetToolTipMessageString(UINT nID, CString& rMessage) const
////{
////   AFX_MANAGE_STATE(AfxGetStaticModuleState());
////   CString string;
////   // load appropriate string
////	if ( string.LoadString(nID) )
////	{
////		// tip is after first newline 
////      int pos = string.Find('\n');
////      if ( 0 < pos )
////         rMessage = string.Mid(pos+1);
////	}
////	else
////	{
////		// not found
////		TRACE1("Warning (CXBeamRateAgent): no tool tip for ID 0x%04X.\n", nID);
////	}
////
////   return TRUE;
////}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditBridgeData* pBridgeData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditBridgeData* pBridgeData)
//{
//   return NULL;
//}
//
//void CXBeamRateAgent::EditPier_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pPierPropertyPage)
//{
//}
//
//void CXBeamRateAgent::EditTemporarySupport_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pTempSupportPropertyPage)
//{
//}
//
//void CXBeamRateAgent::EditSpan_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pSpanPropertyPage)
//{
//}
//
CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditPierData* pEditPierData)
{
   GET_IFACE(IBridgeDescription,pIBridgeDesc);
   const CPierData2* pPier = pIBridgeDesc->GetPier(pEditPierData->GetPier());
   if ( pPier->IsAbutment() )
   {
      return NULL;
   }
   else
   {
      AFX_MANAGE_STATE(AfxGetStaticModuleState());
      return new CPropertyPage(IDD_PIER_PAGE);
   }
}

CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditPierData* pEditPierData,CPropertyPage* pBridgePropertyPage)
{
   ATLASSERT(false); // should never get here
   return NULL;
}

txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditPierData* pEditPierData)
{
   return NULL;
}

IDType CXBeamRateAgent::GetEditBridgeCallbackID()
{
   return INVALID_ID;
}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditTemporarySupportData* pEditTemporarySupportData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditTemporarySupportData* pEditTemporarySupportData,CPropertyPage* pBridgePropertyPage)
//{
//   ATLASSERT(false);
//   return NULL;
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditTemporarySupportData* pEditTemporarySupportData)
//{
//   return NULL;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditSpanData* pSpanData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditSpanData* pEditSpanData,CPropertyPage* pBridgePropertyPage)
//{
//   ATLASSERT(false);
//   return NULL;
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditSpanData* pSpanData)
//{
//   return NULL;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditSegmentData* pSegmentData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditSegmentData* pSegmentData)
//{
//   return NULL;
//}
//
//IDType CXBeamRateAgent::GetEditSplicedGirderCallbackID()
//{
//   return INVALID_ID;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditSegmentData* pEditSegmentData,CPropertyPage* pSplicedGirderPropertyPage)
//{
//   return NULL;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditClosureJointData* pClosureJointData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditClosureJointData* pClosureJointData)
//{
//   return NULL;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditClosureJointData* pEditClosureJointData,CPropertyPage* pSplicedGirderPropertyPage)
//{
//   return NULL;
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditSplicedGirderData* pGirderData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditSplicedGirderData* pGirderData)
//{
//   return NULL;
//}
//
//void CXBeamRateAgent::EditSegment_OnOK(CPropertyPage* pSplicedGirderPropertyPage,CPropertyPage* pSegmentPropertyPage)
//{
//}
//
//void CXBeamRateAgent::EditClosureJoint_OnOK(CPropertyPage* pSplicedGirderPropertyPage,CPropertyPage* pClosureJointPropertyPage)
//{
//}
//
//CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditGirderData* pGirderData)
//{
//   AFX_MANAGE_STATE(AfxGetStaticModuleState());
//   return new CPropertyPage(IDD_EDIT_PIER_PAGE);
//}
//
//txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditGirderData* pGirderData)
//{
//   return NULL;
//}
//
//HRESULT CXBeamRateAgent::OnHintsReset()
//{
//   AfxMessageBox(_T("Example Extension Agent - Hints Reset"));
//   return S_OK;
//}

//IProjectPropertiesEventSink
using namespace XBR;
HRESULT CXBeamRateAgent::OnProjectPropertiesChanged()
{
   GET_IFACE(IProjectProperties,pProjectProps);
   GET_IFACE(IProject, pProject);
   pProject->SetProjectName(pProjectProps->GetBridgeName());
   return S_OK;
}
