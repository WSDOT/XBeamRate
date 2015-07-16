#include "stdafx.h"
#include <IFace\StatusCenter.h>
#include "StatusItem.h"

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

   if ( GetDescription() != other->GetDescription())
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
   EAFShowStatusMessage(pStatusItem,m_Severity,0);
}
