
#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <XBeamRateExt\PointOfInterest.h>
#include <XBeamRateExt\MomentRatingArtifact.h>
#include <XBeamRateExt\ShearRatingArtifact.h>

/*****************************************************************************
CLASS 
   xbrRatingArtifact

   Artifact for load rating analysis


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

class XBREXTCLASS xbrRatingArtifact
{
public:
   typedef std::vector<std::pair<xbrPointOfInterest,xbrMomentRatingArtifact>> MomentRatings;
   typedef std::vector<std::pair<xbrPointOfInterest,xbrShearRatingArtifact>>  ShearRatings;

   xbrRatingArtifact();
   xbrRatingArtifact(const xbrRatingArtifact& rOther);
   virtual ~xbrRatingArtifact();

   xbrRatingArtifact& operator = (const xbrRatingArtifact& rOther);

   void AddArtifact(const xbrPointOfInterest& poi,const xbrMomentRatingArtifact& artifact,bool bPositiveMoment);
   void AddArtifact(const xbrPointOfInterest& poi,const xbrShearRatingArtifact& artifact);

   const MomentRatings& GetMomentRatings(bool bPositiveMoment) const;
   const ShearRatings& GetShearRatings() const;

   Float64 GetMomentRatingFactor(bool bPositiveMoment) const;
   Float64 GetMomentRatingFactorEx(bool bPositiveMoment,const xbrMomentRatingArtifact** ppArtifact) const;

   Float64 GetShearRatingFactor() const;
   Float64 GetShearRatingFactorEx(const xbrShearRatingArtifact** ppArtifact) const;

   Float64 GetRatingFactor() const;
   Float64 GetRatingFactorEx(const xbrMomentRatingArtifact** ppPositiveMoment,const xbrMomentRatingArtifact** ppNegativeMoment,
                             const xbrShearRatingArtifact** ppShear) const;

   void GetSafePostingLoad(Float64* pPostingLoad,Float64* pWeight,Float64* pRF,std::_tstring* pVehicle) const;

protected:
   void MakeCopy(const xbrRatingArtifact& rOther);
   virtual void MakeAssignment(const xbrRatingArtifact& rOther);

   MomentRatings m_PositiveMomentRatings;
   MomentRatings m_NegativeMomentRatings;
   ShearRatings  m_ShearRatings;
};
