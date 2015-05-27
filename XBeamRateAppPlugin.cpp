// Plugin.cpp : Implementation of CXBeamRateAppPlugin
#include "stdafx.h"
#include "resource.h"

#include "XBeamRatePlugin_i.h"
#include "XBeamRateAppPlugin.h"

#include "XBeamRatePluginApp.h"

#include "XBeamRateDoc.h"
#include "XBeamRateView.h"
#include "XBeamRateChildFrame.h"

#include <MFCTools\AutoRegistry.h>

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateAppPlugin
HRESULT CXBeamRateAppPlugin::FinalConstruct()
{
   return S_OK;
}

void CXBeamRateAppPlugin::FinalRelease()
{
}

BOOL CXBeamRateAppPlugin::Init(CEAFApp* pParent)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   CAutoRegistry autoReg(GetAppName());

   int i = pApp->GetProfileInt(_T("Settings"),_T("Placeholder"),0);

   return TRUE;
}

void CXBeamRateAppPlugin::Terminate()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   CAutoRegistry autoReg(GetAppName());

   pApp->WriteProfileInt(_T("Settings"),_T("Placeholder"),1);
}

void CXBeamRateAppPlugin::IntegrateWithUI(BOOL bIntegrate)
{
}

std::vector<CEAFDocTemplate*> CXBeamRateAppPlugin::CreateDocTemplates()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   std::vector<CEAFDocTemplate*> templates;

   CEAFDocTemplate* pDocTemplate;
	pDocTemplate = new CEAFDocTemplate(
      IDR_XBEAMRATE,
      NULL,
		RUNTIME_CLASS(CXBeamRateDoc),
		RUNTIME_CLASS(CXBeamRateChildFrame),
		RUNTIME_CLASS(CXBeamRateView),
      NULL,1);

   pDocTemplate->SetPlugin(this);
   pDocTemplate->CreateDefaultItem(AfxGetApp()->LoadStandardIcon(IDI_WINLOGO));

   templates.push_back(pDocTemplate);
   return templates;
}

HMENU CXBeamRateAppPlugin::GetSharedMenuHandle()
{
   return NULL;
}

UINT CXBeamRateAppPlugin::GetDocumentResourceID()
{
   return IDR_XBEAMRATE;
}

CString CXBeamRateAppPlugin::GetName()
{
   return CString("XBeam Rate");
}
