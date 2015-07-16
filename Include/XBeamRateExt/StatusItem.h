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
   virtual eafTypes::StatusSeverityType GetSeverity();
   virtual void Execute(CEAFStatusItem* pStatusItem);

private:
   eafTypes::StatusSeverityType m_Severity;
   UINT m_HelpID;
};
