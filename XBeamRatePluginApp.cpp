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

// BridgeLinkTestPlugin.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f BridgeLinkTestPluginps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <MFCTools\VersionInfo.h>

#include <WBFLGeometry.h>
#include <WBFLUnitServer.h>
#include <WBFLDManip.h>
#include <WBFLDManipTools.h>
#include <WBFLReportManagerAgent.h>
#include <WBFLGraphManagerAgent.h>


#include <initguid.h>
#include <EAF\EAFAppPlugin.h>
#include "CLSID.h"
#include "SectionCut.h"

// EAF Common Interfaces
#include <EAF\EAFDisplayUnits.h>

// XBeam Rate Interfaces
#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\Pier.h>
#include <IFace\RatingSpecification.h>
#include <IFace\Test.h>

// PGSuper/PGSplice Interfaces
#include <IFace\ExtendUI.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <\ARP\PGSuper\Include\IFace\DocumentType.h>
#include <IFace\EditByUI.h>
#include <IFace\Bridge.h>
#include <IFace\Selection.h>
#include <IFace\ViewEvents.h>
#include <IFace\Intervals.h>
#include <\ARP\PGSuper\Include\IFace\PointOfInterest.h>
#include <IFace\Alignment.h>

#include "PGSComponentInfo.h"

#include <WBFLCore_i.c>
#include <WBFLUnitServer_i.c>
#include <WBFLReportManagerAgent_i.c>
#include <WBFLGraphManagerAgent_i.c>
#include <WBFLGeometry_i.c>
#include <WBFLDManip_i.c>
#include <WBFLDManipTools_i.c>
#include <DManip\DManip.h>

#include "XBeamRateAppPlugin.h"
#include "XBeamRateDocProxyAgent.h"
#include <XBeamRateCatCom.h>

#include <BridgeLinkCATID.h>
#include <PGSuperCatCom.h>
#include <PGSpliceCatCom.h>
#include <System\ComCatMgr.h>
#include <Plugins\PGSuperIEPlugin.h>

#include "XBeamRatePluginApp.h"

#include <EAF\EAFDocTemplate.h>
#include "XBeamRateDoc.h"
#include "ComponentInfo.h"
#include "PGSuperExporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
   OBJECT_ENTRY(CLSID_XBeamRateAppPlugin, CXBeamRateAppPlugin)
   OBJECT_ENTRY(CLSID_XBeamRateComponentInfo, CXBeamRateComponentInfo)
   OBJECT_ENTRY(CLSID_PGSuperDataExporter,    CPGSuperDataExporter)
END_OBJECT_MAP()

BEGIN_MESSAGE_MAP(CXBeamRatePluginApp, CWinApp)
	//{{AFX_MSG_MAP(CXBeamRatePluginApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXBeamRatePluginApp theApp;

CXBeamRatePluginApp::CXBeamRatePluginApp()
{
}

BOOL CXBeamRatePluginApp::InitInstance()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GXInit();

   SetRegistryKey( _T("Washington State Department of Transportation") );

   // Using a shared menu
   // See MSKB Article ID: Q118435, "Sharing Menus Between MDI Child Windows"
   m_hSharedMenu = nullptr; /*::LoadMenu( m_hInstance, MAKEINTRESOURCE(IDR_???) );*/

   _Module.Init(ObjectMap, m_hInstance);
   return CWinApp::InitInstance();
}

int CXBeamRatePluginApp::ExitInstance()
{
   // if the doc template uses a shared menu, destroy it here
   // release the shared menu
   //::DestroyMenu( m_hSharedMenu );

   GXForceTerminate();
   _Module.Term();
   return CWinApp::ExitInstance();
}

CString CXBeamRatePluginApp::GetVersion(bool bIncludeBuildNumber) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CWinApp* pApp = AfxGetApp();
   CString strExe( pApp->m_pszExeName );
   strExe += ".dll";

   CVersionInfo verInfo;
   verInfo.Load(strExe);
   
#if defined _DEBUG || defined _BETA_VERSION
   // always include the build number in debug and beta versions
   bIncludeBuildNumber = true;
#endif
   CString strVersion = verInfo.GetProductVersionAsString(bIncludeBuildNumber);

   return strVersion;
}

BOOL CXBeamRatePluginApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
   // in order scan the message map for this plugin, the module state must be set
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return CWinApp::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

void RegisterComponents(bool bRegister)
{
   // Component information objects
   sysComCatMgr::RegWithCategory(CLSID_PGSuperComponentInfo,CATID_PGSuperComponentInfo,bRegister);
   sysComCatMgr::RegWithCategory(CLSID_PGSpliceComponentInfo,CATID_PGSpliceComponentInfo,bRegister);

   // These components are for the "stand alone" XBeam Rate application
   sysComCatMgr::RegWithCategory(CLSID_XBeamRateAppPlugin,    CATID_BridgeLinkAppPlugin, bRegister);
   sysComCatMgr::RegWithCategory(CLSID_XBeamRateComponentInfo,CATID_BridgeLinkComponentInfo,bRegister);

   // These components are for the PGSuper and PGSplice extension agents
   sysComCatMgr::RegWithCategory(CLSID_XBeamRateAgent, CATID_PGSuperExtensionAgent,  bRegister);
   sysComCatMgr::RegWithCategory(CLSID_XBeamRateAgent, CATID_PGSpliceExtensionAgent, bRegister);

   // These components are exporter plugins
   sysComCatMgr::RegWithCategory(CLSID_PGSuperDataExporter,   CATID_PGSuperDataExporter,    bRegister);
   sysComCatMgr::RegWithCategory(CLSID_PGSuperDataExporter,   CATID_PGSpliceDataExporter,   bRegister);
}

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib

   HRESULT hr = _Module.RegisterServer(FALSE);
   if ( FAILED(hr) )
      return hr;

   RegisterComponents(true);

   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
   RegisterComponents(false);

   return _Module.UnregisterServer(FALSE);
}
