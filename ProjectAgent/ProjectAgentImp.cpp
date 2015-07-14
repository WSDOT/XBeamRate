///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
// Copyright � 1999-2015  Washington State Department of Transportation
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

#include <XBeamRateExt\XBeamRateUtilities.h>

#include <EAF\EAFDisplayUnits.h>
#include <IFace\XBeamRateAgent.h>

#include <WBFLUnitServer\OpenBridgeML.h>

#include <PgsExt\BridgeDescription2.h>

#include <IFace\Bridge.h>
#include <IFace\Alignment.h>

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
   m_SysFactorShear   = 1.0;
   m_vbRateForShear[pgsTypes::lrDesign_Inventory] = true;
   m_vbRateForShear[pgsTypes::lrDesign_Operating] = true;
   m_vbRateForShear[pgsTypes::lrLegal_Routine]    = true;
   m_vbRateForShear[pgsTypes::lrLegal_Special]    = true;
   m_vbRateForShear[pgsTypes::lrPermit_Routine]   = true;
   m_vbRateForShear[pgsTypes::lrPermit_Special]   = true;

   m_gDC = 1.25;
   m_gDW = 1.50;
   m_gCR = 1.00;
   m_gSH = 1.00;
   m_gPS = 1.00;
   m_gRE = 1.00;

   m_gLL[pgsTypes::lrDesign_Inventory][INVALID_ID] = 1.75;
   m_gLL[pgsTypes::lrDesign_Operating][INVALID_ID] = 1.35;
   m_gLL[pgsTypes::lrLegal_Routine][INVALID_ID]    = 1.80;
   m_gLL[pgsTypes::lrLegal_Special][INVALID_ID]    = 1.60;
   m_gLL[pgsTypes::lrPermit_Routine][INVALID_ID]   = 1.15;
   m_gLL[pgsTypes::lrPermit_Special][INVALID_ID]   = 1.30;

   m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Design Truck + Lane"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Design Tandem + Lane"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Truck Train [90%(Truck + Lane)]"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Low Boy (Dual Tandem + Lane)]"),0));

   m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Design Truck + Lane"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Design Tandem + Lane"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Truck Train [90%(Truck + Lane)]"),0));
   m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].push_back(LiveLoadReaction(_T("LRFD Low Boy (Dual Tandem + Lane)]"),0));

   m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].push_back(LiveLoadReaction(_T("Type 3"),0));
   m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].push_back(LiveLoadReaction(_T("Type 3S2"),0));
   m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].push_back(LiveLoadReaction(_T("Type 3-3"),0));
   m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].push_back(LiveLoadReaction(_T("0.75(Two Type 3-3 separated by 30ft) + Lane Load"),0));

   m_LiveLoadReactions[pgsTypes::lrLegal_Special][INVALID_ID].push_back(LiveLoadReaction(_T("Notional Rating Load (NRL)"),0));

   xbrPierData pierData;
   IndexType nBearingLines = pierData.GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      std::vector<BearingReactions> vBearingReactions;
      IndexType nBearings = pierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         BearingReactions reactions;
         vBearingReactions.push_back(reactions);
      }
      m_BearingReactions[brgLineIdx].insert(std::make_pair(INVALID_ID,vBearingReactions));
   }

   // pier data for the stand alone case
   m_PierData.insert(std::make_pair(INVALID_ID,pierData));
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
   EAF_AGENT_INIT; // this macro defines pStatusCenter
   m_XBeamRateID = pStatusCenter->CreateStatusGroupID();

   //// Register status callbacks that we want to use
   //m_scidInformationalError       = pStatusCenter->RegisterCallback(new pgsInformationalStatusCallback(eafTypes::statusError)); 
   //m_scidInformationalWarning     = pStatusCenter->RegisterCallback(new pgsInformationalStatusCallback(eafTypes::statusWarning)); 

   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CProjectAgentImp::Init2()
{
   //
   // Attach to connection points for interfaces this agent depends on
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   // Connection point for the bridge description
   hr = pBrokerInit->FindConnectionPoint( IID_IBridgeDescriptionEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Advise( GetUnknown(), &m_dwBridgeDescCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

   return S_OK;
}

STDMETHODIMP CProjectAgentImp::Reset()
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
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint(IID_IBridgeDescriptionEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Unadvise( m_dwBridgeDescCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the connection point
   }

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

   CComPtr<IXBeamRateAgent> pAgent;
   bool bIsStandAlone = (SUCCEEDED(m_pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent)) ? false : true);

   // Save project data, properties, reactions, settings, etc if we are in "stand alone" mode
   // otherwise we are a plug-in and all we want to save is the pier description
   if ( bIsStandAlone )
   {
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

      pStrSave->BeginUnit(_T("LoadFactors"),1.0);
         pStrSave->put_Property(_T("DC"),CComVariant(m_gDC));
         pStrSave->put_Property(_T("DW"),CComVariant(m_gDW));
         pStrSave->put_Property(_T("CR"),CComVariant(m_gCR));
         pStrSave->put_Property(_T("SH"),CComVariant(m_gSH));
         pStrSave->put_Property(_T("PS"),CComVariant(m_gPS));
         pStrSave->put_Property(_T("RE"),CComVariant(m_gRE));
         pStrSave->put_Property(_T("LL_Design_Inventory"),CComVariant(m_gLL[pgsTypes::lrDesign_Inventory][INVALID_ID]));
         pStrSave->put_Property(_T("LL_Design_Operating"),CComVariant(m_gLL[pgsTypes::lrDesign_Operating][INVALID_ID]));
         pStrSave->put_Property(_T("LL_Legal_Routine"),CComVariant(m_gLL[pgsTypes::lrLegal_Routine][INVALID_ID]));
         pStrSave->put_Property(_T("LL_Legal_Special"),CComVariant(m_gLL[pgsTypes::lrLegal_Special][INVALID_ID]));
         pStrSave->put_Property(_T("LL_Permit_Routine"),CComVariant(m_gLL[pgsTypes::lrPermit_Routine][INVALID_ID]));
         pStrSave->put_Property(_T("LL_Permit_Special"),CComVariant(m_gLL[pgsTypes::lrPermit_Special][INVALID_ID]));
      pStrSave->EndUnit(); // LoadFactors

      pStrSave->BeginUnit(_T("Reactions"),1.0);
         pStrSave->BeginUnit(_T("DeadLoad"),1.0);
         IndexType nBearingLines = m_PierData[INVALID_ID].GetBearingLineCount();
         for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
         {
            std::vector<BearingReactions>& vBearingReactions = m_BearingReactions[brgLineIdx][INVALID_ID];
            pStrSave->BeginUnit(_T("BearingLine"),1.0);
            pStrSave->put_Property(_T("ReactionType"),CComVariant(m_BearingReactionType[brgLineIdx][INVALID_ID]));
            BOOST_FOREACH(BearingReactions& brgReaction,vBearingReactions)
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("DC"),CComVariant(brgReaction.DC));
                  pStrSave->put_Property(_T("DW"),CComVariant(brgReaction.DW));
                  pStrSave->put_Property(_T("CR"),CComVariant(brgReaction.CR));
                  pStrSave->put_Property(_T("SH"),CComVariant(brgReaction.SH));
                  pStrSave->put_Property(_T("PS"),CComVariant(brgReaction.PS));
                  pStrSave->put_Property(_T("RE"),CComVariant(brgReaction.RE));
                  pStrSave->put_Property(_T("W"), CComVariant(brgReaction.W));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // BearingLine
         }
         pStrSave->EndUnit(); // DeadLoad

         pStrSave->BeginUnit(_T("LiveLoad"),1.0);
            pStrSave->BeginUnit(_T("Design_Inventory"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Design_Inventory

            pStrSave->BeginUnit(_T("Design_Operating"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Design_Operating

            pStrSave->BeginUnit(_T("Legal_Routine"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Legal_Routine

            pStrSave->BeginUnit(_T("Legal_Special"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrLegal_Special][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Legal_Special

            pStrSave->BeginUnit(_T("Permit_Routine"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrPermit_Routine][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Permit_Routine

            pStrSave->BeginUnit(_T("Permit_Special"),1.0);
            BOOST_FOREACH(LiveLoadReaction& llReaction,m_LiveLoadReactions[pgsTypes::lrPermit_Special][INVALID_ID])
            {
               pStrSave->BeginUnit(_T("Reaction"),1.0);
                  pStrSave->put_Property(_T("Name"),CComVariant(llReaction.Name.c_str()));
                  pStrSave->put_Property(_T("LLIM"),CComVariant(llReaction.LLIM));
               pStrSave->EndUnit(); // Reaction
            }
            pStrSave->EndUnit(); // Permit_Special

         pStrSave->EndUnit(); // LiveLoad
      pStrSave->EndUnit(); // Reactions
   } // end if bIsStandAlone

   // Save the pier description information
   if ( bIsStandAlone )
   {
      m_PierData[INVALID_ID].Save(pStrSave,NULL);
   }
   else
   {
      std::map<PierIDType,xbrPierData>::iterator iter(m_PierData.begin());
      std::map<PierIDType,xbrPierData>::iterator end(m_PierData.end());
      for ( ; iter != end; iter++ )
      {
         if (iter->first == INVALID_ID )
         {
            continue; // this is the data for the stand alone case... skip it
         }

         xbrPierData& pierData(iter->second);
         ATLASSERT(pierData.GetID() != INVALID_ID);
         pierData.Save(pStrSave,NULL);
      }
   }

   if ( bIsStandAlone )
   {
      pStrSave->EndUnit(); // ProjectData
   }

   return hr;
}

STDMETHODIMP CProjectAgentImp::Load(IStructuredLoad* pStrLoad)
{
   USES_CONVERSION;
   CHRException hr;
   CComVariant var;

   CComPtr<IXBeamRateAgent> pAgent;
   bool bIsStandAlone = (SUCCEEDED(m_pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent)) ? false : true);

   try
   {
      if ( bIsStandAlone )
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

         {
            hr = pStrLoad->BeginUnit(_T("LoadFactors"));
            
            var.vt = VT_R8;
            hr = pStrLoad->get_Property(_T("DC"),&var);
            m_gDC = var.dblVal;

            hr = pStrLoad->get_Property(_T("DW"),&var);
            m_gDW = var.dblVal;

            hr = pStrLoad->get_Property(_T("CR"),&var);
            m_gCR = var.dblVal;

            hr = pStrLoad->get_Property(_T("SH"),&var);
            m_gSH = var.dblVal;

            hr = pStrLoad->get_Property(_T("PS"),&var);
            m_gPS = var.dblVal;

            hr = pStrLoad->get_Property(_T("RE"),&var);
            m_gRE = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Design_Inventory"),&var);
            m_gLL[pgsTypes::lrDesign_Inventory][INVALID_ID] = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Design_Operating"),&var);
            m_gLL[pgsTypes::lrDesign_Operating][INVALID_ID] = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Legal_Routine"),&var);
            m_gLL[pgsTypes::lrLegal_Routine][INVALID_ID] = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Legal_Special"),&var);
            m_gLL[pgsTypes::lrLegal_Special][INVALID_ID] = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Permit_Routine"),&var);
            m_gLL[pgsTypes::lrPermit_Routine][INVALID_ID] = var.dblVal;

            hr = pStrLoad->get_Property(_T("LL_Permit_Special"),&var);
            m_gLL[pgsTypes::lrPermit_Special][INVALID_ID] = var.dblVal;

            hr = pStrLoad->EndUnit(); // LoadFactors
         }

         {
            hr = pStrLoad->BeginUnit(_T("Reactions"));
            {
               {
                  hr = pStrLoad->BeginUnit(_T("DeadLoad"));
                  m_BearingReactions[0].clear();
                  m_BearingReactions[1].clear();
                  IndexType brgLineIdx = 0;
                  while ( SUCCEEDED(pStrLoad->BeginUnit(_T("BearingLine"))) )
                  {
                     var.vt = VT_I4;
                     hr = pStrLoad->get_Property(_T("ReactionType"),&var);
                     m_BearingReactionType[brgLineIdx][INVALID_ID] = (xbrTypes::ReactionLoadType)var.lVal;
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

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("CR"),&var);
                        reaction.CR = var.dblVal;

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("SH"),&var);
                        reaction.SH = var.dblVal;

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("PS"),&var);
                        reaction.PS = var.dblVal;

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("RE"),&var);
                        reaction.RE = var.dblVal;

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("W"),&var);
                        reaction.W = var.dblVal;

                        vBearingReactions.push_back(reaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }

                     m_BearingReactions[brgLineIdx].insert(std::make_pair(INVALID_ID,vBearingReactions));
                     brgLineIdx++;
                  }
                  hr = pStrLoad->EndUnit(); // BearingLine
                  hr = pStrLoad->EndUnit(); // DeadLoad
               }

               {
                  hr = pStrLoad->BeginUnit(_T("LiveLoad"));
                  {
                     hr = pStrLoad->BeginUnit(_T("Design_Inventory"));
                     m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrDesign_Inventory][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Design_Inventory

                     hr = pStrLoad->BeginUnit(_T("Design_Operating"));
                     m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrDesign_Operating][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Design_Operating

                     hr = pStrLoad->BeginUnit(_T("Legal_Routine"));
                     m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrLegal_Routine][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Legal_Routine

                     hr = pStrLoad->BeginUnit(_T("Legal_Special"));
                     m_LiveLoadReactions[pgsTypes::lrLegal_Special][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrLegal_Special][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Legal_Special

                     hr = pStrLoad->BeginUnit(_T("Permit_Routine"));
                     m_LiveLoadReactions[pgsTypes::lrPermit_Routine][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrPermit_Routine][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Permit_Routine

                     hr = pStrLoad->BeginUnit(_T("Permit_Special"));
                     m_LiveLoadReactions[pgsTypes::lrPermit_Special][INVALID_ID].clear();
                     while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Reaction"))) )
                     {
                        LiveLoadReaction llReaction;
                        var.vt = VT_BSTR;
                        hr = pStrLoad->get_Property(_T("Name"),&var);
                        llReaction.Name = OLE2T(var.bstrVal);

                        var.vt = VT_R8;
                        hr = pStrLoad->get_Property(_T("LLIM"),&var);
                        llReaction.LLIM = var.dblVal;

                        m_LiveLoadReactions[pgsTypes::lrPermit_Special][INVALID_ID].push_back(llReaction);
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
                     hr = pStrLoad->EndUnit(); // Permit_Special
                  }
                  hr = pStrLoad->EndUnit(); // LiveLoad
               }
            }
            hr = pStrLoad->EndUnit(); // Reactions
         }
      } // end if bIsStandAlone

      // Load the basic pier information
      if ( bIsStandAlone )
      {
         hr = m_PierData[INVALID_ID].Load(pStrLoad,NULL);
      }
      else
      {
         xbrPierData pierData;
         while ( SUCCEEDED(pierData.Load(pStrLoad,NULL)) )
         {
            PierIDType pierID = pierData.GetID();
            ATLASSERT(pierID != INVALID_ID);
            m_PierData.insert(std::make_pair(pierID,pierData));
         }
      }

      if ( bIsStandAlone )
      {
         hr = pStrLoad->EndUnit(); // ProjectData
      }
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }

   if ( !bIsStandAlone )
   {
      // since reactions are stored/loaded, need to resize the bearing reaction structure
      // so that it is the correct size
      m_BearingReactions[0].clear();
      m_BearingReactions[1].clear();
      std::map<PierIDType,xbrPierData>::iterator iter(m_PierData.begin());
      std::map<PierIDType,xbrPierData>::iterator end(m_PierData.end());
      for ( ; iter != end; iter++ )
      {
         PierIDType pierID = iter->first;
         xbrPierData& pierData(iter->second);
         IndexType nBearingLines = pierData.GetBearingLineCount();
         for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
         {
            std::vector<BearingReactions> vBearingReactions;
            xbrBearingLineData& brgLineData = pierData.GetBearingLineData(brgLineIdx);
            vBearingReactions.resize(brgLineData.GetBearingCount());
            m_BearingReactions[brgLineIdx].insert(std::make_pair(pierID,vBearingReactions));
         }
      }

      UpdatePiers();
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
   std::map<PierIDType,xbrPierData>::iterator found(m_PierData.find(pierData.GetID()));
   if ( found == m_PierData.end() )
   {
      ATLASSERT(pierData.GetID() != INVALID_ID);
      m_PierData.insert(std::make_pair(pierData.GetID(),pierData));
   }
   else
   {
      m_PierData[pierData.GetID()] = pierData;
   }

   // Resize the dead load reaction containers to match the number of bearings
   IndexType nBearingLines = pierData.GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      std::map<PierIDType,std::vector<BearingReactions>>::iterator foundReactions(m_BearingReactions[brgLineIdx].find(pierData.GetID()));
      if ( foundReactions == m_BearingReactions[brgLineIdx].end() )
      {
         ATLASSERT(pierData.GetID() != INVALID_ID);
         std::vector<BearingReactions> vBearingReactions;
         m_BearingReactions[brgLineIdx].insert(std::make_pair(pierData.GetID(),vBearingReactions));
         foundReactions = m_BearingReactions[brgLineIdx].find(pierData.GetID());
      }

      std::vector<BearingReactions>& vBearingReactions = foundReactions->second;
      IndexType nBearings = pierData.GetBearingCount(brgLineIdx);
      if ( vBearingReactions.size() != nBearings )
      {
         vBearingReactions.resize(nBearings);
      }
   }

   Fire_OnProjectChanged();
}

const xbrPierData& CProjectAgentImp::GetPierData(PierIDType pierID)
{
   return GetPrivatePierData(pierID);
}

xbrTypes::SuperstructureConnectionType CProjectAgentImp::GetPierType(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetSuperstructureConnectionType();
}

void CProjectAgentImp::SetPierType(PierIDType pierID,xbrTypes::SuperstructureConnectionType pierType)
{
   GetPrivatePierData(pierID).SetSuperstructureConnectionType(pierType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetDeckElevation(PierIDType pierID,Float64 deckElevation)
{
   GetPrivatePierData(pierID).SetDeckElevation(deckElevation);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDeckElevation(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetDeckElevation();
}

void CProjectAgentImp::SetCrownPointOffset(PierIDType pierID,Float64 cpo)
{
   GetPrivatePierData(pierID).SetCrownPointOffset(cpo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetCrownPointOffset(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetCrownPointOffset();
}

void CProjectAgentImp::SetBridgeLineOffset(PierIDType pierID,Float64 blo)
{
   GetPrivatePierData(pierID).SetBridgeLineOffset(blo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBridgeLineOffset(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetBridgeLineOffset();
}

void CProjectAgentImp::SetOrientation(PierIDType pierID,LPCTSTR strOrientation)
{
   GetPrivatePierData(pierID).SetSkew(strOrientation);
   Fire_OnProjectChanged();
}

LPCTSTR CProjectAgentImp::GetOrientation(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetSkew();
}

pgsTypes::OffsetMeasurementType CProjectAgentImp::GetCurbLineDatum(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetCurbLineDatum();
}

void CProjectAgentImp::SetCurbLineDatum(PierIDType pierID,pgsTypes::OffsetMeasurementType datumType)
{
   GetPrivatePierData(pierID).SetCurbLineDatum(datumType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetCurbLineOffset(PierIDType pierID,Float64 leftCLO,Float64 rightCLO)
{
   GetPrivatePierData(pierID).SetCurbLineOffset(leftCLO,rightCLO);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCurbLineOffset(PierIDType pierID,Float64* pLeftCLO,Float64* pRightCLO)
{
   GetPrivatePierData(pierID).GetCurbLineOffset(pLeftCLO,pRightCLO);
}

void CProjectAgentImp::SetCrownSlopes(PierIDType pierID,Float64 sl,Float64 sr)
{
   GetPrivatePierData(pierID).SetCrownSlope(sl,sr);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCrownSlopes(PierIDType pierID,Float64* psl,Float64* psr)
{
   GetPrivatePierData(pierID).GetCrownSlope(psl,psr);
}

void CProjectAgentImp::GetDiaphragmDimensions(PierIDType pierID,Float64* pH,Float64* pW)
{
   GetPrivatePierData(pierID).GetDiaphragmDimensions(pH,pW);
}

void CProjectAgentImp::SetDiaphragmDimensions(PierIDType pierID,Float64 H,Float64 W)
{
   GetPrivatePierData(pierID).SetDiaphragmDimensions(H,W);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetBearingLineCount(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetBearingLineCount();
}

void CProjectAgentImp::SetBearingLineCount(PierIDType pierID,IndexType nBearingLines)
{
   GetPrivatePierData(pierID).SetBearingLineCount(nBearingLines);
}

IndexType CProjectAgentImp::GetBearingCount(PierIDType pierID,IndexType brgLineIdx)
{
   return GetPrivatePierData(pierID).GetBearingCount(brgLineIdx);
}

void CProjectAgentImp::SetBearingCount(PierIDType pierID,IndexType brgLineIdx,IndexType nBearings)
{
   GetPrivatePierData(pierID).SetBearingCount(brgLineIdx,nBearings);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx)
{
   return GetPrivatePierData(pierID).GetBearingSpacing(brgLineIdx,brgIdx);
}

void CProjectAgentImp::SetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 spacing)
{
   GetPrivatePierData(pierID).SetBearingSpacing(brgLineIdx,brgIdx,spacing);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetBearingReactionType(PierIDType pierID,IndexType brgLineIdx,xbrTypes::ReactionLoadType brgReactionType)
{
   GetPrivateBearingReactionType(pierID,brgLineIdx) = brgReactionType;
   Fire_OnProjectChanged();
}

xbrTypes::ReactionLoadType CProjectAgentImp::GetBearingReactionType(PierIDType pierID,IndexType brgLineIdx)
{
   return GetPrivateBearingReactionType(pierID,brgLineIdx);
}

void CProjectAgentImp::SetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW,Float64 CR,Float64 SH,Float64 PS,Float64 RE,Float64 W)
{
   std::vector<BearingReactions>& vReactions = GetPrivateBearingReactions(pierID,brgLineIdx);
   vReactions[brgIdx].DC = DC;
   vReactions[brgIdx].DW = DW;
   vReactions[brgIdx].CR = CR;
   vReactions[brgIdx].SH = SH;
   vReactions[brgIdx].PS = PS;
   vReactions[brgIdx].RE = RE;
   vReactions[brgIdx].W  = W;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW,Float64* pCR,Float64* pSH,Float64* pPS,Float64* pRE,Float64* pW)
{
   std::vector<BearingReactions>& vReactions = GetPrivateBearingReactions(pierID,brgLineIdx);
   *pDC = vReactions[brgIdx].DC;
   *pDW = vReactions[brgIdx].DW;
   *pCR = vReactions[brgIdx].CR;
   *pSH = vReactions[brgIdx].SH;
   *pPS = vReactions[brgIdx].PS;
   *pRE = vReactions[brgIdx].RE;
   *pW  = vReactions[brgIdx].W;
}

void CProjectAgentImp::GetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum)
{
   GetPrivatePierData(pierID).GetBearingLineData(brgLineIdx).GetReferenceBearing(pRefBearingDatum,pRefIdx,pRefBearingOffset);
}

void CProjectAgentImp::SetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum)
{
   GetPrivatePierData(pierID).GetBearingLineData(brgLineIdx).SetReferenceBearing(refBearingDatum,refIdx,refBearingOffset);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetLiveLoadReactionCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType)
{
   return m_LiveLoadReactions[ratingType][pierID].size();
}

void CProjectAgentImp::SetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM)
{
   m_LiveLoadReactions[ratingType][pierID].clear();
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iter(vLLIM.begin());
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iterEnd(vLLIM.end());
   for ( ; iter != iterEnd; iter++ )
   {
      LiveLoadReaction ll;
      ll.Name = iter->first;
      ll.LLIM = iter->second;
      m_LiveLoadReactions[ratingType][pierID].push_back(ll);
   }

   Fire_OnProjectChanged();
}

std::vector<std::pair<std::_tstring,Float64>> CProjectAgentImp::GetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType)
{
   std::vector<std::pair<std::_tstring,Float64>> vReactions;
   BOOST_FOREACH(LiveLoadReaction& ll,m_LiveLoadReactions[ratingType][pierID])
   {
      vReactions.push_back(std::make_pair(ll.Name,ll.LLIM));
   }
   return vReactions;
}

LPCTSTR CProjectAgentImp::GetLiveLoadName(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx)
{
   return m_LiveLoadReactions[ratingType][pierID][vehIdx].Name.c_str();
}

Float64 CProjectAgentImp::GetLiveLoadReaction(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx)
{
   return m_LiveLoadReactions[ratingType][pierID][vehIdx].LLIM;
}

void CProjectAgentImp::SetRebarMaterial(PierIDType pierID,matRebar::Type type,matRebar::Grade grade)
{
   GetPrivatePierData(pierID).SetRebarMaterial(type,grade);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRebarMaterial(PierIDType pierID,matRebar::Type* pType,matRebar::Grade* pGrade)
{
   GetPrivatePierData(pierID).GetRebarMaterial(pType,pGrade);
}

void CProjectAgentImp::SetConcrete(PierIDType pierID,const CConcreteMaterial& concrete)
{
   GetPrivatePierData(pierID).SetConcreteMaterial(concrete);
   Fire_OnProjectChanged();
}

const CConcreteMaterial& CProjectAgentImp::GetConcrete(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetConcreteMaterial();
}

void CProjectAgentImp::SetLowerXBeamDimensions(PierIDType pierID,Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 x3,Float64 x4,Float64 w)
{
   GetPrivatePierData(pierID).SetLowerXBeamDimensions(h1,h2,h3,h4,x1,x2,x3,x4,w);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetLowerXBeamDimensions(PierIDType pierID,Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* px3,Float64* px4,Float64* pw)
{
   GetPrivatePierData(pierID).GetLowerXBeamDimensions(ph1,ph2,ph3,ph4,px1,px2,px3,px4,pw);
}

Float64 CProjectAgentImp::GetXBeamLeftOverhang(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetX5();
}

Float64 CProjectAgentImp::GetXBeamRightOverhang(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetX6();
}

Float64 CProjectAgentImp::GetXBeamWidth(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetW();
}

void CProjectAgentImp::SetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset)
{
   GetPrivatePierData(pierID).SetRefColumnLocation(refColumnDatum,refColumnIdx,refColumnOffset);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset)
{
   GetPrivatePierData(pierID).GetRefColumnLocation(prefColumnDatum,prefColumnIdx,prefColumnOffset);
}

IndexType CProjectAgentImp::GetColumnCount(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetColumnCount();
}

Float64 CProjectAgentImp::GetColumnHeight(PierIDType pierID,ColumnIndexType colIdx)
{
   return GetPrivatePierData(pierID).GetColumnData(colIdx).GetColumnHeight();
}

CColumnData::ColumnHeightMeasurementType CProjectAgentImp::GetColumnHeightMeasurementType(PierIDType pierID,ColumnIndexType colIdx)
{
   return GetPrivatePierData(pierID).GetColumnData(colIdx).GetColumnHeightMeasurementType();
}

Float64 CProjectAgentImp::GetColumnSpacing(PierIDType pierID,SpacingIndexType spaceIdx)
{
   return GetPrivatePierData(pierID).GetColumnSpacing(spaceIdx);
}

void CProjectAgentImp::SetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H)
{
   CColumnData& columnData = GetPrivatePierData(pierID).GetColumnData(colIdx);
   
   columnData.SetColumnHeight(H,heightType);
   columnData.SetColumnShape(shapeType);
   columnData.SetColumnDimensions(D1,D2);

   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH)
{
   CColumnData& columnData = GetPrivatePierData(pierID).GetColumnData(colIdx);

   *pshapeType = columnData.GetColumnShape();
   columnData.GetColumnDimensions(pD1,pD2);
   *pH = columnData.GetColumnHeight();
   *pheightType = columnData.GetColumnHeightMeasurementType();
}

pgsTypes::ColumnFixityType CProjectAgentImp::GetColumnFixity(PierIDType pierID,ColumnIndexType colIdx)
{
   return GetPrivatePierData(pierID).GetColumnData(colIdx).GetTransverseFixity();
}

const xbrLongitudinalRebarData& CProjectAgentImp::GetLongitudinalRebar(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetLongitudinalRebar();
}

void CProjectAgentImp::SetLongitudinalRebar(PierIDType pierID,const xbrLongitudinalRebarData& rebar)
{
   GetPrivatePierData(pierID).SetLongitudinalRebar(rebar);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetLowerXBeamStirrups(PierIDType pierID,const xbrStirrupData& stirrups)
{
   GetPrivatePierData(pierID).SetLowerXBeamStirrups(stirrups);
   Fire_OnProjectChanged();
}

const xbrStirrupData& CProjectAgentImp::GetLowerXBeamStirrups(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetLowerXBeamStirrups();
}

void CProjectAgentImp::SetFullDepthStirrups(PierIDType pierID,const xbrStirrupData& stirrups)
{
   GetPrivatePierData(pierID).SetFullDepthStirrups(stirrups);
   Fire_OnProjectChanged();
}

const xbrStirrupData& CProjectAgentImp::GetFullDepthStirrups(PierIDType pierID)
{
   return GetPrivatePierData(pierID).GetFullDepthStirrups();
}

void CProjectAgentImp::SetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor)
{
   GetPrivatePierData(pierID).SetConditionFactorType(conditionFactorType);
   GetPrivatePierData(pierID).SetConditionFactor(conditionFactor);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor)
{
   *pConditionFactorType = GetPrivatePierData(pierID).GetConditionFactorType();
   *pConditionFactor = GetPrivatePierData(pierID).GetConditionFactor();
}

Float64 CProjectAgentImp::GetConditionFactor(PierIDType pierID)
{
   pgsTypes::ConditionFactorType cfType;
   Float64 CF;
   GetConditionFactor(pierID,&cfType,&CF);

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

void CProjectAgentImp::SetCRLoadFactor(Float64 cr)
{
   m_gCR = cr;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetCRLoadFactor()
{
   return m_gCR;
}

void CProjectAgentImp::SetSHLoadFactor(Float64 sh)
{
   m_gSH = sh;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetSHLoadFactor()
{
   return m_gSH;
}

void CProjectAgentImp::SetPSLoadFactor(Float64 ps)
{
   m_gPS = ps;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetPSLoadFactor()
{
   return m_gPS;
}

void CProjectAgentImp::SetRELoadFactor(Float64 re)
{
   m_gRE = re;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetRELoadFactor()
{
   return m_gRE;
}

void CProjectAgentImp::SetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType,Float64 ll)
{
   m_gLL[ratingType][pierID] = ll;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType)
{
   return m_gLL[ratingType][pierID];
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
// IBridgeDescriptionEventSink
HRESULT CProjectAgentImp::OnBridgeChanged(CBridgeChangedHint* pHint)
{
   UpdatePiers();
   return S_OK;
}

HRESULT CProjectAgentImp::OnGirderFamilyChanged()
{
   return S_OK;
}

HRESULT CProjectAgentImp::OnGirderChanged(const CGirderKey& girderKey,Uint32 lHint)
{
   return S_OK;
}

HRESULT CProjectAgentImp::OnLiveLoadChanged()
{
   return S_OK;
}

HRESULT CProjectAgentImp::OnLiveLoadNameChanged(LPCTSTR strOldName,LPCTSTR strNewName)
{
   return S_OK;
}

HRESULT CProjectAgentImp::OnConstructionLoadChanged()
{
   return S_OK;
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
#if defined _DEBUG
   UINT filePos = pMenu->FindMenuItem(_T("&File"));
   ATLASSERT(filePos+1 == editPos);
#endif
   VERIFY(pMenu->RemoveMenu(editPos,MF_BYPOSITION,this));
}

xbrPierData& CProjectAgentImp::GetPrivatePierData(PierIDType pierID)
{
   std::map<PierIDType,xbrPierData>::const_iterator found(m_PierData.find(pierID));
   if ( found == m_PierData.end() )
   {
      xbrPierData pierData;
      pierData.SetID(pierID);
      m_PierData.insert(std::make_pair(pierID,pierData));
   }
   return m_PierData[pierID];
}

std::vector<CProjectAgentImp::BearingReactions>& CProjectAgentImp::GetPrivateBearingReactions(PierIDType pierID,IndexType brgLineIdx)
{
   std::map<PierIDType,std::vector<BearingReactions>>::const_iterator found(m_BearingReactions[brgLineIdx].find(pierID));
   if ( found == m_BearingReactions[brgLineIdx].end() )
   {
      xbrPierData& pierData = GetPrivatePierData(pierID);
      IndexType nBearingLines = pierData.GetBearingLineCount();
      for ( IndexType i = 0; i < nBearingLines; i++ )
      {
         std::vector<BearingReactions> vBearingReactions;
         vBearingReactions.resize(pierData.GetBearingCount(i));
         m_BearingReactions[i].insert(std::make_pair(pierID,vBearingReactions));
      }
   }

   return m_BearingReactions[brgLineIdx][pierID];
}

xbrTypes::ReactionLoadType& CProjectAgentImp::GetPrivateBearingReactionType(PierIDType pierID,IndexType brgLineIdx)
{
   std::map<PierIDType,xbrTypes::ReactionLoadType>::const_iterator found(m_BearingReactionType[brgLineIdx].find(pierID));
   if ( found == m_BearingReactionType[brgLineIdx].end() )
   {
      xbrPierData& pierData = GetPrivatePierData(pierID);
      IndexType nBearingLines = pierData.GetBearingLineCount();
      for ( IndexType i = 0; i < nBearingLines; i++ )
      {
         m_BearingReactions[i].insert(std::make_pair(pierID,xbrTypes::rltConcentrated));
      }
   }

   return m_BearingReactionType[brgLineIdx][pierID];
}

void CProjectAgentImp::UpdatePiers()
{
   GET_IFACE(IBridgeDescription,pIBridgeDesc);
   const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
   PierIndexType nPiers = pBridgeDesc->GetPierCount();
   for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
   {
      const CPierData2* pPier = pBridgeDesc->GetPier(pierIdx);
      PierIDType pierID = pPier->GetID();
      xbrPierData& pierData = m_PierData[pierID];
      UpdatePierData(pPier,pierData);
   }
}

void CProjectAgentImp::UpdatePierData(const CPierData2* pPier,xbrPierData& pierData)
{
   // Updates our internal pier data with the pier data from the bridge model
   USES_CONVERSION;

   if ( pPier->IsBoundaryPier() )
   {
      pierData.SetSuperstructureConnectionType( GetSuperstructureConnectionType(pPier->GetBoundaryConditionType()) );
   }
   else
   {
      pierData.SetSuperstructureConnectionType( GetSuperstructureConnectionType(pPier->GetSegmentConnectionType()) );
   }

   PierIndexType pierIdx = pPier->GetIndex();
   PierIDType pierID = pPier->GetID();
   
   pierData.SetID(pierID);

   GET_IFACE(IBridgeDescription,pIBridgeDesc);
   Float64 alignment_offset = pIBridgeDesc->GetBridgeDescription()->GetAlignmentOffset();
   pierData.SetBridgeLineOffset(alignment_offset);

   GET_IFACE(IBridge,pBridge);
   Float64 pierStation = pBridge->GetPierStation(pierIdx);

   GET_IFACE(IRoadway,pRoadway);
   Float64 elevation = pRoadway->GetElevation(pierStation,0);
   pierData.SetDeckElevation(elevation);

   Float64 CPO = pRoadway->GetCrownPointOffset(pierStation);
   pierData.SetCrownPointOffset(CPO);

   Float64 leftCLO  = pBridge->GetLeftCurbOffset(pierIdx);
   Float64 rightCLO = pBridge->GetRightCurbOffset(pierIdx);
   pierData.SetCurbLineDatum(pgsTypes::omtAlignment);
   pierData.SetCurbLineOffset(leftCLO,rightCLO);

   Float64 sLeft  = pRoadway->GetSlope(pierStation,leftCLO);
   Float64 sRight = pRoadway->GetSlope(pierStation,rightCLO);
   pierData.SetCrownSlope(-sLeft,sRight);

   // Skew angle
   CComPtr<IAngle> objSkew;
   pBridge->GetPierSkew(pPier->GetIndex(),&objSkew);
   Float64 skewAngle;
   objSkew->get_Value(&skewAngle);

   CComPtr<IAngleDisplayUnitFormatter> angle_formatter;
   angle_formatter.CoCreateInstance(CLSID_AngleDisplayUnitFormatter);
   angle_formatter->put_Signed(VARIANT_TRUE);
   CComBSTR bstrSkew;
   angle_formatter->Format(skewAngle,CComBSTR(),&bstrSkew);
   pierData.SetSkew(OLE2T(bstrSkew));


   // Lower Cross Beam
   Float64 H1, H2, H3, H4;
   Float64 X1, X2, X3, X4, W;
   pPier->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1,&X2);
   pPier->GetXBeamDimensions(pgsTypes::pstRight,&H3,&H4,&X3,&X4);
   W = pPier->GetXBeamWidth();
   pierData.SetLowerXBeamDimensions(H1,H2,H3,H4,X1,X2,X3,X4,W);

   // Upper Cross Beam Diaphragm
   Float64 H = Max(pPier->GetDiaphragmHeight(pgsTypes::Back),pPier->GetDiaphragmHeight(pgsTypes::Ahead));
   GroupIndexType backGrpIdx, aheadGrpIdx;
   pBridge->GetGirderGroupIndex(pierIdx,&backGrpIdx,&aheadGrpIdx);
   Float64 Aback  = pBridge->GetSlabOffset(backGrpIdx,pierIdx,0);
   Float64 Aahead = pBridge->GetSlabOffset(aheadGrpIdx,pierIdx,0);
   H += Max(Aback,Aahead);

   W = pPier->GetDiaphragmWidth(pgsTypes::Back) + pPier->GetDiaphragmWidth(pgsTypes::Ahead);
   pierData.SetDiaphragmDimensions(H,W);

   // Column Layout
   ColumnIndexType refColIdx;
   Float64 refColOffset;
   pgsTypes::OffsetMeasurementType refColMeasure;
   pPier->GetTransverseOffset(&refColIdx,&refColOffset,&refColMeasure);
   pierData.SetRefColumnLocation(refColMeasure,refColIdx,refColOffset);

   Float64 X5, X6;
   pPier->GetXBeamOverhangs(&X5,&X6);
   pierData.SetXBeamOverhangs(X5,X6);

   // Column Properties
   ColumnIndexType nColumns = pPier->GetColumnCount();
   pierData.SetColumnCount(nColumns);
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      if ( colIdx < nColumns-1 )
      {
         Float64 S = pPier->GetColumnSpacing(colIdx);
         pierData.GetColumnSpacing(colIdx) = S;
      }
   
      const CColumnData& columnData = pPier->GetColumnData(colIdx);
      pierData.GetColumnData(colIdx) = columnData;
   }

   // Materials
   pierData.SetConcreteMaterial(pPier->GetConcrete());

   // Bearing Lines and Bearing Locations
   if ( pierData.GetSuperstructureConnectionType() == xbrTypes::pctExpansion )
   {
      // two bearing lines
      pierData.SetBearingLineCount(2);

      GirderIndexType gdrIdx = 0;
      Float64 refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Back,pgsTypes::omtAlignment);

      std::vector<Float64> vBackSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Back,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData backBrgLine;
      backBrgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      backBrgLine.SetBearingCount(vBackSpacing.size()+1);
      backBrgLine.SetSpacing(vBackSpacing);
      pierData.SetBearingLineData(0,backBrgLine);

      m_BearingReactions[pierID][0].resize(backBrgLine.GetBearingCount());

      refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Ahead,pgsTypes::omtAlignment);
      std::vector<Float64> vAheadSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Ahead,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData aheadBrgLine;
      aheadBrgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      aheadBrgLine.SetBearingCount(vAheadSpacing.size()+1);
      aheadBrgLine.SetSpacing(vAheadSpacing);
      pierData.SetBearingLineData(1,aheadBrgLine);

      m_BearingReactions[pierID][1].resize(backBrgLine.GetBearingCount());
   }
   else
   {
      // one bearing line
      pierData.SetBearingLineCount(1);

      GirderIndexType gdrIdx = 0;
      Float64 refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Back,pgsTypes::omtAlignment);

      std::vector<Float64> vSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Back,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData brgLine;
      brgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      brgLine.SetBearingCount(vSpacing.size()+1);
      brgLine.SetSpacing(vSpacing);
      pierData.SetBearingLineData(0,brgLine);

      m_BearingReactions[pierID][0].resize(brgLine.GetBearingCount());
   }

   Fire_OnProjectChanged();
}
