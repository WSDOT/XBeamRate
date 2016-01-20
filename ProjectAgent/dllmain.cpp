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


// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "dllmain.h"

CProjectAgentModule _AtlModule;

class CProjectAgentModuleApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CProjectAgentModuleApp, CWinApp)
END_MESSAGE_MAP()

CProjectAgentModuleApp theApp;

BOOL CProjectAgentModuleApp::InitInstance()
{
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

   return CWinApp::InitInstance();
}

int CProjectAgentModuleApp::ExitInstance()
{
   ::GXTerminate();
	return CWinApp::ExitInstance();
}
