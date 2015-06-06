
#pragma once

#include <WBFLCore.h>

struct xbrEditReinforcementData
{
};

class txnEditReinforcement :
   public txnTransaction
{
public:
   txnEditReinforcement(const xbrEditReinforcementData& oldReinforcement,const xbrEditReinforcementData& newReinforcement);
   ~txnEditReinforcement(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

	xbrEditReinforcementData m_Reinforcement[2];
};
