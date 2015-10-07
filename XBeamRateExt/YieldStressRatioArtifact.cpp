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
#include <XBeamRateExt\YieldStressRatioArtifact.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrYieldStressRatioArtifact
****************************************************************************/
xbrYieldStressRatioArtifact::xbrYieldStressRatioArtifact()
{
   m_bRFComputed = false;

   m_RebarRF = 0;
   m_fsRebar = 0;
   m_fcrRebar = 0;

   m_RatingType = xbrTypes::lrDesign_Inventory;

   m_VehicleIndex = INVALID_INDEX;
   m_VehicleWeight = -9999999;
   m_strVehicleName = _T("Unknown");

   m_AllowableStressRatio = 0.9;
   m_Mdc = 0;
   m_Mdw = 0;
   m_Mcr = 0;
   m_Msh = 0;
   m_Mre = 0;
   m_Mps = 0;
   m_Mllim = 0;
   m_Mcrack = 0;
   m_Icrack = 0;
   m_c = 0;
   m_Eg = 0;
   m_gDC = 1.0;
   m_gDW = 1.0;
   m_gCR = 1.0;
   m_gSH = 1.0;
   m_gRE = 1.0;
   m_gPS = 1.0;
   m_gLL = 1.0;


   m_bRebar = false;
   m_db = 0;
   m_fb = 0;
   m_fyb = 0;
   m_Eb = 0;
}

xbrYieldStressRatioArtifact::xbrYieldStressRatioArtifact(const xbrYieldStressRatioArtifact& rOther)
{
   MakeCopy(rOther);
}

xbrYieldStressRatioArtifact::~xbrYieldStressRatioArtifact()
{
}

xbrYieldStressRatioArtifact& xbrYieldStressRatioArtifact::operator=(const xbrYieldStressRatioArtifact& rOther)
{
   if ( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

void xbrYieldStressRatioArtifact::SetPointOfInterest(const xbrPointOfInterest& poi)
{
   m_POI = poi;
}

const xbrPointOfInterest& xbrYieldStressRatioArtifact::GetPointOfInterest() const
{
   return m_POI;
}

void xbrYieldStressRatioArtifact::SetRatingType(pgsTypes::LoadRatingType ratingType)
{
   m_RatingType = ratingType;
}

pgsTypes::LoadRatingType xbrYieldStressRatioArtifact::GetLoadRatingType() const
{
   return m_RatingType;
}

void xbrYieldStressRatioArtifact::SetVehicleIndex(VehicleIndexType vehicleIdx)
{
   m_VehicleIndex = vehicleIdx;
}

VehicleIndexType xbrYieldStressRatioArtifact::GetVehicleIndex() const
{
   return m_VehicleIndex;
}

void xbrYieldStressRatioArtifact::SetVehicleWeight(Float64 W)
{
   m_VehicleWeight = W;
}

Float64 xbrYieldStressRatioArtifact::GetVehicleWeight() const
{
   return m_VehicleWeight;
}

void xbrYieldStressRatioArtifact::SetVehicleName(LPCTSTR str)
{
   m_strVehicleName = str;
}

std::_tstring xbrYieldStressRatioArtifact::GetVehicleName() const
{
   return m_strVehicleName;
}

void xbrYieldStressRatioArtifact::SetAllowableStressRatio(Float64 K)
{
   m_AllowableStressRatio = K;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetAllowableStressRatio() const
{
   return m_AllowableStressRatio;
}

void xbrYieldStressRatioArtifact::SetDeadLoadFactor(Float64 gDC)
{
   m_gDC = gDC;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetDeadLoadFactor() const
{
   return m_gDC;
}

void xbrYieldStressRatioArtifact::SetDeadLoadMoment(Float64 Mdc)
{
   m_Mdc = Mdc;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetDeadLoadMoment() const
{
   return m_Mdc;
}

void xbrYieldStressRatioArtifact::SetWearingSurfaceFactor(Float64 gDW)
{
   m_gDW = gDW;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetWearingSurfaceFactor() const
{
   return m_gDW;
}

void xbrYieldStressRatioArtifact::SetWearingSurfaceMoment(Float64 Mdw)
{
   m_Mdw = Mdw;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetWearingSurfaceMoment() const
{
   return m_Mdw;
}

void xbrYieldStressRatioArtifact::SetCreepFactor(Float64 gCR)
{
   m_gCR = gCR;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetCreepFactor() const
{
   return m_gCR;
}

void xbrYieldStressRatioArtifact::SetCreepMoment(Float64 Mcr)
{
   m_Mcr = Mcr;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetCreepMoment() const
{
   return m_Mcr;
}

void xbrYieldStressRatioArtifact::SetShrinkageFactor(Float64 gSH)
{
   m_gSH = gSH;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetShrinkageFactor() const
{
   return m_gSH;
}

void xbrYieldStressRatioArtifact::SetShrinkageMoment(Float64 Msh)
{
   m_Msh = Msh;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetShrinkageMoment() const
{
   return m_Msh;
}

void xbrYieldStressRatioArtifact::SetRelaxationFactor(Float64 gRE)
{
   m_gRE = gRE;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetRelaxationFactor() const
{
   return m_gRE;
}

void xbrYieldStressRatioArtifact::SetRelaxationMoment(Float64 Mre)
{
   m_Mre = Mre;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetRelaxationMoment() const
{
   return m_Mre;
}

void xbrYieldStressRatioArtifact::SetSecondaryEffectsFactor(Float64 gPS)
{
   m_gPS = gPS;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetSecondaryEffectsFactor() const
{
   return m_gPS;
}

void xbrYieldStressRatioArtifact::SetSecondaryEffectsMoment(Float64 Mps)
{
   m_Mps = Mps;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetSecondaryEffectsMoment() const
{
   return m_Mps;
}

void xbrYieldStressRatioArtifact::SetLiveLoadFactor(Float64 gLL)
{
   m_gLL = gLL;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetLiveLoadFactor() const
{
   return m_gLL;
}

void xbrYieldStressRatioArtifact::SetLiveLoadMoment(Float64 Mllim)
{
   m_Mllim = Mllim;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetLiveLoadMoment() const
{
   return m_Mllim;
}

void xbrYieldStressRatioArtifact::SetCrackingMoment(Float64 Mcr)
{
   m_Mcrack = Mcr;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetCrackingMoment() const
{
   return m_Mcrack;
}

void xbrYieldStressRatioArtifact::SetIcr(Float64 Icr)
{
   m_Icrack = Icr;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetIcr() const
{
   return m_Icrack;
}

void xbrYieldStressRatioArtifact::SetCrackDepth(Float64 c)
{
   m_c = c;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetCrackDepth() const
{
   return m_c;
}

void xbrYieldStressRatioArtifact::SetRebar(Float64 db,Float64 fb,Float64 fyb,Float64 Eb)
{
   m_db  = db;
   m_fb  = fb;
   m_fyb = fyb;
   m_Eb  = Eb;
   m_bRebar = true;
}

bool xbrYieldStressRatioArtifact::GetRebar(Float64* pdb, Float64* pfb,Float64* pfyb,Float64* pEb) const
{
   *pdb  = m_db;
   *pfb  = m_fb;
   *pfyb = m_fyb;
   *pEb  = m_Eb;
   return m_bRebar;
}

void xbrYieldStressRatioArtifact::SetEg(Float64 Eg)
{
   m_Eg = Eg;
   m_bRFComputed = false;
}

Float64 xbrYieldStressRatioArtifact::GetEg() const
{
   return m_Eg;
}

Float64 xbrYieldStressRatioArtifact::GetExcessMoment() const
{
   Float64 M = m_gDC*m_Mdc + m_gDW*m_Mdw + m_gCR*m_Mcr + m_gSH*m_Msh + m_gRE*m_Mre + m_gPS*m_Mps + m_gLL*m_Mllim;
   // NOTE: m_Mllim includes the LLDF... don't include m_gM here

   if ( m_Mcrack < 0 )
   {
      // negative moment
      if ( m_Mcrack < M )
      {
         return 0; // section isn't cracked
      }
      else
      {
         return M - m_Mcrack;
      }
   }
   else
   {
      // positive moment
      if ( M < m_Mcrack )
      {
         return 0; // section isn't cracked
      }
      else
      {
         return M - m_Mcrack;
      }
   }
}

Float64 xbrYieldStressRatioArtifact::GetRebarCrackingStressIncrement() const
{
   ComputeStressRatios();
   return m_fcrRebar;
}

Float64 xbrYieldStressRatioArtifact::GetRebarStress() const
{
   ComputeStressRatios();
   return m_fsRebar;
}

Float64 xbrYieldStressRatioArtifact::GetRebarStressRatio() const
{
   ComputeStressRatios();
   return m_RebarRF;
}

Float64 xbrYieldStressRatioArtifact::GetRebarAllowableStress() const
{
   return m_AllowableStressRatio*m_fyb;
}

Float64 xbrYieldStressRatioArtifact::GetStrandCrackingStressIncrement() const
{
   ComputeStressRatios();
   return m_fcrStrand;
}

Float64 xbrYieldStressRatioArtifact::GetStrandStress() const
{
   ComputeStressRatios();
   return m_fsStrand;
}

Float64 xbrYieldStressRatioArtifact::GetStrandStressRatio() const
{
   ComputeStressRatios();
   return m_StrandRF;
}

Float64 xbrYieldStressRatioArtifact::GetStrandAllowableStress() const
{
   return m_AllowableStressRatio*m_fyps;
}

Float64 xbrYieldStressRatioArtifact::GetTendonCrackingStressIncrement() const
{
   ComputeStressRatios();
   return m_fcrTendon;
}

Float64 xbrYieldStressRatioArtifact::GetTendonStress() const
{
   ComputeStressRatios();
   return m_fsTendon;
}

Float64 xbrYieldStressRatioArtifact::GetTendonStressRatio() const
{
   ComputeStressRatios();
   return m_TendonRF;
}

Float64 xbrYieldStressRatioArtifact::GetTendonAllowableStress() const
{
   return m_AllowableStressRatio*m_fypt;
}

Float64 xbrYieldStressRatioArtifact::GetStressRatio() const
{
   return Min(GetRebarStressRatio(),GetStrandStressRatio(),GetTendonStressRatio());
}

void xbrYieldStressRatioArtifact::MakeCopy(const xbrYieldStressRatioArtifact& rOther)
{
   m_POI            = rOther.m_POI;
   m_RatingType     = rOther.m_RatingType;
   m_VehicleIndex   = rOther.m_VehicleIndex;
   m_bRFComputed    = rOther.m_bRFComputed;
   m_VehicleWeight  = rOther.m_VehicleWeight;
   m_strVehicleName = rOther.m_strVehicleName;
   m_RebarRF        = rOther.m_RebarRF;
   m_AllowableStressRatio = rOther.m_AllowableStressRatio;
   m_fcrRebar       = rOther.m_fcrRebar;
   m_fsRebar        = rOther.m_fsRebar;
   m_Mdc            = rOther.m_Mdc;
   m_Mdw            = rOther.m_Mdw;
   m_Mcr            = rOther.m_Mcr;
   m_Msh            = rOther.m_Msh;
   m_Mre            = rOther.m_Mre;
   m_Mps            = rOther.m_Mps;
   m_Mllim          = rOther.m_Mllim;
   m_Mcrack         = rOther.m_Mcrack;
   m_Icrack         = rOther.m_Icrack;
   m_c              = rOther.m_c;
   m_Eg             = rOther.m_Eg;
   m_gDC            = rOther.m_gDC;
   m_gDW            = rOther.m_gDW;
   m_gCR            = rOther.m_gCR;
   m_gSH            = rOther.m_gSH;
   m_gRE            = rOther.m_gRE;
   m_gPS            = rOther.m_gPS;
   m_gLL            = rOther.m_gLL;

   m_bRebar = rOther.m_bRebar;
   m_db     = rOther.m_db;
   m_fb     = rOther.m_fb;
   m_fyb    = rOther.m_fyb;
   m_Eb     = rOther.m_Eb;
}

void xbrYieldStressRatioArtifact::MakeAssignment(const xbrYieldStressRatioArtifact& rOther)
{
   MakeCopy( rOther );
}


void xbrYieldStressRatioArtifact::ComputeStressRatios() const
{
   if ( m_bRFComputed )
   {
      return;
   }

   if ( m_bRebar )
   {
      ComputeStressRatio(m_db,m_Eb,m_fb,m_fyb,&m_fcrRebar,&m_fsRebar,&m_RebarRF);
   }
   else
   {
      m_RebarRF = DBL_MAX;
      m_fcrRebar = 0;
      m_fsRebar = 0;
   }
}

void xbrYieldStressRatioArtifact::ComputeStressRatio(Float64 d,Float64 E,Float64 fbcr,Float64 fy,Float64* pfcr,Float64* pfs,Float64* pRF) const
{
   // moment in excess of cracking
   Float64 M = GetExcessMoment();

   Float64 fcr = (E/m_Eg)*fabs(M)*(d-m_c)/m_Icrack; // stress added to strand at instance of cracking
   Float64 fs = fbcr + fcr; // total stress in strand just after cracking
   if ( IsLE(fs,0.0) )
   {
      *pRF = DBL_MAX;
   }
   else
   {
      *pRF = m_AllowableStressRatio*fy/fs;
   }

   ATLASSERT( 0 <= *pRF );

   *pfcr = fcr;
   *pfs = fs;
}
