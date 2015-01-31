#pragma once

/*****************************************************************************
INTERFACE
   IAnalysisResults

DESCRIPTION
   Interface for getting analysis results
*****************************************************************************/
// {40AC2396-A43A-4ed4-8980-1B0485969CB2}
DEFINE_GUID(IID_IAnalysisResults, 
0x40ac2396, 0xa43a, 0x4ed4, 0x89, 0x80, 0x1b, 0x4, 0x85, 0x96, 0x9c, 0xb2);
interface IAnalysisResults : IUnknown
{
   virtual Float64 GetResult() = 0;
};
