
#pragma once

#include <WBFLCore.h>

#include <XBeamRateExt\LongitudinalRebarData.h>
#include <XBeamRateExt\StirrupData.h>

struct xbrEditReinforcementData
{
   matRebar::Type Type;
   matRebar::Grade Grade;
   xbrLongitudinalRebarData LongitudinalRebar;
   xbrStirrupData LowerXBeamStirrups;
   xbrStirrupData FullDepthStirrups;
};

class txnEditReinforcement :
   public txnTransaction
{
public:
   txnEditReinforcement(PierIDType pierID,const xbrEditReinforcementData& oldReinforcement,const xbrEditReinforcementData& newReinforcement);
   ~txnEditReinforcement(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

   PierIDType m_PierID;
	xbrEditReinforcementData m_Reinforcement[2];
};
