///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2016  Washington State Department of Transportation
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

// Plugin.cpp : Implementation of CXBeamRateAppPlugin
#include "stdafx.h"
#include "resource.h"

#include "XBeamRatePlugin_i.h"
#include "XBeamRateAppPlugin.h"

#include "XBeamRatePluginApp.h"

#include "XBeamRateDocTemplate.h"

#include "XBeamRateDoc.h"
#include "XBeamRateView.h"
#include "XBeamRateChildFrame.h"

#include <EAF\EAFUnits.h>
#include <MFCTools\AutoRegistry.h>

#include "XBeamRateCommandLineInfo.h"
#include "XBeamRateCommandLineProcessor.h"

#include <XBeamRateVersionInfoImpl.h>

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateAppPlugin
HRESULT CXBeamRateAppPlugin::FinalConstruct()
{
   if ( !EAFCreateAppUnitSystem(&m_AppUnitSystem) )
   {
      return E_FAIL;
   }

   return S_OK;
}

void CXBeamRateAppPlugin::FinalRelease()
{
}

void CXBeamRateAppPlugin::GetAppUnitSystem(IAppUnitSystem** ppAppUnitSystem)
{
   m_AppUnitSystem.CopyTo(ppAppUnitSystem);
}

BOOL CXBeamRateAppPlugin::Init(CEAFApp* pParent)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   //CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   //CAutoRegistry autoReg(GetAppName());

   //int i = pApp->GetProfileInt(_T("Settings"),_T("Placeholder"),0);

   LoadRegistryValues();

   m_DocumentationImpl.Init(GetName());

   return TRUE;
}

void CXBeamRateAppPlugin::Terminate()
{
   //AFX_MANAGE_STATE(AfxGetStaticModuleState());
   //CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   //CAutoRegistry autoReg(GetAppName());

   //pApp->WriteProfileInt(_T("Settings"),_T("Placeholder"),1);
   SaveRegistryValues();
}

void CXBeamRateAppPlugin::IntegrateWithUI(BOOL bIntegrate)
{
}

std::vector<CEAFDocTemplate*> CXBeamRateAppPlugin::CreateDocTemplates()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   std::vector<CEAFDocTemplate*> templates;

   CXBeamRateDocTemplate* pDocTemplate;
	pDocTemplate = new CXBeamRateDocTemplate(
      IDR_XBEAMRATE,
      NULL,
		RUNTIME_CLASS(CXBeamRateDoc),
		RUNTIME_CLASS(CXBeamRateChildFrame),
		RUNTIME_CLASS(CXBeamRateView),
      NULL,1);

   pDocTemplate->SetPlugin(this);

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
   return CString("XBRate");
}

CString CXBeamRateAppPlugin::GetDocumentationSetName()
{
   return GetName();
}

CString CXBeamRateAppPlugin::GetDocumentationURL()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return m_DocumentationImpl.GetDocumentationURL();
}

CString CXBeamRateAppPlugin::GetDocumentationMapFile()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return m_DocumentationImpl.GetDocumentationMapFile();
}

void CXBeamRateAppPlugin::LoadDocumentationMap()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return m_DocumentationImpl.LoadDocumentationMap();
}

eafTypes::HelpResult CXBeamRateAppPlugin::GetDocumentLocation(LPCTSTR lpszDocSetName,UINT nID,CString& strURL)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return m_DocumentationImpl.GetDocumentLocation(lpszDocSetName,nID,strURL);
}


void CXBeamRateAppPlugin::LoadRegistryValues()
{
   LoadCustomReportInformation();
}

void CXBeamRateAppPlugin::SaveRegistryValues()
{
   SaveCustomReportInformation();
}

void CXBeamRateAppPlugin::LoadCustomReportInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CEAFCustomReportMixin::LoadCustomReportInformation();
}

void CXBeamRateAppPlugin::SaveCustomReportInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CEAFCustomReportMixin::SaveCustomReportInformation();
}

////////////////////////////////////////////////////////
// IEAFCommandLineInfo
CString CXBeamRateAppPlugin::GetUsageMessage()
{
   CXBRateCommandLineInfo cmdInfo;
   return cmdInfo.GetUsageMessage();
}

BOOL CXBeamRateAppPlugin::ProcessCommandLineOptions(CEAFCommandLineInfo& cmdInfo)
{
   CXBRateCommandLineProcessor processor;
   return processor.ProcessCommandLineOptions(cmdInfo);
}
