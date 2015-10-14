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

// PGSuperExporter.cpp : Implementation of CPGSuperExporter
#include "stdafx.h"
#include "XBeamRateAgent.h"
#include "PGSuperExporter.h"

#include <IFace\Project.h>
#include <IFace\Bridge.h>
#include <MFCTools\Prompts.h>
#include <PgsExt\GirderLabel.h>

HRESULT CPGSuperDataExporter::FinalConstruct()
{
   return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPGSuperDataExporter

STDMETHODIMP CPGSuperDataExporter::Init(UINT nCmdID)
{
   return S_OK;
}

STDMETHODIMP CPGSuperDataExporter::GetMenuText(BSTR*  bstrText)
{
   *bstrText = CComBSTR("Pier to XBeam Rate");
   return S_OK;
}

STDMETHODIMP CPGSuperDataExporter::GetBitmapHandle(HBITMAP* phBmp)
{
   *phBmp = NULL;
   return S_OK;
}

STDMETHODIMP CPGSuperDataExporter::GetCommandHintText(BSTR*  bstrText)
{
   *bstrText = CComBSTR("Export a pier to XBeam Rate\nTool tip text");
   return S_OK;   
}

STDMETHODIMP CPGSuperDataExporter::Export(IBroker* pBroker)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE2(pBroker,IBridge,pBridge);
   PierIndexType nPiers = pBridge->GetPierCount();
   CString strPiers;
   for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
   {
      pgsTypes::PierModelType modelType = pBridge->GetPierModelType(pierIdx);
      if ( modelType == pgsTypes::pmtPhysical )
      {
         CString str;
         str.Format(_T("Pier %d"),LABEL_PIER(pierIdx));
         if ( strPiers.GetLength() == 0 )
         {
            strPiers = str;
         }
         else
         {
            strPiers += _T("\n") + str;
         }
      }
   }

   if ( strPiers.GetLength() == 0 )
   {
      AfxMessageBox(_T("There aren't any physical pier models for this bridge. There is nothing to export."));
      return S_OK;
   }
   else
   {
      int result = AfxChoose(_T("Select Pier"),_T("Select pier model to export to XBeam Rate"),strPiers,0,TRUE);
      if ( result < 0 )
      {
         // Cancel button was pressed
         return S_OK;
      }

      PierIndexType pierIdx = (PierIndexType)result;
      CString str;
      str.Format(_T("Pier_%d.xbr"),LABEL_PIER(pierIdx));
   	CFileDialog  fileDlg(FALSE,_T("xbr"),str,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("XBeam Rate File (*.xbr)|*.xbr||"));
   	if (fileDlg.DoModal() == IDOK)
	   {
		   CString strFileName = fileDlg.GetPathName();

         GET_IFACE2(pBroker,IXBRExport,pExport);
         HRESULT hr = pExport->Export(pierIdx,strFileName);
      }
   }
   return S_OK;
}
