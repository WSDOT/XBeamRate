
#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <Material\Rebar.h>
#include <Units\Units.h>


/*****************************************************************************
CLASS 
   xbrStirrupData

   Utility class for shear zone description data.
*****************************************************************************/

class XBREXTCLASS xbrStirrupData
{
public:
   class XBREXTCLASS StirrupZone
   {
   public:
      Float64 Length;
      Float64 BarSpacing;
      matRebar::Size BarSize;
      Float64 nBars;

      StirrupZone():
      Length(0), BarSpacing(::ConvertToSysUnits(6.0,unitMeasure::Inch)), BarSize(matRebar::bs6), nBars(4)
      {;}

      bool operator==(const StirrupZone& other) const
      {
         if (BarSize != other.BarSize) return false;
         if ( !IsEqual(Length,  other.Length) ) return false;
         if ( !IsEqual(BarSpacing,  other.BarSpacing) ) return false;
         if ( !IsEqual(nBars,other.nBars) ) return false;

         return true;
      };
   };

   bool Symmetric;
   std::vector<StirrupZone> Zones;

   // GROUP: LIFECYCLE

   //------------------------------------------------------------------------
   // Default constructor
   xbrStirrupData();

   //------------------------------------------------------------------------
   // Copy constructor
   xbrStirrupData(const xbrStirrupData& rOther);

   //------------------------------------------------------------------------
   // Destructor
   ~xbrStirrupData();

   // GROUP: OPERATORS
   //------------------------------------------------------------------------
   // Assignment operator
   xbrStirrupData& operator = (const xbrStirrupData& rOther);
   bool operator == (const xbrStirrupData& rOther) const;
   bool operator != (const xbrStirrupData& rOther) const;

   // GROUP: OPERATIONS

	HRESULT Load(IStructuredLoad* pStrLoad);
	HRESULT Save(IStructuredSave* pStrSave);

   // GROUP: ACCESS
   // GROUP: INQUIRY

protected:
   // GROUP: DATA MEMBERS
   // GROUP: LIFECYCLE
   // GROUP: OPERATORS
   // GROUP: OPERATIONS
   //------------------------------------------------------------------------
   void MakeCopy(const xbrStirrupData& rOther);

   //------------------------------------------------------------------------
   virtual void MakeAssignment(const xbrStirrupData& rOther);

   // GROUP: ACCESS
   // GROUP: INQUIRY

private:
   // GROUP: DATA MEMBERS
   // GROUP: LIFECYCLE
   // GROUP: OPERATORS
   // GROUP: OPERATIONS
   // GROUP: ACCESS
   // GROUP: INQUIRY
};

// INLINE METHODS
//

// EXTERNAL REFERENCES
//
//class PSGLIBCLASS HorizontalInterfaceZoneDataLess
//{
//public:
//   bool operator()(const CHorizontalInterfaceZoneData& a, const CHorizontalInterfaceZoneData& b)
//   {
//      return a.ZoneNum < b.ZoneNum;
//   }
//};

