
#pragma once

#include <WBFLCore.h>
#include <XBeamRateExt\PierData.h>

class txnDeadLoadReaction
{
public:
   txnDeadLoadReaction();
   Float64 m_DC;
   Float64 m_DW;
};

class txnLiveLoadReactions
{
public:
   txnLiveLoadReactions();
   std::vector<std::pair<std::_tstring,Float64>> m_LLIM;
};

class txnEditPierData
{
public:
   txnEditPierData();

   xbrPierData m_PierData;

   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gLL[6]; // use pgsTypes::LoadRatingType to access array

   std::vector<txnDeadLoadReaction> m_DeadLoadReactions[2]; // use bearing line index to access array

   txnLiveLoadReactions m_DesignLiveLoad;
   txnLiveLoadReactions m_LegalRoutineLiveLoad;
   txnLiveLoadReactions m_LegalSpecialLiveLoad;
   txnLiveLoadReactions m_PermitRoutineLiveLoad;
   txnLiveLoadReactions m_PermitSpecialLiveLoad;
};

class txnEditPier :
   public txnTransaction
{
public:
   txnEditPier(const txnEditPierData& oldPierData,const txnEditPierData& newPierData);
   ~txnEditPier(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

   txnEditPierData m_PierData[2];

};
