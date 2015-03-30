#pragma once

#include <XBeamRateExt\PointOfInterest.h>

/*****************************************************************************
INTERFACE
   IAnalysisResults

DESCRIPTION
   Interface for getting analysis results
*****************************************************************************/
// {D8B2E731-710A-48ae-9916-AC152FEF2031}
DEFINE_GUID(IID_IAnalysisResults, 
0xd8b2e731, 0x710a, 0x48ae, 0x99, 0x16, 0xac, 0x15, 0x2f, 0xef, 0x20, 0x31);
interface IAnalysisResults : IUnknown
{
   virtual Float64 GetResult() = 0;

   virtual Float64 GetMoment(const xbrPointOfInterest& poi) = 0;
};
