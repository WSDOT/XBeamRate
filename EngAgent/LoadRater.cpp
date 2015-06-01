#include "stdafx.h"
#include "LoadRater.h"
#include <IFace\PointOfInterest.h>

//void special_transform(IBridge* pBridge,IPointOfInterest* pPoi,IIntervals* pIntervals,
//                       std::vector<pgsPointOfInterest>::const_iterator poiBeginIter,
//                       std::vector<pgsPointOfInterest>::const_iterator poiEndIter,
//                       std::vector<Float64>::iterator forceBeginIter,
//                       std::vector<Float64>::iterator resultBeginIter,
//                       std::vector<Float64>::iterator outputBeginIter);
//
//bool AxleHasWeight(AxlePlacement& placement)
//{
//   return !IsZero(placement.Weight);
//}

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

xbrRatingArtifact xbrLoadRater::RateXBeam(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx)
{
   //GET_IFACE(IRatingSpecification,pRatingSpec);

   GET_IFACE(IXBRPointOfInterest,pPOI);
   std::vector<xbrPointOfInterest> vPoi( pPOI->GetXBeamPointsOfInterest() );

   xbrRatingArtifact ratingArtifact;

   // Rate for positive moment - flexure
   MomentRating(vPoi,true,ratingType,vehicleIdx,ratingArtifact);

   // Rate for negative moment - flexure
   MomentRating(vPoi,false,ratingType,vehicleIdx,ratingArtifact);

   // Rate for shear if applicable
   //if ( pRatingSpec->RateForShear(ratingType) )
   //{
#pragma Reminder("UPDATE: need to make shear rating optional")
      ShearRating(vPoi,ratingType,vehicleIdx,ratingArtifact);
   //}

   return ratingArtifact;
}

void xbrLoadRater::MomentRating(const std::vector<xbrPointOfInterest>& vPoi,bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact)
{
   //std::vector<Float64> vDCmin, vDCmax;
   //std::vector<Float64> vDWmin, vDWmax;
   //std::vector<Float64> vLLIMmin,vLLIMmax;
   //std::vector<Float64> vPLmin,vPLmax;
   //std::vector<VehicleIndexType> vMinTruckIndex, vMaxTruckIndex;

   //GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   //GET_IFACE(IProgress, pProgress);
   //CEAFAutoProgress ap(pProgress);
   //pProgress->UpdateMessage(_T("Load rating for moment"));

   //GetMoments(girderKey,bPositiveMoment,ratingType, vehicleIdx, vPoi, vDCmin, vDCmax, vDWmin, vDWmax, vCRmin, vCRmax, vSHmin, vSHmax, vREmin, vREmax, vPSmin, vPSmax, vLLIMmin, vMinTruckIndex, vLLIMmax, vMaxTruckIndex, vPLmin, vPLmax);

   //CGirderKey thisGirderKey(girderKey);
   //if ( thisGirderKey.groupIndex == ALL_GROUPS )
   //{
   //   thisGirderKey.groupIndex = 0;
   //}

   //GET_IFACE(IIntervals,pIntervals);
   //IntervalIndexType loadRatingIntervalIdx = pIntervals->GetLoadRatingInterval();

   //GET_IFACE(IMomentCapacity,pMomentCapacity);
   //std::vector<MOMENTCAPACITYDETAILS> vM = pMomentCapacity->GetMomentCapacityDetails(loadRatingIntervalIdx,vPoi,bPositiveMoment);
   //std::vector<MINMOMENTCAPDETAILS> vMmin = pMomentCapacity->GetMinMomentCapacityDetails(loadRatingIntervalIdx,vPoi,bPositiveMoment);

   //ATLASSERT(vPoi.size()     == vDCmax.size());
   //ATLASSERT(vPoi.size()     == vDWmax.size());
   //ATLASSERT(vPoi.size()     == vLLIMmax.size());
   //ATLASSERT(vPoi.size()     == vM.size());
   //ATLASSERT(vDCmin.size()   == vDCmax.size());
   //ATLASSERT(vDWmin.size()   == vDWmax.size());
   //ATLASSERT(vCRmin.size()   == vCRmax.size());
   //ATLASSERT(vSHmin.size()   == vSHmax.size());
   //ATLASSERT(vREmin.size()   == vREmax.size());
   //ATLASSERT(vPSmin.size()   == vPSmax.size());
   //ATLASSERT(vLLIMmin.size() == vLLIMmax.size());

   //GET_IFACE(IRatingSpecification,pRatingSpec);
   //Float64 system_factor    = pRatingSpec->GetSystemFactorFlexure();
   //bool bIncludePL = pRatingSpec->IncludePedestrianLiveLoad();

   //pgsTypes::LimitState ls = GetStrengthLimitStateType(ratingType);

   //Float64 gDC = pRatingSpec->GetDeadLoadFactor(ls);
   //Float64 gDW = pRatingSpec->GetWearingSurfaceFactor(ls);
   //Float64 gCR = pRatingSpec->GetCreepFactor(ls);
   //Float64 gSH = pRatingSpec->GetShrinkageFactor(ls);
   //Float64 gRE = pRatingSpec->GetRelaxationFactor(ls);
   //Float64 gPS = pRatingSpec->GetSecondaryEffectsFactor(ls);

   //GET_IFACE(IProductLoads,pProductLoads);
   //pgsTypes::LiveLoadType llType = GetLiveLoadType(ratingType);
   //std::vector<std::_tstring> strLLNames = pProductLoads->GetVehicleNames(llType,girderKey);

   //CollectionIndexType nPOI = vPoi.size();
   //for ( CollectionIndexType i = 0; i < nPOI; i++ )
   //{
   //   const pgsPointOfInterest& poi = vPoi[i];

   //   Float64 condition_factor = (bPositiveMoment ? pRatingSpec->GetGirderConditionFactor(poi.GetSegmentKey()) 
   //                                               : pRatingSpec->GetDeckConditionFactor() );

   //   Float64 DC   = (bPositiveMoment ? vDCmax[i]   : vDCmin[i]);
   //   Float64 DW   = (bPositiveMoment ? vDWmax[i]   : vDWmin[i]);
   //   Float64 CR   = (bPositiveMoment ? vCRmax[i]   : vCRmin[i]);
   //   Float64 SH   = (bPositiveMoment ? vSHmax[i]   : vSHmin[i]);
   //   Float64 RE   = (bPositiveMoment ? vREmax[i]   : vREmin[i]);
   //   Float64 PS   = (bPositiveMoment ? vPSmax[i]   : vPSmin[i]);
   //   Float64 LLIM = (bPositiveMoment ? vLLIMmax[i] : vLLIMmin[i]);
   //   Float64 PL   = (bIncludePL ? (bPositiveMoment ? vPLmax[i]   : vPLmin[i]) : 0.0);

   //   VehicleIndexType truck_index = vehicleIdx;
   //   if ( vehicleIdx == INVALID_INDEX )
   //   {
   //      truck_index = (bPositiveMoment ? vMaxTruckIndex[i] : vMinTruckIndex[i]);
   //   }

   //   std::_tstring strVehicleName = strLLNames[truck_index];

   //   CString strProgress;
   //   if ( poi.HasGirderCoordinate() )
   //   {
   //      strProgress.Format(_T("Load rating %s for %s moment at %s"),strVehicleName.c_str(),bPositiveMoment ? _T("positive") : _T("negative"),
   //         ::FormatDimension(poi.GetGirderCoordinate(),pDisplayUnits->GetSpanLengthUnit()));
   //   }
   //   else
   //   {
   //      strProgress.Format(_T("Load rating %s for %s moment"),strVehicleName.c_str(),bPositiveMoment ? _T("positive") : _T("negative"));
   //   }
   //   pProgress->UpdateMessage(strProgress);

   //   Float64 gLL = pRatingSpec->GetLiveLoadFactor(ls,true);
   //   if ( gLL < 0 )
   //   {
   //      if ( ::IsStrengthLimitState(ls) )
   //      {
   //         GET_IFACE(IProductForces,pProductForces);

   //         pgsTypes::BridgeAnalysisType batMin = pProductForces->GetBridgeAnalysisType(pgsTypes::Minimize);
   //         pgsTypes::BridgeAnalysisType batMax = pProductForces->GetBridgeAnalysisType(pgsTypes::Maximize);

   //         Float64 Mmin, Mmax, Dummy;
   //         AxleConfiguration MinAxleConfig, MaxAxleConfig, DummyAxleConfig;
   //         pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,truck_index,poi,batMin,true,true,&Mmin,&Dummy,&MinAxleConfig,&DummyAxleConfig);
   //         pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,truck_index,poi,batMax,true,true,&Dummy,&Mmax,&DummyAxleConfig,&MaxAxleConfig);
   //         gLL = GetStrengthLiveLoadFactor(ratingType,bPositiveMoment ? MaxAxleConfig : MinAxleConfig);
   //      }
   //      else
   //      {
   //         gLL = GetServiceLiveLoadFactor(ratingType);
   //      }
   //   }

   //   Float64 phi_moment = vM[i].Phi; 
   //   Float64 Mn = vM[i].Mn;

   //   // NOTE: K can be less than zero when we are rating for negative moment and the minumum moment demand (Mu)
   //   // is positive. This happens near the simple ends of spans. For example Mr < 0 because we are rating for
   //   // negative moment and Mmin = min (1.2Mcr and 1.33Mu)... Mcr < 0 because we are looking at negative moment
   //   // and Mu > 0.... Since we are looking at the negative end of things, Mmin = 1.33Mu. +/- = -... it doesn't
   //   // make since for K to be negative... K < 0 indicates that the section is most definate NOT under reinforced.
   //   // No adjustment needs to be made for underreinforcement so take K = 1.0
   //   Float64 K = (IsZero(vMmin[i].MrMin) ? 1.0 : vMmin[i].Mr/vMmin[i].MrMin);
   //   if ( K < 0.0 || 1.0 < K )
   //   {
   //      K = 1.0;
   //   }

   //   Float64 W = pProductLoads->GetVehicleWeight(llType,truck_index);

   //   pgsMomentRatingArtifact momentArtifact;
   //   momentArtifact.SetRatingType(ratingType);
   //   momentArtifact.SetPointOfInterest(poi);
   //   momentArtifact.SetVehicleIndex(truck_index);
   //   momentArtifact.SetVehicleWeight(W);
   //   momentArtifact.SetVehicleName(strVehicleName.c_str());
   //   momentArtifact.SetSystemFactor(system_factor);
   //   momentArtifact.SetConditionFactor(condition_factor);
   //   momentArtifact.SetCapacityReductionFactor(phi_moment);
   //   momentArtifact.SetMinimumReinforcementFactor(K);
   //   momentArtifact.SetNominalMomentCapacity(Mn);
   //   momentArtifact.SetDeadLoadFactor(gDC);
   //   momentArtifact.SetDeadLoadMoment(DC);
   //   momentArtifact.SetWearingSurfaceFactor(gDW);
   //   momentArtifact.SetWearingSurfaceMoment(DW);
   //   momentArtifact.SetCreepFactor(gCR);
   //   momentArtifact.SetCreepMoment(CR);
   //   momentArtifact.SetShrinkageFactor(gSH);
   //   momentArtifact.SetShrinkageMoment(SH);
   //   momentArtifact.SetRelaxationFactor(gRE);
   //   momentArtifact.SetRelaxationMoment(RE);
   //   momentArtifact.SetSecondaryEffectsFactor(gPS);
   //   momentArtifact.SetSecondaryEffectsMoment(PS);
   //   momentArtifact.SetLiveLoadFactor(gLL);
   //   momentArtifact.SetLiveLoadMoment(LLIM+PL);

   //   ratingArtifact.AddArtifact(poi,momentArtifact,bPositiveMoment);
   //}
}

void xbrLoadRater::ShearRating(const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact)
{
   //GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   //GET_IFACE(IProgress, pProgress);
   //CEAFAutoProgress ap(pProgress);
   //pProgress->UpdateMessage(_T("Load rating for shear"));

   //CGirderKey thisGirderKey(girderKey);
   //if ( thisGirderKey.groupIndex == ALL_GROUPS )
   //{
   //   thisGirderKey.groupIndex = 0;
   //}

   //pgsTypes::LimitState ls = GetStrengthLimitStateType(ratingType);

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

   //   Float64 condition_factor = pRatingSpec->GetGirderConditionFactor(poi.GetSegmentKey());

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

pgsTypes::LimitState xbrLoadRater::GetStrengthLimitStateType(pgsTypes::LoadRatingType ratingType)
{
   pgsTypes::LimitState ls;
   switch(ratingType)
   {
   case pgsTypes::lrDesign_Inventory:
      ls = pgsTypes::StrengthI_Inventory;
      break;

   case pgsTypes::lrDesign_Operating:
      ls = pgsTypes::StrengthI_Operating;
      break;

   case pgsTypes::lrLegal_Routine:
      ls = pgsTypes::StrengthI_LegalRoutine;
      break;

   case pgsTypes::lrLegal_Special:
      ls = pgsTypes::StrengthI_LegalSpecial;
      break;

   case pgsTypes::lrPermit_Routine:
      ls = pgsTypes::StrengthII_PermitRoutine;
      break;

   case pgsTypes::lrPermit_Special:
      ls = pgsTypes::StrengthII_PermitSpecial;
      break;

   default:
      ATLASSERT(false); // SHOULD NEVER GET HERE (unless there is a new rating type)
   }

   return ls;
}

void xbrLoadRater::GetMoments(bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx, const std::vector<xbrPointOfInterest>& vPOI, std::vector<Float64>& vDC,std::vector<Float64>& vDW, std::vector<Float64>& vLLIMmin, std::vector<VehicleIndexType>& vMinTruckIndex,std::vector<Float64>& vLLIMmax,std::vector<VehicleIndexType>& vMaxTruckIndex, std::vector<Float64>& vAdjLLIMmin, std::vector<VehicleIndexType>& vAdjMinTruckIndex,std::vector<Float64>& vAdjLLIMmax,std::vector<VehicleIndexType>& vAdjMaxTruckIndex)
{
   pgsTypes::LiveLoadType llType = GetLiveLoadType(ratingType);

   //std::vector<Float64> vUnused;
   //std::vector<VehicleIndexType> vUnusedIndex;

   //GET_IFACE(IProductForces,pProdForces);
   //pgsTypes::BridgeAnalysisType batMin = pProdForces->GetBridgeAnalysisType(pgsTypes::Minimize);
   //pgsTypes::BridgeAnalysisType batMax = pProdForces->GetBridgeAnalysisType(pgsTypes::Maximize);

   //GET_IFACE(ICombinedForces2,pCombinedForces);
   //GET_IFACE(IProductForces2,pProductForces);
   //if ( bPositiveMoment || (!bPositiveMoment && bIncludeNoncompositeMoments) )
   //{
   //   vDCmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcDC,vPoi,batMin,rtCumulative);
   //   vDCmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcDC,vPoi,batMax,rtCumulative);

   //   vDWmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcDWRating,vPoi,batMin,rtCumulative);
   //   vDWmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcDWRating,vPoi,batMax,rtCumulative);

   //   vCRmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcCR,vPoi,batMin,rtCumulative);
   //   vCRmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcCR,vPoi,batMax,rtCumulative);

   //   vSHmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcSH,vPoi,batMin,rtCumulative);
   //   vSHmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcSH,vPoi,batMax,rtCumulative);

   //   vREmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcRE,vPoi,batMin,rtCumulative);
   //   vREmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcRE,vPoi,batMax,rtCumulative);

   //   vPSmin = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcPS,vPoi,batMin,rtCumulative);
   //   vPSmax = pCombinedForces->GetMoment(loadRatingIntervalIdx,lcPS,vPoi,batMax,rtCumulative);

   //   if ( vehicleIdx == INVALID_INDEX )
   //   {
   //      pProductForces->GetLiveLoadMoment(loadRatingIntervalIdx,llType,vPoi,batMin,true,true,&vLLIMmin,&vUnused,&vMinTruckIndex,&vUnusedIndex);
   //      pProductForces->GetLiveLoadMoment(loadRatingIntervalIdx,llType,vPoi,batMax,true,true,&vUnused,&vLLIMmax,&vUnusedIndex,&vMaxTruckIndex);
   //   }
   //   else
   //   {
   //      pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,vehicleIdx,vPoi,batMin,true,true,&vLLIMmin,&vUnused,NULL,NULL);
   //      pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,vehicleIdx,vPoi,batMax,true,true,&vUnused,&vLLIMmax,NULL,NULL);
   //   }

   //   pCombinedForces->GetCombinedLiveLoadMoment( loadRatingIntervalIdx, pgsTypes::lltPedestrian, vPoi, batMin, &vPLmin, &vUnused );
   //   pCombinedForces->GetCombinedLiveLoadMoment( loadRatingIntervalIdx, pgsTypes::lltPedestrian, vPoi, batMax, &vUnused, &vPLmax );
   //}
   //else
   //{
   //   // Because of the construction sequence, some loads don't contribute to the negative moment..
   //   // Those loads applied to the simple span girders before continuity don't contribute to the
   //   // negative moment resisted by the deck.
   //   //
   //   // Special load processing is required
   //   GET_IFACE(IBridge,pBridge);

   //   std::vector<Float64> vConstructionMin, vConstructionMax;
   //   std::vector<Float64> vSlabMin, vSlabMax;
   //   std::vector<Float64> vSlabPanelMin, vSlabPanelMax;
   //   std::vector<Float64> vDiaphragmMin, vDiaphragmMax;
   //   std::vector<Float64> vShearKeyMin, vShearKeyMax;
   //   std::vector<Float64> vUserDC1Min, vUserDC1Max;
   //   std::vector<Float64> vUserDW1Min, vUserDW1Max;
   //   std::vector<Float64> vUserDC2Min, vUserDC2Max;
   //   std::vector<Float64> vUserDW2Min, vUserDW2Max;
   //   std::vector<Float64> vTrafficBarrierMin, vTrafficBarrierMax;
   //   std::vector<Float64> vSidewalkMin, vSidewalkMax;
   //   std::vector<Float64> vOverlayMin, vOverlayMax;
   //   std::vector<Float64> vCreepMin, vCreepMax;
   //   std::vector<Float64> vShrinkageMin, vShrinkageMax;
   //   std::vector<Float64> vRelaxationMin, vRelaxationMax;
   //   std::vector<Float64> vSecondaryMin, vSecondaryMax;

   //   bool bFutureOverlay = pBridge->HasOverlay() && pBridge->IsFutureOverlay();

   //   // Get all the product load responces
   //   GET_IFACE(IProductForces2,pProductForces);

   //   vConstructionMin = pProductForces->GetMoment(castDeckIntervalIdx,pftConstruction,vPoi,batMin, rtIncremental);
   //   vConstructionMax = pProductForces->GetMoment(castDeckIntervalIdx,pftConstruction,vPoi,batMax, rtIncremental);

   //   vSlabMin = pProductForces->GetMoment(castDeckIntervalIdx,pftSlab,vPoi,batMin, rtIncremental);
   //   vSlabMax = pProductForces->GetMoment(castDeckIntervalIdx,pftSlab,vPoi,batMax, rtIncremental);

   //   vSlabPanelMin = pProductForces->GetMoment(castDeckIntervalIdx,pftSlabPanel,vPoi,batMin, rtIncremental);
   //   vSlabPanelMax = pProductForces->GetMoment(castDeckIntervalIdx,pftSlabPanel,vPoi,batMax, rtIncremental);

   //   vDiaphragmMin = pProductForces->GetMoment(castDeckIntervalIdx,pftDiaphragm,vPoi,batMin, rtIncremental);
   //   vDiaphragmMax = pProductForces->GetMoment(castDeckIntervalIdx,pftDiaphragm,vPoi,batMax, rtIncremental);

   //   vShearKeyMin = pProductForces->GetMoment(castDeckIntervalIdx,pftShearKey,vPoi,batMin, rtIncremental);
   //   vShearKeyMax = pProductForces->GetMoment(castDeckIntervalIdx,pftShearKey,vPoi,batMax, rtIncremental);

   //   vUserDC1Min = pProductForces->GetMoment(castDeckIntervalIdx,pftUserDC,vPoi,batMin, rtIncremental);
   //   vUserDC1Max = pProductForces->GetMoment(castDeckIntervalIdx,pftUserDC,vPoi,batMax, rtIncremental);

   //   vUserDW1Min = pProductForces->GetMoment(castDeckIntervalIdx,pftUserDW,vPoi,batMin, rtIncremental);
   //   vUserDW1Max = pProductForces->GetMoment(castDeckIntervalIdx,pftUserDW,vPoi,batMax, rtIncremental);

   //   vUserDC2Min = pProductForces->GetMoment(compositeDeckIntervalIdx,pftUserDC,vPoi,batMin, rtIncremental);
   //   vUserDC2Max = pProductForces->GetMoment(compositeDeckIntervalIdx,pftUserDC,vPoi,batMax, rtIncremental);

   //   vUserDW2Min = pProductForces->GetMoment(compositeDeckIntervalIdx,pftUserDW,vPoi,batMin, rtIncremental);
   //   vUserDW2Max = pProductForces->GetMoment(compositeDeckIntervalIdx,pftUserDW,vPoi,batMax, rtIncremental);

   //   vTrafficBarrierMin = pProductForces->GetMoment(railingSystemIntervalIdx,pftTrafficBarrier,vPoi,batMin, rtIncremental);
   //   vTrafficBarrierMax = pProductForces->GetMoment(railingSystemIntervalIdx,pftTrafficBarrier,vPoi,batMax, rtIncremental);

   //   vSidewalkMin = pProductForces->GetMoment(railingSystemIntervalIdx,pftSidewalk,vPoi,batMin, rtIncremental);
   //   vSidewalkMax = pProductForces->GetMoment(railingSystemIntervalIdx,pftSidewalk,vPoi,batMax, rtIncremental);

   //   if ( !bFutureOverlay )
   //   {
   //      vOverlayMin = pProductForces->GetMoment(overlayIntervalIdx,pftOverlay,vPoi,batMin, rtIncremental);
   //      vOverlayMax = pProductForces->GetMoment(overlayIntervalIdx,pftOverlay,vPoi,batMax, rtIncremental);
   //   }
   //   else
   //   {
   //      vOverlayMin.resize(vPoi.size(),0);
   //      vOverlayMax.resize(vPoi.size(),0);
   //   }

   //   vCreepMin = pProductForces->GetMoment(loadRatingIntervalIdx,pftCreep,vPoi,batMin, rtCumulative);
   //   vCreepMax = pProductForces->GetMoment(loadRatingIntervalIdx,pftCreep,vPoi,batMax, rtCumulative);

   //   vShrinkageMin = pProductForces->GetMoment(loadRatingIntervalIdx,pftShrinkage,vPoi,batMin, rtCumulative);
   //   vShrinkageMax = pProductForces->GetMoment(loadRatingIntervalIdx,pftShrinkage,vPoi,batMax, rtCumulative);

   //   vRelaxationMin = pProductForces->GetMoment(loadRatingIntervalIdx,pftRelaxation,vPoi,batMin, rtCumulative);
   //   vRelaxationMax = pProductForces->GetMoment(loadRatingIntervalIdx,pftRelaxation,vPoi,batMax, rtCumulative);

   //   vSecondaryMin = pProductForces->GetMoment(loadRatingIntervalIdx,pftSecondaryEffects,vPoi,batMin, rtCumulative);
   //   vSecondaryMax = pProductForces->GetMoment(loadRatingIntervalIdx,pftSecondaryEffects,vPoi,batMax, rtCumulative);

   //   if ( vehicleIdx == INVALID_INDEX )
   //   {
   //      pProductForces->GetLiveLoadMoment( loadRatingIntervalIdx, llType, vPoi, batMin, true, true, &vLLIMmin, &vUnused, &vMinTruckIndex, &vUnusedIndex );
   //      pProductForces->GetLiveLoadMoment( loadRatingIntervalIdx, llType, vPoi, batMax, true, true, &vUnused, &vLLIMmax, &vUnusedIndex, &vMaxTruckIndex );
   //   }
   //   else
   //   {
   //      pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,vehicleIdx,vPoi,batMin,true,true,&vLLIMmin,&vUnused,NULL,NULL);
   //      pProductForces->GetVehicularLiveLoadMoment(loadRatingIntervalIdx,llType,vehicleIdx,vPoi,batMax,true,true,&vUnused,&vLLIMmax,NULL,NULL);
   //   }

   //   // sum DC and DW

   //   // initialize
   //   vDCmin.resize(vPoi.size(),0);
   //   vDCmax.resize(vPoi.size(),0);
   //   vDWmin.resize(vPoi.size(),0);
   //   vDWmax.resize(vPoi.size(),0);
   //   vCRmin.resize(vPoi.size(),0);
   //   vCRmax.resize(vPoi.size(),0);
   //   vSHmin.resize(vPoi.size(),0);
   //   vSHmax.resize(vPoi.size(),0);
   //   vREmin.resize(vPoi.size(),0);
   //   vREmax.resize(vPoi.size(),0);
   //   vPSmin.resize(vPoi.size(),0);
   //   vPSmax.resize(vPoi.size(),0);

   //   GET_IFACE(IPointOfInterest,pPoi);
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vConstructionMin.begin(),vDCmin.begin(),vDCmin.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vConstructionMax.begin(),vDCmax.begin(),vDCmax.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vSlabMin.begin(),vDCmin.begin(),vDCmin.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vSlabMax.begin(),vDCmax.begin(),vDCmax.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vSlabPanelMin.begin(),vDCmin.begin(),vDCmin.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vSlabPanelMax.begin(),vDCmax.begin(),vDCmax.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vDiaphragmMin.begin(),vDCmin.begin(),vDCmin.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vDiaphragmMax.begin(),vDCmax.begin(),vDCmax.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vShearKeyMin.begin(),vDCmin.begin(),vDCmin.begin());
   //   special_transform(pBridge,pPoi,pIntervals,vPoi.begin(),vPoi.end(),vShearKeyMax.begin(),vDCmax.begin(),vDCmax.begin());

   //   std::transform(vUserDC1Min.begin(),vUserDC1Min.end(),vDCmin.begin(),vDCmin.begin(),std::plus<Float64>());
   //   std::transform(vUserDC1Max.begin(),vUserDC1Max.end(),vDCmax.begin(),vDCmax.begin(),std::plus<Float64>());

   //   std::transform(vUserDW1Min.begin(),vUserDW1Min.end(),vDWmin.begin(),vDWmin.begin(),std::plus<Float64>());
   //   std::transform(vUserDW1Max.begin(),vUserDW1Max.end(),vDWmax.begin(),vDWmax.begin(),std::plus<Float64>());

   //   std::transform(vUserDC2Min.begin(),vUserDC2Min.end(),vDCmin.begin(),vDCmin.begin(),std::plus<Float64>());
   //   std::transform(vUserDC2Max.begin(),vUserDC2Max.end(),vDCmax.begin(),vDCmax.begin(),std::plus<Float64>());

   //   std::transform(vUserDW2Min.begin(),vUserDW2Min.end(),vDWmin.begin(),vDWmin.begin(),std::plus<Float64>());
   //   std::transform(vUserDW2Max.begin(),vUserDW2Max.end(),vDWmax.begin(),vDWmax.begin(),std::plus<Float64>());

   //   std::transform(vTrafficBarrierMin.begin(),vTrafficBarrierMin.end(),vDCmin.begin(),vDCmin.begin(),std::plus<Float64>());
   //   std::transform(vTrafficBarrierMax.begin(),vTrafficBarrierMax.end(),vDCmax.begin(),vDCmax.begin(),std::plus<Float64>());

   //   std::transform(vSidewalkMin.begin(),vSidewalkMin.end(),vDCmin.begin(),vDCmin.begin(),std::plus<Float64>());
   //   std::transform(vSidewalkMax.begin(),vSidewalkMax.end(),vDCmax.begin(),vDCmax.begin(),std::plus<Float64>());

   //   std::transform(vOverlayMin.begin(),vOverlayMin.end(),vDWmin.begin(),vDWmin.begin(),std::plus<Float64>());
   //   std::transform(vOverlayMax.begin(),vOverlayMax.end(),vDWmax.begin(),vDWmax.begin(),std::plus<Float64>());

   //   std::transform(vCreepMin.begin(),vCreepMin.end(),vCRmin.begin(),vCRmin.begin(),std::plus<Float64>());
   //   std::transform(vCreepMax.begin(),vCreepMax.end(),vCRmax.begin(),vCRmax.begin(),std::plus<Float64>());

   //   std::transform(vShrinkageMin.begin(),vShrinkageMin.end(),vSHmin.begin(),vSHmin.begin(),std::plus<Float64>());
   //   std::transform(vShrinkageMax.begin(),vShrinkageMax.end(),vSHmax.begin(),vSHmax.begin(),std::plus<Float64>());

   //   std::transform(vRelaxationMin.begin(),vRelaxationMin.end(),vREmin.begin(),vREmin.begin(),std::plus<Float64>());
   //   std::transform(vRelaxationMax.begin(),vRelaxationMax.end(),vREmax.begin(),vREmax.begin(),std::plus<Float64>());

   //   std::transform(vSecondaryMin.begin(),vSecondaryMin.end(),vPSmin.begin(),vPSmin.begin(),std::plus<Float64>());
   //   std::transform(vSecondaryMax.begin(),vSecondaryMax.end(),vPSmax.begin(),vPSmax.begin(),std::plus<Float64>());
   //}
}

//void special_transform(IBridge* pBridge,IPointOfInterest* pPoi,IIntervals* pIntervals,
//                       std::vector<pgsPointOfInterest>::const_iterator poiBeginIter,
//                       std::vector<pgsPointOfInterest>::const_iterator poiEndIter,
//                       std::vector<Float64>::iterator forceBeginIter,
//                       std::vector<Float64>::iterator resultBeginIter,
//                       std::vector<Float64>::iterator outputBeginIter)
//{
//   std::vector<pgsPointOfInterest>::const_iterator poiIter( poiBeginIter );
//   std::vector<Float64>::iterator forceIter( forceBeginIter );
//   std::vector<Float64>::iterator resultIter( resultBeginIter );
//   std::vector<Float64>::iterator outputIter( outputBeginIter );
//
//
//   for ( ; poiIter != poiEndIter; poiIter++, forceIter++, resultIter++, outputIter++ )
//   {
//      const pgsPointOfInterest& poi = *poiIter;
//      const CSegmentKey& segmentKey = poi.GetSegmentKey();
//      CSpanKey spanKey;
//      Float64 Xspan;
//      pPoi->ConvertPoiToSpanPoint(poi,&spanKey,&Xspan);
//
//      EventIndexType start,end,dummy;
//      PierIndexType prevPierIdx = spanKey.spanIndex;
//      PierIndexType nextPierIdx = prevPierIdx + 1;
//
//      pBridge->GetContinuityEventIndex(prevPierIdx,&dummy,&start);
//      pBridge->GetContinuityEventIndex(nextPierIdx,&end,&dummy);
//
//      IntervalIndexType compositeDeckIntervalIdx       = pIntervals->GetCompositeDeckInterval();
//      IntervalIndexType startPierContinuityIntervalIdx = pIntervals->GetInterval(start);
//      IntervalIndexType endPierContinuityIntervalIdx   = pIntervals->GetInterval(end);
//
//      if ( startPierContinuityIntervalIdx == compositeDeckIntervalIdx && 
//           endPierContinuityIntervalIdx   == compositeDeckIntervalIdx )
//      {
//         *outputIter = (*forceIter + *resultIter);
//      }
//   }
//}
//
//Float64 pgsLoadRater::GetStrengthLiveLoadFactor(pgsTypes::LoadRatingType ratingType,AxleConfiguration& axleConfig)
//{
//   Float64 sum_axle_weight = 0; // sum of axle weights on the bridge
//   Float64 firstAxleLocation = -1;
//   Float64 lastAxleLocation = 0;
//   BOOST_FOREACH(AxlePlacement& axle_placement,axleConfig)
//   {
//      sum_axle_weight += axle_placement.Weight;
//
//      if ( !IsZero(axle_placement.Weight) )
//      {
//         if ( firstAxleLocation < 0 )
//         {
//            firstAxleLocation = axle_placement.Location;
//         }
//
//         lastAxleLocation = axle_placement.Location;
//      }
//   }
//   
//   Float64 AL = fabs(firstAxleLocation - lastAxleLocation); // front axle to rear axle length (for axles on the bridge)
//
//   Float64 gLL = 0;
//   GET_IFACE(IRatingSpecification,pRatingSpec);
//   GET_IFACE(ILibrary,pLibrary);
//   const RatingLibraryEntry* pRatingEntry = pLibrary->GetRatingEntry( pRatingSpec->GetRatingSpecification().c_str() );
//   if ( pRatingEntry->GetSpecificationVersion() < lrfrVersionMgr::SecondEditionWith2013Interims )
//   {
//      CLiveLoadFactorModel model;
//      if ( ratingType == pgsTypes::lrPermit_Routine )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(pgsTypes::lrPermit_Routine);
//      }
//      else if ( ratingType == pgsTypes::lrPermit_Special )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(pRatingSpec->GetSpecialPermitType());
//      }
//      else
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(ratingType);
//      }
//
//      gLL = model.GetStrengthLiveLoadFactor(pRatingSpec->GetADTT(),sum_axle_weight);
//   }
//   else
//   {
//      Float64 GVW = sum_axle_weight;
//      Float64 PermitWeightRatio = IsZero(AL) ? 0 : GVW/AL;
//      CLiveLoadFactorModel2 model;
//      if ( ratingType == pgsTypes::lrPermit_Routine )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(pgsTypes::lrPermit_Routine);
//      }
//      else if ( ratingType == pgsTypes::lrPermit_Special )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(pRatingSpec->GetSpecialPermitType());
//      }
//      else
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(ratingType);
//      }
//
//      gLL = model.GetStrengthLiveLoadFactor(pRatingSpec->GetADTT(),PermitWeightRatio);
//   }
//
//   return gLL;
//}
//
//Float64 pgsLoadRater::GetServiceLiveLoadFactor(pgsTypes::LoadRatingType ratingType)
//{
//   Float64 gLL = 0;
//   GET_IFACE(IRatingSpecification,pRatingSpec);
//   GET_IFACE(ILibrary,pLibrary);
//   const RatingLibraryEntry* pRatingEntry = pLibrary->GetRatingEntry( pRatingSpec->GetRatingSpecification().c_str() );
//   if ( pRatingEntry->GetSpecificationVersion() < lrfrVersionMgr::SecondEditionWith2013Interims )
//   {
//      CLiveLoadFactorModel model;
//      if ( ratingType == pgsTypes::lrPermit_Routine )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(pgsTypes::lrPermit_Routine);
//      }
//      else if ( ratingType == pgsTypes::lrPermit_Special )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(pRatingSpec->GetSpecialPermitType());
//      }
//      else
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel(ratingType);
//      }
//
//      gLL = model.GetServiceLiveLoadFactor(pRatingSpec->GetADTT());
//   }
//   else
//   {
//      CLiveLoadFactorModel2 model;
//      if ( ratingType == pgsTypes::lrPermit_Routine )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(pgsTypes::lrPermit_Routine);
//      }
//      else if ( ratingType == pgsTypes::lrPermit_Special )
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(pRatingSpec->GetSpecialPermitType());
//      }
//      else
//      {
//         model = pRatingEntry->GetLiveLoadFactorModel2(ratingType);
//      }
//
//      gLL = model.GetServiceLiveLoadFactor(pRatingSpec->GetADTT());
//   }
//
//   return gLL;
//}
