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
   //AFX_MANAGE_STATE(AfxGetStaticModuleState());
   //CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   //CAutoRegistry autoReg(GetAppName());

   //int i = pApp->GetProfileInt(_T("Settings"),_T("Placeholder"),0);

   LoadRegistryValues();

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
   return CString("XBeam Rate");
}


void CXBeamRateAppPlugin::LoadRegistryValues()
{
   LoadReportOptions();
}

void CXBeamRateAppPlugin::LoadReportOptions()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   CAutoRegistry autoReg(GetAppName());

   // Favorite reports
   m_bDisplayFavoriteReports = pApp->GetProfileInt(_T("Options"),_T("DisplayFavoriteReports"),FALSE);

   // Favorite report names are stored as Tab separated values
   CString ReportList = pApp->GetProfileString(_T("Options"),_T("FavoriteReportsList"),_T(""));
   m_FavoriteReports.clear();
   sysTokenizer tokenizer(_T("\t"));
   tokenizer.push_back(ReportList);
   sysTokenizer::iterator it = tokenizer.begin();
   while( it != tokenizer.end() )
   {
      m_FavoriteReports.push_back( *it );
      it++;
   }

   // Custom Reports
   m_CustomReports.LoadFromRegistry(pApp);
}

void CXBeamRateAppPlugin::SaveRegistryValues()
{
   SaveReportOptions();
}

void CXBeamRateAppPlugin::SaveReportOptions()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CXBeamRatePluginApp* pApp = (CXBeamRatePluginApp*)AfxGetApp();

   CAutoRegistry autoReg(GetAppName());

   // Favorite reports
   pApp->WriteProfileInt(_T("Options"),_T("DisplayFavoriteReports"),m_bDisplayFavoriteReports);

   // report names are stored as Tab separated values
   CString Favorites;
   std::vector<std::_tstring>::const_iterator it = m_FavoriteReports.begin();
   while (it != m_FavoriteReports.end())
   {
      if (it!= m_FavoriteReports.begin())
      {
         Favorites += _T("\t");
      }

      Favorites += it->c_str();

      it++;
   }

   pApp->WriteProfileString(_T("Options"),_T("FavoriteReportsList"),Favorites);

   // Custom Reports
   m_CustomReports.SaveToRegistry(pApp);
}


bool CXBeamRateAppPlugin::GetDoDisplayFavoriteReports() const
{
   return m_bDisplayFavoriteReports!=FALSE;
}

void CXBeamRateAppPlugin::SetDoDisplayFavoriteReports(bool doDisplay)
{
   m_bDisplayFavoriteReports = doDisplay ? TRUE : FALSE;
}

const std::vector<std::_tstring>& CXBeamRateAppPlugin::GetFavoriteReports() const
{
   return m_FavoriteReports;
}

void CXBeamRateAppPlugin::SetFavoriteReports(const std::vector<std::_tstring>& reports)
{
   m_FavoriteReports = reports;
}

const CEAFCustomReports& CXBeamRateAppPlugin::GetCustomReports() const
{
   return m_CustomReports;
}

void CXBeamRateAppPlugin::SetCustomReports(const CEAFCustomReports& reports)
{
   m_CustomReports = reports;
}
