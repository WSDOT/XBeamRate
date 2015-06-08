#include "stdafx.h"
#include "ReinforcementPageParent.h"

CReinforcementPageParent::CReinforcementPageParent()
{
   m_pEditPierData = NULL;
}

void CReinforcementPageParent::SetEditPierData(IEditPierData* pEditPierData,const xbrPierData& pierData)
{
   m_pEditPierData = pEditPierData;
   m_PierData = pierData;
}

CPierData2* CReinforcementPageParent::GetPierData()
{
   return m_pEditPierData->GetPierData();
}

CConcreteMaterial& CReinforcementPageParent::GetConcrete()
{
   return m_pEditPierData->GetPierData()->GetConcrete();
}

xbrTypes::SuperstructureConnectionType CReinforcementPageParent::GetSuperstructureConnectionType()
{
   if ( m_pEditPierData->GetPierData()->IsBoundaryPier() )
   {
      switch( m_pEditPierData->GetPierData()->GetBoundaryConditionType() )
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
   }
   else
   {
      switch ( m_pEditPierData->GetPierData()->GetSegmentConnectionType() )
      {
      case pgsTypes::psctContinousClosureJoint:
      case pgsTypes::psctContinuousSegment:
         return xbrTypes::pctContinuous;

      case pgsTypes::psctIntegralClosureJoint:
      case pgsTypes::psctIntegralSegment:
         return xbrTypes::pctIntegral;
      }
   }

   ATLASSERT(false); // should never get here
   return xbrTypes::pctIntegral;
}

matRebar::Type& CReinforcementPageParent::GetRebarType()
{
   return m_PierData.GetRebarType();
}

matRebar::Grade& CReinforcementPageParent::GetRebarGrade()
{
   return m_PierData.GetRebarGrade();
}

CConcreteMaterial& CReinforcementPageParent::GetConcreteMaterial()
{
   return m_PierData.GetConcreteMaterial();
}

xbrLongitudinalRebarData& CReinforcementPageParent::GetLongitudinalRebar()
{
   return m_PierData.GetLongitudinalRebar();
}

xbrStirrupData& CReinforcementPageParent::GetLowerXBeamStirrups()
{
   return m_PierData.GetLowerXBeamStirrups();
}

xbrStirrupData& CReinforcementPageParent::GetFullDepthStirrups()
{
   return m_PierData.GetFullDepthStirrups();
}
