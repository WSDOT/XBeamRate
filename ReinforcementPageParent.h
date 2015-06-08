#pragma once
#include <IFace\ExtendUI.h>
#include <XBeamRateExt\ReinforcementPage.h>
#include <XBeamRateExt\PierData.h>

// This class is a wrapper between the reinforcement propery page
// and the pier data
class CReinforcementPageParent : public IReinforcementPageParent
{
public:
   CReinforcementPageParent();
   void SetEditPierData(IEditPierData* pEditPierData,const xbrPierData& pierData);

// IReinforcementPageParent
   virtual CPierData2* GetPierData();
   virtual CConcreteMaterial& GetConcrete();
   virtual xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType();
   virtual matRebar::Type& GetRebarType();
   virtual matRebar::Grade& GetRebarGrade();
   virtual CConcreteMaterial& GetConcreteMaterial();
   virtual xbrLongitudinalRebarData& GetLongitudinalRebar();
   virtual xbrStirrupData& GetLowerXBeamStirrups();
   virtual xbrStirrupData& GetFullDepthStirrups();

private:
   IEditPierData* m_pEditPierData;
   xbrPierData m_PierData;
};