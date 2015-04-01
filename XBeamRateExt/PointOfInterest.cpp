#include "stdafx.h"
#include <XBeamRateExt\PointOfInterest.h>

#include <System\Flags.h>

xbrPointOfInterest::xbrPointOfInterest(PoiIDType id,ColumnIndexType colIdx,Float64 Xpoi,PoiAttributeType attribute) :
m_ID(id),m_ColumnIndex(colIdx),m_Xpoi(Xpoi),m_Attributes(attribute)
{
}

xbrPointOfInterest::xbrPointOfInterest(PoiIDType id,Float64 Xpoi,PoiAttributeType attribute) :
m_ID(id),m_ColumnIndex(INVALID_INDEX),m_Xpoi(Xpoi),m_Attributes(attribute)
{
}

bool xbrPointOfInterest::operator==(const xbrPointOfInterest& other) const
{
   if ( m_ColumnIndex == other.m_ColumnIndex && IsEqual(m_Xpoi,other.m_Xpoi) && m_Attributes == other.GetAttributes() )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool xbrPointOfInterest::operator<(const xbrPointOfInterest& other) const
{
   if ( m_ColumnIndex == other.m_ColumnIndex )
   {
      return m_Xpoi < other.m_Xpoi;
   }

   return m_ColumnIndex < other.m_ColumnIndex;
}

PoiIDType xbrPointOfInterest::GetID() const
{
   return m_ID;
}

bool xbrPointOfInterest::IsColumnPOI() const
{
   return m_ColumnIndex == INVALID_INDEX ? false : true;
}

ColumnIndexType xbrPointOfInterest::GetColumnIndex() const
{
   return m_ColumnIndex;
}

void xbrPointOfInterest::SetColumnIndex(ColumnIndexType colIdx)
{
   m_ColumnIndex = colIdx;
}

void xbrPointOfInterest::SetDistFromStart(Float64 Xpoi)
{
   m_Xpoi = Xpoi;
}

Float64 xbrPointOfInterest::GetDistFromStart() const
{
   return m_Xpoi;
}

void xbrPointOfInterest::SetAttributes(PoiAttributeType attribute)
{
   m_Attributes = attribute;
}

PoiAttributeType xbrPointOfInterest::GetAttributes() const
{
   return m_Attributes;
}

bool xbrPointOfInterest::HasAttribute(PoiAttributeType attribute) const
{
   return sysFlags<PoiAttributeType>::IsSet(m_Attributes,attribute) ? true : false;
}
