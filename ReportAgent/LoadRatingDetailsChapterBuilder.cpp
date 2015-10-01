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
#include "LoadRatingDetailsChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\LoadRating.h>
#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>

#include "XBeamRateReportSpecification.h"

#include <PgsExt\CapacityToDemand.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   CLoadRatingDetailsChapterBuilder
****************************************************************************/
CLoadRatingDetailsChapterBuilder::CLoadRatingDetailsChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CLoadRatingDetailsChapterBuilder::GetName() const
{
   return TEXT("Load Rating Details");
}

rptChapter* CLoadRatingDetailsChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptLengthUnitValue, location, pDisplayUnits->GetSpanLengthUnit(), true );
   INIT_UV_PROTOTYPE( rptMomentUnitValue, moment, pDisplayUnits->GetMomentUnit(), false );
   rptCapacityToDemand rating_factor;

   GET_IFACE2(pBroker,IXBRArtifact,pArtifact);
   GET_IFACE2(pBroker,IXBRRatingSpecification,pRatingSpec);
   GET_IFACE2(pBroker,IXBRProject,pProject);

   xbrTypes::PermitRatingMethod permitRatingMethod = pRatingSpec->GetPermitRatingMethod();

   for ( int i = 0; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;

      rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
      *pChapter << pPara;

      *pPara << ::GetLiveLoadTypeName(ratingType) << rptNewLine;

      IndexType nVehicles = pProject->GetLiveLoadReactionCount(pierID,ratingType);
      for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nVehicles; vehicleIdx++ )
      {
         std::_tstring strLiveLoadName = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);
         const xbrRatingArtifact* pRatingArtifact = pArtifact->GetXBeamRatingArtifact(pierID,ratingType,vehicleIdx);

         for ( int i = 0; i < 2; i++ )
         {
            bool bPositiveMoment = (i == 0 ? true : false);

            rptParagraph* pPara = new rptParagraph;
            *pChapter << pPara;

            CString strTitle;
            strTitle.Format(_T("%s Moment Rating - %s"),(bPositiveMoment ? _T("Positive") : _T("Negative")),strLiveLoadName.c_str());

            bool bIsWSDOTPermitRating = (::IsPermitRatingType(ratingType) && permitRatingMethod == xbrTypes::prmWSDOT ? true : false);
            ColumnIndexType nColumns = (bIsWSDOTPermitRating ? 22 : 21);

            rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(nColumns,strTitle);
            pTable->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
            pTable->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));

            *pPara << pTable << rptNewLine;

            ColumnIndexType col = 0;
            (*pTable)(0,col++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
            (*pTable)(0,col++) << Sub2(symbol(phi),_T("c"));
            (*pTable)(0,col++) << Sub2(symbol(phi),_T("s"));
            (*pTable)(0,col++) << Sub2(symbol(phi),_T("n"));
            (*pTable)(0,col++) << _T("K");
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("n")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("DC"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("DC")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("DW"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("DW")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("CR"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("CR")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("SR"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("SR")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("RE"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("RE")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("PS"));
            (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("PS")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            (*pTable)(0,col++) << Sub2(symbol(gamma),_T("LL"));
            if ( bIsWSDOTPermitRating )
            {
               (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("LL+IM")) << rptNewLine << _T("Permit"), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
               (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("LL+IM")) << rptNewLine << _T("Legal"),  rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            }
            else
            {
               (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("LL+IM")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
            }
            (*pTable)(0,col++) << _T("RF");

            RowIndexType row = pTable->GetNumberOfHeaderRows();
            const xbrRatingArtifact::MomentRatings& momentRatings = pRatingArtifact->GetMomentRatings(bPositiveMoment);
            xbrRatingArtifact::MomentRatings::const_iterator iter(momentRatings.begin());
            xbrRatingArtifact::MomentRatings::const_iterator end(momentRatings.end());
            for ( ; iter != end; iter++, row++ )
            {
               col = 0;
               const xbrPointOfInterest& poi = iter->first;
               const xbrMomentRatingArtifact& artifact = iter->second;

               ATLASSERT(poi == artifact.GetPointOfInterest());

               (*pTable)(row,col++) << location.SetValue(poi.GetDistFromStart());
               (*pTable)(row,col++) << artifact.GetSystemFactor();
               (*pTable)(row,col++) << artifact.GetConditionFactor();
               (*pTable)(row,col++) << artifact.GetCapacityReductionFactor();
               (*pTable)(row,col++) << artifact.GetMinimumReinforcementFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetNominalMomentCapacity());
               (*pTable)(row,col++) << artifact.GetDeadLoadFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetDeadLoadMoment());
               (*pTable)(row,col++) << artifact.GetWearingSurfaceFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetWearingSurfaceMoment());
               (*pTable)(row,col++) << artifact.GetCreepFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetCreepMoment());
               (*pTable)(row,col++) << artifact.GetShrinkageFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetShrinkageMoment());
               (*pTable)(row,col++) << artifact.GetRelaxationFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetRelaxationMoment());
               (*pTable)(row,col++) << artifact.GetSecondaryEffectsFactor();
               (*pTable)(row,col++) << moment.SetValue(artifact.GetSecondaryEffectsMoment());
               (*pTable)(row,col++) << artifact.GetLiveLoadFactor();

               if ( bIsWSDOTPermitRating )
               {
                  IndexType llConfigIdx;
                  IndexType permitLaneIdx;
                  Float64 Mpermit;
                  Float64 Mlegal;
                  artifact.GetWSDOTPermitConfiguration(&llConfigIdx,&permitLaneIdx,&Mpermit,&Mlegal);
                  (*pTable)(row,col++) << moment.SetValue(Mpermit);
                  (*pTable)(row,col++) << moment.SetValue(Mlegal);
               }
               else
               {
                  (*pTable)(row,col++) << moment.SetValue(artifact.GetLiveLoadMoment());
               }

               Float64 RF = artifact.GetRatingFactor();

               if ( RF < 1 )
               {
                  (*pTable)(row,col++) << RF_FAIL(rating_factor,RF);
               }
               else
               {
                  (*pTable)(row,col++) << RF_PASS(rating_factor,RF);
               }
            } // next poi
         } // next pos/neg moment
      } // next vehicle
   } // next rating type

   return pChapter;
}


CChapterBuilder* CLoadRatingDetailsChapterBuilder::Clone() const
{
   return new CLoadRatingDetailsChapterBuilder;
}
