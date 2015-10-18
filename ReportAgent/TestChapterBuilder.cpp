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

#include "StdAfx.h"
#include "TestChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\RatingSpecification.h>

#include "XBeamRateReportSpecification.h"

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

   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptMomentUnitValue, moment, pDisplayUnits->GetMomentUnit(), false );
   INIT_UV_PROTOTYPE( rptForceSectionValue, shear, pDisplayUnits->GetShearUnit(), false);
   INIT_UV_PROTOTYPE( rptForcePerLengthUnitValue, fpl, pDisplayUnits->GetForcePerLengthUnit(), false);

   INIT_UV_PROTOTYPE( rptForceUnitValue, force, pDisplayUnits->GetGeneralForceUnit(), true );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, location, pDisplayUnits->GetSpanLengthUnit(), true );

   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   //*pPara << _T("Project Name = ") << pProject->GetProjectName() << rptNewLine;

   rptRcTable* pTable = new rptRcTable(2,0);
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("Column");
   (*pTable)(0,1) << COLHDR(_T("Height"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());


   RowIndexType tableRowIdx = 1;
   ColumnIndexType nColumns = pProject->GetColumnCount(pierID);
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++, tableRowIdx++ )
   {
      ColumnIndexType tableColIdx = 0;
      Float64 height = pProject->GetColumnHeight(pierID,colIdx);
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
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID);
   RowIndexType row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 Mz = pResults->GetMoment(pierID,xbrTypes::pftLowerXBeam,poi);
      sysSectionValue V = pResults->GetShear(pierID,xbrTypes::pftLowerXBeam,poi);
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
      Float64 Mz = pResults->GetMoment(pierID,xbrTypes::pftUpperXBeam,poi);
      sysSectionValue V = pResults->GetShear(pierID,xbrTypes::pftUpperXBeam,poi);
      (*pTable)(row,0) << poi.GetID();
      (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());
      (*pTable)(row,2) << moment.SetValue(Mz);
      (*pTable)(row,3) << shear.SetValue(V);

      row++;
   }

   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
   const std::vector<LowerXBeamLoad>& vLoads = pProductForces->GetLowerCrossBeamLoading(pierID);
   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Lower XBeam Dead Load");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << COLHDR(_T("Xstart"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,1) << COLHDR(_T("Xend"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("Wstart"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
   (*pTable)(0,3) << COLHDR(_T("Wend"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
   row = 1;
   BOOST_FOREACH(const LowerXBeamLoad& load,vLoads)
   {
      (*pTable)(row,0) << length.SetValue(load.Xs);
      (*pTable)(row,1) << length.SetValue(load.Xe);
      (*pTable)(row,2) << fpl.SetValue(load.Ws);
      (*pTable)(row,3) << fpl.SetValue(load.We);
      row++;
   }

   fpl.ShowUnitTag(true);
   *pPara << _T("Upper XBeam Dead Load, w = ") << fpl.SetValue(pProductForces->GetUpperCrossBeamLoading(pierID)) << rptNewLine;
   *pPara << rptNewLine;

   // Write live load results for design and legal cases
   GET_IFACE2(pBroker,IXBRRatingSpecification,pRatingSpec);
   int nRatingTypes = (pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmAASHTO ? 6 : 4);
   for ( int i = 0; i < nRatingTypes; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      IndexType nVehicles = pProject->GetLiveLoadReactionCount(pierID,ratingType);
      if ( nVehicles == 0 )
      {
         continue;
      }

      std::_tstring strLiveLoadName = pProject->GetLiveLoadName(pierID,ratingType,INVALID_INDEX);
/*
      // Live Load Moment Results
      pTable = new rptRcTable(6,0);
      pTable->TableCaption() << strLiveLoadName.c_str();
      *pPara << pTable << rptNewLine;
      (*pTable)(0,0) << _T("POI ID");
      (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      (*pTable)(0,2) << COLHDR(_T("Mmin"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
      (*pTable)(0,3) << _T("Mmin Configuration");
      (*pTable)(0,4) << COLHDR(_T("Mmax"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
      (*pTable)(0,5) << _T("Mmax Configuration");
      row = pTable->GetNumberOfHeaderRows();
      BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
      {
         (*pTable)(row,0) << poi.GetID();
         (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());

         Float64 Mmin,Mmax;
         VehicleIndexType minVehIdx,maxVehIdx;
         pResults->GetMoment(pierID,ratingType,poi,&Mmin,&Mmax,&minVehIdx,&maxVehIdx);

         Float64 vehMmin, vehMmax, dummy;
         WheelLineConfiguration minConfig, maxConfig;
         pResults->GetMoment(pierID,ratingType,minVehIdx,poi,&vehMmin,&dummy,&minConfig,NULL);
         pResults->GetMoment(pierID,ratingType,maxVehIdx,poi,&dummy,&vehMmax,NULL,&maxConfig);

         ATLASSERT(IsEqual(Mmin,vehMmin));
         ATLASSERT(IsEqual(Mmax,vehMmax));

         (*pTable)(row,2) << moment.SetValue(Mmin);
         
         (*pTable)(row,3) << pProject->GetLiveLoadName(pierID,ratingType,minVehIdx) << rptNewLine;
         BOOST_FOREACH(WheelLinePlacement& placement,minConfig)
         {
            (*pTable)(row,3) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
         }

         (*pTable)(row,4) << moment.SetValue(Mmax);
         
         (*pTable)(row,5) << pProject->GetLiveLoadName(pierID,ratingType,maxVehIdx) << rptNewLine;
         BOOST_FOREACH(WheelLinePlacement& placement,maxConfig)
         {
            (*pTable)(row,5) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
         }

         row++;
      }

      // Live Load Shear Results
      pTable = new rptRcTable(6,0);
      pTable->TableCaption() << strLiveLoadName.c_str();
      *pPara << pTable << rptNewLine;
      (*pTable)(0,0) << _T("POI ID");
      (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      (*pTable)(0,2) << COLHDR(_T("Vmin"), rptForceUnitTag, pDisplayUnits->GetShearUnit());
      (*pTable)(0,3) << _T("Vmin Configuration");
      (*pTable)(0,4) << COLHDR(_T("Vmax"), rptForceUnitTag, pDisplayUnits->GetShearUnit());
      (*pTable)(0,5) << _T("Vmax Configuration");
      row = pTable->GetNumberOfHeaderRows();
      BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
      {
         (*pTable)(row,0) << poi.GetID();
         (*pTable)(row,1) << length.SetValue(poi.GetDistFromStart());

         sysSectionValue Vmin,Vmax;
         VehicleIndexType minLeftVehIdx,minRightVehIdx,maxLeftVehIdx,maxRightVehIdx;
         pResults->GetShear(pierID,ratingType,poi,&Vmin,&Vmax,&minLeftVehIdx,&minRightVehIdx,&maxLeftVehIdx,&maxRightVehIdx);

         sysSectionValue vehVmin, vehVmax, dummy;
         WheelLineConfiguration minLeftConfig, minRightConfig, maxLeftConfig, maxRightConfig;
         pResults->GetShear(pierID,ratingType,minLeftVehIdx, poi,&vehVmin,&dummy,&minLeftConfig,NULL,NULL,NULL);
         pResults->GetShear(pierID,ratingType,minRightVehIdx,poi,&vehVmin,&dummy,NULL,&minRightConfig,NULL,NULL);
         pResults->GetShear(pierID,ratingType,maxLeftVehIdx, poi,&dummy,&vehVmax,NULL,NULL,&maxLeftConfig,NULL);
         pResults->GetShear(pierID,ratingType,maxRightVehIdx,poi,&dummy,&vehVmax,NULL,NULL,NULL,&maxRightConfig);

         ATLASSERT(IsEqual(Vmin.Left(),vehVmin.Left()));
         ATLASSERT(IsEqual(Vmax.Left(),vehVmax.Left()));
         ATLASSERT(IsEqual(Vmin.Right(),vehVmin.Right()));
         ATLASSERT(IsEqual(Vmax.Right(),vehVmax.Right()));

         (*pTable)(row,2) << shear.SetValue(Vmin);
         
         (*pTable)(row,3) << pProject->GetLiveLoadName(pierID,ratingType,minLeftVehIdx) << rptNewLine;
         BOOST_FOREACH(WheelLinePlacement& placement,minLeftConfig)
         {
            (*pTable)(row,3) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
         }

         if ( minLeftVehIdx != minRightVehIdx )
         {
            (*pTable)(row,3) << _T("--------------") << rptNewLine;
            (*pTable)(row,3) << pProject->GetLiveLoadName(pierID,ratingType,minRightVehIdx) << rptNewLine;
            BOOST_FOREACH(WheelLinePlacement& placement,minRightConfig)
            {
               (*pTable)(row,3) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
            }
         }

         (*pTable)(row,4) << shear.SetValue(Vmax);
         
         (*pTable)(row,5) << pProject->GetLiveLoadName(pierID,ratingType,maxLeftVehIdx) << rptNewLine;
         BOOST_FOREACH(WheelLinePlacement& placement,maxLeftConfig)
         {
            (*pTable)(row,5) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
         }

         if ( maxLeftVehIdx != maxRightVehIdx )
         {
            (*pTable)(row,5) << _T("--------------") << rptNewLine;
            (*pTable)(row,5) << pProject->GetLiveLoadName(pierID,ratingType,maxRightVehIdx) << rptNewLine;
            BOOST_FOREACH(WheelLinePlacement& placement,maxRightConfig)
            {
               (*pTable)(row,5) << force.SetValue(placement.P) << _T(" @ ") << location.SetValue(placement.Xxb) << rptNewLine;
            }
         }

         row++;
      }
*/
   }
   pTable = new rptRcTable(4,0);
   pTable->TableCaption() << _T("Capacity");
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("POI ID");
   (*pTable)(0,1) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,2) << COLHDR(_T("+Mn"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
   (*pTable)(0,3) << COLHDR(_T("-Mn"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());

   GET_IFACE2(pBroker,IXBRMomentCapacity,pLoadRating);
   row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 pM = pLoadRating->GetMomentCapacity(pierID,xbrTypes::Stage2,poi,true);
      Float64 nM = pLoadRating->GetMomentCapacity(pierID,xbrTypes::Stage2,poi,false);
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
