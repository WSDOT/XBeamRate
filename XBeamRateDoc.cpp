// XBeamRateDoc.cpp : implementation of the CXBeamRateDoc class
//

#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePluginApp.h"
#include "XBeamRateDoc.h"
#include "XBeamRateDocProxyAgent.h"

#include <XBeamRateCatCom.h>

#include <EAF\EAFMainFrame.h>

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

   // The reporting sub-system doesn't use the WBFLUnitServer implementation. It uses the old regular C++
   // units sytem. That system is in kms units, so we will create a unit server here also in the kms system
   // so that the data, after loading is in set of consistent base units we want.
   // If the report system could handle the WBFLUnitServer, the <ConsistentUnits> declaration in the
   // instance document would work throughout this program because we are working exclusively in
   // consistent units.
   m_DocUnitServer.CoCreateInstance(CLSID_UnitServer);
   m_DocUnitServer->SetBaseUnits(CComBSTR(unitSysUnitsMgr::GetMassUnit().UnitTag().c_str()),
                            CComBSTR(unitSysUnitsMgr::GetLengthUnit().UnitTag().c_str()),
                            CComBSTR(unitSysUnitsMgr::GetTimeUnit().UnitTag().c_str()),
                            CComBSTR(unitSysUnitsMgr::GetTemperatureUnit().UnitTag().c_str()),
                            CComBSTR(unitSysUnitsMgr::GetAngleUnit().UnitTag().c_str()));  
   m_DocUnitServer->QueryInterface(&m_DocConvert);

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

void CXBeamRateDoc::LoadToolbarState()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   __super::LoadToolbarState();
}

void CXBeamRateDoc::SaveToolbarState()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   __super::SaveToolbarState();
}

CString CXBeamRateDoc::GetToolbarSectionName()
{
   return "XBeamRate";
}

BOOL CXBeamRateDoc::GetStatusBarMessageString(UINT nID,CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return __super::GetStatusBarMessageString(nID,rMessage);
}

BOOL CXBeamRateDoc::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return __super::GetToolTipMessageString(nID,rMessage);
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

void CXBeamRateDoc::OnCreateFinalize()
{
   CEAFBrokerDocument::OnCreateFinalize();
   PopulateReportMenu();
   PopulateGraphMenu();
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
