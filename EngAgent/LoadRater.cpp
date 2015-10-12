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

#include "stdafx.h"
#include "LoadRater.h"
#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\RatingSpecification.h>
#include <IFace\LoadRating.h>
#include <IFace\Pier.h>
#include <EAF\EAFDisplayUnits.h>
#include <WBFLGenericBridge.h>

#include <PGSuperUnits.h> // for FormatDimension

xbrLoadRater::xbrLoadRater(IBroker* pBroker)
{
   //CREATE_LOGFILE("LoadRating");
   m_pBroker = pBroker;
}

xbrLoadRater::~xbrLoadRater(void)
{
   //CLOSE_LOGFILE;
}

void xbrLoadRater::SetBroker(IBroker* pBroker)
{
   m_pBroker = pBroker;
}

xbrRatingArtifact xbrLoadRater::RateXBeam(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx)
{
   GET_IFACE(IXBRPointOfInterest,pPOI);
   std::vector<xbrPointOfInterest> vPoi( pPOI->GetXBeamPointsOfInterest(pierID) );

   xbrRatingArtifact ratingArtifact;

   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   xbrTypes::PermitRatingMethod permitRatingMethod = pRatingSpec->GetPermitRatingMethod();

   // Rate for positive moment - flexure
   MomentRating(pierID,vPoi,true,ratingType,permitRatingMethod,vehicleIdx,ratingArtifact);

   // Rate for negative moment - flexure
   MomentRating(pierID,vPoi,false,ratingType,permitRatingMethod,vehicleIdx,ratingArtifact);

   // Rate for shear if applicable
   if ( pRatingSpec->RateForShear(ratingType) )
   {
      ShearRating(pierID,vPoi,ratingType,permitRatingMethod,vehicleIdx,ratingArtifact);
   }

   if ( ::IsPermitRatingType(ratingType) && pRatingSpec->CheckYieldStressLimit() )
   {
      CheckReinforcementYielding(pierID,vPoi,ratingType,vehicleIdx,true /*positive moment*/,ratingArtifact);
      CheckReinforcementYielding(pierID,vPoi,ratingType,vehicleIdx,false/*negative moment*/,ratingArtifact);
   }

   return ratingArtifact;
}

void xbrLoadRater::MomentRating(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,xbrTypes::PermitRatingMethod permitRatingMethod,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact)
{
   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);

   GET_IFACE(IProgress, pProgress);
   CEAFAutoProgress ap(pProgress);
   pProgress->UpdateMessage(_T("Load rating for moment"));

   std::vector<Float64> vDC, vDW, vCR, vSH, vRE, vPS;
   std::vector<Float64> vLLIMmin,vLLIMmax;
   GetMoments(pierID, bPositiveMoment, ratingType, permitRatingMethod, vehicleIdx, vPoi, vDC, vDW, vCR, vSH, vRE, vPS, vLLIMmin, vLLIMmax);

   GET_IFACE(IXBRMomentCapacity,pMomentCapacity);

   GET_IFACE(IXBRProject,pProject);
   Float64 system_factor    = pProject->GetSystemFactorFlexure();
   Float64 condition_factor = pProject->GetConditionFactor(pierID);

   pgsTypes::LimitState ls = ::GetStrengthLimitStateType(ratingType);

   xbrTypes::Stage stage = xbrTypes::Stage2;

   Float64 gDC = pProject->GetDCLoadFactor(ls);
   Float64 gDW = pProject->GetDWLoadFactor(ls);
   Float64 gCR = pProject->GetCRLoadFactor(ls);
   Float64 gSH = pProject->GetSHLoadFactor(ls);
   Float64 gRE = pProject->GetRELoadFactor(ls);
   Float64 gPS = pProject->GetPSLoadFactor(ls);
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ls,vehicleIdx);

   std::_tstring strVehicleName = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);

   Float64 W = (vehicleIdx == INVALID_INDEX ? 0 : pProject->GetVehicleWeight(pierID,ratingType,vehicleIdx));

   CollectionIndexType nPOI = vPoi.size();
   for ( CollectionIndexType i = 0; i < nPOI; i++ )
   {
      const xbrPointOfInterest& poi = vPoi[i];

      Float64 DC   = vDC[i];
      Float64 DW   = vDW[i];
      Float64 CR   = vCR[i];
      Float64 SH   = vSH[i];
      Float64 RE   = vRE[i];
      Float64 PS   = vPS[i];

      Float64 LLIM     = (bPositiveMoment ? vLLIMmax[i] : vLLIMmin[i]); // Live load (includes mpf)

      CString strProgress;
      strProgress.Format(_T("Load rating %s for %s moment at %s"),
                         strVehicleName.c_str(),bPositiveMoment ? _T("positive") : _T("negative"),
                         ::FormatDimension(poi.GetDistFromStart(),pDisplayUnits->GetSpanLengthUnit()));
      pProgress->UpdateMessage(strProgress);

      const MomentCapacityDetails& momentCapacityDetails = pMomentCapacity->GetMomentCapacityDetails(pierID,stage,poi,bPositiveMoment);
      Float64 phi_moment = momentCapacityDetails.phi;
      Float64 Mn = momentCapacityDetails.Mn;

      Float64 K = 1.0;
      bool bPermitRating = ::IsPermitRatingType(ratingType);
      if ( (bPermitRating && permitRatingMethod != xbrTypes::prmWSDOT) || !bPermitRating )
      {
         // NOTE: For WSDOT Method - Permit cases, K has to be computed for each combination
         // of legal and permit loading... K will be computed in the moment rating artifact for the WSDOT/permit case.

         // NOTE: K can be less than zero when we are rating for negative moment and the minumum moment demand (Mu)
         // is positive. This happens near the simple ends of spans. For example Mr < 0 because we are rating for
         // negative moment and Mmin = min (1.2Mcr and 1.33Mu)... Mcr < 0 because we are looking at negative moment
         // and Mu > 0.... Since we are looking at the negative end of things, Mmin = 1.33Mu. +/- = -... it doesn't
         // make since for K to be negative... K < 0 indicates that the section is most definate NOT under reinforced.
         // No adjustment needs to be made for underreinforcement so take K = 1.0
         const MinMomentCapacityDetails& minCapacityDetails = pMomentCapacity->GetMinMomentCapacityDetails(pierID,ls,stage,poi,bPositiveMoment);
         Float64 Mr = minCapacityDetails.Mr;
         Float64 MrMin = minCapacityDetails.MrMin;
         Float64 K = (IsZero(MrMin) ? 1.0 : Mr/MrMin); // MBE 6A.5.6
         if ( K < 0.0 || 1.0 < K )
         {
            K = 1.0;
         }
      }

      xbrMomentRatingArtifact momentArtifact;
      momentArtifact.SetRatingType(ratingType);
      momentArtifact.SetPermitRatingMethod(permitRatingMethod);
      momentArtifact.SetPierID(pierID);
      momentArtifact.SetPointOfInterest(poi);
      momentArtifact.SetVehicleIndex(vehicleIdx);
      momentArtifact.SetVehicleWeight(W);
      momentArtifact.SetVehicleName(strVehicleName.c_str());
      momentArtifact.SetSystemFactor(system_factor);
      momentArtifact.SetConditionFactor(condition_factor);
      momentArtifact.SetCapacityReductionFactor(phi_moment);
      momentArtifact.SetMinimumReinforcementFactor(K);
      momentArtifact.SetNominalMomentCapacity(Mn);
      momentArtifact.SetDeadLoadFactor(gDC);
      momentArtifact.SetDeadLoadMoment(DC);
      momentArtifact.SetWearingSurfaceFactor(gDW);
      momentArtifact.SetWearingSurfaceMoment(DW);
      momentArtifact.SetCreepFactor(gCR);
      momentArtifact.SetCreepMoment(CR);
      momentArtifact.SetShrinkageFactor(gSH);
      momentArtifact.SetShrinkageMoment(SH);
      momentArtifact.SetRelaxationFactor(gRE);
      momentArtifact.SetRelaxationMoment(RE);
      momentArtifact.SetSecondaryEffectsFactor(gPS);
      momentArtifact.SetSecondaryEffectsMoment(PS);
      momentArtifact.SetLiveLoadFactor(gLL);
      momentArtifact.SetLiveLoadMoment(LLIM);

      ratingArtifact.AddArtifact(poi,momentArtifact,bPositiveMoment);
   }
}

void xbrLoadRater::ShearRating(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,xbrTypes::PermitRatingMethod permitRatingMethod,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact)
{
#pragma Reminder("WORKING HERE - shear rating")
   //GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   //GET_IFACE(IProgress, pProgress);
   //CEAFAutoProgress ap(pProgress);
   //pProgress->UpdateMessage(_T("Load rating for shear"));

   //CGirderKey thisGirderKey(girderKey);
   //if ( thisGirderKey.groupIndex == ALL_GROUPS )
   //{
   //   thisGirderKey.groupIndex = 0;
   //}

   //pgsTypes::LimitState ls = ::GetStrengthLimitStateType(ratingType);

   //GET_IFACE(IIntervals,pIntervals);
   //IntervalIndexType loadRatingIntervalIdx = pIntervals->GetLoadRatingInterval();

   //std::vector<sysSectionValue> vDCmin, vDCmax;
   //std::vector<sysSectionValue> vDWmin, vDWmax;
   //std::vector<sysSectionValue> vCRmin, vCRmax;
   //std::vector<sysSectionValue> vSHmin, vSHmax;
   //std::vector<sysSectionValue> vREmin, vREmax;
   //std::vector<sysSectionValue> vPSmin, vPSmax;
   //std::vector<sysSectionValue> vLLIMmin,vLLIMmax;
   //std::vector<sysSectionValue> vUnused;
   //std::vector<VehicleIndexType> vMinTruckIndex, vMaxTruckIndex, vUnusedIndex;
   //std::vector<sysSectionValue> vPLmin, vPLmax;

   //pgsTypes::LiveLoadType llType = GetLiveLoadType(ratingType);

   //GET_IFACE(IProductForces,pProdForces);
   //pgsTypes::BridgeAnalysisType batMin = pProdForces->GetBridgeAnalysisType(pgsTypes::Minimize);
   //pgsTypes::BridgeAnalysisType batMax = pProdForces->GetBridgeAnalysisType(pgsTypes::Maximize);

   //GET_IFACE(ICombinedForces2,pCombinedForces);
   //GET_IFACE(IProductForces2,pProductForces);
   //vDCmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcDC,vPoi,batMin,rtCumulative);
   //vDCmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcDC,vPoi,batMax,rtCumulative);

   //vDWmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcDWRating,vPoi,batMin,rtCumulative);
   //vDWmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcDWRating,vPoi,batMax,rtCumulative);

   //vCRmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcCR,vPoi,batMin,rtCumulative);
   //vCRmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcCR,vPoi,batMax,rtCumulative);

   //vSHmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcSH,vPoi,batMin,rtCumulative);
   //vSHmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcSH,vPoi,batMax,rtCumulative);

   //vREmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcRE,vPoi,batMin,rtCumulative);
   //vREmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcRE,vPoi,batMax,rtCumulative);

   //vPSmin = pCombinedForces->GetShear(loadRatingIntervalIdx,lcPS,vPoi,batMin,rtCumulative);
   //vPSmax = pCombinedForces->GetShear(loadRatingIntervalIdx,lcPS,vPoi,batMax,rtCumulative);

   //if ( vehicleIdx == INVALID_INDEX )
   //{
   //   pProductForces->GetLiveLoadShear( loadRatingIntervalIdx, llType, vPoi, batMin, true, true, &vLLIMmin, &vUnused, &vMinTruckIndex, &vUnusedIndex );
   //   pProductForces->GetLiveLoadShear( loadRatingIntervalIdx, llType, vPoi, batMax, true, true, &vUnused, &vLLIMmax, &vUnusedIndex, &vMaxTruckIndex );
   //}
   //else
   //{
   //   pProductForces->GetVehicularLiveLoadShear( loadRatingIntervalIdx, llType, vehicleIdx, vPoi, batMin, true, true, &vLLIMmin, &vUnused, NULL,NULL,NULL,NULL);
   //   pProductForces->GetVehicularLiveLoadShear( loadRatingIntervalIdx, llType, vehicleIdx, vPoi, batMax, true, true, &vUnused, &vLLIMmax, NULL,NULL,NULL,NULL);
   //}

   //pCombinedForces->GetCombinedLiveLoadShear( loadRatingIntervalIdx, pgsTypes::lltPedestrian, vPoi, batMax, false, &vUnused, &vPLmax );
   //pCombinedForces->GetCombinedLiveLoadShear( loadRatingIntervalIdx, pgsTypes::lltPedestrian, vPoi, batMin, false, &vPLmin, &vUnused );

   //GET_IFACE(IShearCapacity,pShearCapacity);
   //std::vector<SHEARCAPACITYDETAILS> vVn = pShearCapacity->GetShearCapacityDetails(ls,loadRatingIntervalIdx,vPoi);

   //ATLASSERT(vPoi.size()     == vDCmax.size());
   //ATLASSERT(vPoi.size()     == vDWmax.size());
   //ATLASSERT(vPoi.size()     == vLLIMmax.size());
   //ATLASSERT(vPoi.size()     == vVn.size());
   //ATLASSERT(vDCmin.size()   == vDCmax.size());
   //ATLASSERT(vDWmin.size()   == vDWmax.size());
   //ATLASSERT(vCRmin.size()   == vCRmax.size());
   //ATLASSERT(vSHmin.size()   == vSHmax.size());
   //ATLASSERT(vREmin.size()   == vREmax.size());
   //ATLASSERT(vPSmin.size()   == vPSmax.size());
   //ATLASSERT(vLLIMmin.size() == vLLIMmax.size());
   //ATLASSERT(vPLmin.size()   == vPLmax.size());

   //GET_IFACE(IRatingSpecification,pRatingSpec);
   //Float64 system_factor    = pRatingSpec->GetSystemFactorShear();
   //bool bIncludePL = pRatingSpec->IncludePedestrianLiveLoad();
   //   Float64 condition_factor = pProject->GetConditionFactor(pierID);

   //Float64 gDC = pRatingSpec->GetDeadLoadFactor(ls);
   //Float64 gDW = pRatingSpec->GetWearingSurfaceFactor(ls);
   //Float64 gCR = pRatingSpec->GetCreepFactor(ls);
   //Float64 gSH = pRatingSpec->GetShrinkageFactor(ls);
   //Float64 gRE = pRatingSpec->GetRelaxationFactor(ls);
   //Float64 gPS = pRatingSpec->GetSecondaryEffectsFactor(ls);

   //GET_IFACE(IProductLoads,pProductLoads);
   //std::vector<std::_tstring> strLLNames = pProductLoads->GetVehicleNames(llType,girderKey);

   //CollectionIndexType nPOI = vPoi.size();
   //for ( CollectionIndexType i = 0; i < nPOI; i++ )
   //{
   //   const pgsPointOfInterest& poi = vPoi[i];

   //   Float64 DCmin   = Min(vDCmin[i].Left(),  vDCmin[i].Right());
   //   Float64 DCmax   = Max(vDCmax[i].Left(),  vDCmax[i].Right());
   //   Float64 DWmin   = Min(vDWmin[i].Left(),  vDWmin[i].Right());
   //   Float64 DWmax   = Max(vDWmax[i].Left(),  vDWmax[i].Right());
   //   Float64 CRmin   = Min(vCRmin[i].Left(),  vCRmin[i].Right());
   //   Float64 CRmax   = Max(vCRmax[i].Left(),  vCRmax[i].Right());
   //   Float64 SHmin   = Min(vSHmin[i].Left(),  vSHmin[i].Right());
   //   Float64 SHmax   = Max(vSHmax[i].Left(),  vSHmax[i].Right());
   //   Float64 REmin   = Min(vREmin[i].Left(),  vREmin[i].Right());
   //   Float64 REmax   = Max(vREmax[i].Left(),  vREmax[i].Right());
   //   Float64 PSmin   = Min(vPSmin[i].Left(),  vPSmin[i].Right());
   //   Float64 PSmax   = Max(vPSmax[i].Left(),  vPSmax[i].Right());
   //   Float64 LLIMmin = Min(vLLIMmin[i].Left(),vLLIMmin[i].Right());
   //   Float64 LLIMmax = Max(vLLIMmax[i].Left(),vLLIMmax[i].Right());
   //   Float64 PLmin   = Min(vPLmin[i].Left(),  vPLmin[i].Right());
   //   Float64 PLmax   = Max(vPLmax[i].Left(),  vPLmax[i].Right());

   //   Float64 DC   = Max(fabs(DCmin),fabs(DCmax));
   //   Float64 DW   = Max(fabs(DWmin),fabs(DWmax));
   //   Float64 CR   = Max(fabs(CRmin),fabs(CRmax));
   //   Float64 SH   = Max(fabs(SHmin),fabs(SHmax));
   //   Float64 RE   = Max(fabs(REmin),fabs(REmax));
   //   Float64 PS   = Max(fabs(PSmin),fabs(PSmax));
   //   Float64 LLIM = Max(fabs(LLIMmin),fabs(LLIMmax));
   //   Float64 PL   = (bIncludePL ? Max(fabs(PLmin),fabs(PLmax)) : 0);
   //   VehicleIndexType truck_index = vehicleIdx;
   //   if ( vehicleIdx == INVALID_INDEX )
   //   {
   //      truck_index = (fabs(LLIMmin) < fabs(LLIMmax) ? vMaxTruckIndex[i] : vMinTruckIndex[i]);
   //   }

   //   std::_tstring strVehicleName = strLLNames[truck_index];

   //   CString strProgress;
   //   if ( poi.HasGirderCoordinate() )
   //   {
   //      strProgress.Format(_T("Load rating %s for shear at %s"),strVehicleName.c_str(),
   //         ::FormatDimension(poi.GetGirderCoordinate(),pDisplayUnits->GetSpanLengthUnit()));
   //   }
   //   else
   //   {
   //      strProgress.Format(_T("Load rating %s for shear"),strVehicleName.c_str());
   //   }
   //   pProgress->UpdateMessage(strProgress);

   //   Float64 gLL = pRatingSpec->GetLiveLoadFactor(ls,true);
   //   if ( gLL < 0 )
   //   {
   //      // need to compute gLL based on axle weights
   //      if ( ::IsStrengthLimitState(ls) )
   //      {
   //         GET_IFACE(IProductForces,pProductForce);
   //         pgsTypes::BridgeAnalysisType batMin = pProductForce->GetBridgeAnalysisType(pgsTypes::Minimize);
   //         pgsTypes::BridgeAnalysisType batMax = pProductForce->GetBridgeAnalysisType(pgsTypes::Maximize);

   //         sysSectionValue Vmin, Vmax, Dummy;
   //         AxleConfiguration MinLeftAxleConfig, MaxLeftAxleConfig, MinRightAxleConfig, MaxRightAxleConfig, DummyLeftAxleConfig, DummyRightAxleConfig;
   //         pProductForce->GetVehicularLiveLoadShear(loadRatingIntervalIdx,llType,truck_index,poi,batMin,true,true,&Vmin,&Dummy,&MinLeftAxleConfig,&MinRightAxleConfig,&DummyLeftAxleConfig,&DummyRightAxleConfig);
   //         pProductForce->GetVehicularLiveLoadShear(loadRatingIntervalIdx,llType,truck_index,poi,batMax,true,true,&Dummy,&Vmax,&DummyLeftAxleConfig,&DummyRightAxleConfig,&MaxLeftAxleConfig,&MaxRightAxleConfig);

   //         if ( fabs(LLIMmin) < fabs(LLIMmax) )
   //         {
   //            if (IsEqual(fabs(vLLIMmax[i].Left()),fabs(LLIMmax)))
   //            {
   //               gLL = GetStrengthLiveLoadFactor(ratingType,MaxLeftAxleConfig);
   //            }
   //            else
   //            {
   //               gLL = GetStrengthLiveLoadFactor(ratingType,MaxRightAxleConfig);
   //            }
   //         }
   //         else
   //         {
   //            if (IsEqual(fabs(vLLIMmin[i].Left()),fabs(LLIMmin)))
   //            {
   //               gLL = GetStrengthLiveLoadFactor(ratingType,MinLeftAxleConfig);
   //            }
   //            else
   //            {
   //               gLL = GetStrengthLiveLoadFactor(ratingType,MinRightAxleConfig);
   //            }
   //         }
   //      }
   //      else
   //      {
   //         gLL = GetServiceLiveLoadFactor(ratingType);
   //      }
   //   }

   //   Float64 phi_shear = vVn[i].Phi; 
   //   Float64 Vn = vVn[i].Vn;

   //   Float64 W = pProductLoads->GetVehicleWeight(llType,truck_index);

   //   pgsShearRatingArtifact shearArtifact;
   //   shearArtifact.SetRatingType(ratingType);
   //   shearArtifact.SetPermitRatingMethod(permitRatingMethod);
   //   shearArtifact.SetPierID(pierID);
   //   shearArtifact.SetPointOfInterest(poi);
   //   shearArtifact.SetVehicleIndex(truck_index);
   //   shearArtifact.SetVehicleWeight(W);
   //   shearArtifact.SetVehicleName(strVehicleName.c_str());
   //   shearArtifact.SetSystemFactor(system_factor);
   //   shearArtifact.SetConditionFactor(condition_factor);
   //   shearArtifact.SetCapacityReductionFactor(phi_shear);
   //   shearArtifact.SetNominalShearCapacity(Vn);
   //   shearArtifact.SetDeadLoadFactor(gDC);
   //   shearArtifact.SetDeadLoadShear(DC);
   //   shearArtifact.SetWearingSurfaceFactor(gDW);
   //   shearArtifact.SetWearingSurfaceShear(DW);
   //   shearArtifact.SetCreepFactor(gCR);
   //   shearArtifact.SetCreepShear(CR);
   //   shearArtifact.SetShrinkageFactor(gSH);
   //   shearArtifact.SetShrinkageShear(SH);
   //   shearArtifact.SetRelaxationFactor(gRE);
   //   shearArtifact.SetRelaxationShear(RE);
   //   shearArtifact.SetSecondaryEffectsFactor(gPS);
   //   shearArtifact.SetSecondaryEffectsShear(PS);
   //   shearArtifact.SetLiveLoadFactor(gLL);
   //   shearArtifact.SetLiveLoadShear(LLIM+PL);

   //   // longitudinal steel check
   //   pgsLongReinfShearArtifact l_artifact;
   //   SHEARCAPACITYDETAILS scd;
   //   pgsDesigner2 designer;
   //   designer.SetBroker(m_pBroker);
   //   pShearCapacity->GetShearCapacityDetails(ls,loadRatingIntervalIdx,poi,&scd);
   //   designer.InitShearCheck(poi.GetSegmentKey(),loadRatingIntervalIdx,ls,NULL);
   //   designer.CheckLongReinfShear(poi,loadRatingIntervalIdx,ls,scd,NULL,&l_artifact);
   //   shearArtifact.SetLongReinfShearArtifact(l_artifact);

   //   ratingArtifact.AddArtifact(poi,shearArtifact);
   //}
}

void xbrLoadRater::CheckReinforcementYielding(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,bool bPositiveMoment,xbrRatingArtifact& ratingArtifact)
{
   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   GET_IFACE(IProgress, pProgress);
   CEAFAutoProgress ap(pProgress);
   pProgress->UpdateMessage(_T("Checking for reinforcement yielding"));

   ATLASSERT(::IsPermitRatingType(ratingType));

   pgsTypes::LimitState ls = ::GetServiceLimitStateType(ratingType);
   ATLASSERT(::IsServiceLimitState(ls));

   xbrTypes::Stage stage = xbrTypes::Stage2;

   GET_IFACE(IXBRCrackedSection,pCrackedSection);
   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   GET_IFACE(IXBRProject,pProject);
   GET_IFACE(IXBRSectionProperties,pSectProps);

   GET_IFACE(IXBRMaterial,pMaterial);
   Float64 Exbm = pMaterial->GetXBeamEc(pierID);

   // Get load factors
   Float64 gDC = pProject->GetDCLoadFactor(ls);
   Float64 gDW = pProject->GetDWLoadFactor(ls);
   Float64 gCR = pProject->GetCRLoadFactor(ls);
   Float64 gSH = pProject->GetSHLoadFactor(ls);
   Float64 gRE = pProject->GetRELoadFactor(ls);
   Float64 gPS = pProject->GetPSLoadFactor(ls);
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ls,vehicleIdx);

   // Create artifacts
   CollectionIndexType nPOI = vPoi.size();
   for ( CollectionIndexType i = 0; i < nPOI; i++ )
   {
      const xbrPointOfInterest& poi = vPoi[i];

      // assume yield stress ratio is minimized for the controlling flexure rating at this location
      // this also gives us accees to Mlegal and Mpermit for the controlling case (WSDOT method)
      const xbrMomentRatingArtifact* pMomentRatingArtifact = ratingArtifact.GetMomentRatingArtifact(poi,bPositiveMoment);

      CString strProgress;
      strProgress.Format(_T("Checking for reinforcement yielding %s for %s moment at %s"),
                         pMomentRatingArtifact->GetVehicleName().c_str(),bPositiveMoment ? _T("positive") : _T("negative"),
                         ::FormatDimension(poi.GetDistFromStart(),pDisplayUnits->GetSpanLengthUnit()));
      pProgress->UpdateMessage(strProgress);


      const CrackedSectionDetails& crackedSectionDetails_PermanentLoads = pCrackedSection->GetCrackedSectionDetails(pierID,stage,poi,bPositiveMoment,xbrTypes::ltPermanent);
      const CrackedSectionDetails& crackedSectionDetails_TransientLoads = pCrackedSection->GetCrackedSectionDetails(pierID,stage,poi,bPositiveMoment,xbrTypes::ltTransient);


      // use this to get all the loading information
      Float64 DC = pMomentRatingArtifact->GetDeadLoadMoment();
      Float64 DW = pMomentRatingArtifact->GetWearingSurfaceMoment();
      Float64 CR = pMomentRatingArtifact->GetCreepMoment();
      Float64 SH = pMomentRatingArtifact->GetShrinkageMoment();
      Float64 RE = pMomentRatingArtifact->GetRelaxationMoment();
      Float64 PS = pMomentRatingArtifact->GetSecondaryEffectsMoment();

      // NOTE, moments include multiple presence factor
      Float64 LLIMpermit = 0;
      Float64 LLIMlegal = 0;
      if ( pMomentRatingArtifact->GetPermitRatingMethod() == xbrTypes::prmAASHTO )
      {
         LLIMpermit = pMomentRatingArtifact->GetLiveLoadMoment(); // this is for AASHTO method
      }
      else
      {
         IndexType llConfigIdx;
         IndexType permitLaneIdx;
         IndexType vehIdx;
         Float64 K;
         pMomentRatingArtifact->GetWSDOTPermitConfiguration(&llConfigIdx,&permitLaneIdx,&vehIdx,&LLIMpermit,&LLIMlegal,&K);
      }

      // NOTE: not sure if we need to do this part of the analysis (rebar stress)
      Float64 Hxb = pSectProps->GetDepth(pierID,stage,poi);

      Float64 Es, fy, fu;
      pMaterial->GetRebarProperties(pierID,&Es,&fy,&fu);

      // Get distance to reinforcement from extreme compression face
      GET_IFACE(IXBRRebar,pRebar);
      CComPtr<IRebarSection> rebarSection;
      pRebar->GetRebarSection(pierID,stage,poi,&rebarSection);
      Float64 Y = (bPositiveMoment ? -DBL_MAX : DBL_MAX);
      CComPtr<IEnumRebarSectionItem> enumRebarSectionItem;
      rebarSection->get__EnumRebarSectionItem(&enumRebarSectionItem);

      CComPtr<IRebarSectionItem> rebarSectionItem;
      while ( enumRebarSectionItem->Next(1,&rebarSectionItem,NULL) != S_FALSE )
      {
        CComPtr<IPoint2d> location;
        rebarSectionItem->get_Location(&location);
        Float64 y = pRebar->GetRebarDepth(pierID,poi,location);
        ATLASSERT(0 <= y);
        Y = (bPositiveMoment ? Max(Y,y) : Min(Y,y));
        rebarSectionItem.Release();
     }

      Float64 ds = (bPositiveMoment ? Y : Hxb-Y); // depth to rebar layer, measured from compression face
      
      // Get allowable
      Float64 K = pRatingSpec->GetYieldStressLimitCoefficient();

      // NOTE: stresses are computed in artifact (Es/Exbr)(M)c/Icr use 2n for permanet load stresses
      xbrYieldStressRatioArtifact stressRatioArtifact;
      stressRatioArtifact.SetRatingType(ratingType);
      stressRatioArtifact.SetPermitRatingMethod(pMomentRatingArtifact->GetPermitRatingMethod());
      stressRatioArtifact.SetPointOfInterest(poi);
      stressRatioArtifact.SetVehicleIndex(pMomentRatingArtifact->GetVehicleIndex());
      stressRatioArtifact.SetVehicleWeight(pMomentRatingArtifact->GetVehicleWeight());
      stressRatioArtifact.SetVehicleName(pMomentRatingArtifact->GetVehicleName().c_str());
      stressRatioArtifact.SetAllowableStressRatio(K);
      stressRatioArtifact.SetDeadLoadFactor(gDC);
      stressRatioArtifact.SetDeadLoadMoment(DC);
      stressRatioArtifact.SetWearingSurfaceFactor(gDW);
      stressRatioArtifact.SetWearingSurfaceMoment(DW);
      stressRatioArtifact.SetCreepFactor(gCR);
      stressRatioArtifact.SetCreepMoment(CR);
      stressRatioArtifact.SetShrinkageFactor(gSH);
      stressRatioArtifact.SetShrinkageMoment(SH);
      stressRatioArtifact.SetRelaxationFactor(gRE);
      stressRatioArtifact.SetRelaxationMoment(RE);
      stressRatioArtifact.SetSecondaryEffectsFactor(gPS);
      stressRatioArtifact.SetSecondaryEffectsMoment(PS);
      stressRatioArtifact.SetLiveLoadFactor(gLL);
      stressRatioArtifact.SetLiveLoadMoment(LLIMpermit);
      stressRatioArtifact.SetAdjLiveLoadMoment(LLIMlegal);
      stressRatioArtifact.SetIcr(xbrTypes::ltPermanent,crackedSectionDetails_PermanentLoads.Icr);
      stressRatioArtifact.SetIcr(xbrTypes::ltTransient,crackedSectionDetails_TransientLoads.Icr);
      stressRatioArtifact.SetCrackDepth(xbrTypes::ltPermanent,crackedSectionDetails_PermanentLoads.c); // distance from compression face to cracked centroid
      stressRatioArtifact.SetCrackDepth(xbrTypes::ltTransient,crackedSectionDetails_TransientLoads.c); // distance from compression face to cracked centroid
      stressRatioArtifact.SetModularRatio(xbrTypes::ltPermanent,crackedSectionDetails_PermanentLoads.n);
      stressRatioArtifact.SetModularRatio(xbrTypes::ltTransient,crackedSectionDetails_TransientLoads.n);
      stressRatioArtifact.SetYbar(ds);
      stressRatioArtifact.SetYieldStrength(fy);
      ratingArtifact.AddArtifact(poi,stressRatioArtifact,bPositiveMoment);
   } // next poi
}

void xbrLoadRater::GetMoments(PierIDType pierID,bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,xbrTypes::PermitRatingMethod permitRatingMethod,VehicleIndexType vehicleIdx, const std::vector<xbrPointOfInterest>& vPoi, std::vector<Float64>& vDC,std::vector<Float64>& vDW,std::vector<Float64>& vCR,std::vector<Float64>& vSH,std::vector<Float64>& vRE,std::vector<Float64>& vPS, std::vector<Float64>& vLLIMmin, std::vector<Float64>& vLLIMmax)
{
   pgsTypes::LiveLoadType llType = ::GetLiveLoadType(ratingType);

   GET_IFACE(IXBRAnalysisResults,pResults);
   vDC = pResults->GetMoment(pierID,xbrTypes::lcDC,vPoi);
   vDW = pResults->GetMoment(pierID,xbrTypes::lcDW,vPoi);
   vCR = pResults->GetMoment(pierID,xbrTypes::lcCR,vPoi);
   vSH = pResults->GetMoment(pierID,xbrTypes::lcSH,vPoi);
   vRE = pResults->GetMoment(pierID,xbrTypes::lcRE,vPoi);
   vPS = pResults->GetMoment(pierID,xbrTypes::lcPS,vPoi);

   bool bPermitRating = ::IsPermitRatingType(ratingType);
   if ( (bPermitRating && permitRatingMethod != xbrTypes::prmWSDOT) || !bPermitRating )
   {
      // moments include multiple presence factor
      if ( vehicleIdx == INVALID_INDEX )
      {
         pResults->GetMoment(pierID,ratingType,vPoi,&vLLIMmin,&vLLIMmax,NULL,NULL);
      }
      else
      {
         pResults->GetMoment(pierID,ratingType,vehicleIdx,vPoi,&vLLIMmin,&vLLIMmax,NULL,NULL);
      }
   }
   else
   {
      // for WSDOT permit rating, the analysis is done in the rating artifact object
      ATLASSERT(bPermitRating && permitRatingMethod == xbrTypes::prmWSDOT);
      vLLIMmin.resize(vPoi.size(),0);
      vLLIMmax.resize(vPoi.size(),0);
   }
}
