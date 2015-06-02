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

// ProjectAgentImp.cpp : Implementation of CProjectAgentImp
#include "stdafx.h"
#include "ProjectAgent.h"
#include "ProjectAgentImp.h"

#include <EAF\EAFDisplayUnits.h>
#include <IFace\XBeamRateAgent.h>

#include <WBFLUnitServer\OpenBridgeML.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
CProjectAgentImp::CProjectAgentImp()
{
   m_pBroker = 0;
   m_EventHoldCount = 0;

   m_SysFactorFlexure = 1.0;
   m_SysFactorShear = 1.0;
   m_vbRateForShear[pgsTypes::lrDesign_Inventory] = true;
   m_vbRateForShear[pgsTypes::lrDesign_Operating] = true;
   m_vbRateForShear[pgsTypes::lrLegal_Routine]    = true;
   m_vbRateForShear[pgsTypes::lrLegal_Special]    = true;
   m_vbRateForShear[pgsTypes::lrPermit_Routine]   = true;
   m_vbRateForShear[pgsTypes::lrPermit_Special]   = true;

   m_gDC = 1.25;
   m_gDW = 1.50;
   m_gLL[pgsTypes::lrDesign_Inventory] = 1.75;
   m_gLL[pgsTypes::lrDesign_Operating] = 1.35;
   m_gLL[pgsTypes::lrLegal_Routine]    = 1.80;
   m_gLL[pgsTypes::lrLegal_Special]    = 1.60;
   m_gLL[pgsTypes::lrPermit_Routine]   = 1.15;
   m_gLL[pgsTypes::lrPermit_Special]   = 1.30;

   m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].push_back(LiveLoadReaction(_T("LRFD Design Truck + Lane"),0));
   m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].push_back(LiveLoadReaction(_T("LRFD Design Tandem + Lane"),0));
   m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].push_back(LiveLoadReaction(_T("LRFD Truck Train [90%(Truck + Lane)]"),0));
   m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].push_back(LiveLoadReaction(_T("LRFD Low Boy (Dual Tandem + Lane)]"),0));

   m_vLiveLoadReactions[pgsTypes::lrDesign_Operating] = m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory];

   m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].push_back(LiveLoadReaction(_T("Type 3"),0));
   m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].push_back(LiveLoadReaction(_T("Type 3S2"),0));
   m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].push_back(LiveLoadReaction(_T("Type 3-3"),0));
   m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].push_back(LiveLoadReaction(_T("0.75(Two Type 3-3 separated by 30ft) + Lane Load"),0));

   m_vLiveLoadReactions[pgsTypes::lrLegal_Special].push_back(LiveLoadReaction(_T("Notional Rating Load (NRL)"),0));

   IndexType nBearingLines = m_PierData.GetBearingLineCount();
   m_vvBearingReactions.resize(nBearingLines);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = m_PierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         BearingReactions reactions;
         reactions.DC = 0;
         reactions.DW = 0;
         m_vvBearingReactions[brgLineIdx].push_back(reactions);
      }
   }
}

CProjectAgentImp::~CProjectAgentImp()
{
}

HRESULT CProjectAgentImp::FinalConstruct()
{
   return S_OK;
}

void CProjectAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CProjectAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CProjectAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   m_CommandTarget.Init(pBroker);
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface( IID_IXBRProject,             this );
   pBrokerInit->RegInterface( IID_IXBRRatingSpecification, this );
   pBrokerInit->RegInterface( IID_IXBRProjectEdit,         this );
   pBrokerInit->RegInterface( IID_IXBREvents,              this );

   return S_OK;
};

STDMETHODIMP CProjectAgentImp::Init()
{
   //EAF_AGENT_INIT;

   ////
   //// Attach to connection points for interfaces this agent depends on
   ////
   //CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   //CComPtr<IConnectionPoint> pCP;
   //HRESULT hr = S_OK;

#pragma Reminder("WORKING HERE - Need better default values")

   //// Create default data model
   //XBeamRate::ApplicationSettings settings(XBeamRate::UnitModeEnum::US,_T("XBeam Rating Project"));

   //XBeamRate::PierTypeEnum pierType = XBeamRate::PierTypeEnum::Integral;

   //OpenBridgeML::Pier::CapBeamType capBeam(::ConvertToSysUnits(5.0,unitMeasure::Feet),
   //   ::ConvertToSysUnits(5.0,unitMeasure::Feet),
   //   ::ConvertToSysUnits(5.0,unitMeasure::Feet),
   //   ::ConvertToSysUnits(5.0,unitMeasure::Feet),::ConvertToSysUnits(1.0,unitMeasure::Feet),::ConvertToSysUnits(3.0,unitMeasure::Feet),
   //   ::ConvertToSysUnits(5.0,unitMeasure::Feet),::ConvertToSysUnits(1.0,unitMeasure::Feet),::ConvertToSysUnits(3.0,unitMeasure::Feet));

   //OpenBridgeML::Pier::FoundationType foundation(OpenBridgeML::Pier::IdealizedFoundationEnum::Fixed);
   //OpenBridgeML::Pier::PrismaticColumnType column(foundation);
   //OpenBridgeML::Pier::CicularColumnSectionType circularSection(::ConvertToSysUnits(3.0,unitMeasure::Feet));
   //column.CircularSection().set(circularSection);
   //column.Height() = ::ConvertToSysUnits(30.0,unitMeasure::Feet);

   //ColumnIndexType refColIdx = 0;
   //OpenBridgeML::Pier::PierLocationType pierLocation(::ConvertToSysUnits(-5.0,unitMeasure::Feet),OpenBridgeML::Types::OffsetMeasurementEnum::Alignment,refColIdx);

   //OpenBridgeML::Pier::ColumnsType columns;
   //columns.PrismaticColumn().push_back( column );
   //columns.Spacing().push_back(::ConvertToSysUnits(10.0,unitMeasure::Feet));
   //columns.PrismaticColumn().push_back(column);

   //OpenBridgeML::Pier::PierType pier(pierLocation,capBeam,columns);


   //Float64 modE = ::ConvertToSysUnits(5000,unitMeasure::PSI);
   //Float64 fc = ::ConvertToSysUnits(4000,unitMeasure::PSI);


   //Float64 deckElevation = 0;
   //Float64 bridgeLineOffset = 0;
   //Float64 crownPointOffset = 0;
   //OpenBridgeML::Types::OffsetMeasurementEnum curbLineDatum = OpenBridgeML::Types::OffsetMeasurementEnum::Alignment;
   //Float64 LCO = ::ConvertToSysUnits(10,unitMeasure::Feet);
   //Float64 RCO = ::ConvertToSysUnits(10,unitMeasure::Feet);
   //Float64 SL = -0.02;
   //Float64 SR = -0.02;

   //Float64 diaphragmWidth = capBeam.Width();
   //Float64 diaphragmHeight = ::ConvertToSysUnits(8,unitMeasure::Feet);

   //LPCTSTR strOrientation = _T("00 00 0.0 L");

   //XBeamRate::LiveLoadReactionsType designLiveLoad;
   //designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Design Truck + Lane"),0));
   //designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Design Tandem + Lane"),0));
   //designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Truck Train [90%(Truck + Lane)]"),0));
   //designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Low Boy (Dual Tandem + Lane)"),0));

   //XBeamRate::LiveLoadReactionsType legalRoutineLiveLoad;
   //legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3"),0));
   //legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3S2"),0));
   //legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3-3"),0));
   ////legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("0.75(Type 3-3) + Lane Load"),0)); // spans must be greater than 200 feet for this loading
   //legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("0.75(Two Type 3-3 separated by 30ft) + Lane Load"),0));

   //XBeamRate::LiveLoadReactionsType legalSpecialLiveLoad;
   //legalSpecialLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Notional Rating Load (NRL)"),0));

   //XBeamRate::LiveLoadReactionsType permitRoutineLiveLoad;
   //XBeamRate::LiveLoadReactionsType permitSpecialLiveLoad;

   //XBeamRate::SystemFactor systemFactor(1.0,1.0);
   //XBeamRate::ConditionFactorType conditionFactor(1.0,XBeamRate::ConditionFactorEnum::Good);
   //XBeamRate::DeadLoadFactors deadLoadFactors(1.25,1.5);
   //XBeamRate::LiveLoadFactors liveLoadFactors(1.75,1.35,1.80,1.60,1.30,1.15);
   //m_XBeamRateXML = std::auto_ptr<XBeamRate::XBeamRate>(new XBeamRate::XBeamRate(settings,systemFactor,conditionFactor,deadLoadFactors,liveLoadFactors,pierType,deckElevation,bridgeLineOffset,crownPointOffset,strOrientation,curbLineDatum,LCO,RCO,SL,SR,diaphragmHeight,diaphragmWidth,designLiveLoad,legalRoutineLiveLoad,legalSpecialLiveLoad,permitRoutineLiveLoad,permitSpecialLiveLoad,modE,fc,pier));

   //// Start off with one bearing line that has one bearing
   //XBeamRate::BearingLocatorType bearingLocator(0,OpenBridgeML::Types::OffsetMeasurementEnum::Alignment,::ConvertToSysUnits(-6.0,unitMeasure::Feet));
   //XBeamRate::BearingLineType backBearingLine(bearingLocator);
   //XBeamRate::BearingType bearing1(0,0,0);
   //XBeamRate::BearingType bearing2(0,0,0);
   //XBeamRate::BearingType bearing3(0,0,0);
   //Float64 S = ::ConvertToSysUnits(6.0,unitMeasure::Feet);

   //backBearingLine.Bearing().push_back(bearing1);
   //backBearingLine.Spacing().push_back(S);
   //backBearingLine.Bearing().push_back(bearing2);
   //backBearingLine.Spacing().push_back(S);
   //backBearingLine.Bearing().push_back(bearing3);

   //m_XBeamRateXML->BearingLine().push_back(backBearingLine);

   //// create a dummy rebar row
   //XBeamRate::LongitudinalRebarRowType rebarRow(XBeamRate::LongitudinalRebarDatumEnum::Bottom,::ConvertToSysUnits(2.0,unitMeasure::Inch),OpenBridgeML::StandardReinforcement::USBarEnum::US11,5,::ConvertToSysUnits(6.0,unitMeasure::Inch));
   //m_XBeamRateXML->LongitudinalRebar().push_back(rebarRow);

   return S_OK;
}

STDMETHODIMP CProjectAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_ProjectAgent;
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::ShutDown()
{
   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IAgentUIIntegration
STDMETHODIMP CProjectAgentImp::IntegrateWithUI(BOOL bIntegrate)
{
   CComPtr<IXBeamRateAgent> pXBR;
   HRESULT hr = m_pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pXBR);
   if ( SUCCEEDED(hr) )
   {
      // XBeam Rate is acting as an extension to PGSuper/PGSplice
      return S_OK;
   }

   if ( bIntegrate )
   {
      CreateMenus();
   }
   else
   {
      RemoveMenus();
   }

   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IAgentPersist
STDMETHODIMP CProjectAgentImp::Save(IStructuredSave* pStrSave)
{
   HRESULT hr = S_OK;

   pStrSave->BeginUnit(_T("ProjectData"),1.0);

   pStrSave->BeginUnit(_T("ProjectProperties"),1.0);
      pStrSave->put_Property(_T("BridgeName"), CComVariant(m_strBridgeName));
      pStrSave->put_Property(_T("BridgeId"),CComVariant(m_strBridgeId));
      pStrSave->put_Property(_T("Pier"),CComVariant(m_PierIdx));
      pStrSave->put_Property(_T("JobNumber"),CComVariant(m_strJobNumber));
      pStrSave->put_Property(_T("Engineer"),CComVariant(m_strEngineer));
      pStrSave->put_Property(_T("Company"),CComVariant(m_strCompany));
      pStrSave->put_Property(_T("Comments"),CComVariant(m_strComments));
   pStrSave->EndUnit(); // ProjectProperties

   // Need to save stuff like units and project properties,system factors
   // Also, need to save/load this data per pier
   // when this is a PGSuper/PGSplice extension, there can be many piers

   pStrSave->BeginUnit(_T("RatingSpecification"),1.0);
      pStrSave->put_Property(_T("SystemFactorFlexure"),CComVariant(m_SysFactorFlexure));
      pStrSave->put_Property(_T("SystemFactorShear"),CComVariant(m_SysFactorShear));
      pStrSave->put_Property(_T("RateForShear_Design_Inventory"),CComVariant(m_vbRateForShear[pgsTypes::lrDesign_Inventory]));
      pStrSave->put_Property(_T("RateForShear_Design_Operating"),CComVariant(m_vbRateForShear[pgsTypes::lrDesign_Operating]));
      pStrSave->put_Property(_T("RateForShear_Legal_Routine"),CComVariant(m_vbRateForShear[pgsTypes::lrLegal_Routine]));
      pStrSave->put_Property(_T("RateForShear_Legal_Special"),CComVariant(m_vbRateForShear[pgsTypes::lrLegal_Special]));
      pStrSave->put_Property(_T("RateForShear_Permit_Routine"),CComVariant(m_vbRateForShear[pgsTypes::lrPermit_Routine]));
      pStrSave->put_Property(_T("RateForShear_Permit_Special"),CComVariant(m_vbRateForShear[pgsTypes::lrPermit_Special]));
   pStrSave->EndUnit(); // RatingSpecification


   m_PierData.Save(pStrSave);

   pStrSave->BeginUnit(_T("LoadFactors"),1.0);
      pStrSave->put_Property(_T("DC"),CComVariant(m_gDC));
      pStrSave->put_Property(_T("DW"),CComVariant(m_gDW));
      pStrSave->put_Property(_T("LL_Design_Inventory"),CComVariant(m_gLL[pgsTypes::lrDesign_Inventory]));
      pStrSave->put_Property(_T("LL_Design_Operating"),CComVariant(m_gLL[pgsTypes::lrDesign_Operating]));
      pStrSave->put_Property(_T("LL_Legal_Routine"),CComVariant(m_gLL[pgsTypes::lrLegal_Routine]));
      pStrSave->put_Property(_T("LL_Legal_Special"),CComVariant(m_gLL[pgsTypes::lrLegal_Special]));
      pStrSave->put_Property(_T("LL_Permit_Routine"),CComVariant(m_gLL[pgsTypes::lrPermit_Routine]));
      pStrSave->put_Property(_T("LL_Permit_Special"),CComVariant(m_gLL[pgsTypes::lrPermit_Special]));
   pStrSave->EndUnit(); // LoadFactors

   pStrSave->BeginUnit(_T("Reactions"),1.0);
      pStrSave->BeginUnit(_T("DeadLoad"),1.0);
      std::vector<std::vector<BearingReactions>>::iterator brgLineIter(m_vvBearingReactions.begin());
      std::vector<std::vector<BearingReactions>>::iterator brgLineIterEnd(m_vvBearingReactions.end());
      for ( ; brgLineIter != brgLineIterEnd; brgLineIter++ )
      {
         std::vector<BearingReactions>& vBearingReactions(*brgLineIter);
         pStrSave->BeginUnit(_T("BearingLine"),1.0);
         BOOST_FOREACH(BearingReactions& brgReaction,vBearingReactions)
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("DC"),CComVariant(brgReaction.DC));
               pStrSave->put_Property(_T("DW"),CComVariant(brgReaction.DW));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // BearingLine
      }
      pStrSave->EndUnit(); // DeadLoad

      pStrSave->BeginUnit(_T("LiveLoad"),1.0);
         pStrSave->BeginUnit(_T("Design_Inventory"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Design_Inventory

         pStrSave->BeginUnit(_T("Design_Operating"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrDesign_Operating])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Design_Operating

         pStrSave->BeginUnit(_T("Legal_Routine"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrLegal_Routine])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Legal_Routine

         pStrSave->BeginUnit(_T("Legal_Special"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrLegal_Special])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Legal_Special

         pStrSave->BeginUnit(_T("Permit_Routine"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrPermit_Routine])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Permit_Routine

         pStrSave->BeginUnit(_T("Permit_Special"),1.0);
         BOOST_FOREACH(LiveLoadReaction& llReaction,m_vLiveLoadReactions[pgsTypes::lrPermit_Special])
         {
            pStrSave->BeginUnit(_T("Reaction"),1.0);
               pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
               pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
            pStrSave->EndUnit(); // Reaction
         }
         pStrSave->EndUnit(); // Permit_Special

      pStrSave->EndUnit(); // LiveLoad
   pStrSave->EndUnit(); // Reactions

   pStrSave->EndUnit(); // ProjectData

   return hr;
}

STDMETHODIMP CProjectAgentImp::Load(IStructuredLoad* pStrLoad)
{
   USES_CONVERSION;
   CHRException hr;
   CComVariant var;

   try
   {
      hr = pStrLoad->BeginUnit(_T("ProjectData"));

      {
         hr = pStrLoad->BeginUnit(_T("ProjectProperties"));
         var.vt = VT_BSTR;
         hr = pStrLoad->get_Property(_T("BridgeName"), &var);
         m_strBridgeName = OLE2T(var.bstrVal);

         hr = pStrLoad->get_Property(_T("BridgeId"),&var);
         m_strBridgeId = OLE2T(var.bstrVal);

         var.vt = VT_INDEX;
         hr = pStrLoad->get_Property(_T("Pier"),&var);
         m_PierIdx = VARIANT2INDEX(var);

         var.vt = VT_BSTR;
         hr = pStrLoad->get_Property(_T("JobNumber"),&var);
         m_strJobNumber = OLE2T(var.bstrVal);

         hr = pStrLoad->get_Property(_T("Engineer"),&var);
         m_strEngineer = OLE2T(var.bstrVal);

         hr = pStrLoad->get_Property(_T("Company"),&var);
         m_strCompany = OLE2T(var.bstrVal);

         hr = pStrLoad->get_Property(_T("Comments"),&var);
         m_strComments = OLE2T(var.bstrVal);

         hr = pStrLoad->EndUnit(); // ProjectProperties
      }


      {
         hr = pStrLoad->BeginUnit(_T("RatingSpecification"));
         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("SystemFactorFlexure"),&var);
         m_SysFactorFlexure = var.dblVal;

         hr = pStrLoad->get_Property(_T("SystemFactorShear"),&var);
         m_SysFactorShear = var.dblVal;

         var.vt = VT_BOOL;
         hr = pStrLoad->get_Property(_T("RateForShear_Design_Inventory"),&var);
         m_vbRateForShear[pgsTypes::lrDesign_Inventory] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->get_Property(_T("RateForShear_Design_Operating"),&var);
         m_vbRateForShear[pgsTypes::lrDesign_Operating] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->get_Property(_T("RateForShear_Legal_Routine"),&var);
         m_vbRateForShear[pgsTypes::lrLegal_Routine] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->get_Property(_T("RateForShear_Legal_Special"),&var);
         m_vbRateForShear[pgsTypes::lrLegal_Special] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->get_Property(_T("RateForShear_Permit_Routine"),&var);
         m_vbRateForShear[pgsTypes::lrPermit_Routine] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->get_Property(_T("RateForShear_Permit_Special"),&var);
         m_vbRateForShear[pgsTypes::lrPermit_Special] = (var.boolVal == VARIANT_TRUE ? true : false);

         hr = pStrLoad->EndUnit(); // RatingSpecification
      }

      hr = m_PierData.Load(pStrLoad);

      {
         hr = pStrLoad->BeginUnit(_T("LoadFactors"));
         
         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("DC"),&var);
         m_gDC = var.dblVal;

         hr = pStrLoad->get_Property(_T("DW"),&var);
         m_gDW = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Design_Inventory"),&var);
         m_gLL[pgsTypes::lrDesign_Inventory] = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Design_Operating"),&var);
         m_gLL[pgsTypes::lrDesign_Operating] = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Legal_Routine"),&var);
         m_gLL[pgsTypes::lrLegal_Routine] = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Legal_Special"),&var);
         m_gLL[pgsTypes::lrLegal_Special] = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Permit_Routine"),&var);
         m_gLL[pgsTypes::lrPermit_Routine] = var.dblVal;

         hr = pStrLoad->get_Property(_T("LL_Permit_Special"),&var);
         m_gLL[pgsTypes::lrPermit_Special] = var.dblVal;

         hr = pStrLoad->EndUnit(); // LoadFactors
      }

      {
         hr = pStrLoad->BeginUnit(_T("Reactions"));
         {
            {
               hr = pStrLoad->BeginUnit(_T("DeadLoad"));
               m_vvBearingReactions.clear();
               while ( SUCCEEDED(pStrLoad->BeginUnit(_T("BearingLine"))) )
               {
                  std::vector<BearingReactions> vBearingReactions;
                  while( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     BearingReactions reaction;
                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("DC"),&var);
                     reaction.DC = var.dblVal;

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("DW"),&var);
                     reaction.DW = var.dblVal;

                     vBearingReactions.push_back(reaction);
                  }
                  hr = pStrLoad->EndOfStorage(); // Reaction

                  m_vvBearingReactions.push_back(vBearingReactions);
               }
               hr = pStrLoad->EndUnit(); // BearingLine
               hr = pStrLoad->EndUnit(); // DeadLoad
            }

            {
               hr = pStrLoad->BeginUnit(_T("LiveLoad"));
               {
                  hr = pStrLoad->BeginUnit(_T("Design_Inventory"));
                  m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrDesign_Inventory].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Design_Inventory

                  hr = pStrLoad->BeginUnit(_T("Design_Operating"));
                  m_vLiveLoadReactions[pgsTypes::lrDesign_Operating].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrDesign_Operating].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Design_Operating

                  hr = pStrLoad->BeginUnit(_T("Legal_Routine"));
                  m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrLegal_Routine].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Legal_Routine

                  hr = pStrLoad->BeginUnit(_T("Legal_Special"));
                  m_vLiveLoadReactions[pgsTypes::lrLegal_Special].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrLegal_Special].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Legal_Special

                  hr = pStrLoad->BeginUnit(_T("Permit_Routine"));
                  m_vLiveLoadReactions[pgsTypes::lrPermit_Routine].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrPermit_Routine].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Permit_Routine

                  hr = pStrLoad->BeginUnit(_T("Permit_Special"));
                  m_vLiveLoadReactions[pgsTypes::lrPermit_Special].clear();
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                  {
                     LiveLoadReaction llReaction;
                     var.vt = VT_BSTR;
                     hr = pStrLoad->get_Property(_T("Name"),&var);
                     llReaction.Name = OLE2T(var.bstrVal);

                     var.vt = VT_R8;
                     hr = pStrLoad->get_Property(_T("LLIM"),&var);
                     llReaction.LLIM = var.dblVal;

                     m_vLiveLoadReactions[pgsTypes::lrPermit_Special].push_back(llReaction);
                  }
                  hr = pStrLoad->EndUnit(); // Reaction
                  hr = pStrLoad->EndUnit(); // Permit_Special
               }
               hr = pStrLoad->EndUnit(); // LiveLoad
            }
         }
         hr = pStrLoad->EndUnit(); // Reactions
      }

      hr = pStrLoad->EndUnit(); // ProjectData
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }

#pragma Reminder("FINISH - Set units")
   //// extract our data from the binding object
   //GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   //XBeamRate::ApplicationSettings& settings( m_XBeamRateXML->Settings() );
   //if ( settings.Units() == XBeamRate::UnitModeEnum::SI )
   //{
   //   pDisplayUnits->SetUnitMode(eafTypes::umSI);
   //}
   //else
   //{
   //   pDisplayUnits->SetUnitMode(eafTypes::umUS);
   //}

   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IEAFCommandCallback
BOOL CProjectAgentImp::OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
   return m_CommandTarget.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}

BOOL CProjectAgentImp::GetStatusBarMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // load appropriate string
	if ( rMessage.LoadString(nID) )
	{
		// first newline terminates actual string
      rMessage.Replace('\n','\0');
	}
	else
	{
		// not found
      TRACE1("Warning (XBeamRate::ProjectAgent): no message line prompt for ID 0x%04X.\n", nID);
	}

   return TRUE;
}

BOOL CProjectAgentImp::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CString string;
   // load appropriate string
	if ( string.LoadString(nID) )
	{
		// tip is after first newline 
      int pos = string.Find('\n');
      if ( 0 < pos )
      {
         rMessage = string.Mid(pos+1);
      }
	}
	else
	{
		// not found
      TRACE1("Warning (XBeamRate::ProjectAgent): no tool tip for ID 0x%04X.\n", nID);
	}

   return TRUE;
}

//////////////////////////////////////////////////////////////////////
// IXBRProjectProperties
LPCTSTR CProjectAgentImp::GetBridgeName() const
{
   return m_strBridgeName;
}

void CProjectAgentImp::SetBridgeName(LPCTSTR name)
{
   m_strBridgeName = name;
   Fire_OnProjectPropertiesChanged();
}

LPCTSTR CProjectAgentImp::GetBridgeID() const
{
   return m_strBridgeId;
}

void CProjectAgentImp::SetBridgeID(LPCTSTR bid)
{
   m_strBridgeId = bid;
   Fire_OnProjectPropertiesChanged();
}

PierIndexType CProjectAgentImp::GetPierIndex()
{
   return m_PierIdx;
}

void CProjectAgentImp::SetPierIndex(PierIndexType pierIdx)
{
   m_PierIdx = pierIdx;
   Fire_OnProjectPropertiesChanged();
}

LPCTSTR CProjectAgentImp::GetJobNumber() const
{
   return m_strJobNumber;
}

void CProjectAgentImp::SetJobNumber(LPCTSTR jid)
{
   m_strJobNumber = jid;
   Fire_OnProjectPropertiesChanged();
}

LPCTSTR CProjectAgentImp::GetEngineer() const
{
   return m_strEngineer;
}

void CProjectAgentImp::SetEngineer(LPCTSTR eng)
{
   m_strEngineer = eng;
   Fire_OnProjectPropertiesChanged();
}

LPCTSTR CProjectAgentImp::GetCompany() const
{
   return m_strCompany;
}

void CProjectAgentImp::SetCompany(LPCTSTR company)
{
   m_strCompany = company;
   Fire_OnProjectPropertiesChanged();
}

LPCTSTR CProjectAgentImp::GetComments() const
{
   return m_strComments;
}

void CProjectAgentImp::SetComments(LPCTSTR comments)
{
   m_strComments = comments;
   Fire_OnProjectPropertiesChanged();
}

//////////////////////////////////////////////////////////////////////
// IXBRProject
void CProjectAgentImp::SetPierData(const xbrPierData& pierData)
{
   m_PierData = pierData;

   // Resize the dead load reaction containers to match the number of bearings
   IndexType nBearingLines = m_PierData.GetBearingLineCount();
   if ( m_vvBearingReactions.size() != nBearingLines )
   {
      m_vvBearingReactions.resize(nBearingLines);
   }

   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = m_PierData.GetBearingCount(brgLineIdx);
      if ( m_vvBearingReactions[brgLineIdx].size() != nBearings )
      {
         m_vvBearingReactions[brgLineIdx].resize(nBearings);
      }
   }

   Fire_OnProjectChanged();
}

const xbrPierData& CProjectAgentImp::GetPierData()
{
   return m_PierData;
}

xbrTypes::SuperstructureConnectionType CProjectAgentImp::GetPierType()
{
   return m_PierData.GetSuperstructureConnectionType();
}

void CProjectAgentImp::SetPierType(xbrTypes::SuperstructureConnectionType pierType)
{
   m_PierData.SetSuperstructureConnectionType(pierType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetDeckElevation(Float64 deckElevation)
{
   m_PierData.SetDeckElevation(deckElevation);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDeckElevation()
{
   return m_PierData.GetDeckElevation();
}

void CProjectAgentImp::SetCrownPointOffset(Float64 cpo)
{
   m_PierData.SetCrownPointOffset(cpo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetCrownPointOffset()
{
   return m_PierData.GetCrownPointOffset();
}

void CProjectAgentImp::SetBridgeLineOffset(Float64 blo)
{
   m_PierData.SetBridgeLineOffset(blo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBridgeLineOffset()
{
   return m_PierData.GetBridgeLineOffset();
}

void CProjectAgentImp::SetOrientation(LPCTSTR strOrientation)
{
   m_PierData.SetSkew(strOrientation);
   Fire_OnProjectChanged();
}

LPCTSTR CProjectAgentImp::GetOrientation()
{
   return m_PierData.GetSkew();
}

pgsTypes::OffsetMeasurementType CProjectAgentImp::GetCurbLineDatum()
{
   return m_PierData.GetCurbLineDatum();
}

void CProjectAgentImp::SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType)
{
   m_PierData.SetCurbLineDatum(datumType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO)
{
   m_PierData.SetCurbLineOffset(leftCLO,rightCLO);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO)
{
   m_PierData.GetCurbLineOffset(pLeftCLO,pRightCLO);
}

void CProjectAgentImp::SetCrownSlopes(Float64 sl,Float64 sr)
{
   m_PierData.SetCrownSlope(sl,sr);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCrownSlopes(Float64* psl,Float64* psr)
{
   m_PierData.GetCrownSlope(psl,psr);
}

void CProjectAgentImp::GetDiaphragmDimensions(Float64* pH,Float64* pW)
{
   m_PierData.GetDiaphragmDimensions(pH,pW);
}

void CProjectAgentImp::SetDiaphragmDimensions(Float64 H,Float64 W)
{
   m_PierData.SetDiaphragmDimensions(H,W);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetBearingLineCount()
{
   return m_PierData.GetBearingLineCount();
}

void CProjectAgentImp::SetBearingLineCount(IndexType nBearingLines)
{
   m_PierData.SetBearingLineCount(nBearingLines);
}

IndexType CProjectAgentImp::GetBearingCount(IndexType brgLineIdx)
{
   return m_PierData.GetBearingCount(brgLineIdx);
}

void CProjectAgentImp::SetBearingCount(IndexType brgLineIdx,IndexType nBearings)
{
   m_PierData.SetBearingCount(brgLineIdx,nBearings);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx)
{
   return m_PierData.GetBearingSpacing(brgLineIdx,brgIdx);
}

void CProjectAgentImp::SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 spacing)
{
   m_PierData.SetBearingSpacing(brgLineIdx,brgIdx,spacing);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW)
{
   m_vvBearingReactions[brgLineIdx][brgIdx].DC = DC;
   m_vvBearingReactions[brgLineIdx][brgIdx].DW = DW;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW)
{
   *pDC = m_vvBearingReactions[brgLineIdx][brgIdx].DC;
   *pDW = m_vvBearingReactions[brgLineIdx][brgIdx].DW;
}

void CProjectAgentImp::GetReferenceBearing(IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum)
{
   m_PierData.GetBearingLineData(brgLineIdx).GetReferenceBearing(pRefBearingDatum,pRefIdx,pRefBearingOffset);
}

void CProjectAgentImp::SetReferenceBearing(IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum)
{
   m_PierData.GetBearingLineData(brgLineIdx).SetReferenceBearing(refBearingDatum,refIdx,refBearingOffset);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetLiveLoadReactionCount(pgsTypes::LoadRatingType ratingType)
{
   return m_vLiveLoadReactions[ratingType].size();
}

void CProjectAgentImp::SetLiveLoadReactions(pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM)
{
   m_vLiveLoadReactions[ratingType].clear();
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iter(vLLIM.begin());
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iterEnd(vLLIM.end());
   for ( ; iter != iterEnd; iter++ )
   {
      LiveLoadReaction ll;
      ll.Name = iter->first;
      ll.LLIM = iter->second;
      m_vLiveLoadReactions[ratingType].push_back(ll);
   }

   Fire_OnProjectChanged();
}

std::vector<std::pair<std::_tstring,Float64>> CProjectAgentImp::GetLiveLoadReactions(pgsTypes::LoadRatingType ratingType)
{
   std::vector<std::pair<std::_tstring,Float64>> vReactions;
   BOOST_FOREACH(LiveLoadReaction& ll,m_vLiveLoadReactions[ratingType])
   {
      vReactions.push_back(std::make_pair(ll.Name,ll.LLIM));
   }
   return vReactions;
}

LPCTSTR CProjectAgentImp::GetLiveLoadName(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx)
{
   return m_vLiveLoadReactions[ratingType][vehIdx].Name.c_str();
}

Float64 CProjectAgentImp::GetLiveLoadReaction(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx)
{
   return m_vLiveLoadReactions[ratingType][vehIdx].LLIM;
}

void CProjectAgentImp::SetRebarMaterial(matRebar::Type type,matRebar::Grade grade)
{
   m_PierData.SetRebarMaterial(type,grade);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade)
{
   m_PierData.GetRebarMaterial(pType,pGrade);
}

void CProjectAgentImp::SetConcrete(const xbrConcreteMaterial& concrete)
{
   m_PierData.SetConcreteMaterial(concrete);
   Fire_OnProjectChanged();
}

const xbrConcreteMaterial& CProjectAgentImp::GetConcrete()
{
   return m_PierData.GetConcreteMaterial();
}

void CProjectAgentImp::SetLowerXBeamDimensions(Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w)
{
   m_PierData.SetLowerXBeamDimensions(h1,h2,h3,h4,x1,x2,w);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetLowerXBeamDimensions(Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw)
{
   m_PierData.GetLowerXBeamDimensions(ph1,ph2,ph3,ph4,px1,px2,pw);
}

Float64 CProjectAgentImp::GetXBeamLeftOverhang()
{
   return m_PierData.GetX3();
}

Float64 CProjectAgentImp::GetXBeamRightOverhang()
{
   return m_PierData.GetX4();
}

Float64 CProjectAgentImp::GetXBeamWidth()
{
   return m_PierData.GetW();
}

void CProjectAgentImp::SetColumnLayout(IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s)
{
   m_PierData.SetColumnLayout(nColumns,refColumnDatum,refColumnIdx,refColumnOffset,x3,x4,s);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnLayout(IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps)
{
   m_PierData.GetColumnLayout(pnColumns,prefColumnDatum,prefColumnIdx,prefColumnOffset,px3,px4,ps);
}

IndexType CProjectAgentImp::GetColumnCount()
{
   return m_PierData.GetColumnCount();
}

Float64 CProjectAgentImp::GetColumnHeight()
{
   return m_PierData.GetColumnHeight();
}

CColumnData::ColumnHeightMeasurementType CProjectAgentImp::GetColumnHeightMeasurementType()
{
   return m_PierData.GetColumnHeightMeasure();
}

Float64 CProjectAgentImp::GetColumnSpacing()
{
   return m_PierData.GetColumnSpacing();
}

void CProjectAgentImp::SetColumnProperties(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H)
{
   m_PierData.SetColumnProperties(shapeType,D1,D2,heightType,H);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnProperties(CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH)
{
   m_PierData.GetColumnProperties(pshapeType,pD1,pD2,pheightType,pH);
}

Float64 CProjectAgentImp::GetXBeamLength()
{
   return m_PierData.GetXBeamLength();
}

IndexType CProjectAgentImp::GetRebarRowCount()
{
   return m_PierData.GetLongitudinalRebar().RebarRows.size();
}

void CProjectAgentImp::AddRebarRow(xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing)
{
   xbrLongitudinalRebarData::RebarRow row;
   row.Datum = datum;
   row.Cover = cover;
   row.BarSize = barSize;
   row.BarSpacing = spacing;
   row.NumberOfBars = nBars;
   m_PierData.GetLongitudinalRebar().RebarRows.push_back(row);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing)
{
   xbrLongitudinalRebarData::RebarRow row;
   row.Datum = datum;
   row.Cover = cover;
   row.BarSize = barSize;
   row.BarSpacing = spacing;
   row.NumberOfBars = nBars;
   m_PierData.GetLongitudinalRebar().RebarRows[rowIdx] = row;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,IndexType* pnBars,Float64* pSpacing)
{
   *pDatum   = m_PierData.GetLongitudinalRebar().RebarRows[rowIdx].Datum;
   *pCover   = m_PierData.GetLongitudinalRebar().RebarRows[rowIdx].Cover;
   *pBarSize = m_PierData.GetLongitudinalRebar().RebarRows[rowIdx].BarSize;
   *pnBars   = m_PierData.GetLongitudinalRebar().RebarRows[rowIdx].NumberOfBars;
   *pSpacing = m_PierData.GetLongitudinalRebar().RebarRows[rowIdx].BarSpacing;
}

void CProjectAgentImp::RemoveRebarRow(IndexType rowIdx)
{
   m_PierData.GetLongitudinalRebar().RebarRows.erase(m_PierData.GetLongitudinalRebar().RebarRows.begin()+rowIdx);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::RemoveRebarRows()
{
   m_PierData.GetLongitudinalRebar().RebarRows.clear();
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetConditionFactor(pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor)
{
   m_PierData.SetConditionFactorType(conditionFactorType);
   m_PierData.SetConditionFactor(conditionFactor);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetConditionFactor(pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor)
{
   *pConditionFactorType = m_PierData.GetConditionFactorType();
   *pConditionFactor = m_PierData.GetConditionFactor();
}

Float64 CProjectAgentImp::GetConditionFactor()
{
   pgsTypes::ConditionFactorType cfType;
   Float64 CF;
   GetConditionFactor(&cfType,&CF);

   // MBE 6A.4.2.3
   switch(cfType)
   {
   case pgsTypes::cfGood:
      return 1.0;

   case pgsTypes::cfFair:
      return 0.95;

   case pgsTypes::cfPoor:
      return 0.85;

   case pgsTypes::cfOther:
      return CF;
   }
   ATLASSERT(false);
   return 1.0;
}

void CProjectAgentImp::SetDCLoadFactor(Float64 dc)
{
   m_gDC = dc;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDCLoadFactor()
{
   return m_gDC;
}

void CProjectAgentImp::SetDWLoadFactor(Float64 dw)
{
   m_gDW = dw;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDWLoadFactor()
{
   return m_gDW;
}

void CProjectAgentImp::SetLiveLoadFactor(pgsTypes::LoadRatingType ratingType,Float64 ll)
{
   m_gLL[ratingType] = ll;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetLiveLoadFactor(pgsTypes::LoadRatingType ratingType)
{
   return m_gLL[ratingType];
}

//////////////////////////////////////////////////////////
// IXBRRatingSpecification
void CProjectAgentImp::SetSystemFactorFlexure(Float64 sysFactor)
{
   m_SysFactorFlexure = sysFactor;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetSystemFactorFlexure()
{
   return m_SysFactorFlexure;
}

void CProjectAgentImp::SetSystemFactorShear(Float64 sysFactor)
{
   m_SysFactorShear = sysFactor;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetSystemFactorShear()
{
   return m_SysFactorShear;
}

void CProjectAgentImp::RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear)
{
   m_vbRateForShear[ratingType] = bRateForShear;
   Fire_OnProjectChanged();
}

bool CProjectAgentImp::RateForShear(pgsTypes::LoadRatingType ratingType)
{
   return m_vbRateForShear[ratingType];
}

//////////////////////////////////////////////////////////
// IXBRProjectEdit
void CProjectAgentImp::EditPier(int nPage)
{
   m_CommandTarget.OnEditPier();
}

////////////////////////////////////////////////////////////////////////
// IXBREvents
void CProjectAgentImp::HoldEvents()
{
   ATLASSERT(0 <= m_EventHoldCount);
   m_EventHoldCount++;

   Fire_OnHoldEvents();
}

void CProjectAgentImp::FirePendingEvents()
{
   if ( m_EventHoldCount == 0 )
   {
      return;
   }

   if ( m_EventHoldCount == 1 )
   {
      m_EventHoldCount--;

      // Fire our events
	   if ( sysFlags<Uint32>::IsSet(m_PendingEvents,EVT_PROJECTPROPERTIES) )
      {
	      Fire_OnProjectPropertiesChanged();
      }

	   if ( sysFlags<Uint32>::IsSet(m_PendingEvents,EVT_PROJECT) )
      {
	      Fire_OnProjectChanged();
      }

      // tell event listeners that it is time to fire their events
      Fire_OnFirePendingEvents(); 
   }
   else
   {
      m_EventHoldCount--;
   }
}

void CProjectAgentImp::CancelPendingEvents()
{
   m_EventHoldCount--;
   if ( m_EventHoldCount <= 0 )
   {
      m_EventHoldCount = 0;
      Fire_OnCancelPendingEvents();
   }
}

//////////////////////////////////////////////////////////
void CProjectAgentImp::CreateMenus()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   // add an "Edit" menu to the right of the file menu
   UINT filePos = pMenu->FindMenuItem(_T("&File"));

   CEAFMenu* pEditMenu = pMenu->CreatePopupMenu(filePos+1,_T("&Edit"));
   pEditMenu->LoadMenu(IDR_EDIT_MENU,this);
}

void CProjectAgentImp::RemoveMenus()
{
   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   // remove the Edit menu
   UINT editPos = pMenu->FindMenuItem(_T("&Edit"));
   VERIFY(pMenu->RemoveMenu(editPos,MF_BYPOSITION,this));
}
