#pragma once

#include <XBeamRateExt\PointOfInterest.h>

// {7F260544-5BBC-4be3-87E7-5DF89A45F35D}
DEFINE_GUID(IID_IXBRPier, 
0x7f260544, 0x5bbc, 0x4be3, 0x87, 0xe7, 0x5d, 0xf8, 0x9a, 0x45, 0xf3, 0x5d);
interface IXBRPier : public IUnknown
{
   virtual Float64 GetArea(const xbrPointOfInterest& poi) = 0;
};

// {BE372349-0F8D-48e4-90F2-536AC90BEBBE}
DEFINE_GUID(IID_IXBRMaterial, 
0xbe372349, 0xf8d, 0x48e4, 0x90, 0xf2, 0x53, 0x6a, 0xc9, 0xb, 0xeb, 0xbe);
interface IXBRMaterial : public IUnknown
{
   virtual Float64 GetXBeamDensity() = 0;
};
