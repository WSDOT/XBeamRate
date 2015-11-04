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

#include <XBeamRateExt\PointOfInterest.h>
#include <System\SectionValue.h>

struct LowerXBeamLoad
{
   Float64 Xs;
   Float64 Xe;
   Float64 Ws;
   Float64 We;
};

struct WheelLinePlacement
{
   Float64 P;
   Float64 Xxb;
};

typedef std::vector<WheelLinePlacement> WheelLineConfiguration;


/*****************************************************************************
INTERFACE
   IProductLoads

DESCRIPTION
   Interface for getting loads
*****************************************************************************/
// {7ECAB28C-6887-4170-83F4-BA5DCAA53305}
DEFINE_GUID(IID_IXBRProductForces, 
0x7ecab28c, 0x6887, 0x4170, 0x83, 0xf4, 0xba, 0x5d, 0xca, 0xa5, 0x33, 0x5);
interface IXBRProductForces : public IUnknown
{
   virtual const std::vector<LowerXBeamLoad>& GetLowerCrossBeamLoading(PierIDType pierID) = 0;
   virtual Float64 GetUpperCrossBeamLoading(PierIDType pierID) = 0;

   virtual IndexType GetLiveLoadConfigurationCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType) = 0;
   virtual IndexType GetLoadedLaneCount(PierIDType pierID,IndexType liveLoadConfigIdx) = 0;
   virtual std::vector<Float64> GetWheelLineLocations(PierIDType pierID) = 0;
   virtual WheelLineConfiguration GetLiveLoadConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType liveLoadConfigIdx,IndexType permitLaneIdx) = 0;

   // Returns vectors contain the live load configuration indicies associated with the N min/max moments
   // Used only for WSDOT Permit rating method
   virtual void GetGoverningMomentLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvMin,std::vector<IndexType>* pvMax) = 0;
   virtual void GetGoverningShearLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvLLConfig) = 0;
};


/*****************************************************************************
INTERFACE
   IAnalysisResults

DESCRIPTION
   Interface for getting analysis results
*****************************************************************************/
// {D8B2E731-710A-48ae-9916-AC152FEF2031}
DEFINE_GUID(IID_IXBRAnalysisResults, 
0xd8b2e731, 0x710a, 0x48ae, 0x99, 0x16, 0xac, 0x15, 0x2f, 0xef, 0x20, 0x31);
interface IXBRAnalysisResults : IUnknown
{
   // Product Load Results
   virtual Float64 GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi) = 0;
   virtual sysSectionValue GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi) = 0;
   virtual std::vector<Float64> GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi) = 0;
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi) = 0;

   // Combined Load Results
   virtual Float64 GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi) = 0;
   virtual sysSectionValue GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi) = 0;
   virtual std::vector<Float64> GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi) = 0;
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi) = 0;

   // Live Load Results

   // Live load results for a specified vehicle in a specified loading configuration
   virtual Float64 GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi) = 0;
   virtual sysSectionValue GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi) = 0;
   virtual std::vector<Float64> GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi) = 0;
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi) = 0;

   // Live results for a specified vehcile in a specified permit loading configuration with the permit vehicle in a specified lane
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,Float64* pMpermit,Float64* pMlegal) = 0;
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMpermit,std::vector<Float64>* pvMlegal) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,sysSectionValue* pVpermit,sysSectionValue* pVlegal) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvVpermit,std::vector<sysSectionValue>* pvVlegal) = 0;

   // Min/Max live load results for a specified vehicle
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration*/) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*,WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration*/) = 0;
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration*/) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration*/) = 0;

   // Min/Max live load results for an envelope of all the vehicles associated with a rating type
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,VehicleIndexType* pMinVehicleIdx,VehicleIndexType* pMaxVehicleIdx) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,VehicleIndexType* pMinLeftVehicleIdx,VehicleIndexType* pMinRightVehicleIdx,VehicleIndexType* pMaxLeftVehicleIdx,VehicleIndexType* pMaxRightVehicleIdx) = 0;
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<VehicleIndexType>* pvMinVehicleIdx,std::vector<VehicleIndexType>* pvMaxVehicleIdx) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<VehicleIndexType>* pvMinLeftVehicleIdx,std::vector<VehicleIndexType>* pvMinRightVehicleIdx,std::vector<VehicleIndexType>* pvMaxLeftVehicleIdx,std::vector<VehicleIndexType>* pvMaxRightVehicleIdx) = 0;

   // Limit State Results
   virtual void GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax) = 0;
   virtual void GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax) = 0;
   virtual void GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax) = 0;
};
