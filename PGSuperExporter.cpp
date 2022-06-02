///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2022  Washington State Department of Transportation
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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

STDMETHODIMP CPGSuperDataExporter::GetMenuText(BSTR*  bstrText) const
{
   *bstrText = CComBSTR("Piers to XBRate");
   return S_OK;
}

STDMETHODIMP CPGSuperDataExporter::GetBitmapHandle(HBITMAP* phBmp) const
{
   *phBmp = nullptr;
   return S_OK;
}

STDMETHODIMP CPGSuperDataExporter::GetCommandHintText(BSTR*  bstrText) const
{
   *bstrText = CComBSTR("Export a pier to XBRate\nTool tip text");
   return S_OK;   
}

STDMETHODIMP CPGSuperDataExporter::Export(IBroker* pBroker)
{
   CComPtr<IXBRExport> pExport;
   if ( FAILED(pBroker->GetInterface(IID_IXBRExport,(IUnknown**)&pExport)) )
   {
      AfxMessageBox(_T("XBRate Extension is not enabled. Piers cannot be exported"),MB_ICONEXCLAMATION | MB_OK);
      return E_FAIL;
   }

   return pExport->BatchExport();
}
