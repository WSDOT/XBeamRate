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
      Top,           // rebar cover is measured from top of upper cross beam
      TopLowerXBeam, // rebar cover is measured from top of lower cross beam
      Bottom,        // rebar cover is measured from bottom of cross beam
   } LongitudinalRebarDatumType;

   typedef enum LongitudinalRebarLayoutType
   {
      blLeftEnd,  // start location is measured from left end of cross beam
      blRightEnd,  // start location is measured from right end of cross beam
      blFullLength // bar is full length of cross beam (start and length parameters ignored)
   } LongitudinalRebarLayoutType;

   typedef enum ProductForceType
   {
      pftLowerXBeam,
      pftUpperXBeam,
      pftDCReactions,
      pftDWReactions,
      pftCRReactions,
      pftSHReactions,
      pftPSReactions,
      pftREReactions
   } ProductForceType;

   typedef enum CombinedForceType
   {
      lcDC,
      lcDW,
      lcCR,
      lcSH,
      lcRE,
      lcPS
   } CombinedForceType;

   typedef enum ReactionLoadType
   {
      rltConcentrated, // DC/DW reaction values are concentrated forces
      rltUniform // DC/DW are uniform load forces (FORCE/LENGTH) centered about the CL Bearing
   } ReactionLoadType;

} xbrTypes;