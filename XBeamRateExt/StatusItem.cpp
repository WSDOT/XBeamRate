///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2019  Washington State Department of Transportation
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
#include <IFace\StatusCenter.h>
#include <XBeamRateExt\StatusItem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


xbrBridgeStatusItem::xbrBridgeStatusItem(StatusGroupIDType statusGroupID,StatusCallbackIDType callbackID,LPCTSTR strDescription) :
CEAFStatusItem(statusGroupID,callbackID,strDescription)
{
}

bool xbrBridgeStatusItem::IsEqual(CEAFStatusItem* pOther)
{
   xbrBridgeStatusItem* other = dynamic_cast<xbrBridgeStatusItem*>(pOther);
   if ( !other )
   {
      return false;
   }

   if ( CString(GetDescription()) != CString(other->GetDescription()) )
   {
      return false;
   }

   return true;
}

//////////////////////////////
xbrBridgeStatusCallback::xbrBridgeStatusCallback(eafTypes::StatusSeverityType severity,UINT helpID):
m_Severity(severity), m_HelpID(helpID)
{
}

eafTypes::StatusSeverityType xbrBridgeStatusCallback::GetSeverity()
{
   return m_Severity;
}

void xbrBridgeStatusCallback::Execute(CEAFStatusItem* pStatusItem)
{
   EAFShowStatusMessage(pStatusItem,m_Severity,TRUE,TRUE);
}
