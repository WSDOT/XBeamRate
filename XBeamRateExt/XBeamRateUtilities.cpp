#include "stdafx.h"
#include <XBeamRateExt\XBeamRateUtilities.h>
#include <IFace\XBeamRateAgent.h>
#include <EAF\EAFStatusCenter.h>

#include <XBeamRateExt\StatusItem.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <PgsExt\BridgeDescription2.h>
#include <PgsExt\GirderLabel.h>

#include <MFCTools\Exceptions.h>

xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType(pgsTypes::BoundaryConditionType bcType)
{
   switch( bcType )
   {
   case pgsTypes::bctHinge:
   case pgsTypes::bctRoller:
      return xbrTypes::pctExpansion;

   case pgsTypes::bctContinuousAfterDeck:
   case pgsTypes::bctContinuousBeforeDeck:
      return xbrTypes::pctContinuous;

   case pgsTypes::bctIntegralAfterDeck:
   case pgsTypes::bctIntegralBeforeDeck:
      return xbrTypes::pctIntegral;

   case pgsTypes::bctIntegralAfterDeckHingeBack:
   case pgsTypes::bctIntegralBeforeDeckHingeBack:
   case pgsTypes::bctIntegralAfterDeckHingeAhead:
   case pgsTypes::bctIntegralBeforeDeckHingeAhead:
      return xbrTypes::pctIntegral;
   }

   ATLASSERT(false); // should never get here
   return xbrTypes::pctIntegral;
}

xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType(pgsTypes::PierSegmentConnectionType connType)
{
   switch ( connType )
   {
   case pgsTypes::psctContinousClosureJoint:
   case pgsTypes::psctContinuousSegment:
      return xbrTypes::pctContinuous;

   case pgsTypes::psctIntegralClosureJoint:
   case pgsTypes::psctIntegralSegment:
      return xbrTypes::pctIntegral;
   }

   ATLASSERT(false); // should never get here
   return xbrTypes::pctIntegral;
}

bool IsStandAlone()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   CComPtr<IXBeamRateAgent> pAgent;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent);
   return FAILED(hr) ? true : false;
}

bool IsPGSExtension()
{
   return !IsStandAlone();
}

#define REASON_OK 0
#define REASON_BC 1
#define REASON_NG 2 

Uint16 CanModel(PierIDType pierID)
{
   if ( pierID == INVALID_ID )
   {
      ATLASSERT(IsStandAlone());
      return REASON_OK;
   }

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
   const CPierData2* pPier = pIBridgeDesc->FindPier(pierID);
   if ( pPier->IsBoundaryPier() )
   {
      pgsTypes::BoundaryConditionType bcType = pPier->GetBoundaryConditionType();
      switch(bcType)
      {
      case pgsTypes::bctIntegralAfterDeckHingeBack:
      case pgsTypes::bctIntegralBeforeDeckHingeBack:
      case pgsTypes::bctIntegralAfterDeckHingeAhead:
      case pgsTypes::bctIntegralBeforeDeckHingeAhead:
         return REASON_BC;
      }

      const CGirderGroupData* pBackGroup  = pPier->GetGirderGroup(pgsTypes::Back);
      const CGirderGroupData* pAheadGroup = pPier->GetGirderGroup(pgsTypes::Ahead);
      ATLASSERT(pBackGroup->GetIndex() != pAheadGroup->GetIndex());
      GirderIndexType nGirdersBack  = pBackGroup->GetGirderCount();
      GirderIndexType nGirdersAhead = pAheadGroup->GetGirderCount();
      if ( nGirdersBack != nGirdersAhead )
      {
         return REASON_NG;
      }
   }

   return REASON_OK;
}

bool CanModelPier(PierIDType pierID,StatusGroupIDType statusGroupID,StatusCallbackIDType callbackID)
{
   Uint16 reason = CanModel(pierID);
   if ( reason != REASON_OK )
   {
      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);

      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      const CPierData2* pPier = pIBridgeDesc->FindPier(pierID);

      CString strReason;
      if ( reason == REASON_BC )
      {
         strReason = _T("Boundary condition not supported");
      }
      else if ( reason == REASON_NG )
      {
         strReason = _T("Must have same number of girders on each side of the pier");
      }
      else
      {
         ATLASSERT(false); // should never get here
         strReason = _T("Unspecified reason");
      }


      CString strMsg;
      strMsg.Format(_T("XBeam Rate cannot model Pier %d\r\n%s"),LABEL_PIER(pPier->GetIndex()),strReason);
      xbrBridgeStatusItem* pStatusItem = new xbrBridgeStatusItem(statusGroupID,callbackID,strMsg);

      GET_IFACE2(pBroker,IEAFStatusCenter,pStatusCenter);
      pStatusCenter->Add(pStatusItem);

      strMsg += _T("\n\nOpen status center to clear error");

      THROW_UNWIND(strMsg,-1);
   }

   return true;
}