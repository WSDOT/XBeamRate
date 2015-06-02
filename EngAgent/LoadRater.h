
#pragma once

#include <XBeamRateExt\RatingArtifact.h>

class xbrLoadRater
{
public:
   xbrLoadRater(IBroker* pBroker);
   virtual ~xbrLoadRater(void);

   void SetBroker(IBroker* pBroker);

   xbrRatingArtifact RateXBeam(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);

protected:
   IBroker* m_pBroker;

   void MomentRating(const std::vector<xbrPointOfInterest>& vPoi,bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact);
   void ShearRating(const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,xbrRatingArtifact& ratingArtifact);

   pgsTypes::LimitState GetStrengthLimitStateType(pgsTypes::LoadRatingType ratingType);

   void GetMoments(bool bPositiveMoment,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx, const std::vector<xbrPointOfInterest>& vPOI, std::vector<Float64>& vDC,std::vector<Float64>& vDW, std::vector<Float64>& vLLIMmin, std::vector<Float64>& vLLIMmax, std::vector<Float64>& vAdjLLIMmin, std::vector<Float64>& vAdjLLIMmax);
   //Float64 GetStrengthLiveLoadFactor(pgsTypes::LoadRatingType ratingType,AxleConfiguration& axleConfig);
   //Float64 GetServiceLiveLoadFactor(pgsTypes::LoadRatingType ratingType);
};
