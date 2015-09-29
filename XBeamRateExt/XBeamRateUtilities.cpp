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

void GetLaneInfo(Float64 Wcc,Float64* pWlane,IndexType* pnLanes,Float64* pWloadedLane)
{
   // Wlane = width of lane
   // WloadedLane = width of lane that is loaded
   // Typically we have 12 ft lanes and the load is spread over 10 ft.
   //
   //   |    |
   // 2'| 6' |2'
   //   v    v
   // ==========

   // LRFR 6A.2.3.2
   Float64 w10 = ::ConvertToSysUnits(10.0,unitMeasure::Feet);
   Float64 w12 = ::ConvertToSysUnits(12.0,unitMeasure::Feet);
   Float64 w18 = ::ConvertToSysUnits(18.0,unitMeasure::Feet);
   Float64 w20 = ::ConvertToSysUnits(20.0,unitMeasure::Feet);
   if ( Wcc < w12 )
   {
      *pWlane = 0;
      *pnLanes = 0;
      *pWloadedLane = 0;
   }
   else if ( w12 <= Wcc && Wcc < w18 )
   {
      *pWlane = w12;
      *pnLanes = 1;
      *pWloadedLane = w10;
   }
   else if ( ::InRange(w18,Wcc,w20) )
   {
      *pWlane = Wcc/2;
      *pnLanes = 2;
      if ( w12 <= *pWlane )
      {
         *pWloadedLane = w10;
      }
      else
      {
         *pWloadedLane = *pWlane;
      }
   }
   else
   {
      *pWlane = w12;
      *pnLanes = (IndexType)floor(Wcc/(*pWlane));
      *pWloadedLane = w10;
   }
}
