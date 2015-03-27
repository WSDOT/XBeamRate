#pragma once

#include <PGSuperTypes.h>

typedef struct xbrTypes
{
   typedef enum PierConnectionType
   {
      pctContinuous, // superstructure is continuous but hinged with substructure
      pctIntegral,   // superstructure is fully continuous with substructure
      pctExpansion   // no moment connectivity at pier
   } PierConnectionType;

   typedef enum ColumnBaseType
   {
      cbtFixed,
      cbtPinned
   } ColumnBaseType;

   typedef enum TransverseDimensionMeasurementType
   {
      tdmNormalToAlignment, // transverse dimensions are normal to the alignment
      tdmPlaneOfPier        // transverse dimensions are in the plane of the pier
   } TransverseDimensionMeasurementType;

} xbrTypes;