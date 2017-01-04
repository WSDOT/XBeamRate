///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2017  Washington State Department of Transportation
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

// Plugin.h : Declaration of the CXBeamRateAppPlugin

#ifndef __PLUGIN_H_
#define __PLUGIN_H_

#include "resource.h"       // main symbols
#include "XBeamRatePlugin_i.h"
#include <EAF\EAFAppPlugIn.h>
#include <WBFLUnitServer.h>
#include <EAF\EAFCustomReport.h>
#include <EAF\EAFAppPluginDocumentationImpl.h>

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateAppPlugin
class ATL_NO_VTABLE CXBeamRateAppPlugin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXBeamRateAppPlugin, &CLSID_XBeamRateAppPlugin>,
   public CEAFCustomReportMixin,
	public IEAFAppPlugin,
   public IEAFAppCommandLine
{
public:
	CXBeamRateAppPlugin()
	{
      m_bDisplayFavoriteReports = FALSE;
	}

   HRESULT FinalConstruct();
   void FinalRelease();

   LPCTSTR GetAppName() { return _T("XBeamRate"); }

   void GetAppUnitSystem(IAppUnitSystem** ppAppUnitSystem);

DECLARE_REGISTRY_RESOURCEID(IDR_XBEAMRATEAPP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CXBeamRateAppPlugin)
	COM_INTERFACE_ENTRY(IEAFAppPlugin)
   COM_INTERFACE_ENTRY(IEAFAppCommandLine)
END_COM_MAP()

// IEAFAppPlugin
public:
   virtual BOOL Init(CEAFApp* pParent);
   virtual void Terminate();
   virtual void IntegrateWithUI(BOOL bIntegrate);
   virtual std::vector<CEAFDocTemplate*> CreateDocTemplates();
   virtual HMENU GetSharedMenuHandle();
   virtual UINT GetDocumentResourceID();
   virtual CString GetName();
   virtual CString GetDocumentationSetName();
   virtual CString GetDocumentationURL();
   virtual CString GetDocumentationMapFile();
   virtual void LoadDocumentationMap();
   virtual eafTypes::HelpResult GetDocumentLocation(LPCTSTR lpszDocSetName,UINT nID,CString& strURL);

   virtual void LoadCustomReportInformation();
   virtual void SaveCustomReportInformation();

// IEAFAppCommandLine
public:
   virtual CString GetUsageMessage();
   virtual BOOL ProcessCommandLineOptions(CEAFCommandLineInfo& cmdInfo);

private:
   CComPtr<IAppUnitSystem> m_AppUnitSystem;

   void LoadRegistryValues();
   void SaveRegistryValues();

   CEAFAppPluginDocumentationImpl m_DocumentationImpl;
};

#endif //__PLUGIN_H_
