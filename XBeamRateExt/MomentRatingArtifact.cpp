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
#include <XBeamRateExt\MomentRatingArtifact.h>
#include <IFace\AnalysisResults.h>
#include <IFace\Project.h>
#include <IFace\LoadRating.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrMomentRatingArtifact
****************************************************************************/
xbrMomentRatingArtifact::xbrMomentRatingArtifact() :
m_strVehicleName(_T("Unknown"))
{
   m_bRFComputed = false;
   m_RF = 0;

   m_LLConfigIdx = INVALID_INDEX;
   m_PermitLaneIdx = INVALID_INDEX;
   m_PermitVehicleIdx = INVALID_INDEX;
   m_Mpermit = -DBL_MAX;
   m_Mlegal  = -DBL_MAX;
   m_K       = -DBL_MAX;

   m_PierID = INVALID_ID;

   m_RatingType = pgsTypes::lrDesign_Inventory;
   m_PermitRatingMethod = xbrTypes::prmAASHTO;

   m_VehicleIdx = INVALID_INDEX;
   m_VehicleWeight = -999999;

   m_SystemFactor = 1.0;
   m_ConditionFactor = 1.0;
   m_MinimumReinforcementFactor = 1.0;
   m_CapacityRedutionFactor = 1.0;
   m_Mn = 0;
   m_gDC = 1;
   m_gDW = 1;
   m_gLL = 1;
   m_Mdc = 0;
   m_Mdw = 0;
   m_Mllim = 0;
}

xbrMomentRatingArtifact::xbrMomentRatingArtifact(const xbrMomentRatingArtifact& rOther)
{
   MakeCopy(rOther);
}

xbrMomentRatingArtifact::~xbrMomentRatingArtifact()
{
}

xbrMomentRatingArtifact& xbrMomentRatingArtifact::operator=(const xbrMomentRatingArtifact& rOther)
{
   if ( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

void xbrMomentRatingArtifact::SetPierID(PierIDType pierID)
{
   m_PierID = pierID;
   m_bRFComputed = false;
}

PierIDType xbrMomentRatingArtifact::GetPierID() const
{
   return m_PierID;
}

void xbrMomentRatingArtifact::SetPointOfInterest(const xbrPointOfInterest& poi)
{
   m_POI = poi;
   m_bRFComputed = false;
}

const xbrPointOfInterest& xbrMomentRatingArtifact::GetPointOfInterest() const
{
   return m_POI;
}

void xbrMomentRatingArtifact::SetRatingType(pgsTypes::LoadRatingType ratingType)
{
   m_RatingType = ratingType;
   m_bRFComputed = false;
}

pgsTypes::LoadRatingType xbrMomentRatingArtifact::GetLoadRatingType() const
{
   return m_RatingType;
}

void xbrMomentRatingArtifact::SetPermitRatingMethod(xbrTypes::PermitRatingMethod permitRatingMethod)
{
   m_PermitRatingMethod = permitRatingMethod;
   m_bRFComputed = false;
}

xbrTypes::PermitRatingMethod xbrMomentRatingArtifact::GetPermitRatingMethod() const
{
   return m_PermitRatingMethod;
}

void xbrMomentRatingArtifact::SetVehicleIndex(VehicleIndexType vehicleIdx)
{
   m_VehicleIdx = vehicleIdx;
   m_bRFComputed = false;
}

VehicleIndexType xbrMomentRatingArtifact::GetVehicleIndex() const
{
   return m_VehicleIdx;
}

void xbrMomentRatingArtifact::SetVehicleName(LPCTSTR str)
{
   m_strVehicleName = str;
}

std::_tstring xbrMomentRatingArtifact::GetVehicleName() const
{
   return m_strVehicleName;
}

void xbrMomentRatingArtifact::SetVehicleWeight(Float64 W)
{
   m_VehicleWeight = W;
}

Float64 xbrMomentRatingArtifact::GetVehicleWeight() const
{
   return m_VehicleWeight;
}

void xbrMomentRatingArtifact::SetSystemFactor(Float64 systemFactor)
{
   m_SystemFactor = systemFactor;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetSystemFactor() const
{
   return m_SystemFactor;
}

void xbrMomentRatingArtifact::SetConditionFactor(Float64 conditionFactor)
{
   m_ConditionFactor = conditionFactor;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetConditionFactor() const
{
   return m_ConditionFactor;
}

void xbrMomentRatingArtifact::SetCapacityReductionFactor(Float64 phi)
{
   m_CapacityRedutionFactor = phi;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetCapacityReductionFactor() const
{
   return m_CapacityRedutionFactor;
}

void xbrMomentRatingArtifact::SetMinimumReinforcementFactor(Float64 K)
{
   m_MinimumReinforcementFactor = K;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetMinimumReinforcementFactor() const
{
   return m_MinimumReinforcementFactor;
}

void xbrMomentRatingArtifact::SetNominalMomentCapacity(Float64 Mn)
{
   m_Mn = Mn;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetNominalMomentCapacity() const
{
   return m_Mn;
}

void xbrMomentRatingArtifact::SetDeadLoadFactor(Float64 gDC)
{
   m_gDC = gDC;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetDeadLoadFactor() const
{
   return m_gDC;
}

void xbrMomentRatingArtifact::SetDeadLoadMoment(Float64 Mdc)
{
   m_Mdc = Mdc;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetDeadLoadMoment() const
{
   return m_Mdc;
}

void xbrMomentRatingArtifact::SetWearingSurfaceFactor(Float64 gDW)
{
   m_gDW = gDW;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetWearingSurfaceFactor() const
{
   return m_gDW;
}

void xbrMomentRatingArtifact::SetWearingSurfaceMoment(Float64 Mdw)
{
   m_Mdw = Mdw;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetWearingSurfaceMoment() const
{
   return m_Mdw;
}

void xbrMomentRatingArtifact::SetCreepFactor(Float64 gCR)
{
   m_gCR = gCR;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetCreepFactor() const
{
   return m_gCR;
}

void xbrMomentRatingArtifact::SetCreepMoment(Float64 Mcr)
{
   m_Mcr = Mcr;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetCreepMoment() const
{
   return m_Mcr;
}

void xbrMomentRatingArtifact::SetShrinkageFactor(Float64 gSH)
{
   m_gSH = gSH;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetShrinkageFactor() const
{
   return m_gSH;
}

void xbrMomentRatingArtifact::SetShrinkageMoment(Float64 Msh)
{
   m_Msh = Msh;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetShrinkageMoment() const
{
   return m_Msh;
}

void xbrMomentRatingArtifact::SetRelaxationFactor(Float64 gRE)
{
   m_gRE = gRE;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetRelaxationFactor() const
{
   return m_gRE;
}

void xbrMomentRatingArtifact::SetRelaxationMoment(Float64 Mre)
{
   m_Mre = Mre;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetRelaxationMoment() const
{
   return m_Mre;
}

void xbrMomentRatingArtifact::SetSecondaryEffectsFactor(Float64 gPS)
{
   m_gPS = gPS;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetSecondaryEffectsFactor() const
{
   return m_gPS;
}

void xbrMomentRatingArtifact::SetSecondaryEffectsMoment(Float64 Mps)
{
   m_Mps;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetSecondaryEffectsMoment() const
{
   return m_Mps;
}

void xbrMomentRatingArtifact::SetLiveLoadFactor(Float64 gLL)
{
   m_gLL = gLL;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetLiveLoadFactor() const
{
   return m_gLL;
}

void xbrMomentRatingArtifact::SetLiveLoadMoment(Float64 Mllim)
{
   m_Mllim = Mllim;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetLiveLoadMoment() const
{
   return m_Mllim;
}

Float64 xbrMomentRatingArtifact::GetRatingFactor() const
{
   if ( m_bRFComputed )
   {
      return m_RF;
   }

   if ( ::IsPermitRatingType(m_RatingType) && m_PermitRatingMethod == xbrTypes::prmWSDOT )
   {
      // we don't know which combination of permit and legal loads cause the minimum rating factor
      // so we have to check them all.... the min rating factor could happen when the legal load
      // effect is greatest, leaving the least reserve capacity for the permit vehicle, or when
      // the legal load effect is least coupled with a very large permit load response, or something
      // in between.
      Float64 RFmin = DBL_MAX;
      bool bFirst = true;
      pgsTypes::LimitState ls = ::GetStrengthLimitStateType(m_RatingType);
      bool bPositiveMoment = (0 <= m_Mn ? true : false);

      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);
      GET_IFACE2(pBroker,IXBRProject,pProject);
      GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
      GET_IFACE2(pBroker,IXBRAnalysisResults,pAnalysisResults);
      GET_IFACE2(pBroker,IXBRMomentCapacity,pMomentCapacity);

      VehicleIndexType nVehicles = (m_VehicleIdx == INVALID_INDEX ? pProject->GetLiveLoadReactionCount(m_PierID,m_RatingType) : 1);
      VehicleIndexType firstVehicleIdx = (m_VehicleIdx == INVALID_INDEX ? 0 : m_VehicleIdx);
      VehicleIndexType lastVehicleIdx  = (m_VehicleIdx == INVALID_INDEX ? nVehicles-1 : firstVehicleIdx);

      IndexType nLiveLoadConfigurations = pProductForces->GetLiveLoadConfigurationCount(m_PierID,m_RatingType);
      for ( IndexType llConfigIdx = 0; llConfigIdx < nLiveLoadConfigurations; llConfigIdx++ )
      {
         IndexType nLoadedLanes = pProductForces->GetLoadedLaneCount(m_PierID,llConfigIdx);
         for ( IndexType permitLaneIdx = 0; permitLaneIdx < nLoadedLanes; permitLaneIdx++ )
         {
            for ( VehicleIndexType vehicleIdx = firstVehicleIdx; vehicleIdx <= lastVehicleIdx; vehicleIdx++ )
            {
               Float64 rf;

               Float64 Mpermit, Mlegal;
               pAnalysisResults->GetMoment(m_PierID,m_RatingType,vehicleIdx,llConfigIdx,permitLaneIdx,m_POI,&Mpermit,&Mlegal);

               // Mininum reinforcement is a function of Mu... for the WSDOT Permit rating method, we need to compute it
               // for every combination of Mpermit and Mlegal

               // NOTE: K can be less than zero when we are rating for negative moment and the minumum moment demand (Mu)
               // is positive. This happens near the simple ends of spans. For example Mr < 0 because we are rating for
               // negative moment and Mmin = min (1.2Mcr and 1.33Mu)... Mcr < 0 because we are looking at negative moment
               // and Mu > 0.... Since we are looking at the negative end of things, Mmin = 1.33Mu. +/- = -... it doesn't
               // make since for K to be negative... K < 0 indicates that the section is most definate NOT under reinforced.
               // No adjustment needs to be made for underreinforcement so take K = 1.0
               Float64 K = 1.0;

               // it is really expensive to compute K and it is usually 1.0 anyway.
               // Here is the strategy. Assume K to be 1.0 and compute the rating factor
               // if the current rating factor is within 10% of the mininum factor found so far,
               // or if this is our first time through the loop, compute the actual K and re-compute
               // the rating factor using the actual K.

               rf = GetRatingFactor(1.0,Mpermit,Mlegal); // compute rating factor assuming K = 1.0
               if ( bFirst || rf < 1.1*RFmin ) // if first time, or rf with assumed K is near the controlling RF... re-compute it with the actual K
               {
                  // compute the actual K for this case
                  MinMomentCapacityDetails minCapacityDetails = pMomentCapacity->GetMinMomentCapacityDetails(m_PierID,ls,xbrTypes::Stage2,m_POI,bPositiveMoment,vehicleIdx,llConfigIdx,permitLaneIdx);
                  Float64 Mr = minCapacityDetails.Mr;
                  Float64 MrMin = minCapacityDetails.MrMin;
                  Float64 k = (IsZero(MrMin) ? 1.0 : Mr/MrMin); // MBE 6A.5.6
                  if ( k < 0.0 || 1.0 < k )
                  {
                     k = 1.0;
                  }

                  if ( k < 1.0 )
                  {
                     // the actual K is less than one, so it will reduce the rating factor based on the assumed
                     // K of 1.0.
                     // re-compute the rating factor using the actual K
                     K = k;
                     rf = GetRatingFactor(K,Mpermit,Mlegal);
                  }
               }

               if ( rf < RFmin || bFirst )
               {
                  RFmin              = rf;
                  m_LLConfigIdx      = llConfigIdx;
                  m_PermitLaneIdx    = permitLaneIdx;
                  m_PermitVehicleIdx = vehicleIdx;
                  m_Mpermit          = Mpermit;
                  m_Mlegal           = Mlegal;
                  m_K                = K;

                  bFirst = false;
               }
            } // next vehicle
         } // permit truck in next position
      } // next live load configuration

      m_RF = RFmin;
   }
   else
   {
      m_RF = GetRatingFactor(m_MinimumReinforcementFactor,m_Mllim,0);
   }

   m_bRFComputed = true;
   return m_RF;
}

void xbrMomentRatingArtifact::GetWSDOTPermitConfiguration(IndexType* pLLConfigIdx,IndexType* pPermitLaneIdx,VehicleIndexType* pVehicleIdx,Float64 *pMpermit,Float64* pMlegal,Float64* pK) const
{
   Float64 RF = GetRatingFactor(); // causes the rating factor analysis to happen

   *pLLConfigIdx   = m_LLConfigIdx;
   *pPermitLaneIdx = m_PermitLaneIdx;
   *pVehicleIdx    = m_PermitVehicleIdx;
   *pMpermit       = m_Mpermit;
   *pMlegal        = m_Mlegal;
   *pK             = m_K;
}

void xbrMomentRatingArtifact::MakeCopy(const xbrMomentRatingArtifact& rOther)
{
   m_PierID                     = rOther.m_PierID;
   m_POI                        = rOther.m_POI;
   m_RatingType                 = rOther.m_RatingType;
   m_PermitRatingMethod         = rOther.m_PermitRatingMethod;
   m_VehicleIdx                 = rOther.m_VehicleIdx;
   m_VehicleWeight              = rOther.m_VehicleWeight;
   m_strVehicleName             = rOther.m_strVehicleName;
   m_bRFComputed                = rOther.m_bRFComputed;
   m_RF                         = rOther.m_RF;
   m_LLConfigIdx                = rOther.m_LLConfigIdx;
   m_PermitLaneIdx              = rOther.m_PermitLaneIdx;
   m_PermitVehicleIdx           = rOther.m_PermitVehicleIdx;
   m_Mpermit                    = rOther.m_Mpermit;
   m_Mlegal                     = rOther.m_Mlegal;
   m_K                          = rOther.m_K;
   m_SystemFactor               = rOther.m_SystemFactor;
   m_ConditionFactor            = rOther.m_ConditionFactor;
   m_MinimumReinforcementFactor = rOther.m_MinimumReinforcementFactor;
   m_CapacityRedutionFactor     = rOther.m_CapacityRedutionFactor;
   m_Mn                         = rOther.m_Mn;
   m_gDC                        = rOther.m_gDC;
   m_gDW                        = rOther.m_gDW;
   m_gCR                        = rOther.m_gCR;
   m_gSH                        = rOther.m_gSH;
   m_gRE                        = rOther.m_gRE;
   m_gPS                        = rOther.m_gPS;
   m_gLL                        = rOther.m_gLL;
   m_Mdc                        = rOther.m_Mdc;
   m_Mdw                        = rOther.m_Mdw;
   m_Mcr                        = rOther.m_Mcr;
   m_Msh                        = rOther.m_Msh;
   m_Mre                        = rOther.m_Mre;
   m_Mps                        = rOther.m_Mps;
   m_Mllim                      = rOther.m_Mllim;
}

void xbrMomentRatingArtifact::MakeAssignment(const xbrMomentRatingArtifact& rOther)
{
   MakeCopy( rOther );
}


Float64 xbrMomentRatingArtifact::GetRatingFactor(Float64 K,Float64 Mllim,Float64 MllimAdj) const
{
   Float64 RF = -DBL_MAX;

   if ( IsZero(Mllim) || IsZero(m_gLL) )
   {
      RF = DBL_MAX;
   }
   else
   {
      Float64 p = m_SystemFactor * m_ConditionFactor;
      if ( p < 0.85 )
      {
         p = 0.85; // 6A.4.2.1-3)
      }

      Float64 C = p * m_CapacityRedutionFactor * K * m_Mn;
      Float64 RFtop = C - m_gDC*m_Mdc - m_gDW*m_Mdw - m_gCR*m_Mcr - m_gSH*m_Msh - m_gRE*m_Mre - m_gPS*m_Mps;

      if ( ::IsPermitRatingType(m_RatingType) && m_PermitRatingMethod == xbrTypes::prmWSDOT )
      {
         RFtop -= m_gLL*MllimAdj; // WSDOT BDM Eqn. 13.1.1A-2
      }

      Float64 RFbot = m_gLL*Mllim;

      if ( (0 < C && RFtop < 0) || (C < 0 && 0 < RFtop) )
      {
         // There isn't any capacity remaining for live load
         RF = 0;
      }
      else if ( ::BinarySign(RFtop) != ::BinarySign(RFbot) && !IsZero(RFtop) )
      {
         // (C - DL) and LL have opposite signs
         // this case probably shouldn't happen, but if does,
         // the rating is great
         RF = DBL_MAX;
      }
      else
      {
         RF = RFtop/RFbot;
      }
   }

   return RF;
}
