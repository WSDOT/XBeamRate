///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
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

#include "stdafx.h"
#include "txnEditPier.h"

#include <IFace\Project.h>

txnDeadLoadReaction::txnDeadLoadReaction()
{
   m_DC = 0;
   m_DW = 0;
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

   GET_IFACE2(pBroker,IXBRProject,pProject);
   pProject->SetPierData(m_PierData[i].m_PierData);

   //WORKING HERE... NEED TO SET DC/DW REACTIONS
   //   NEED TO DEAL WITH CASE OF # OF REACTIONS INCREASING/DESCREASING

   pProject->SetDCLoadFactor(m_PierData[i].m_gDC);
   pProject->SetDWLoadFactor(m_PierData[i].m_gDW);
   for ( int j = 0; j < 6; j++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)j;
      pProject->SetLiveLoadFactor(ratingType,m_PierData[i].m_gLL[ratingType]);
   }

   pProject->SetLiveLoadReactions(pgsTypes::lrDesign_Inventory,m_PierData[i].m_DesignLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lrDesign_Operating,m_PierData[i].m_DesignLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lrLegal_Routine,m_PierData[i].m_LegalRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lrLegal_Special,m_PierData[i].m_LegalSpecialLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lrPermit_Routine,m_PierData[i].m_PermitRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lrPermit_Special,m_PierData[i].m_PermitSpecialLiveLoad.m_LLIM);

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
