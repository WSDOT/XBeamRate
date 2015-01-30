// BridgeLinkTestPlugin.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f BridgeLinkTestPluginps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include <EAF\EAFAppPlugin.h>
#include "XBeamRatePlugin_i.h"
#include "XBeamRatePlugin_i.c"

#include <IFace\Project.h>

#include <WBFLCore_i.c>

#include "XBeamRateAppPlugin.h"
#include "XBeamRateDocProxyAgent.h"
#include <XBeamRateCatCom.h>

#include <BridgeLinkCATID.h>
#include <System\ComCatMgr.h>

#include "XBeamRatePluginApp.h"

#include <EAF\EAFDocTemplate.h>
#include "XBeamRateDoc.h"
#include "ComponentInfo.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_XBeamRateAppPlugin, CXBeamRateAppPlugin)
OBJECT_ENTRY(CLSID_XBeamRateComponentInfo, CXBeamRateComponentInfo)
END_OBJECT_MAP()

BEGIN_MESSAGE_MAP(CXBeamRatePluginApp, CWinApp)
	//{{AFX_MSG_MAP(CXBeamRatePluginApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXBeamRatePluginApp theApp;

BOOL CXBeamRatePluginApp::InitInstance()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Using a shared menu
   // See MSKB Article ID: Q118435, "Sharing Menus Between MDI Child Windows"
   m_hSharedMenu = NULL; /*::LoadMenu( m_hInstance, MAKEINTRESOURCE(IDR_???) );*/

   _Module.Init(ObjectMap, m_hInstance, &LIBID_XBeamRateLibrary);
   return CWinApp::InitInstance();
}

int CXBeamRatePluginApp::ExitInstance()
{
   // if the doc template uses a shared menu, destroy it here
   // release the shared menu
   //::DestroyMenu( m_hSharedMenu );

   _Module.Term();
   return CWinApp::ExitInstance();
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

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib

   HRESULT hr = _Module.RegisterServer(TRUE);
   if ( FAILED(hr) )
      return hr;

   sysComCatMgr::RegWithCategory(CLSID_XBeamRateAppPlugin, CATID_BridgeLinkAppPlugin, true);

   sysComCatMgr::RegWithCategory(CLSID_XBeamRateComponentInfo,CATID_BridgeLinkComponentInfo,true);

   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
   sysComCatMgr::RegWithCategory(CLSID_XBeamRateAppPlugin, CATID_BridgeLinkAppPlugin, false);

   sysComCatMgr::RegWithCategory(CLSID_XBeamRateComponentInfo,CATID_BridgeLinkComponentInfo,false);

   return _Module.UnregisterServer(TRUE);
}
