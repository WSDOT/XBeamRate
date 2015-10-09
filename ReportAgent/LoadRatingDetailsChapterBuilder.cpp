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

   GET_IFACE2(pBroker,IXBRArtifact,pArtifact);
   GET_IFACE2(pBroker,IXBRRatingSpecification,pRatingSpec);
   GET_IFACE2(pBroker,IXBRProject,pProject);

   xbrTypes::PermitRatingMethod permitRatingMethod = pRatingSpec->GetPermitRatingMethod();

   for ( int i = 0; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;

      if ( !pRatingSpec->IsRatingEnabled(ratingType) )
      {
         continue;
      }

      CString strLiveLoadType = ::GetLiveLoadTypeName(ratingType);
      rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
      *pChapter << pPara;
      pPara->SetName(strLiveLoadType);
      *pPara << pPara->GetName() << rptNewLine;

      std::_tstring strName = pProject->GetLiveLoadName(pierID,ratingType,0);
      if ( strName == NO_LIVE_LOADS_DEFINED )
      {
         pPara = new rptParagraph;
         *pChapter << pPara;
         *pPara << strName << rptNewLine;
         continue;
      }

      IndexType nVehicles = pProject->GetLiveLoadReactionCount(pierID,ratingType);
      VehicleIndexType firstVehicleIdx = 0;
      VehicleIndexType lastVehicleIdx  = (ratingType == pgsTypes::lrDesign_Inventory || ratingType == pgsTypes::lrDesign_Operating ? 0 : nVehicles-1);
      for ( VehicleIndexType vehicleIdx = firstVehicleIdx; vehicleIdx <= lastVehicleIdx; vehicleIdx++ )
      {
         if ( !::IsDesignRatingType(ratingType) )
         {
            std::_tstring strLiveLoadName = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);
            pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
            *pChapter << pPara;
            *pPara << strLiveLoadName << rptNewLine;
         }

         const xbrRatingArtifact* pRatingArtifact = pArtifact->GetXBeamRatingArtifact(pierID,ratingType,(ratingType == pgsTypes::lrDesign_Inventory || ratingType == pgsTypes::lrDesign_Operating) ? INVALID_INDEX : vehicleIdx);

         MomentRatingDetails(pChapter,pBroker,pierID,ratingType,permitRatingMethod,true, pRatingArtifact);
         MomentRatingDetails(pChapter,pBroker,pierID,ratingType,permitRatingMethod,false,pRatingArtifact);

         if ( pRatingSpec->RateForShear(ratingType) )
         {
         // shear
         }

         if ( ::IsPermitRatingType(ratingType) && pRatingSpec->CheckYieldStressLimit() )
         {
            ReinforcementYieldingDetails(pChapter,pBroker,pierID,ratingType,permitRatingMethod,true, pRatingArtifact);
            ReinforcementYieldingDetails(pChapter,pBroker,pierID,ratingType,permitRatingMethod,false,pRatingArtifact);
         }

         if ( ::IsLegalRatingType(ratingType) && pRatingArtifact->GetRatingFactor() < 1 
            )
         {
            LoadPostingDetails(pChapter,pBroker,pRatingArtifact);
         }
      } // next vehicle
   } // next rating type

   return pChapter;
}


CChapterBuilder* CLoadRatingDetailsChapterBuilder::Clone() const
{
   return new CLoadRatingDetailsChapterBuilder;
}

void CLoadRatingDetailsChapterBuilder::MomentRatingDetails(rptChapter* pChapter,IBroker* pBroker,PierIDType pierID,pgsTypes::LoadRatingType ratingType,xbrTypes::PermitRatingMethod permitRatingMethod,bool bPositiveMoment,const xbrRatingArtifact* pRatingArtifact) const
{
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptLengthUnitValue, location, pDisplayUnits->GetSpanLengthUnit(), true );
   INIT_UV_PROTOTYPE( rptMomentUnitValue, moment, pDisplayUnits->GetMomentUnit(), false );
   rptCapacityToDemand rating_factor;

   GET_IFACE2_NOCHECK(pBroker,IXBRProject,pProject);

   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

#pragma Reminder("WORKING HERE - add rating equations")

   CString strTitle;
   strTitle.Format(_T("Rating for %s Moment"),(bPositiveMoment ? _T("Positive") : _T("Negative")));

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

      IndexType llConfigIdx;
      IndexType permitLaneIdx;
      VehicleIndexType permitVehicleIdx;
      Float64 Mpermit;
      Float64 Mlegal;
      Float64 K;
      if ( bIsWSDOTPermitRating )
      {
         artifact.GetWSDOTPermitConfiguration(&llConfigIdx,&permitLaneIdx,&permitVehicleIdx,&Mpermit,&Mlegal,&K);
      }

      (*pTable)(row,col++) << location.SetValue(poi.GetDistFromStart());
      (*pTable)(row,col++) << artifact.GetSystemFactor();
      (*pTable)(row,col++) << artifact.GetConditionFactor();
      (*pTable)(row,col++) << artifact.GetCapacityReductionFactor();
       if ( bIsWSDOTPermitRating )
       {
         (*pTable)(row,col++) << K;
       }
       else
       {
         (*pTable)(row,col++) << artifact.GetMinimumReinforcementFactor();
       }
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
         (*pTable)(row,col++) << moment.SetValue(Mpermit);
#if defined _DEBUG || defined _BETA_VERSION
         (*pTable)(row,col-1) << rptNewLine;
         (*pTable)(row,col-1) << _T("Live Load Configuration ") << (llConfigIdx+1) << rptNewLine;
         (*pTable)(row,col-1) << _T("Permit Vehicle in Lane ") << (permitLaneIdx+1) << rptNewLine;
         (*pTable)(row,col-1) << _T("Permit Vehicle: ") << pProject->GetLiveLoadName(pierID,ratingType,permitVehicleIdx).c_str();
#endif
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
}

void CLoadRatingDetailsChapterBuilder::ReinforcementYieldingDetails(rptChapter* pChapter,IBroker* pBroker,PierIDType pierID,pgsTypes::LoadRatingType ratingType,xbrTypes::PermitRatingMethod permitRatingMethod,bool bPositiveMoment,const xbrRatingArtifact* pRatingArtifact) const
{
   const xbrRatingArtifact::YieldStressRatios& artifacts = pRatingArtifact->GetYieldStressRatios(bPositiveMoment);
   if ( artifacts.size() == 0 )
   {
      return;
   }

   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   if ( bPositiveMoment )
   {
      *pPara << _T("Check Reinforcement Yielding for Positive Moment") << rptNewLine;
   }
   else
   {
      *pPara << _T("Check Reinforcement Yielding for Negative Moment") << rptNewLine;
   }

#pragma Reminder("WORKING HERE - add rating equations")
   //if ( bSplicedGirder )
   //{
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("ReinforcementYieldingEquationPT.png") ) << rptNewLine;
   //}
   //else
   //{
   //   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("ReinforcementYieldingEquation.png") ) << rptNewLine;
   //}

   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("ReinforcementYieldingParameters.png") ) << rptNewLine;

   ColumnIndexType nColumns = 20;
   rptRcTable* table = pgsReportStyleHolder::CreateDefaultTable(nColumns);
   
   table->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   table->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));

   //*pPara << table << rptNewLine; // don't add table here... see below

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   INIT_UV_PROTOTYPE( rptLengthUnitValue,  location, pDisplayUnits->GetSpanLengthUnit(),      false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue,  dim,      pDisplayUnits->GetComponentDimUnit(),    false );
   INIT_UV_PROTOTYPE( rptStressUnitValue,  stress,   pDisplayUnits->GetStressUnit(),          false );
   INIT_UV_PROTOTYPE( rptMomentUnitValue,  moment,   pDisplayUnits->GetMomentUnit(),          false );
   INIT_UV_PROTOTYPE( rptLength4UnitValue, mom_i,    pDisplayUnits->GetMomentOfInertiaUnit(), false );
   INIT_UV_PROTOTYPE( rptStressUnitValue,  mod_e,    pDisplayUnits->GetModEUnit(),            false );

   INIT_SCALAR_PROTOTYPE(rptRcScalar, scalar, pDisplayUnits->GetScalarFormat());

   rptCapacityToDemand rating_factor;

   ColumnIndexType col = 0;

   (*table)(0,col++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("DC")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("DW")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("CR")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("SH")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("RE")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("PS")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("LL+IM")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("cr")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("M"),_T("bcr")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("d"),_T("s")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit() );
   (*table)(0,col++) << COLHDR(_T("c"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("I"),_T("cr")), rptLength4UnitTag, pDisplayUnits->GetMomentOfInertiaUnit() );
   (*table)(0,col++) << COLHDR(RPT_STRESS(_T("pe")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << COLHDR(RPT_STRESS(_T("bcr")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("E"),_T("s")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << COLHDR(Sub2(_T("E"),_T("c")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << COLHDR(RPT_STRESS(_T("s")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << COLHDR(RPT_STRESS(_T("r")), rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*table)(0,col++) << _T("Stress") << rptNewLine << _T("Ratio") << rptNewLine << RPT_STRESS(_T("r")) << _T("/") << RPT_STRESS(_T("s"));

   const xbrYieldStressRatioArtifact* pControllingRating;
   pRatingArtifact->GetYieldStressRatioEx(bPositiveMoment,&pControllingRating);
   xbrPointOfInterest controllingPoi = pControllingRating->GetPointOfInterest();

   pPara = new rptParagraph;
   *pChapter << pPara;

   *pPara << Sub2(symbol(gamma),_T("DC")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetDeadLoadFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("DW")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetWearingSurfaceFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("CR")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetCreepFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("SH")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetShrinkageFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("RE")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetRelaxationFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("PS")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetSecondaryEffectsFactor()) << rptNewLine;
   *pPara << Sub2(symbol(gamma),_T("LL")) << _T(" = ") << scalar.SetValue(artifacts.begin()->second.GetLiveLoadFactor()) << rptNewLine;

   // Add table here
   *pPara << table << rptNewLine;

   RowIndexType row = 1;
   xbrRatingArtifact::YieldStressRatios::const_iterator iter(artifacts.begin());
   xbrRatingArtifact::YieldStressRatios::const_iterator end(artifacts.end());
   for ( ; iter != end; iter++ )
   {
      col = 0;
      const xbrPointOfInterest& poi = iter->first;
      const xbrYieldStressRatioArtifact& artifact = iter->second;

      Float64 SR = artifact.GetRebarStressRatio();

      //if ( 1 <= SR && !ReportAtThisPoi(poi,controllingPoi) )
      //{
      //   continue;
      //}

      Float64 d;
      Float64 E;
      Float64 f;
      Float64 fy;
      artifact.GetRebar(&d,&f,&fy,&E);
      Float64 fcr = artifact.GetRebarCrackingStressIncrement();
      Float64 fs = artifact.GetRebarStress();
      Float64 fallow = artifact.GetRebarAllowableStress();


      (*table)(row,col++) << location.SetValue( poi.GetDistFromStart() );
      (*table)(row,col++) << moment.SetValue(artifact.GetDeadLoadMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetWearingSurfaceMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetCreepMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetShrinkageMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetRelaxationMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetSecondaryEffectsMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetLiveLoadMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetCrackingMoment());
      (*table)(row,col++) << moment.SetValue(artifact.GetExcessMoment());
      (*table)(row,col++) << dim.SetValue(d);
      (*table)(row,col++) << dim.SetValue(artifact.GetCrackDepth());
      (*table)(row,col++) << mom_i.SetValue(artifact.GetIcr());
      (*table)(row,col++) << stress.SetValue(f);
      (*table)(row,col++) << stress.SetValue(fcr);
      (*table)(row,col++) << mod_e.SetValue(E);
      (*table)(row,col++) << mod_e.SetValue(artifact.GetEg());
      (*table)(row,col++) << stress.SetValue(fs);
      (*table)(row,col++) << stress.SetValue(fallow);


      if ( SR < 1 )
      {
         (*table)(row,col++) << RF_FAIL(rating_factor,SR);
      }
      else
      {
         (*table)(row,col++) << RF_PASS(rating_factor,SR);
      }

      row++;
   }
}

void CLoadRatingDetailsChapterBuilder::LoadPostingDetails(rptChapter* pChapter,IBroker* pBroker,const xbrRatingArtifact* pRatingArtifact) const
{
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Load Posting Analysis Details [MBE 6A.8]") << rptNewLine;

   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("SafePostingLoad.png") ) << rptNewLine;

   INIT_UV_PROTOTYPE( rptForceUnitValue, tonnage, pDisplayUnits->GetTonnageUnit(), false );
   rptCapacityToDemand rating_factor;

   rptRcTable* table = pgsReportStyleHolder::CreateDefaultTable(5,_T(""));
   
   table->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   table->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));

   *pPara << table << rptNewLine;

   ColumnIndexType col = 0;
   RowIndexType row = 0;

   (*table)(row,col++) << _T("Truck");
   (*table)(row,col++) << COLHDR(_T("Weight"), rptForceUnitTag, pDisplayUnits->GetTonnageUnit() );
   (*table)(row,col++) << _T("RF");
   (*table)(row,col++) << COLHDR(_T("Safe") << rptNewLine << _T("Load") << rptNewLine << _T("Capacity"), rptForceUnitTag, pDisplayUnits->GetTonnageUnit() );
   (*table)(row,col++) << COLHDR(_T("Safe") << rptNewLine << _T("Posting") << rptNewLine << _T("Load"), rptForceUnitTag, pDisplayUnits->GetTonnageUnit() );

   row++;
   col = 0;

   Float64 posting_load, W, RF;
   std::_tstring strName;
   pRatingArtifact->GetSafePostingLoad(&posting_load,&W,&RF,&strName);
   (*table)(row,col++) << strName;
   (*table)(row,col++) << tonnage.SetValue(W);

   if ( RF < 1 )
   {
      (*table)(row,col++) << RF_FAIL(rating_factor,RF);
   }
   else
   {
      (*table)(row,col++) << RF_PASS(rating_factor,RF);
   }

   (*table)(row,col++) << tonnage.SetValue(::FloorOff(W*RF,0.01));
   if ( RF < 1 )
   {
      (*table)(row,col++) << tonnage.SetValue(posting_load);
   }
   else
   {
      (*table)(row,col++) << _T("-");
   }
}
