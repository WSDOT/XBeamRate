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
#include <XBeamRateExt\RatingArtifact.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrRatingArtifact
****************************************************************************/
xbrRatingArtifact::xbrRatingArtifact()
{
}

xbrRatingArtifact::xbrRatingArtifact(const xbrRatingArtifact& rOther)
{
   MakeCopy(rOther);
}

xbrRatingArtifact::~xbrRatingArtifact()
{
}

xbrRatingArtifact& xbrRatingArtifact::operator=(const xbrRatingArtifact& rOther)
{
   if ( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

void xbrRatingArtifact::AddArtifact(const xbrPointOfInterest& poi,const xbrMomentRatingArtifact& artifact,bool bPositiveMoment)
{
   if ( bPositiveMoment )
   {
      m_PositiveMomentRatings.push_back( std::make_pair(poi,artifact) );
   }
   else
   {
      m_NegativeMomentRatings.push_back( std::make_pair(poi,artifact) );
   }
}

void xbrRatingArtifact::AddArtifact(const xbrPointOfInterest& poi,const xbrShearRatingArtifact& artifact)
{
   m_ShearRatings.push_back( std::make_pair(poi,artifact) );
}

const xbrRatingArtifact::MomentRatings& xbrRatingArtifact::GetMomentRatings(bool bPositiveMoment) const
{
   return (bPositiveMoment ? m_PositiveMomentRatings : m_NegativeMomentRatings);
}

const xbrRatingArtifact::ShearRatings& xbrRatingArtifact::GetShearRatings() const
{
   return m_ShearRatings;
}

Float64 xbrRatingArtifact::GetMomentRatingFactorEx(bool bPositiveMoment,const xbrMomentRatingArtifact** ppArtifact) const
{
   Float64 RF = DBL_MAX;
   (*ppArtifact) = NULL;

   MomentRatings::const_iterator iter;
   const MomentRatings* pRatings = (bPositiveMoment ? &m_PositiveMomentRatings : &m_NegativeMomentRatings);

   for ( iter = pRatings->begin(); iter != pRatings->end(); iter++ )
   {
      const xbrMomentRatingArtifact& artifact = iter->second;
      Float64 rating_factor = artifact.GetRatingFactor();
      if ( rating_factor < RF )
      {
         RF = rating_factor;
         (*ppArtifact) = &artifact;
      }
   }

   if ( *ppArtifact == NULL && 0 < pRatings->size() )
   {
      ATLASSERT(RF == DBL_MAX);
      (*ppArtifact) = &(pRatings->front().second);
   }

   return RF;
}

Float64 xbrRatingArtifact::GetMomentRatingFactor(bool bPositiveMoment) const
{
   const xbrMomentRatingArtifact* pArtifact;
   return GetMomentRatingFactorEx(bPositiveMoment,&pArtifact);
}

Float64 xbrRatingArtifact::GetShearRatingFactorEx(const xbrShearRatingArtifact** ppArtifact) const
{
   Float64 RF = DBL_MAX;
   (*ppArtifact) = NULL;

   ShearRatings::const_iterator iter;
   for ( iter = m_ShearRatings.begin(); iter != m_ShearRatings.end(); iter++ )
   {
      const xbrShearRatingArtifact& artifact = iter->second;
      Float64 rating_factor = artifact.GetRatingFactor();
      if ( rating_factor < RF )
      {
         RF = rating_factor;
         *ppArtifact = &artifact;
      }
   }

   if ( *ppArtifact == NULL && 0 < m_ShearRatings.size() )
   {
      ATLASSERT(RF == DBL_MAX);
      (*ppArtifact) = &(m_ShearRatings.front().second);
   }

   return RF;
}

Float64 xbrRatingArtifact::GetShearRatingFactor() const
{
   const xbrShearRatingArtifact* pArtifact;
   return GetShearRatingFactorEx(&pArtifact);
}

Float64 xbrRatingArtifact::GetRatingFactor() const
{
   const xbrMomentRatingArtifact* pPositiveMoment;
   const xbrMomentRatingArtifact* pNegativeMoment;
   const xbrShearRatingArtifact* pShear;

   return GetRatingFactorEx(&pPositiveMoment,&pNegativeMoment,&pShear);
}

Float64 xbrRatingArtifact::GetRatingFactorEx(const xbrMomentRatingArtifact** ppPositiveMoment,const xbrMomentRatingArtifact** ppNegativeMoment,
                                             const xbrShearRatingArtifact** ppShear) const
{
   Float64 RF_pM = GetMomentRatingFactorEx(true,ppPositiveMoment);
   Float64 RF_nM = GetMomentRatingFactorEx(false,ppNegativeMoment);
   Float64 RF_V  = GetShearRatingFactorEx(ppShear);

   Float64 RF = DBL_MAX;
   int i = -1; // initialize to an invalid value so that we know if a rating factor wasn't found
   if ( RF_pM < RF )
   {
      RF = RF_pM;
      i = 0;
   }

   if ( RF_nM < RF )
   {
      RF = RF_nM;
      i = 1;
   }

   if ( RF_V < RF )
   {
      RF = RF_V;
      i = 2;
   }

   // NULL out all but the controlling rating
   if ( i == 0 )
   {
      //(*ppPositiveMoment)            = NULL;
      (*ppNegativeMoment)            = NULL;
      (*ppShear)                     = NULL;
   }
   else if ( i == 1 )
   {
      (*ppPositiveMoment)            = NULL;
      //(*ppNegativeMoment)            = NULL;
      (*ppShear)                     = NULL;
   }
   else if ( i == 2 )
   {
      (*ppPositiveMoment)            = NULL;
      (*ppNegativeMoment)            = NULL;
      //(*ppShear)                     = NULL;
   }
   else
   {
      // ??? rating wasn't done?
      // this can happen if we are rating with a negative moment only truck
      // but the bridge is simple span... we run the truck because we want
      // reactions, but the rating factors are DBL_MAX...
      // since all types of ratings control equally, use positive moment as controlling factor
      ATLASSERT(i == -1);
      //(*ppPositiveMoment)            = NULL;
      (*ppNegativeMoment)            = NULL;
      (*ppShear)                     = NULL;
   }

   return RF;
}

void xbrRatingArtifact::GetSafePostingLoad(Float64* pPostingLoad,Float64* pWeight,Float64* pRF,std::_tstring* pVehicle) const
{
   Float64 posting_load = DBL_MAX;
   MomentRatings::const_iterator moment_iter;
   for ( moment_iter = m_PositiveMomentRatings.begin(); moment_iter != m_PositiveMomentRatings.end(); moment_iter++ )
   {
      const xbrMomentRatingArtifact& artifact = moment_iter->second;

      std::_tstring strVehicle = artifact.GetVehicleName();
      Float64 W = artifact.GetVehicleWeight();
      Float64 RF = artifact.GetRatingFactor();
      Float64 spl = W*(RF - 0.3)/0.7;
      if ( spl < 0 )
      {
         spl = 0;
      }

      if ( spl < posting_load )
      {
         posting_load = spl;
         *pWeight = W;
         *pRF = RF;
         *pVehicle = strVehicle;
      }
   }

   for ( moment_iter = m_NegativeMomentRatings.begin(); moment_iter != m_NegativeMomentRatings.end(); moment_iter++ )
   {
      const xbrMomentRatingArtifact& artifact = moment_iter->second;

      std::_tstring strVehicle = artifact.GetVehicleName();
      Float64 W = artifact.GetVehicleWeight();
      Float64 RF = artifact.GetRatingFactor();
      Float64 spl = W*(RF - 0.3)/0.7;
      if ( spl < 0 )
      {
         spl = 0;
      }

      if ( spl < posting_load )
      {
         posting_load = spl;
         *pWeight = W;
         *pRF = RF;
         *pVehicle = strVehicle;
      }
   }

   ShearRatings::const_iterator shear_iter;
   for ( shear_iter = m_ShearRatings.begin(); shear_iter != m_ShearRatings.end(); shear_iter++ )
   {
      const xbrShearRatingArtifact& artifact = shear_iter->second;

      std::_tstring strVehicle = artifact.GetVehicleName();
      Float64 W = artifact.GetVehicleWeight();
      Float64 RF = artifact.GetRatingFactor();
      Float64 spl = W*(RF - 0.3)/0.7;
      if ( spl < 0 )
      {
         spl = 0;
      }

      if ( spl < posting_load )
      {
         posting_load = spl;
         *pWeight = W;
         *pRF = RF;
         *pVehicle = strVehicle;
      }
   }

   *pPostingLoad = posting_load;
}

void xbrRatingArtifact::MakeCopy(const xbrRatingArtifact& rOther)
{
   m_PositiveMomentRatings = rOther.m_PositiveMomentRatings;
   m_NegativeMomentRatings = rOther.m_NegativeMomentRatings;
   m_ShearRatings  = rOther.m_ShearRatings;
}

void xbrRatingArtifact::MakeAssignment(const xbrRatingArtifact& rOther)
{
   MakeCopy( rOther );
}
