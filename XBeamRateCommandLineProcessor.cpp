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

/****************************************************************************
CLASS
   CXBRateCommandLineProcessor
****************************************************************************/
#include "stdafx.h"
#include "XBeamRateCommandLineProcessor.h"
#include "XBeamRateCommandLineInfo.h"

#include <EAF\EAFApp.h>
#include <EAF\EAFDocument.h>

#include <IFace\Test.h>

CXBRateCommandLineProcessor::CXBRateCommandLineProcessor()
{
}

CXBRateCommandLineProcessor::~CXBRateCommandLineProcessor()
{
}

BOOL CXBRateCommandLineProcessor::ProcessCommandLineOptions(CEAFCommandLineInfo& cmdInfo)
{
   // cmdInfo is the command line information from the application. The application
   // doesn't know about this plug-in at the time the command line parameters are parsed
   //
   // Re-parse the parameters with our own command line information object
   CXBRateCommandLineInfo xbrCmdInfo;
   EAFGetApp()->ParseCommandLine(xbrCmdInfo);
   cmdInfo = xbrCmdInfo;

   if (xbrCmdInfo.m_bError)
   {
      return FALSE;
   }
   else if (xbrCmdInfo.m_bMyParam)
   {
      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);
      GET_IFACE2(pBroker,IXBRTest,pTest);

      CString strResultsFile;
      if ( !GetResultsFileName(xbrCmdInfo.m_strFileName,strResultsFile) )
      {
         CString msg = CString(_T("Error - Test input files must have .xbr extension"));
         ::AfxMessageBox(msg);
         return FALSE;
      }

      HRESULT hr = pTest->RunTest(strResultsFile);
      if ( FAILED(hr) )
      {
         CString msg = CString(_T("Error - Running test on file "))+xbrCmdInfo.m_strFileName;
         ::AfxMessageBox(msg);
      }
      return TRUE; // command line parameters handled
   }

   BOOL bHandled = FALSE;
   CEAFMainFrame* pFrame = EAFGetMainFrame();
   CEAFDocument* pDoc = pFrame->GetDocument();
   if ( pDoc )
   {
      bHandled = pDoc->ProcessCommandLineOptions(cmdInfo);
   }

   // If we get this far and there is one parameter and it isn't a file name and it isn't handled -OR-
   // if there is more than one parameter and it isn't handled there is something wrong
   if ( ((1 == xbrCmdInfo.m_Count && xbrCmdInfo.m_nShellCommand != CCommandLineInfo::FileOpen) || (1 < xbrCmdInfo.m_Count)) && !bHandled )
   {
      cmdInfo.m_bError = TRUE;
      bHandled = TRUE;
   }

   return bHandled;
}

BOOL CXBRateCommandLineProcessor::GetResultsFileName(LPCTSTR lpszSourceFile,CString& strResultsFile)
{
   CString strSourceFile(lpszSourceFile);
   strSourceFile.MakeLower();
   int loc = strSourceFile.Find(_T(".xbr"),0);
   if ( 0 < loc )
   {
      strResultsFile.Format(_T("%s.dbr"),strSourceFile.Left(strSourceFile.GetLength()-4));
      return TRUE;
   }
   return FALSE;
}
