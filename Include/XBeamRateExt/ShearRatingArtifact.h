
#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <XBeamRateExt\PointOfInterest.h>

/*****************************************************************************
CLASS 
   xbrShearRatingArtifact

   Artifact for shear load rating analysis


DESCRIPTION
   Artifact for load rating analysis. Holds rating artificts for
   design, legal, and permit load ratings.


COPYRIGHT
   Copyright © 1997-2009
   Washington State Department Of Transportation
   All Rights Reserved

LOG
   rab : 12.07.2009 : Created file
*****************************************************************************/

class XBREXTCLASS xbrShearRatingArtifact
{
public:
   xbrShearRatingArtifact();
   xbrShearRatingArtifact(const xbrShearRatingArtifact& rOther);
   virtual ~xbrShearRatingArtifact();

   xbrShearRatingArtifact& operator = (const xbrShearRatingArtifact& rOther);

   void SetPointOfInterest(const xbrPointOfInterest& poi);
   const xbrPointOfInterest& GetPointOfInterest() const;

   void SetRatingType(pgsTypes::LoadRatingType ratingType);
   pgsTypes::LoadRatingType GetLoadRatingType() const;

   void SetVehicleIndex(VehicleIndexType vehicleIdx);
   VehicleIndexType GetVehicleIndex() const;

   void SetVehicleWeight(Float64 W);
   Float64 GetVehicleWeight() const;

   void SetVehicleName(LPCTSTR str);
   std::_tstring GetVehicleName() const;

   void SetSystemFactor(Float64 systemFactor);
   Float64 GetSystemFactor() const;

   void SetConditionFactor(Float64 conditionFactor);
   Float64 GetConditionFactor() const;

   void SetCapacityReductionFactor(Float64 phi);
   Float64 GetCapacityReductionFactor() const;

   void SetNominalShearCapacity(Float64 Vn);
   Float64 GetNominalShearCapacity() const;

   void SetDeadLoadFactor(Float64 gDC);
   Float64 GetDeadLoadFactor() const;

   void SetDeadLoadShear(Float64 Vdc);
   Float64 GetDeadLoadShear() const;

   void SetWearingSurfaceFactor(Float64 gDW);
   Float64 GetWearingSurfaceFactor() const;

   void SetWearingSurfaceShear(Float64 Vdw);
   Float64 GetWearingSurfaceShear() const;

   void SetLiveLoadFactor(Float64 gLL);
   Float64 GetLiveLoadFactor() const;

   void SetLiveLoadShear(Float64 Vllim);
   Float64 GetLiveLoadShear() const;

   void SetAdjacentLaneLiveLoadShear(Float64 Vllim);
   Float64 GetAdjacentLaneLiveLoadShear() const;

   Float64 GetRatingFactor() const;

protected:
   void MakeCopy(const xbrShearRatingArtifact& rOther);
   virtual void MakeAssignment(const xbrShearRatingArtifact& rOther);

   mutable bool m_bRFComputed;
   mutable Float64 m_RF;

   xbrPointOfInterest m_POI;

   pgsTypes::LoadRatingType m_RatingType;

   VehicleIndexType m_VehicleIndex;
   Float64 m_VehicleWeight;
   std::_tstring m_strVehicleName;

   Float64 m_SystemFactor;
   Float64 m_ConditionFactor;
   Float64 m_CapacityRedutionFactor;
   Float64 m_Vn;
   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gLL;
   Float64 m_Vdc;
   Float64 m_Vdw;
   Float64 m_Vllim;
   Float64 m_AdjVllim;
};
