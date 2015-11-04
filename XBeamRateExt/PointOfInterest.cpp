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

#include "stdafx.h"
#include <XBeamRateExt\PointOfInterest.h>

#include <System\Flags.h>

#if defined _DEBUG
#define UPDATE_ATTRIBUTES UpdateAttributeString()
#else
#define UPDATE_ATTRIBUTES
#endif


xbrPointOfInterest::xbrPointOfInterest(PoiIDType id,ColumnIndexType colIdx,Float64 Xpoi,PoiAttributeType attribute) :
m_ID(id),m_ColumnIndex(colIdx),m_Xpoi(Xpoi),m_Attributes(attribute)
{
   UPDATE_ATTRIBUTES;
}

xbrPointOfInterest::xbrPointOfInterest(PoiIDType id,Float64 Xpoi,PoiAttributeType attribute) :
m_ID(id),m_ColumnIndex(INVALID_INDEX),m_Xpoi(Xpoi),m_Attributes(attribute)
{
   UPDATE_ATTRIBUTES;
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
   UPDATE_ATTRIBUTES;
}

PoiAttributeType xbrPointOfInterest::GetAttributes() const
{
   return m_Attributes;
}

bool xbrPointOfInterest::HasAttribute(PoiAttributeType attribute) const
{
   return sysFlags<PoiAttributeType>::IsSet(m_Attributes,attribute) ? true : false;
}

void xbrPointOfInterest::ClearAttributes()
{
   m_Attributes = 0;
   UPDATE_ATTRIBUTES;
}

#if defined _DEBUG
void xbrPointOfInterest::UpdateAttributeString()
{
   m_strAttributes.clear();
   std::_tostringstream os;

   if ( sysFlags<PoiAttributeType>::IsSet(m_Attributes,POI_SECTIONCHANGE) )
   {
      os << _T("POI_SECTIONCHANGE | ");
   }

   if ( sysFlags<PoiAttributeType>::IsSet(m_Attributes,POI_COLUMN) )
   {
      os << _T("POI_COLUMN | ");
   }

   if ( sysFlags<PoiAttributeType>::IsSet(m_Attributes,POI_BRG) )
   {
      os << _T("POI_BRG | ");
   }

   if ( sysFlags<PoiAttributeType>::IsSet(m_Attributes,POI_WHEELLINE) )
   {
      os << _T("POI_WHEELLINE | ");
   }

   if ( sysFlags<PoiAttributeType>::IsSet(m_Attributes,POI_GRID) )
   {
      os << _T("POI_GRID | ");
   }

   os << std::endl;

   m_strAttributes = os.str();
}
#endif // _DEBUG

