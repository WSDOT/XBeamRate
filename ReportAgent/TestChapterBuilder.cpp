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
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>

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
   INIT_UV_PROTOTYPE( rptMomentUnitValue, moment, pDisplayUnits->GetMomentUnit(), false );
   INIT_UV_PROTOTYPE( rptForceSectionValue, shear, pDisplayUnits->GetShearUnit(), false);
   INIT_UV_PROTOTYPE( rptForcePerLengthUnitValue, fpl, pDisplayUnits->GetForcePerLengthUnit(), false);

   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   //*pPara << _T("Project Name = ") << pProject->GetProjectName() << rptNewLine;

   rptRcTable* pTable = new rptRcTable(2,0);
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("Column");
   (*pTable)(0,1) << COLHDR(_T("Height"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());

   RowIndexType tableRowIdx = 1;
   ColumnIndexType nColumns = pProject->GetColumnCount();
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++, tableRowIdx++ )
   {
      ColumnIndexType tableColIdx = 0;
      Float64 height = pProject->GetColumnHeight();
      (*pTable)(tableRowIdx,tableColIdx++) << colIdx;
      (*pTable)(tableRowIdx,tableColIdx++) << length.SetValue(height);
   }

   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Lower XBeam Dead Load");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("POI ID");
   (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("Moment"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
   (*pTable)(0,3) << COLHDR(_T("Shear"), rptForceUnitTag, pDisplayUnits->GetShearUnit());

   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);
   GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest();
   RowIndexType row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 Mz = pResults->GetMoment(pftLowerXBeam,poi);
      sysSectionValue V = pResults->GetShear(pftLowerXBeam,poi);
      (*pTable)(row,0) << poi.GetID();
      (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());
      (*pTable)(row,2) << moment.SetValue(Mz);
      (*pTable)(row,3) << shear.SetValue(V);

      row++;
   }

   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Upper XBeam Dead Load");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("POI ID");
   (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("Moment"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
   (*pTable)(0,3) << COLHDR(_T("Shear"), rptForceUnitTag, pDisplayUnits->GetShearUnit());

   row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 Mz = pResults->GetMoment(pftUpperXBeam,poi);
      sysSectionValue V = pResults->GetShear(pftUpperXBeam,poi);
      (*pTable)(row,0) << poi.GetID();
      (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());
      (*pTable)(row,2) << moment.SetValue(Mz);
      (*pTable)(row,3) << shear.SetValue(V);

      row++;
   }

   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
   const std::vector<LowerXBeamLoad>& loads = pProductForces->GetLowerCrossBeamLoading();
   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Lower XBeam Dead Load");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << COLHDR(_T("Xstart"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,1) << COLHDR(_T("Xend"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("Wstart"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
   (*pTable)(0,3) << COLHDR(_T("Wend"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
   row = 1;
   BOOST_FOREACH(const LowerXBeamLoad& load,loads)
   {
      (*pTable)(row,0) << length.SetValue(load.Xs);
      (*pTable)(row,1) << length.SetValue(load.Xe);
      (*pTable)(row,2) << fpl.SetValue(load.Ws);
      (*pTable)(row,3) << fpl.SetValue(load.We);
      row++;
   }

   fpl.ShowUnitTag(true);
   *pPara << _T("Upper XBeam Dead Load, w = ") << fpl.SetValue(pProductForces->GetUpperCrossBeamLoading()) << rptNewLine;
   *pPara << rptNewLine;

   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Capacity");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("POI ID");
   (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("+Mn"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
   (*pTable)(0,3) << COLHDR(_T("-Mn"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());

   GET_IFACE2(pBroker,IXBRLoadRating,pLoadRating);
   row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 pM = pLoadRating->GetMomentCapacity(poi,true);
      Float64 nM = pLoadRating->GetMomentCapacity(poi,false);
      (*pTable)(row,0) << poi.GetID();
      (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());
      (*pTable)(row,2) << moment.SetValue(pM);
      (*pTable)(row,3) << moment.SetValue(nM);

      row++;
   }

   
   return pChapter;
}


CChapterBuilder* CTestChapterBuilder::Clone() const
{
   return new CTestChapterBuilder;
}
