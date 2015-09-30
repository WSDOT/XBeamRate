///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
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
#include <XBeamRateExt\RatingArtifact.h>

#include <WBFLRCCapacity.h>
typedef struct MomentCapacityDetails
{
   Float64 dt; // distance from compression face to extreme tensile reinforcement
   Float64 de; // distance from compression face to resultant tensile force
   Float64 dc; // distance from compression face to resultant compression force
   Float64 phi; // capacity reduction factor
   Float64 Mn; // nominal capacity
   Float64 Mr; // nominal resistance (phi*Mn)
   CComPtr<IRCSolutionEx> solution;
} MomentCapacityDetails;


typedef struct CrackingMomentDetails
{
   Float64 Mcr;  // Cracking moment
   Float64 Mdnc; // Dead load moment on non-composite XBeam (lower x-beam dead load)
   Float64 fr;   // Rupture stress
   Float64 fcpe; // Stress at bottom of non-composite XBeam due to prestress
   Float64 Snc;  // Section modulus of non-composite XBeam
   Float64 Sc;   // Section modulus of composite XBeam
   Float64 McrLimit; // Limiting cracking moment ... per 2nd Edition + 2003 interims (changed in 2005 interims)

   Float64 g1,g2,g3; // gamma factors from LRFD 5.7.3.3.2 (LRFD 6th Edition, 2012)
} CrackingMomentDetails;

typedef struct MinMomentCapacityDetails
{
   Float64 Mr;     // Nominal resistance (phi*Mn)
   Float64 Mcr;
   Float64 MrMin;  // Minimum nominal resistance Max(MrMin1,MrMin2)
   Float64 MrMin1; // 1.2Mcr
   Float64 MrMin2; // 1.33Mu
   Float64 Mu;
} MinMomentCapacityDetails;


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
   virtual const MomentCapacityDetails& GetMomentCapacityDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;

   virtual Float64 GetCrackingMoment(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;
   virtual const CrackingMomentDetails& GetCrackingMomentDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;

   virtual Float64 GetMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;
   virtual const MinMomentCapacityDetails& GetMinMomentCapacityDetails(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) = 0;
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
