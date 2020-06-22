///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2020  Washington State Department of Transportation
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

#include <XBeamRateExt\XBRExtExp.h>
#include <EAF\EAFStatusItem.h>

class XBREXTCLASS xbrBridgeStatusItem : public CEAFStatusItem
{
public:
   xbrBridgeStatusItem(StatusGroupIDType statusGroupID,StatusCallbackIDType callbackID,LPCTSTR strDescription);
   bool IsEqual(CEAFStatusItem* pOther);
};

class XBREXTCLASS xbrBridgeStatusCallback : public iStatusCallback
{
public:
   xbrBridgeStatusCallback(eafTypes::StatusSeverityType severity,UINT helpID=0);
   virtual eafTypes::StatusSeverityType GetSeverity() const override;
   virtual void Execute(CEAFStatusItem* pStatusItem) override;

private:
   eafTypes::StatusSeverityType m_Severity;
   UINT m_HelpID;
};
