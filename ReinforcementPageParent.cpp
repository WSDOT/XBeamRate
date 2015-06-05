#include "stdafx.h"
#include "ReinforcementPageParent.h"

CReinforcementPageParent::CReinforcementPageParent()
{
   m_pEditPierData = NULL;
}

void CReinforcementPageParent::SetEditPierData(IEditPierData* pEditPierData)
{
   m_pEditPierData = pEditPierData;

   // Initialize the cross beam pier data from the PGSuper/PGSplice pier data

   if ( m_pEditPierData->GetPierData()->IsBoundaryPier() )
   {
      switch( m_pEditPierData->GetPierData()->GetBoundaryConditionType() )
      {
      case pgsTypes::bctHinge:
      case pgsTypes::bctRoller:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctExpansion);
         break;

      case pgsTypes::bctContinuousAfterDeck:
      case pgsTypes::bctContinuousBeforeDeck:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctContinuous);
         break;

      case pgsTypes::bctIntegralAfterDeck:
      case pgsTypes::bctIntegralBeforeDeck:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctIntegral);
         break;

      case pgsTypes::bctIntegralAfterDeckHingeBack:
      case pgsTypes::bctIntegralBeforeDeckHingeBack:
      case pgsTypes::bctIntegralAfterDeckHingeAhead:
      case pgsTypes::bctIntegralBeforeDeckHingeAhead:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctIntegral);
         break;
      }
   }
   else
   {
      switch ( m_pEditPierData->GetPierData()->GetSegmentConnectionType() )
      {
      case pgsTypes::psctContinousClosureJoint:
      case pgsTypes::psctContinuousSegment:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctContinuous);
         break;

      case pgsTypes::psctIntegralClosureJoint:
      case pgsTypes::psctIntegralSegment:
         m_PierData.SetSuperstructureConnectionType(xbrTypes::pctIntegral);
         break;
      }
   }
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
   return m_PierData.GetSuperstructureConnectionType();
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
