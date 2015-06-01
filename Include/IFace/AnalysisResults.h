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

typedef enum XBRProductForceType
{
   pftLowerXBeam,
   pftUpperXBeam,
   pftDCReactions,
   pftDWReactions
} XBRProductForceType;

typedef enum XBRCombinedForceType
{
   lcDC,
   lcDW
} XBRCombinedForceType;


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
   virtual const std::vector<LowerXBeamLoad>& GetLowerCrossBeamLoading() = 0;
   virtual Float64 GetUpperCrossBeamLoading() = 0;
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
   virtual Float64 GetMoment(XBRProductForceType pfType,const xbrPointOfInterest& poi) = 0;
   virtual sysSectionValue GetShear(XBRProductForceType pfType,const xbrPointOfInterest& poi) = 0;

   // Combined Load Results
   virtual Float64 GetMoment(XBRCombinedForceType lcType,const xbrPointOfInterest& poi) = 0;
   virtual sysSectionValue GetShear(XBRCombinedForceType lcType,const xbrPointOfInterest& poi) = 0;

   // Live Load Results
   virtual void GetMoment(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax) = 0;
   virtual void GetShear(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax) = 0;
   virtual void GetMoment(pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax) = 0;
   virtual void GetShear(pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax) = 0;

   // Limit State Results
   virtual void GetMoment(pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax) = 0;
   virtual void GetShear(pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax) = 0;
};
