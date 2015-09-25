#pragma once


#ifndef STRICT
#define STRICT
#endif

#define VC_EXTRALEAN
#define COM_STDMETHOD_CAN_THROW

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#define _USE_MULTITHREADING    // When defined, multi-threading is used

#include <XBeamRateVersion.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxpriv.h>        // Private MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxcmn.h>

#include <atlbase.h>
#include <atlcom.h>

using namespace ATL;

#include <XBeamRateTypes.h>

#include <WBFLAll.h>
#include <WBFLAtlExt.h>
#include <WBFLCore.h>


#if defined _DEBUG
#define WBFL_DEBUG
#endif

#include <WBFLDebug.h>

#include <AgentTools.h>
#include <System\System.h>

