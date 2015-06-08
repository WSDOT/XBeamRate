

#include "stdafx.h"
#include "txnEditReinforcement.h"

#include <IFace\Project.h>

txnEditReinforcement::txnEditReinforcement(PierIDType pierID,const xbrEditReinforcementData& oldReinforcement,const xbrEditReinforcementData& newReinforcement)
{
   ATLASSERT(pierID != INVALID_ID);
   m_PierID = pierID;
   m_Reinforcement[0] = oldReinforcement;
   m_Reinforcement[1] = newReinforcement;
}

txnEditReinforcement::~txnEditReinforcement(void)
{
}

bool txnEditReinforcement::Execute()
{
   Execute(1);
   return true;
}

void txnEditReinforcement::Undo()
{
   Execute(0);
}

void txnEditReinforcement::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRProject,pProject);
   pProject->SetRebarMaterial(m_PierID,m_Reinforcement[i].Type,m_Reinforcement[i].Grade);
   pProject->SetLongitudinalRebar(m_PierID,m_Reinforcement[i].LongitudinalRebar);
   pProject->SetLowerXBeamStirrups(m_PierID,m_Reinforcement[i].LowerXBeamStirrups);
   pProject->SetFullDepthStirrups(m_PierID,m_Reinforcement[i].FullDepthStirrups);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditReinforcement::CreateClone() const
{
   return new txnEditReinforcement(m_PierID,m_Reinforcement[0],m_Reinforcement[1]);
}

std::_tstring txnEditReinforcement::Name() const
{
   return _T("Edit Cross Beam Reinforcement");
}

bool txnEditReinforcement::IsUndoable()
{
   return true;
}

bool txnEditReinforcement::IsRepeatable()
{
   return false;
}
