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

// PGSuperExporter.h : Declaration of the CPGSuperExporter

#ifndef __PGSUPEREXPORTER_H_
#define __PGSUPEREXPORTER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPGSuperDataExporter
class ATL_NO_VTABLE CPGSuperDataExporter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPGSuperDataExporter, &CLSID_PGSuperDataExporter>,
   public IPGSDataExporter
{
public:
	CPGSuperDataExporter()
	{
	}

   HRESULT FinalConstruct();

DECLARE_REGISTRY_RESOURCEID(IDR_PGSUPEREXPORTER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPGSuperDataExporter)
   COM_INTERFACE_ENTRY(IPGSDataExporter)
END_COM_MAP()

// IPGSDataExporter
public:
   STDMETHOD(Init)(UINT nCmdID);
   STDMETHOD(GetMenuText)(/*[out,retval]*/BSTR*  bstrText);
   STDMETHOD(GetBitmapHandle)(/*[out]*/HBITMAP* phBmp);
   STDMETHOD(GetCommandHintText)(BSTR*  bstrText);
   STDMETHOD(Export)(/*[in]*/IBroker* pBroker);
};

#endif //__PGSUPEREXPORTER_H_
