///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2025  Washington State Department of Transportation
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


// ReportAgent.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f ReportAgentps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "dllmain.h"
#include "initguid.h"
#include "ReportAgent.h"

#include "ReportAgentCLSID.h"

#include <WBFLCore_i.c>

#include "ReportAgentImp.h"

#include "XBeamRateCatCom.h"
#include <System\ComCatMgr.h>

#include <IReportManager.h>
#include <EAF\EAFDisplayUnits.h>
#include <EAF\EAFDocument.h>
#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\RatingSpecification.h>
#include <IFace\VersionInfo.h>
#include <IFace\Pier.h>

#include <..\..\PGSuper\Include\IFace\Project.h>
#include <..\..\PGSuper\Include\IFace\Selection.h>
#include <..\..\PGSuper\Include\IFace\Views.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

HRESULT RegisterAgent(bool bRegister)
{
   HRESULT hr = S_OK;
   hr = WBFL::System::ComCatMgr::RegWithCategory(CLSID_ReportAgent,CATID_XBeamRateAgent,bRegister);
   if ( FAILED(hr) )
      return hr;

   return S_OK;
}

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
   HRESULT hr = _AtlModule.DllRegisterServer();

   return RegisterAgent(true);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
   RegisterAgent(false);
	return S_OK;
}
