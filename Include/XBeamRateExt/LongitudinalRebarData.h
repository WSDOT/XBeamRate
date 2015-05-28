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
   Copyright © 1997-2007
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
      matRebar::Size BarSize;
      CollectionIndexType NumberOfBars;
      Float64     Cover;
      Float64     BarSpacing;

      RebarRow():
         Datum(xbrTypes::Bottom), BarSize(matRebar::bsNone), NumberOfBars(0), Cover(0), BarSpacing(0)
      {;}

      bool operator==(const RebarRow& other) const
      {
         if(Datum != other.Datum) return false;
         if(BarSize != other.BarSize) return false;
         if ( !IsEqual(Cover,  other.Cover) ) return false;
         if ( !IsEqual(BarSpacing,  other.BarSpacing) ) return false;
         if ( NumberOfBars != other.NumberOfBars ) return false;

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
