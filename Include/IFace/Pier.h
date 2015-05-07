#pragma once

#include <XBeamRateExt\PointOfInterest.h>

interface IShape;

// {7F04A0B9-FD4E-4965-8F26-8BE78B063803}
DEFINE_GUID(IID_IXBRPier, 
0x7f04a0b9, 0xfd4e, 0x4965, 0x8f, 0x26, 0x8b, 0xe7, 0x8b, 0x6, 0x38, 0x3);
interface IXBRPier : public IUnknown
{
   virtual IndexType GetBearingLineCount() = 0;
   virtual IndexType GetBearingCount(IndexType brgLineIdx) = 0;
   virtual Float64 GetBearingLocation(IndexType brgLineIdx,IndexType brgIdx) = 0;

   virtual IndexType GetColumnCount() = 0;
   virtual Float64 GetColumnLocation(IndexType colIdx) = 0;
   virtual Float64 GetColumnHeight(IndexType colIdx) = 0;
   virtual Float64 GetMaxColumnHeight() = 0;

   virtual void GetUpperXBeamProfile(IShape** ppShape) = 0;
   virtual void GetLowerXBeamProfile(IShape** ppShape) = 0;
};

// {7F260544-5BBC-4be3-87E7-5DF89A45F35D}
DEFINE_GUID(IID_IXBRSectionProperties, 
0x7f260544, 0x5bbc, 0x4be3, 0x87, 0xe7, 0x5d, 0xf8, 0x9a, 0x45, 0xf3, 0x5d);
interface IXBRSectionProperties : public IUnknown
{
   virtual Float64 GetDepth(xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;
   virtual Float64 GetArea(xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;
};

// {BE372349-0F8D-48e4-90F2-536AC90BEBBE}
DEFINE_GUID(IID_IXBRMaterial, 
0xbe372349, 0xf8d, 0x48e4, 0x90, 0xf2, 0x53, 0x6a, 0xc9, 0xb, 0xeb, 0xbe);
interface IXBRMaterial : public IUnknown
{
   virtual Float64 GetXBeamDensity() = 0;
};
