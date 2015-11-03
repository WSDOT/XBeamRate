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


#include "stdafx.h"
#include "txnEditLoadRatingOptions.h"

#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>

txnEditLoadRatingOptions::txnEditLoadRatingOptions(const txnLoadRatingOptions& oldOptions,const txnLoadRatingOptions& newOptions)
{
   m_Options[0] = oldOptions;
   m_Options[1] = newOptions;
}

txnEditLoadRatingOptions::~txnEditLoadRatingOptions(void)
{
}

bool txnEditLoadRatingOptions::Execute()
{
   Execute(1);
   return true;
}

void txnEditLoadRatingOptions::Undo()
{
   Execute(0);
}

void txnEditLoadRatingOptions::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRRatingSpecification,pSpec);
   pSpec->SetAnalysisMethodForReactions(m_Options[i].m_AnalysisType);
   pSpec->SetPermitRatingMethod(m_Options[i].m_PermitRatingMethod);

   GET_IFACE2(pBroker,IXBRProject,pProject);
   pProject->SetMaxLiveLoadStepSize(m_Options[i].m_MaxLLStepSize);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditLoadRatingOptions::CreateClone() const
{
   return new txnEditLoadRatingOptions(m_Options[0],m_Options[1]);
}

std::_tstring txnEditLoadRatingOptions::Name() const
{
   return _T("Edit Cross Beam Load Rating Options");
}

bool txnEditLoadRatingOptions::IsUndoable()
{
   return true;
}

bool txnEditLoadRatingOptions::IsRepeatable()
{
   return false;
}
