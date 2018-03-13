///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2018  Washington State Department of Transportation
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

#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePlugin_i.h"
#include "PGSuperComponentInfo.h"
#include <MFCTools\VersionInfo.h>

HRESULT CPGSuperComponentInfo::FinalConstruct()
{
   return S_OK;
}

void CPGSuperComponentInfo::FinalRelease()
{
}

BOOL CPGSuperComponentInfo::Init(CPGSuperDoc* pDoc)
{
   return TRUE;
}

void CPGSuperComponentInfo::Terminate()
{
}

CString CPGSuperComponentInfo::GetName() const
{
   return _T("XBRate Extensions");
}

CString CPGSuperComponentInfo::GetDescription() const
{
   CString strDesc;
   strDesc.Format(_T("Adds cross beam load rating capabilties to PGSuper"));
   return strDesc;
}

HICON CPGSuperComponentInfo::GetIcon() const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return AfxGetApp()->LoadIcon(IDR_XBEAMRATE);
}

bool CPGSuperComponentInfo::HasMoreInfo() const
{
   return false;
}

void CPGSuperComponentInfo::OnMoreInfo() const
{
}
