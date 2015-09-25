#pragma once

#include <XBeamRateExt\PointOfInterest.h>
#include <XBeamRateExt\RatingArtifact.h>

/*****************************************************************************
INTERFACE
   IXBRMomentCapacity

DESCRIPTION
   Interface for getting moment capacity information
*****************************************************************************/
// {15281829-5F1E-4a78-9A9C-E619A7B1E0F6}
DEFINE_GUID(IID_IXBRMomentCapacity, 
0x15281829, 0x5f1e, 0x4a78, 0x9a, 0x9c, 0xe6, 0x19, 0xa7, 0xb1, 0xe0, 0xf6);
interface IXBRMomentCapacity : IUnknown
{
   virtual Float64 GetMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;
};

/*****************************************************************************
INTERFACE
   IXBRShearCapacity

DESCRIPTION
   Interface for getting shear capacity information
*****************************************************************************/
// {BD0D6DBC-2127-4413-BB1D-16BBD33B054D}
DEFINE_GUID(IID_IXBRShearCapacity, 
0xbd0d6dbc, 0x2127, 0x4413, 0xbb, 0x1d, 0x16, 0xbb, 0xd3, 0x3b, 0x5, 0x4d);
interface IXBRShearCapacity : IUnknown
{
   virtual Float64 GetShearCapacity(PierIDType pierID,const xbrPointOfInterest& poi) = 0;
};

/*****************************************************************************
INTERFACE
   IXBRArtiract

DESCRIPTION
   Interface for getting analysis artifacts
*****************************************************************************/
// {E7B694B8-1B81-4b53-A737-F2153CCCB5CA}
DEFINE_GUID(IID_IXBRArtifact, 
0xe7b694b8, 0x1b81, 0x4b53, 0xa7, 0x37, 0xf2, 0x15, 0x3c, 0xcc, 0xb5, 0xca);
interface IXBRArtifact : IUnknown
{
   virtual const xbrRatingArtifact* GetXBeamRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) = 0;
};
