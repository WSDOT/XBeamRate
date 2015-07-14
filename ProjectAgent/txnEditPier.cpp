

#include "stdafx.h"
#include "txnEditPier.h"

#include <IFace\Project.h>

txnDeadLoadReaction::txnDeadLoadReaction()
{
   m_DC = 0;
   m_DW = 0;
   m_CR = 0;
   m_SH = 0;
   m_PS = 0;
   m_RE = 0;
   m_W  = 0;
}

txnLiveLoadReactions::txnLiveLoadReactions()
{
}

/////////////////////////////////////////////////////////////////
txnEditPierData::txnEditPierData()
{
}

/////////////////////////////////////////////////////////////////
txnEditPier::txnEditPier(const txnEditPierData& oldPierData,const txnEditPierData& newPierData)
{
   m_PierData[0] = oldPierData;
   m_PierData[1] = newPierData;
}

txnEditPier::~txnEditPier(void)
{
}

bool txnEditPier::Execute()
{
   Execute(1);
   return true;
}

void txnEditPier::Undo()
{
   Execute(0);
}

void txnEditPier::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   PierIDType pierID = INVALID_ID;

   GET_IFACE2(pBroker,IXBRProject,pProject);
   ATLASSERT(m_PierData[i].m_PierData.GetID() == pierID);
   pProject->SetPierData(m_PierData[i].m_PierData);

   IndexType nBearingLines = m_PierData[i].m_PierData.GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++)
   {
      xbrTypes::ReactionLoadType brgReactionType = m_PierData[i].m_DeadLoadReactionType[brgLineIdx];
      pProject->SetBearingReactionType(pierID,brgLineIdx,brgReactionType);
      IndexType nBearings = m_PierData[i].m_PierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 DC = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_DC;
         Float64 DW = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_DW;
         Float64 CR = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_CR;
         Float64 SH = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_SH;
         Float64 PS = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_PS;
         Float64 RE = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_RE;
         Float64 W  = m_PierData[i].m_DeadLoadReactions[brgLineIdx][brgIdx].m_W;
         pProject->SetBearingReactions(pierID,brgLineIdx,brgIdx,DC,DW,CR,SH,PS,RE,W);
      }
   }

   pProject->SetDCLoadFactor(m_PierData[i].m_gDC);
   pProject->SetDWLoadFactor(m_PierData[i].m_gDW);
   pProject->SetCRLoadFactor(m_PierData[i].m_gCR);
   pProject->SetSHLoadFactor(m_PierData[i].m_gSH);
   pProject->SetPSLoadFactor(m_PierData[i].m_gPS);
   pProject->SetRELoadFactor(m_PierData[i].m_gCR); // using creep factor for relaxation

   for ( int j = 0; j < 6; j++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)j;
      pProject->SetLiveLoadFactor(pierID,ratingType,m_PierData[i].m_gLL[ratingType]);
   }

   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrDesign_Inventory,m_PierData[i].m_DesignLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrDesign_Operating,m_PierData[i].m_DesignLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrLegal_Routine,m_PierData[i].m_LegalRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrLegal_Special,m_PierData[i].m_LegalSpecialLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrPermit_Routine,m_PierData[i].m_PermitRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pierID,pgsTypes::lrPermit_Special,m_PierData[i].m_PermitSpecialLiveLoad.m_LLIM);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditPier::CreateClone() const
{
   return new txnEditPier(m_PierData[0],m_PierData[1]);
}

std::_tstring txnEditPier::Name() const
{
   return _T("Edit Pier");
}

bool txnEditPier::IsUndoable()
{
   return true;
}

bool txnEditPier::IsRepeatable()
{
   return false;
}
