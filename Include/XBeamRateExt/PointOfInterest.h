#pragma once

#include <XBeamRateExt\XBRExtExp.h>


typedef Uint32 PoiAttributeType; // NOTE: if more bits are needed use a 64-bit type

#define POI_SECTCHANGE   0x00000001
#define POI_COLUMN       0x00000002 // POI at in XBeam at CL of Column
#define POI_BRG          0x00000004 // POI in XBeam at CL Girder Bearing Point

class XBREXTCLASS xbrPointOfInterest
{
public:
   xbrPointOfInterest(PoiIDType id,ColumnIndexType colIdx,Float64 Xpoi,PoiAttributeType attribute=0);
   xbrPointOfInterest(PoiIDType id,Float64 Xpoi,PoiAttributeType attribute=0);

   bool operator==(const xbrPointOfInterest& other) const;
   bool operator<(const xbrPointOfInterest& other) const;

   PoiIDType GetID() const;

   bool IsColumnPOI() const;
   ColumnIndexType GetColumnIndex() const;
   void SetColumnIndex(ColumnIndexType colIdx);

   void SetDistFromStart(Float64 Xpoi);
   Float64 GetDistFromStart() const;

   void SetAttributes(PoiAttributeType attribute);
   PoiAttributeType GetAttributes() const;

   bool HasAttribute(PoiAttributeType attribute) const;

protected:
   PoiIDType m_ID;
   ColumnIndexType m_ColumnIndex;
   Float64 m_Xpoi;
   PoiAttributeType m_Attributes;
};
