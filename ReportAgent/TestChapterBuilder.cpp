///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
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

#include "StdAfx.h"
#include "TestChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\Project.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   CTestChapterBuilder
****************************************************************************/
CTestChapterBuilder::CTestChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CTestChapterBuilder::GetName() const
{
   return TEXT("Test Chapter");
}

rptChapter* CTestChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   USES_CONVERSION;

   CEAFBrokerReportSpecification* pBrokerSpec = dynamic_cast<CEAFBrokerReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pBrokerSpec->GetBroker(&pBroker);

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );

   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   rptRcTable* pTable = new rptRcTable(2,0);
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("Column");
   (*pTable)(0,1) << COLHDR(_T("Height"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());

   RowIndexType tableRowIdx = 1;
   ColumnIndexType nColumns = pProject->GetColumnCount();
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++, tableRowIdx++ )
   {
      ColumnIndexType tableColIdx = 0;
      Float64 height = pProject->GetColumnHeight(colIdx);
      (*pTable)(tableRowIdx,tableColIdx++) << colIdx;
      (*pTable)(tableRowIdx,tableColIdx++) << length.SetValue(height);
   }
   
   return pChapter;
}


CChapterBuilder* CTestChapterBuilder::Clone() const
{
   return new CTestChapterBuilder;
}
