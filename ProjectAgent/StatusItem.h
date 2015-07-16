#pragma once

class xbrBridgeStatusItem : public CEAFStatusItem
{
public:
   xbrBridgeStatusItem(StatusGroupIDType statusGroupID,StatusCallbackIDType callbackID,LPCTSTR strDescription);
   bool IsEqual(CEAFStatusItem* pOther);
};

class xbrBridgeStatusCallback : public iStatusCallback
{
public:
   xbrBridgeStatusCallback(eafTypes::StatusSeverityType severity,UINT helpID=0);
   virtual eafTypes::StatusSeverityType GetSeverity();
   virtual void Execute(CEAFStatusItem* pStatusItem);

private:
   eafTypes::StatusSeverityType m_Severity;
   UINT m_HelpID;
};
