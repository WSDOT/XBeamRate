///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
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

   m_RatingType = pgsTypes::lrDesign_Inventory;

   m_VehicleIndex = INVALID_INDEX;
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
   m_AdjMllim = 0;
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

void xbrMomentRatingArtifact::SetPointOfInterest(const xbrPointOfInterest& poi)
{
   m_POI = poi;
}

const xbrPointOfInterest& xbrMomentRatingArtifact::GetPointOfInterest() const
{
   return m_POI;
}

void xbrMomentRatingArtifact::SetRatingType(pgsTypes::LoadRatingType ratingType)
{
   m_RatingType = ratingType;
}

pgsTypes::LoadRatingType xbrMomentRatingArtifact::GetLoadRatingType() const
{
   return m_RatingType;
}

void xbrMomentRatingArtifact::SetVehicleIndex(VehicleIndexType vehicleIdx)
{
   m_VehicleIndex = vehicleIdx;
}

VehicleIndexType xbrMomentRatingArtifact::GetVehicleIndex() const
{
   return m_VehicleIndex;
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

void xbrMomentRatingArtifact::SetAdjacentLaneLiveLoadMoment(Float64 Mllim)
{
   m_AdjMllim = Mllim;
   m_bRFComputed = false;
}

Float64 xbrMomentRatingArtifact::GetAdjacentLaneLiveLoadMoment() const
{
   return m_AdjMllim;
}

Float64 xbrMomentRatingArtifact::GetRatingFactor() const
{
   if ( m_bRFComputed )
   {
      return m_RF;
   }


   if ( IsZero(m_Mllim) || IsZero(m_gLL) )
   {
      m_RF = DBL_MAX;
   }
   else
   {
      Float64 p = m_SystemFactor * m_ConditionFactor;
      if ( p < 0.85 )
      {
         p = 0.85; // 6A.4.2.1-3)
      }

      Float64 C = p * m_CapacityRedutionFactor * m_MinimumReinforcementFactor * m_Mn;
      Float64 RFtop = C - m_gDC*m_Mdc - m_gDW*m_Mdw - m_gCR*m_Mcr - m_gSH*m_Msh - m_gRE*m_Mre - m_gPS*m_Mps;
      if ( IsPermitRatingType(m_RatingType) )
      {
         RFtop -= m_gLL*m_AdjMllim; // BDM Eqn. 13.1.1A-2
      }

      Float64 RFbot = m_gLL*m_Mllim;

      if ( (0 < m_Mllim && RFtop < 0) || (m_Mllim < 0 && 0 < RFtop) )
      {
         // There isn't any capacity remaining for live load
         m_RF = 0;
      }
      else if ( ::BinarySign(RFtop) != ::BinarySign(RFbot) && !IsZero(RFtop) )
      {
         // (C - DL) and LL have opposite signs
         // this case probably shouldn't happen, but if does,
         // the rating is great
         m_RF = DBL_MAX;
      }
      else
      {
         m_RF = RFtop/RFbot;
      }
   }

   m_bRFComputed = true;
   return m_RF;
}

void xbrMomentRatingArtifact::MakeCopy(const xbrMomentRatingArtifact& rOther)
{
   m_POI                        = rOther.m_POI;
   m_RatingType                 = rOther.m_RatingType;
   m_VehicleIndex               = rOther.m_VehicleIndex;
   m_VehicleWeight              = rOther.m_VehicleWeight;
   m_strVehicleName             = rOther.m_strVehicleName;
   m_bRFComputed                = rOther.m_bRFComputed;
   m_RF                         = rOther.m_RF;
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
   m_AdjMllim                   = rOther.m_AdjMllim;
}

void xbrMomentRatingArtifact::MakeAssignment(const xbrMomentRatingArtifact& rOther)
{
   MakeCopy( rOther );
}
