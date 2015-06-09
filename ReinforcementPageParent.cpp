#include "stdafx.h"
#include "ReinforcementPageParent.h"
#include <XBeamRateExt\XBeamRateUtilities.h>

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
      return ::GetSuperstructureConnectionType(m_pEditPierData->GetPierData()->GetBoundaryConditionType());
   }
   else
   {
      return ::GetSuperstructureConnectionType(m_pEditPierData->GetPierData()->GetSegmentConnectionType());
   }
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
