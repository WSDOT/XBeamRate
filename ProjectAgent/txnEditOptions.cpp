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
#include "txnEditOptions.h"

#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>

txnEditOptions::txnEditOptions(const txnEditOptionsData& oldOptions,const txnEditOptionsData& newOptions)
{
   m_Options[0] = oldOptions;
   m_Options[1] = newOptions;
}

txnEditOptions::~txnEditOptions(void)
{
}

bool txnEditOptions::Execute()
{
   Execute(1);
   return true;
}

void txnEditOptions::Undo()
{
   Execute(0);
}

void txnEditOptions::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRRatingSpecification,pRatingSpec);

   pRatingSpec->SetPermitRatingMethod(m_Options[i].m_PermitRatingMethod);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditOptions::CreateClone() const
{
   return new txnEditOptions(m_Options[0],m_Options[1]);
}

std::_tstring txnEditOptions::Name() const
{
   return _T("Edit Options");
}

bool txnEditOptions::IsUndoable()
{
   return true;
}

bool txnEditOptions::IsRepeatable()
{
   return false;
}
