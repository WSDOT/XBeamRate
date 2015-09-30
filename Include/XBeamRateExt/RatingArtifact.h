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
