///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2019  Washington State Department of Transportation
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
#include "XBeamRateAgent.h"
#include <XBeamRateCatCom.h>

#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>
#include <PgsExt\BridgeDescription2.h>

#include <IFace\DocumentType.h>
#include <IFace\Bridge.h>
#include <IFace\Selection.h>
#include <IFace\EditByUI.h>

#include <EAF\EAFDocument.h>

#include <MFCTools\VersionInfo.h>

#include <XBeamRateExt\ReinforcementPage.h>

#include "XBeamRateChildFrame.h"
#include "XBeamRateView.h"

#include "txnEditReinforcement.h"
#include "txnEditLoadRatingOptions.h"

#include "LoadRatingOptionsPage.h"

#include "XBeamRateVersionInfoImpl.h"
#include "XBeamRateCommandLineProcessor.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CMyCommandTarget, CCmdTarget)
	ON_COMMAND(ID_VIEW_PIER, OnViewPier)
   //ON_UPDATE_COMMAND_UI(ID_VIEW_PIER,OnPierCommandUpdate)
   ON_COMMAND(IDM_EXPORT_PIER,OnExportPier)
   ON_UPDATE_COMMAND_UI(IDM_EXPORT_PIER,OnPierCommandUpdate)
END_MESSAGE_MAP()

void CMyCommandTarget::OnViewPier()
{
   m_pMyAgent->CreatePierView();
}

void CMyCommandTarget::OnExportPier()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,ISelection,pSelection);
   PierIndexType selPierIdx = pSelection->GetSelectedPier();

   GET_IFACE2(pBroker,IXBRExport,pExport);
   pExport->Export(selPierIdx);
}

void CMyCommandTarget::OnPierCommandUpdate(CCmdUI* pCmdUI)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
   PierIndexType nPiers = pIBridgeDesc->GetPierCount();

   GET_IFACE2(pBroker,ISelection,pSelection);
   PierIndexType selPierIdx = pSelection->GetSelectedPier();
   const CPierData2* pPier = pIBridgeDesc->GetPier(selPierIdx);
   if ( pPier == nullptr || pPier->GetPierModelType() == pgsTypes::pmtPhysical )
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}


// IBridgePlanViewEventCallback
void CMyCommandTarget::OnBackgroundContextMenu(CEAFMenu* pMenu)
{
}

void CMyCommandTarget::OnPierContextMenu(PierIndexType pierIdx,CEAFMenu* pMenu)
{
   pMenu->AppendMenu(ID_VIEW_PIER,_T("View Pier"),m_pMyAgent);
   pMenu->AppendMenu(IDM_EXPORT_PIER,_T("Export to XBRate"),m_pMyAgent);
}

void CMyCommandTarget::OnSpanContextMenu(SpanIndexType spanIdx,CEAFMenu* pMenu) {}
void CMyCommandTarget::OnDeckContextMenu(CEAFMenu* pMenu) {}
void CMyCommandTarget::OnAlignmentContextMenu(CEAFMenu* pMenu) {}
void CMyCommandTarget::OnSectionCutContextMenu(CEAFMenu* pMenu) {}
void CMyCommandTarget::OnGirderContextMenu(const CGirderKey& girderKey,CEAFMenu* pMenu) {}
void CMyCommandTarget::OnTemporarySupportContextMenu(SupportIDType tsID,CEAFMenu* pMenu) {}
void CMyCommandTarget::OnGirderSegmentContextMenu(const CSegmentKey& segmentKey,CEAFMenu* pMenu) {}
void CMyCommandTarget::OnClosureJointContextMenu(const CSegmentKey& closureKey,CEAFMenu* pMenu) {}

// CXBeamRateAgent


HRESULT CXBeamRateAgent::FinalConstruct()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   CComQIPtr<IBrokerInitEx2> pBrokerInit(pBroker);

   CComPtr<ICatRegister> pICatReg;
   HRESULT hr;
   hr = ::CoCreateInstance( CLSID_StdComponentCategoriesMgr,
                            nullptr,
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
   pICatInfo->EnumClassesOfCategories(nID,ID,0,nullptr,&pIEnumCLSID);

   // load up to 10 agents at a time
   const int nMaxAgents = 10;
   CLSID clsid[nMaxAgents];

   ULONG nAgentsLoaded = 0;
   while (SUCCEEDED(pIEnumCLSID->Next(nMaxAgents,clsid,&nAgentsLoaded)) && 0 < nAgentsLoaded )
   {
      // load the agents... this method only gets called if we are an extenion agent to PGSuper/PGSplice
      // since we are an extension, all of our sub-agents should also be considered extensions...
      // that is why we use LoadExtensionAgents instead of LoadAgents
      CComPtr<IIndexArray> lErrArray;
      hr = pBrokerInit->LoadExtensionAgents( clsid, nAgentsLoaded, &lErrArray );
      if ( FAILED(hr) )
      {
         return E_FAIL;
      }
   }

	return S_OK;
}

void CXBeamRateAgent::RegisterViews()
{
   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
   m_PierViewKey = pViewRegistrar->RegisterView(IDR_XBEAMRATE,this,RUNTIME_CLASS(CXBeamRateChildFrame),RUNTIME_CLASS(CXBeamRateView));
}

void CXBeamRateAgent::UnregisterViews()
{
   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
   pViewRegistrar->RemoveView(m_PierViewKey);
}

void CXBeamRateAgent::CreatePierView()
{
   GET_IFACE(IEAFViewRegistrar,pViewRegistrar);
   pViewRegistrar->CreateView(m_PierViewKey,nullptr);
}

void CXBeamRateAgent::CreateMenus()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   UINT viewPos = pMenu->FindMenuItem(_T("View"));
   CEAFMenu* pViewMenu = pMenu->GetSubMenu(viewPos);
   UINT graphsPos = pViewMenu->FindMenuItem(_T("Graphs"));
   pViewMenu->InsertMenu(graphsPos-1,ID_VIEW_PIER,_T("&Pier View"),this);
}

void CXBeamRateAgent::RemoveMenus()
{
   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();
   UINT viewPos = pMenu->FindMenuItem(_T("View"));
   CEAFMenu* pViewMenu = pMenu->GetSubMenu(viewPos);
   pViewMenu->RemoveMenu(ID_VIEW_PIER,MF_BYCOMMAND,this);
}

void CXBeamRateAgent::CreateToolbar()
{
   // add a button to the standard PGSuper/PGSplice toolbar to view a pier
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   GET_IFACE(IEAFToolbars,pToolBars);
   GET_IFACE(IEditByUI,pEditUI);
   UINT stdID = pEditUI->GetStdToolBarID();
   CEAFToolBar* pStdToolBar = pToolBars->GetToolBar(stdID);

   // Put the button to the right of PGSuper/PGSplice view girder button
   // The IDs from PGSuper/PGSplice aren't available to plugins so we'll just
   // hard code the command ID.
   int idx = pStdToolBar->CommandToIndex(36896/*ID_VIEW_GIRDEREDITOR*/,nullptr); 
   pStdToolBar->InsertButton(idx+1,ID_VIEW_PIER,IDB_VIEW_PIER,nullptr,this);
}

void CXBeamRateAgent::RemoveToolbar()
{
   // Remove all the buttons we added to the standard toolbar
   GET_IFACE(IEAFToolbars,pToolBars);
   GET_IFACE(IEditByUI,pEditUI);
   UINT stdID = pEditUI->GetStdToolBarID();
   CEAFToolBar* pStdToolBar = pToolBars->GetToolBar(stdID);
   pStdToolBar->RemoveButtons(this);
}

//void CXBeamRateAgent::RegisterReports()
//{
//   // Register our reports
//   GET_IFACE(IReportManager,pRptMgr);
//
//   //
//   // Create report spec builders
//   //
//
//   std::shared_ptr<CReportSpecificationBuilder> pMyRptSpecBuilder( std::make_shared<CMyReportSpecificationBuilder>(m_pBroker) );
//
//   // My report
//   std::unique_ptr<CReportBuilder> pRptBuilder(std::make_unique<CReportBuilder>(_T("Extension Agent Report"));
//   pRptBuilder->SetMenuBitmap(&m_bmpMenu);
//   pRptBuilder->AddTitlePageBuilder( std::make_shared<CPGSuperTitlePageBuilder>(m_pBroker,pRptBuilder->GetName(),false)) );
//   pRptBuilder->SetReportSpecificationBuilder( pMyRptSpecBuilder );
//   pRptBuilder->AddChapterBuilder( std::make_shared<CMyChapterBuilder>) );
//   pRptMgr->AddReportBuilder( pRptBuilder.release() );
//}

void CXBeamRateAgent::RegisterUIExtensions()
{
   // Tell PGSuper/PGSplice that we want to add stuff to its dialogs
   GET_IFACE(IExtendPGSuperUI,pExtendUI);
   m_EditPierCallbackID = pExtendUI->RegisterEditPierCallback(this);
   m_EditLoadRatingOptionsCallbackID = pExtendUI->RegisterEditLoadRatingOptionsCallback(this);

   // Add a command callback to the bridge view
   GET_IFACE(IRegisterViewEvents,pBridgeViewEvents);
   m_BridgePlanViewCallbackID = pBridgeViewEvents->RegisterBridgePlanViewCallback(&m_CommandTarget);
}

void CXBeamRateAgent::UnregisterUIExtensions()
{
   // We are done adding stuff to PGS's dialogs
   GET_IFACE(IExtendPGSuperUI,pExtendUI);
   pExtendUI->UnregisterEditPierCallback(m_EditPierCallbackID);
   pExtendUI->UnregisterEditLoadRatingOptionsCallback(m_EditLoadRatingOptionsCallbackID);

   // Done with handling commands from the bridge view
   GET_IFACE(IRegisterViewEvents,pBridgeViewEvents);
   pBridgeViewEvents->UnregisterBridgePlanViewCallback(m_BridgePlanViewCallbackID);
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
   pBrokerInit->RegInterface( IID_IXBRVersionInfo, this );

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

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

   return AGENT_S_SECONDPASSINIT;
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

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Unadvise( m_dwProjectCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_XBeamRateAgent;
   return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////
// IXBRVersionInfo
CString CXBeamRateAgent::GetVersionString(bool bIncludeBuildNumber)
{
   CXBeamRateVersionInfoImpl vi;
   return vi.GetVersionString(bIncludeBuildNumber);
}

CString CXBeamRateAgent::GetVersion(bool bIncludeBuildNumber)
{
   CXBeamRateVersionInfoImpl vi;
   return vi.GetVersion(bIncludeBuildNumber);
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

////////////////////////////////////////////////////////////////////
// IAgentUIIntegration
STDMETHODIMP CXBeamRateAgent::IntegrateWithUI(BOOL bIntegrate)
{
   if ( bIntegrate )
   {
      CreateMenus();
      CreateToolbar();
      RegisterViews();
      RegisterUIExtensions();
   }
   else
   {
      RemoveMenus();
      RemoveToolbar();
      UnregisterViews();
      UnregisterUIExtensions();
   }

   return S_OK;
}

////////////////////////////////////////////////////////////////////
// IAgentDocumentationIntegration
STDMETHODIMP CXBeamRateAgent::GetDocumentationSetName(BSTR* pbstrName)
{
   *pbstrName = CComBSTR(_T("XBRate"));
   return S_OK;
}

CString CXBeamRateAgent::GetDocumentationURL()
{
   USES_CONVERSION;

   CComBSTR bstrDocSetName;
   GetDocumentationSetName(&bstrDocSetName);
   CString strDocSetName(OLE2T(bstrDocSetName));

   CEAFApp* pApp = EAFGetApp();
   CString strDocumentationRootLocation = pApp->GetDocumentationRootLocation();

   CString strDocumentationURL;
   strDocumentationURL.Format(_T("%s%s/"),strDocumentationRootLocation,strDocSetName);

   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CVersionInfo verInfo;
   CString strAppName = AfxGetAppName(); // needs module state
   strAppName += _T(".dll");
   verInfo.Load(strAppName);

   CString strVersion = verInfo.GetProductVersionAsString(false);
   std::_tstring v(strVersion);
   auto count = std::count(std::begin(v), std::end(v), _T('.'));

   // remove the hot fix and build/release number
   for (auto i = 0; i < count - 1; i++)
   {
      int pos = strVersion.ReverseFind(_T('.')); // find the last '.'
      strVersion = strVersion.Left(pos);
   }

   CString strURL;
   strURL.Format(_T("%s%s/"),strDocumentationURL,strVersion);
   strDocumentationURL = strURL;

   return strDocumentationURL;
}

STDMETHODIMP CXBeamRateAgent::LoadDocumentationMap()
{
   USES_CONVERSION;

   CComBSTR bstrDocSetName;
   GetDocumentationSetName(&bstrDocSetName);

   CString strDocSetName(OLE2T(bstrDocSetName));

   CEAFApp* pApp = EAFGetApp();

   CString strDocumentationURL = GetDocumentationURL();

   CString strDocMapFile = EAFGetDocumentationMapFile(strDocSetName,strDocumentationURL);

   VERIFY(EAFLoadDocumentationMap(strDocMapFile,m_HelpTopics));
   return S_OK;
}

STDMETHODIMP CXBeamRateAgent::GetDocumentLocation(UINT nHID,BSTR* pbstrURL)
{
   std::map<UINT,CString>::iterator found = m_HelpTopics.find(nHID);
   if ( found == m_HelpTopics.end() )
   {
      return E_FAIL;
   }

   CString strURL;
   strURL.Format(_T("%s%s"),GetDocumentationURL(),found->second);
   CComBSTR bstrURL(strURL);
   bstrURL.CopyTo(pbstrURL);
   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
// IEditPierCallback
CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditPierData* pEditPierData)
{
   // The PGSuper/PGSplice Edit Pier dialog is being displayed... here is our
   // chance to add a property page to the dialog
   const CPierData2* pPier = pEditPierData->GetPierData();
   GET_IFACE(IXBRProject,pProject);
   m_ReinforcementPageParent.SetEditPierData(pEditPierData,pProject->GetPierData(pPier->GetID()));
   return new CReinforcementPage(&m_ReinforcementPageParent);
}

CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditPierData* pEditPierData,CPropertyPage* pBridgePropertyPage)
{
   ATLASSERT(false); // should never get here
   return nullptr;
}

txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditPierData* pEditPierData)
{
   if ( pPage == nullptr )
   {
      // we aren't extending the dialog for this pier
      return nullptr;
   }

   xbrEditReinforcementData oldReinforcement;
   GET_IFACE(IXBRProject,pProject);
   const xbrPierData& pierData = pProject->GetPierData(pEditPierData->GetPierData()->GetID());
   pierData.GetRebarMaterial(&oldReinforcement.Type,&oldReinforcement.Grade);
   oldReinforcement.LongitudinalRebar  = pierData.GetLongitudinalRebar();
   oldReinforcement.LowerXBeamStirrups = pierData.GetLowerXBeamStirrups();
   oldReinforcement.FullDepthStirrups  = pierData.GetFullDepthStirrups();
   oldReinforcement.ConditionFactorType = pierData.GetConditionFactorType();
   oldReinforcement.ConditionFactor = pierData.GetConditionFactor();

   CReinforcementPage* pReinforcementPage = (CReinforcementPage*)pPage;
   xbrEditReinforcementData newReinforcement;

   IReinforcementPageParent* pParent  = pReinforcementPage->GetPageParent();
   newReinforcement.Type               = pParent->GetRebarType();
   newReinforcement.Grade              = pParent->GetRebarGrade();
   newReinforcement.LongitudinalRebar  = pParent->GetLongitudinalRebar();
   newReinforcement.LowerXBeamStirrups = pParent->GetLowerXBeamStirrups();
   newReinforcement.FullDepthStirrups  = pParent->GetFullDepthStirrups();
   newReinforcement.ConditionFactorType = pParent->GetConditionFactorType();
   newReinforcement.ConditionFactor = pParent->GetConditionFactor();

   txnEditReinforcement* pTxn = new txnEditReinforcement(pEditPierData->GetPierData()->GetID(),oldReinforcement,newReinforcement);
   return pTxn;
}

IDType CXBeamRateAgent::GetEditBridgeCallbackID()
{
   return INVALID_ID;
}

/////////////////////////////////////////////////////////////////////////////////
// IEditLoadRatingOptionsCallback
CPropertyPage* CXBeamRateAgent::CreatePropertyPage(IEditLoadRatingOptions* pLoadRatingOptions)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CLoadRatingOptionsPage* pPage = new CLoadRatingOptionsPage();

   GET_IFACE(IXBRRatingSpecification,pSpec);
   pPage->m_AnalysisType = pSpec->GetAnalysisMethodForReactions();
   pPage->m_PermitRatingMethod = pSpec->GetPermitRatingMethod();
   pPage->m_EmergencyRatingMethod = pSpec->GetEmergencyRatingMethod();

   GET_IFACE(IXBRProject,pProject);
   pPage->m_MaxLLStepSize = pProject->GetMaxLiveLoadStepSize();
   pPage->m_MaxLoadedLanes = pProject->GetMaxLoadedLanes();
   pPage->m_SystemFactorFlexure = pProject->GetSystemFactorFlexure();
   pPage->m_SystemFactorShear = pProject->GetSystemFactorShear();

   return pPage;
}

txnTransaction* CXBeamRateAgent::OnOK(CPropertyPage* pPage,IEditLoadRatingOptions* pLoadRatingOptions)
{
   CLoadRatingOptionsPage* pLROPage = (CLoadRatingOptionsPage*)pPage;

   GET_IFACE(IXBRRatingSpecification,pSpec);

   txnLoadRatingOptions oldOptions;
   oldOptions.m_AnalysisType = pSpec->GetAnalysisMethodForReactions();
   oldOptions.m_PermitRatingMethod = pSpec->GetPermitRatingMethod();
   oldOptions.m_EmergencyRatingMethod = pSpec->GetEmergencyRatingMethod();

   GET_IFACE(IXBRProject,pProject);
   oldOptions.m_MaxLLStepSize = pProject->GetMaxLiveLoadStepSize();
   oldOptions.m_MaxLoadedLanes = pProject->GetMaxLoadedLanes();
   oldOptions.m_SystemFactorFlexure = pProject->GetSystemFactorFlexure();
   oldOptions.m_SystemFactorShear = pProject->GetSystemFactorShear();

   txnLoadRatingOptions newOptions;
   newOptions.m_AnalysisType = pLROPage->m_AnalysisType;
   newOptions.m_PermitRatingMethod = pLROPage->m_PermitRatingMethod;
   newOptions.m_EmergencyRatingMethod = pLROPage->m_EmergencyRatingMethod;
   newOptions.m_MaxLLStepSize = pLROPage->m_MaxLLStepSize;
   newOptions.m_MaxLoadedLanes = pLROPage->m_MaxLoadedLanes;
   newOptions.m_SystemFactorFlexure = pLROPage->m_SystemFactorFlexure;
   newOptions.m_SystemFactorShear = pLROPage->m_SystemFactorShear;


   txnEditLoadRatingOptions* pTxn = new txnEditLoadRatingOptions(oldOptions,newOptions);
   return pTxn;
}

/////////////////////////////////////////////////////////////////////////////////
//IProjectPropertiesEventSink
HRESULT CXBeamRateAgent::OnProjectPropertiesChanged()
{
   GET_IFACE(IProjectProperties,pProjectProps);
   GET_IFACE(IXBRProjectProperties, pXBRProjectProps);
   pXBRProjectProps->SetBridgeName(pProjectProps->GetBridgeName());
   pXBRProjectProps->SetBridgeID(pProjectProps->GetBridgeID());
   pXBRProjectProps->SetJobNumber(pProjectProps->GetJobNumber());
   pXBRProjectProps->SetEngineer(pProjectProps->GetEngineer());
   pXBRProjectProps->SetCompany(pProjectProps->GetCompany());
   pXBRProjectProps->SetComments(pProjectProps->GetComments());
   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
// IXBRProjectEventSink
HRESULT CXBeamRateAgent::OnProjectChanged()
{
   if ( 0 <= m_PierViewKey )
   {
      CEAFDocument* pDoc = EAFGetDocument();
      pDoc->SetModifiedFlag();
      pDoc->UpdateRegisteredView(m_PierViewKey,0,0,0);
   }

   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
// IEAFCommandCallback
BOOL CXBeamRateAgent::OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
   return m_CommandTarget.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}

BOOL CXBeamRateAgent::GetStatusBarMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // load appropriate string
	if ( rMessage.LoadString(nID) )
	{
		// first newline terminates actual string
      rMessage.Replace('\n','\0');
	}
	else
	{
		// not found
		TRACE1("Warning (CXBeamRateAgent): no message line prompt for ID %d.\n", nID);
	}

   return TRUE;
}

BOOL CXBeamRateAgent::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CString string;
   // load appropriate string
	if ( string.LoadString(nID) )
	{
		// tip is after first newline 
      int pos = string.Find('\n');
      if ( 0 < pos )
         rMessage = string.Mid(pos+1);
	}
	else
	{
		// not found
		TRACE1("Warning (CXBeamRateAgent): no tool tip for ID %d.\n", nID);
	}

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// IEAFProcessCommandLine
BOOL CXBeamRateAgent::ProcessCommandLineOptions(CEAFCommandLineInfo& cmdInfo)
{
   CXBRateCommandLineProcessor processor;
   return processor.ProcessCommandLineOptions(cmdInfo);
}
