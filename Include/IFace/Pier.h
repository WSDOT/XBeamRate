#pragma once

#include <XBeamRateExt\PointOfInterest.h>

interface IShape;
interface IPoint2d;
interface IPoint2dCollection;

// {7F04A0B9-FD4E-4965-8F26-8BE78B063803}
DEFINE_GUID(IID_IXBRPier, 
0x7f04a0b9, 0xfd4e, 0x4965, 0x8f, 0x26, 0x8b, 0xe7, 0x8b, 0x6, 0x38, 0x3);
interface IXBRPier : public IUnknown
{
   virtual IndexType GetBearingLineCount(PierIDType pierID) = 0;
   virtual IndexType GetBearingCount(PierIDType pierID,IndexType brgLineIdx) = 0;
   virtual Float64 GetBearingLocation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx) = 0;

   virtual IndexType GetColumnCount(PierIDType pierID) = 0;
   virtual Float64 GetColumnLocation(PierIDType pierID,IndexType colIdx) = 0;
   virtual Float64 GetColumnHeight(PierIDType pierID,IndexType colIdx) = 0;
   virtual Float64 GetMaxColumnHeight(PierIDType pierID) = 0;
   virtual Float64 GetTopColumnElevation(PierIDType pierID,IndexType colIdx) = 0;
   virtual Float64 GetBottomColumnElevation(PierIDType pierID,IndexType colIdx) = 0;

   virtual void GetUpperXBeamPoints(PierIDType pierID,IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBC,IPoint2d** ppBR) = 0;
   virtual void GetLowerXBeamPoints(PierIDType pierID,IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBL2,IPoint2d** ppBR2,IPoint2d** ppBR) = 0;

   virtual void GetUpperXBeamProfile(PierIDType pierID,IShape** ppShape) = 0;
   virtual void GetLowerXBeamProfile(PierIDType pierID,IShape** ppShape) = 0;

   virtual Float64 GetElevation(PierIDType pierID,Float64 distFromLeftEdge) = 0;
};

// {7F260544-5BBC-4be3-87E7-5DF89A45F35D}
DEFINE_GUID(IID_IXBRSectionProperties, 
0x7f260544, 0x5bbc, 0x4be3, 0x87, 0xe7, 0x5d, 0xf8, 0x9a, 0x45, 0xf3, 0x5d);
interface IXBRSectionProperties : public IUnknown
{
   virtual Float64 GetDepth(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;
   virtual Float64 GetArea(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;
   virtual Float64 GetIxx(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;
   virtual Float64 GetIyy(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) = 0;

   virtual void GetUpperXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape) = 0;
   virtual void GetLowerXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape) = 0;
};

// {BE372349-0F8D-48e4-90F2-536AC90BEBBE}
DEFINE_GUID(IID_IXBRMaterial, 
0xbe372349, 0xf8d, 0x48e4, 0x90, 0xf2, 0x53, 0x6a, 0xc9, 0xb, 0xeb, 0xbe);
interface IXBRMaterial : public IUnknown
{
   virtual Float64 GetXBeamDensity(PierIDType pierID) = 0;
   virtual Float64 GetXBeamEc(PierIDType pierID) = 0;
   virtual Float64 GetColumnEc(PierIDType pierID,IndexType colIdx) = 0;
};

// {80B9F943-F0BF-4c4b-BCE9-70BBB3A55188}
DEFINE_GUID(IID_IXBRRebar, 
0x80b9f943, 0xf0bf, 0x4c4b, 0xbc, 0xe9, 0x70, 0xbb, 0xb3, 0xa5, 0x51, 0x88);
interface IXBRRebar : public IUnknown
{
   virtual IndexType GetRebarRowCount(PierIDType pierID) = 0;
   virtual IndexType GetRebarCount(PierIDType pierID,IndexType rowIdx) = 0;
   virtual void GetRebarProfile(PierIDType pierID,IndexType rowIdx,IPoint2dCollection** ppPoints) = 0;

   // Returns the depth to a rebar row, measured from the top of the cross beam
   virtual Float64 GetRebarRowLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx) = 0;
   virtual void GetRebarLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx,IndexType barIdx,IPoint2d** ppPoint) = 0;
};

// {025A63FF-9FE0-4733-8AB9-B1B6B96E0F7B}
DEFINE_GUID(IID_IXBRStirrups, 
0x25a63ff, 0x9fe0, 0x4733, 0x8a, 0xb9, 0xb1, 0xb6, 0xb9, 0x6e, 0xf, 0x7b);
interface IXBRStirrups : public IUnknown
{
   // Stage 1 = Lower cross beam
   // Stage 2 = Full depth cross beam

   virtual ZoneIndexType GetStirrupZoneCount(PierIDType pierID,xbrTypes::Stage stage) = 0;
   virtual void GetStirrupZoneBoundary(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx,Float64* pXstart,Float64* pXend) = 0;
   virtual Float64 GetStirrupZoneSpacing(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx) = 0;
   virtual Float64 GetStirrupZoneReinforcement(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx) = 0;
   virtual IndexType GetStirrupCount(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx) = 0;
};
