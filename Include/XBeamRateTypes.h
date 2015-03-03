#pragma once

#include <PGSuperTypes.h>

typedef struct xbrTypes
{
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