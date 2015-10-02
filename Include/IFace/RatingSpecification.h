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

#include <WbflTypes.h>

/*****************************************************************************
INTERFACE
   IRatingSpecification

   Interface to manipulating the rating specification input

DESCRIPTION
   Interface to manipulating the rating specification input
*****************************************************************************/
// {34BA2439-C0A2-4171-9FDC-B4307036C07C}
DEFINE_GUID(IID_IXBRRatingSpecification, 
0x34ba2439, 0xc0a2, 0x4171, 0x9f, 0xdc, 0xb4, 0x30, 0x70, 0x36, 0xc0, 0x7c);
interface IXBRRatingSpecification : IUnknown
{
   virtual bool IsRatingEnabled(pgsTypes::LoadRatingType ratingType) = 0;
   virtual void EnableRating(pgsTypes::LoadRatingType ratingType,bool bEnable) = 0;

   //virtual std::_tstring GetRatingSpecification() = 0;
   //virtual void SetRatingSpecification(const std::_tstring& spec) = 0;

   //virtual void SetADTT(Int16 adtt) = 0;
   //virtual Int16 GetADTT() = 0; // < 0 = Unknown

   virtual void RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear) = 0;
   virtual bool RateForShear(pgsTypes::LoadRatingType ratingType) = 0;

   //// Per last paragraph in MBE 6A.4.4.2.1a, the lane load may be excluded and the 0.75 truck factor taken as 1.0
   //// for ADTT < 500 and in the Engineer's judgement it is warranted
   //virtual void ExcludeLegalLoadLaneLoading(bool bExclude) = 0;
   //virtual bool ExcludeLegalLoadLaneLoading() = 0;

   // Evalute yield stress in reinforcement MBE 6A.5.4.2.2b
   virtual void CheckYieldStressLimit(bool bCheckYieldStress) = 0;
   virtual bool CheckYieldStressLimit() = 0;

   // returns fraction of yield stress that reinforcement can be stressed to during
   // a permit load rating evaluation MBE 6A.5.4.2.2b
   virtual void SetYieldStressLimitCoefficient(Float64 x) = 0;
   virtual Float64 GetYieldStressLimitCoefficient() = 0;

   //// Permit type for rating for special/limited crossing permit vehicle
   //virtual void SetSpecialPermitType(pgsTypes::SpecialPermitType type) = 0;
   //virtual pgsTypes::SpecialPermitType GetSpecialPermitType() = 0;

   // Controls the analysis type to be used for superstructure reactions obtained
   // from PGSuper when PGSuper is in Envelope mode
   virtual pgsTypes::AnalysisType GetAnalysisMethodForReactions() = 0;
   virtual void SetAnalysisMethodForReactions(pgsTypes::AnalysisType analysisType) = 0;

   // Controls the method used to rating factors for permit loading cases
   virtual xbrTypes::PermitRatingMethod GetPermitRatingMethod() = 0;
   virtual void SetPermitRatingMethod(xbrTypes::PermitRatingMethod permitRatingMethod) = 0;
};

/*****************************************************************************
INTERFACE
   IRatingSpecificationEventSink

   Callback interface for rating specification input change events.

DESCRIPTION
   Callback interface for rating specification input change events.
*****************************************************************************/
// {A6C8DDDC-572F-4e59-9B58-009A147DEB6B}
DEFINE_GUID(IID_IXBRRatingSpecificationEventSink, 
0xa6c8dddc, 0x572f, 0x4e59, 0x9b, 0x58, 0x0, 0x9a, 0x14, 0x7d, 0xeb, 0x6b);
interface IXBRRatingSpecificationEventSink : IUnknown
{
   virtual HRESULT OnRatingSpecificationChanged() = 0;
};
