#pragma once

#include <PGSuperTypes.h>

typedef struct xbrTypes
{
   typedef enum SuperstructureConnectionType
   {
      pctContinuous, // superstructure is continuous but hinged with substructure
      pctIntegral,   // superstructure is fully continuous with substructure
      pctExpansion   // no moment connectivity at pier
   } SuperstructureConnectionType;

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

   typedef enum Stage
   {
      Stage1, // lower cross beam only
      Stage2  // upper and lower cross beam together
   } Stage;

   typedef enum LongitudinalRebarDatumType
   {
      Top,
      TopLowerXBeam,
      Bottom,
   } LongitudinalRebarDatumType;

} xbrTypes;