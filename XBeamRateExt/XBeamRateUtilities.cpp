#include "stdafx.h"
#include <XBeamRateExt\XBeamRateUtilities.h>

xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType(pgsTypes::BoundaryConditionType bcType)
{
   switch( bcType )
   {
   case pgsTypes::bctHinge:
   case pgsTypes::bctRoller:
      return xbrTypes::pctExpansion;

   case pgsTypes::bctContinuousAfterDeck:
   case pgsTypes::bctContinuousBeforeDeck:
      return xbrTypes::pctContinuous;

   case pgsTypes::bctIntegralAfterDeck:
   case pgsTypes::bctIntegralBeforeDeck:
      return xbrTypes::pctIntegral;

   case pgsTypes::bctIntegralAfterDeckHingeBack:
   case pgsTypes::bctIntegralBeforeDeckHingeBack:
   case pgsTypes::bctIntegralAfterDeckHingeAhead:
   case pgsTypes::bctIntegralBeforeDeckHingeAhead:
      return xbrTypes::pctIntegral;
   }

   ATLASSERT(false); // should never get here
   return xbrTypes::pctIntegral;
}

xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType(pgsTypes::PierSegmentConnectionType connType)
{
   switch ( connType )
   {
   case pgsTypes::psctContinousClosureJoint:
   case pgsTypes::psctContinuousSegment:
      return xbrTypes::pctContinuous;

   case pgsTypes::psctIntegralClosureJoint:
   case pgsTypes::psctIntegralSegment:
      return xbrTypes::pctIntegral;
   }

   ATLASSERT(false); // should never get here
   return xbrTypes::pctIntegral;
}
