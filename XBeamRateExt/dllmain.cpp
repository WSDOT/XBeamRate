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

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <initguid.h>

#include <EAF\EAFDisplayUnits.h>
#include <IFace\XBeamRateAgent.h>
#include <IFace\AnalysisResults.h>
#include <IFace\Project.h>
#include <IFace\LoadRating.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <EAF\EAFStatusCenter.h>

#include <WBFLGeometry_i.c>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt
// See main.cpp for the implementation of this class
//

class CXBeamRateExt : public CWinApp
{
public:
	CXBeamRateExt();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateExt)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXBeamRateExt)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt

BEGIN_MESSAGE_MAP(CXBeamRateExt, CWinApp)
	//{{AFX_MSG_MAP(CXBeamRateExt)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt construction

CXBeamRateExt::CXBeamRateExt()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXBeamRateExt object

CXBeamRateExt theApp;

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt initialization

BOOL CXBeamRateExt::InitInstance()
{
   CWinApp::InitInstance();
   ::GXInit();


   // Help file defaults to the location of the application
   // In our development environment, it is in the \ARP\BridgeLink folder
   //
   // Change help file name
   CString strHelpFile(m_pszHelpFilePath);
#if defined _DEBUG
#if defined _WIN64
   strHelpFile.Replace(_T("RegFreeCOM\\x64\\Debug\\"),_T(""));
#else
   strHelpFile.Replace(_T("RegFreeCOM\\Win32\\Debug\\"),_T(""));
#endif
#else
   // in a real release, the path doesn't contain RegFreeCOM\\Release, but that's
   // ok... the replace will fail and the string wont be altered.
#if defined _WIN64
   strHelpFile.Replace(_T("RegFreeCOM\\x64\\Release\\"),_T(""));
#else
   strHelpFile.Replace(_T("RegFreeCOM\\Win32\\Release\\"),_T(""));
#endif
#endif

   // rename the file itself
   int loc = strHelpFile.ReverseFind('\\');
   strHelpFile = strHelpFile.Left(loc+1);
   strHelpFile += _T("XBRate.chm");

   free((void*)m_pszHelpFilePath);
   m_pszHelpFilePath = _tcsdup(strHelpFile);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

int CXBeamRateExt::ExitInstance() 
{
   ::GXTerminate();
   return CWinApp::ExitInstance();
}
