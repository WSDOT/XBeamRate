// XBeamRateDoc.cpp : implementation of the CXBeamRateDoc class
//

#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePluginApp.h"
#include "XBeamRateDoc.h"
#include "XBeamRateDocProxyAgent.h"

#include <XBeamRateCatCom.h>

#include <EAF\EAFMainFrame.h>

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
}

CXBeamRateDoc::~CXBeamRateDoc()
{
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

   return TRUE;
}

CATID CXBeamRateDoc::GetAgentCategoryID()
{
   return CATID_XBeamRateAgent;
}

BOOL CXBeamRateDoc::OnNewDocument()
{
	if (!CEAFBrokerDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

HRESULT CXBeamRateDoc::WriteTheDocument(IStructuredSave* pStrSave)
{
   return S_OK;
}

HRESULT CXBeamRateDoc::LoadTheDocument(IStructuredLoad* pStrLoad)
{
   return S_OK;
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

void CXBeamRateDoc::BrokerShutDown()
{
   CEAFBrokerDocument::BrokerShutDown();

   m_pMyDocProxyAgent = NULL;
}
