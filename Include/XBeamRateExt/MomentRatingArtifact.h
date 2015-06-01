
#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <XBeamRateExt\PointOfInterest.h>

/*****************************************************************************
CLASS 
   xbrMomentRatingArtifact

   Artifact for moment load rating analysis


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

class XBREXTCLASS xbrMomentRatingArtifact
{
public:
   xbrMomentRatingArtifact();
   xbrMomentRatingArtifact(const xbrMomentRatingArtifact& rOther);
   virtual ~xbrMomentRatingArtifact();

   xbrMomentRatingArtifact& operator = (const xbrMomentRatingArtifact& rOther);

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

   void SetCapacityReductionFactor(Float64 phiMoment);
   Float64 GetCapacityReductionFactor() const;

   void SetMinimumReinforcementFactor(Float64 K);
   Float64 GetMinimumReinforcementFactor() const;

   void SetNominalMomentCapacity(Float64 Mn);
   Float64 GetNominalMomentCapacity() const;

   void SetDeadLoadFactor(Float64 gDC);
   Float64 GetDeadLoadFactor() const;

   void SetDeadLoadMoment(Float64 Mdc);
   Float64 GetDeadLoadMoment() const;

   void SetWearingSurfaceFactor(Float64 gDW);
   Float64 GetWearingSurfaceFactor() const;

   void SetWearingSurfaceMoment(Float64 Mdw);
   Float64 GetWearingSurfaceMoment() const;

   void SetLiveLoadFactor(Float64 gLL);
   Float64 GetLiveLoadFactor() const;

   void SetLiveLoadMoment(Float64 Mllim);
   Float64 GetLiveLoadMoment() const;

   void SetAdjacentLaneLiveLoadMoment(Float64 Mllim);
   Float64 GetAdjacentLaneLiveLoadMoment() const;

   Float64 GetRatingFactor() const;

protected:
   void MakeCopy(const xbrMomentRatingArtifact& rOther);
   virtual void MakeAssignment(const xbrMomentRatingArtifact& rOther);

   mutable bool m_bRFComputed;
   mutable Float64 m_RF;

   xbrPointOfInterest m_POI;

   pgsTypes::LoadRatingType m_RatingType;

   VehicleIndexType m_VehicleIndex;
   Float64 m_VehicleWeight;
   std::_tstring m_strVehicleName;

   Float64 m_SystemFactor;
   Float64 m_ConditionFactor;
   Float64 m_MinimumReinforcementFactor;
   Float64 m_CapacityRedutionFactor;
   Float64 m_Mn;
   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gLL;
   Float64 m_Mdc;
   Float64 m_Mdw;
   Float64 m_Mllim;
   Float64 m_AdjMllim;
};
