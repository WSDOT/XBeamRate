///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
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

#include <XBeamRateExt\XBRExtExp.h>

/*****************************************************************************
CLASS 
   CLongitudinalRebarData

   Utility class for longitudinal rebar description data.

DESCRIPTION
   Utility class for longitudinal rebar. This class encapsulates 
   the input data a row of rebar and implements the IStructuredLoad 
   and IStructuredSave persistence interfaces.

COPYRIGHT
   Copyright � 1997-2007
   Washington State Department Of Transportation
   All Rights Reserved

LOG
   rab : 02.08.2007 : Created file
*****************************************************************************/

class XBREXTCLASS xbrLongitudinalRebarData
{
public:
   class XBREXTCLASS RebarRow 
   {
   public:
      xbrTypes::LongitudinalRebarDatumType Datum;
      xbrTypes::LongitudinalRebarLayoutType LayoutType;
      Float64 Start;
      Float64 Length;
      matRebar::Size BarSize;
      CollectionIndexType NumberOfBars;
      Float64     Cover;
      Float64     BarSpacing;
      bool bHookStart;
      bool bHookEnd;

      RebarRow():
         Datum(xbrTypes::Bottom), 
         LayoutType(xbrTypes::blLeftEnd), 
         Start(0), 
         Length(0), 
         BarSize(matRebar::bs3), 
         NumberOfBars(0), 
         Cover(0), 
         BarSpacing(0),
         bHookStart(false),
         bHookEnd(false)
      {;}

      bool operator==(const RebarRow& rOther) const
      {
         if(Datum != rOther.Datum) return false;
         if(LayoutType != rOther.LayoutType) return false;
         if ( !IsEqual(Start,rOther.Start) ) return false;
         if ( !IsEqual(Length,rOther.Length) ) return false;
         if(BarSize != rOther.BarSize) return false;
         if ( !IsEqual(Cover,  rOther.Cover) ) return false;
         if ( !IsEqual(BarSpacing,  rOther.BarSpacing) ) return false;
         if ( NumberOfBars != rOther.NumberOfBars ) return false;
         if ( bHookStart != rOther.bHookStart) return false;
         if ( bHookEnd   != rOther.bHookEnd) return false;

         return true;
      };
   };

   std::vector<RebarRow> RebarRows;

   xbrLongitudinalRebarData();
   xbrLongitudinalRebarData(const xbrLongitudinalRebarData& rOther);
   virtual ~xbrLongitudinalRebarData();

   xbrLongitudinalRebarData& operator = (const xbrLongitudinalRebarData& rOther);
   bool operator == (const xbrLongitudinalRebarData& rOther) const;
   bool operator != (const xbrLongitudinalRebarData& rOther) const;

	HRESULT Load(IStructuredLoad* pStrLoad,IProgress* pProgress);
	HRESULT Save(IStructuredSave* pStrSave,IProgress* pProgress);

protected:
   void MakeCopy(const xbrLongitudinalRebarData& rOther);
   virtual void MakeAssignment(const xbrLongitudinalRebarData& rOther);
};
