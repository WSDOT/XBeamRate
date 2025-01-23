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

#pragma once

class CProjectAgentImp;

class CPierExporter
{
public:
   CPierExporter(IBroker* pBroker,CProjectAgentImp* pProjectAgent);
   HRESULT Export(PierIndexType pierIdx);
   HRESULT BatchExport();

private:
   IBroker* m_pBroker; // weak reference
   CProjectAgentImp* m_pProjectAgent;

   INT_PTR GetFileName(const CString& strDefaultFileName,CString& strFileName);
   CString GetDefaultPierExportFile(PierIndexType pierIdx);
   HRESULT ExportPierModel(PierIndexType pierIdx,LPCTSTR lpszPathName);
};