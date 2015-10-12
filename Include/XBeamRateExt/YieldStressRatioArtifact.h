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

#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <XBeamRateExt\PointOfInterest.h>

/*****************************************************************************
CLASS 
   xbrYieldStressRatioArtifact

   Artifact for permit rating yield stress ratio evaluation


DESCRIPTION
   Artifact for permit rating yield stress ratio evaluation. See LRFR 6A.5.4.2.2b


COPYRIGHT
   Copyright © 1997-2010
   Washington State Department Of Transportation
   All Rights Reserved

LOG
   rab : 01.07.2010 : Created file
*****************************************************************************/

class XBREXTCLASS xbrYieldStressRatioArtifact
{
public:
   xbrYieldStressRatioArtifact();
   xbrYieldStressRatioArtifact(const xbrYieldStressRatioArtifact& rOther);
   virtual ~xbrYieldStressRatioArtifact();

   xbrYieldStressRatioArtifact& operator = (const xbrYieldStressRatioArtifact& rOther);

   void SetPointOfInterest(const xbrPointOfInterest& poi);
   const xbrPointOfInterest& GetPointOfInterest() const;

   void SetRatingType(pgsTypes::LoadRatingType ratingType);
   pgsTypes::LoadRatingType GetLoadRatingType() const;

   void SetPermitRatingMethod(xbrTypes::PermitRatingMethod permitRatingMethod);
   xbrTypes::PermitRatingMethod GetPermitRatingMethod() const;

   void SetVehicleIndex(VehicleIndexType vehicleIdx);
   VehicleIndexType GetVehicleIndex() const;

   void SetVehicleWeight(Float64 W);
   Float64 GetVehicleWeight() const;

   void SetVehicleName(LPCTSTR str);
   std::_tstring GetVehicleName() const;

   void SetAllowableStressRatio(Float64 K);
   Float64 GetAllowableStressRatio() const;

   void SetDeadLoadFactor(Float64 gDC);
   Float64 GetDeadLoadFactor() const;

   void SetDeadLoadMoment(Float64 Mdc);
   Float64 GetDeadLoadMoment() const;

   void SetWearingSurfaceFactor(Float64 gDW);
   Float64 GetWearingSurfaceFactor() const;

   void SetWearingSurfaceMoment(Float64 Mdw);
   Float64 GetWearingSurfaceMoment() const;

   void SetCreepFactor(Float64 gCR);
   Float64 GetCreepFactor() const;

   void SetCreepMoment(Float64 Mcr);
   Float64 GetCreepMoment() const;

   void SetShrinkageFactor(Float64 gSH);
   Float64 GetShrinkageFactor() const;

   void SetShrinkageMoment(Float64 Msh);
   Float64 GetShrinkageMoment() const;

   void SetRelaxationFactor(Float64 gRE);
   Float64 GetRelaxationFactor() const;

   void SetRelaxationMoment(Float64 Mre);
   Float64 GetRelaxationMoment() const;

   void SetSecondaryEffectsFactor(Float64 gPS);
   Float64 GetSecondaryEffectsFactor() const;

   void SetSecondaryEffectsMoment(Float64 Mps);
   Float64 GetSecondaryEffectsMoment() const;

   void SetLiveLoadFactor(Float64 gLL);
   Float64 GetLiveLoadFactor() const;

   void SetLiveLoadMoment(Float64 Mllim);
   Float64 GetLiveLoadMoment() const;

   void SetCrackingMoment(Float64 Mcr);
   Float64 GetCrackingMoment() const;

   void SetIcr(Float64 Icr);
   Float64 GetIcr() const;

   void SetCrackDepth(Float64 c);
   Float64 GetCrackDepth() const;

   void SetRebar(Float64 db,Float64 fb,Float64 fyb,Float64 Eb);
   bool GetRebar(Float64* pdb, Float64* pfb,Float64* pfyb,Float64* pEb) const;

   void SetEg(Float64 Eg);
   Float64 GetEg() const;

   Float64 GetExcessMoment() const;

   Float64 GetRebarCrackingStressIncrement() const;
   Float64 GetRebarStress() const;
   Float64 GetRebarStressRatio() const;
   Float64 GetRebarAllowableStress() const;

   Float64 GetStressRatio() const;

protected:
   void MakeCopy(const xbrYieldStressRatioArtifact& rOther);
   virtual void MakeAssignment(const xbrYieldStressRatioArtifact& rOther);
   void ComputeStressRatios() const;
   void ComputeStressRatio(Float64 d,Float64 E,Float64 fbcr,Float64 fy,Float64* pfcr,Float64* pfs,Float64* pRF) const;

   mutable bool m_bRFComputed;

   mutable Float64 m_RebarRF;
   mutable Float64 m_fcrRebar;
   mutable Float64 m_fsRebar;

   xbrPointOfInterest m_POI;

   pgsTypes::LoadRatingType m_RatingType;
   xbrTypes::PermitRatingMethod m_PermitRatingMethod;

   VehicleIndexType m_VehicleIndex;
   Float64 m_VehicleWeight;
   std::_tstring m_strVehicleName;

   Float64 m_AllowableStressRatio; // stress in reinforcement should not exceed this times the yield strength
   Float64 m_Mdc;
   Float64 m_Mdw;
   Float64 m_Mcr;
   Float64 m_Msh;
   Float64 m_Mre;
   Float64 m_Mps;
   Float64 m_Mllim;
   Float64 m_Mcrack;
   Float64 m_Icrack;
   Float64 m_c;
   Float64 m_Eg;
   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gCR;
   Float64 m_gSH;
   Float64 m_gRE;
   Float64 m_gPS;
   Float64 m_gLL;

   bool m_bRebar;
   Float64 m_db;  // depth to reinforcement from extreme compression face
   Float64 m_fb;  // stress in reinforcement beforc cracking
   Float64 m_fyb; // yield strength
   Float64 m_Eb;  // mod E.
};
