///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2015  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

#pragma once

#include <WBFLCore.h>
#include <XBeamRateExt\PierData.h>

class txnDeadLoadReaction
{
public:
   txnDeadLoadReaction();
   Float64 m_DC;
   Float64 m_DW;
   Float64 m_CR;
   Float64 m_SH;
   Float64 m_PS;
   Float64 m_RE;
   Float64 m_W; // width of the reaction force if DC/DW are uniform loads
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
   Float64 m_gCR;
   Float64 m_gSH;
   Float64 m_gPS;
   Float64 m_gLL[6]; // use GET_INDEX with a limit state to access the array

   xbrTypes::ReactionLoadType m_DeadLoadReactionType[2];
   std::vector<txnDeadLoadReaction> m_DeadLoadReactions[2]; // use bearing line index to access array

   xbrTypes::ReactionLoadApplicationType m_LiveLoadReactionApplication;

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
