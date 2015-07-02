#pragma once

#include <XBeamRateExt\PointOfInterest.h>

// {DC508193-9C4A-4047-9B5E-7EEFEC1E18B0}
DEFINE_GUID(IID_IXBRPointOfInterest, 
0xdc508193, 0x9c4a, 0x4047, 0x9b, 0x5e, 0x7e, 0xef, 0xec, 0x1e, 0x18, 0xb0);
interface IXBRPointOfInterest : public IUnknown
{
   virtual std::vector<xbrPointOfInterest> GetXBeamPointsOfInterest(PierIDType pierID,PoiAttributeType attrib = 0) = 0;
   virtual std::vector<xbrPointOfInterest> GetColumnPointsOfInterest(PierIDType pierID,ColumnIndexType colIdx) = 0;

   virtual Float64 ConvertPoiToPierCoordinate(PierIDType pierID,const xbrPointOfInterest& poi) = 0;
   virtual xbrPointOfInterest ConvertPierCoordinateToPoi(PierIDType pierID,Float64 Xp) = 0;
   virtual xbrPointOfInterest GetNearestPointOfInterest(PierIDType pierID,const xbrPointOfInterest& poi) = 0;
   virtual xbrPointOfInterest GetNextPointOfInterest(PierIDType pierID,PoiIDType poiID) = 0;
   virtual xbrPointOfInterest GetPrevPointOfInterest(PierIDType pierID,PoiIDType poiID) = 0;
};
