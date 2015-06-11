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

   xbrPierData pierData;
   IndexType nBearingLines = pierData.GetBearingLineCount();
   m_vvBearingReactions.resize(nBearingLines);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         BearingReactions reactions;
         reactions.DC = 0;
         reactions.DW = 0;
         m_vvBearingReactions[brgLineIdx].push_back(reactions);
      }
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
   } // end if bIsStandAlone

   // Save the pier description information
   if ( bIsStandAlone )
   {
      m_PierData[INVALID_ID].Save(pStrSave);
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
         pierData.Save(pStrSave);
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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }

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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
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
                        hr = pStrLoad->EndUnit(); // Reaction
                     }
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
         hr = m_PierData[INVALID_ID].Load(pStrLoad);
      }
      else
      {
         xbrPierData pierData;
         while ( SUCCEEDED(pierData.Load(pStrLoad)) )
         {
            PierIDType id = pierData.GetID();
            ATLASSERT(id != INVALID_ID);
            m_PierData.insert(std::make_pair(id,pierData));
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
   if ( m_vvBearingReactions.size() != nBearingLines )
   {
      m_vvBearingReactions.resize(nBearingLines);
   }

   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pierData.GetBearingCount(brgLineIdx);
      if ( m_vvBearingReactions[brgLineIdx].size() != nBearings )
      {
         m_vvBearingReactions[brgLineIdx].resize(nBearings);
      }
   }

   Fire_OnProjectChanged();
}

const xbrPierData& CProjectAgentImp::GetPierData(PierIDType id)
{
   return GetPrivatePierData(id);
}

xbrTypes::SuperstructureConnectionType CProjectAgentImp::GetPierType(PierIDType id)
{
   return GetPrivatePierData(id).GetSuperstructureConnectionType();
}

void CProjectAgentImp::SetPierType(PierIDType id,xbrTypes::SuperstructureConnectionType pierType)
{
   GetPrivatePierData(id).SetSuperstructureConnectionType(pierType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetDeckElevation(PierIDType id,Float64 deckElevation)
{
   GetPrivatePierData(id).SetDeckElevation(deckElevation);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDeckElevation(PierIDType id)
{
   return GetPrivatePierData(id).GetDeckElevation();
}

void CProjectAgentImp::SetCrownPointOffset(PierIDType id,Float64 cpo)
{
   GetPrivatePierData(id).SetCrownPointOffset(cpo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetCrownPointOffset(PierIDType id)
{
   return GetPrivatePierData(id).GetCrownPointOffset();
}

void CProjectAgentImp::SetBridgeLineOffset(PierIDType id,Float64 blo)
{
   GetPrivatePierData(id).SetBridgeLineOffset(blo);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBridgeLineOffset(PierIDType id)
{
   return GetPrivatePierData(id).GetBridgeLineOffset();
}

void CProjectAgentImp::SetOrientation(PierIDType id,LPCTSTR strOrientation)
{
   GetPrivatePierData(id).SetSkew(strOrientation);
   Fire_OnProjectChanged();
}

LPCTSTR CProjectAgentImp::GetOrientation(PierIDType id)
{
   return GetPrivatePierData(id).GetSkew();
}

pgsTypes::OffsetMeasurementType CProjectAgentImp::GetCurbLineDatum(PierIDType id)
{
   return GetPrivatePierData(id).GetCurbLineDatum();
}

void CProjectAgentImp::SetCurbLineDatum(PierIDType id,pgsTypes::OffsetMeasurementType datumType)
{
   GetPrivatePierData(id).SetCurbLineDatum(datumType);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetCurbLineOffset(PierIDType id,Float64 leftCLO,Float64 rightCLO)
{
   GetPrivatePierData(id).SetCurbLineOffset(leftCLO,rightCLO);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCurbLineOffset(PierIDType id,Float64* pLeftCLO,Float64* pRightCLO)
{
   GetPrivatePierData(id).GetCurbLineOffset(pLeftCLO,pRightCLO);
}

void CProjectAgentImp::SetCrownSlopes(PierIDType id,Float64 sl,Float64 sr)
{
   GetPrivatePierData(id).SetCrownSlope(sl,sr);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetCrownSlopes(PierIDType id,Float64* psl,Float64* psr)
{
   GetPrivatePierData(id).GetCrownSlope(psl,psr);
}

void CProjectAgentImp::GetDiaphragmDimensions(PierIDType id,Float64* pH,Float64* pW)
{
   GetPrivatePierData(id).GetDiaphragmDimensions(pH,pW);
}

void CProjectAgentImp::SetDiaphragmDimensions(PierIDType id,Float64 H,Float64 W)
{
   GetPrivatePierData(id).SetDiaphragmDimensions(H,W);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetBearingLineCount(PierIDType id)
{
   return GetPrivatePierData(id).GetBearingLineCount();
}

void CProjectAgentImp::SetBearingLineCount(PierIDType id,IndexType nBearingLines)
{
   GetPrivatePierData(id).SetBearingLineCount(nBearingLines);
}

IndexType CProjectAgentImp::GetBearingCount(PierIDType id,IndexType brgLineIdx)
{
   return GetPrivatePierData(id).GetBearingCount(brgLineIdx);
}

void CProjectAgentImp::SetBearingCount(PierIDType id,IndexType brgLineIdx,IndexType nBearings)
{
   GetPrivatePierData(id).SetBearingCount(brgLineIdx,nBearings);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetBearingSpacing(PierIDType id,IndexType brgLineIdx,IndexType brgIdx)
{
   return GetPrivatePierData(id).GetBearingSpacing(brgLineIdx,brgIdx);
}

void CProjectAgentImp::SetBearingSpacing(PierIDType id,IndexType brgLineIdx,IndexType brgIdx,Float64 spacing)
{
   GetPrivatePierData(id).SetBearingSpacing(brgLineIdx,brgIdx,spacing);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetBearingReactions(PierIDType id,IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW)
{
   m_vvBearingReactions[brgLineIdx][brgIdx].DC = DC;
   m_vvBearingReactions[brgLineIdx][brgIdx].DW = DW;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetBearingReactions(PierIDType id,IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW)
{
   *pDC = m_vvBearingReactions[brgLineIdx][brgIdx].DC;
   *pDW = m_vvBearingReactions[brgLineIdx][brgIdx].DW;
}

void CProjectAgentImp::GetReferenceBearing(PierIDType id,IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum)
{
   GetPrivatePierData(id).GetBearingLineData(brgLineIdx).GetReferenceBearing(pRefBearingDatum,pRefIdx,pRefBearingOffset);
}

void CProjectAgentImp::SetReferenceBearing(PierIDType id,IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum)
{
   GetPrivatePierData(id).GetBearingLineData(brgLineIdx).SetReferenceBearing(refBearingDatum,refIdx,refBearingOffset);
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetLiveLoadReactionCount(pgsTypes::LoadRatingType ratingType)
{
   return m_vLiveLoadReactions[ratingType].size();
}

void CProjectAgentImp::SetLiveLoadReactions(PierIDType id,pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM)
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

std::vector<std::pair<std::_tstring,Float64>> CProjectAgentImp::GetLiveLoadReactions(PierIDType id,pgsTypes::LoadRatingType ratingType)
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

Float64 CProjectAgentImp::GetLiveLoadReaction(PierIDType id,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx)
{
   return m_vLiveLoadReactions[ratingType][vehIdx].LLIM;
}

void CProjectAgentImp::SetRebarMaterial(PierIDType id,matRebar::Type type,matRebar::Grade grade)
{
   GetPrivatePierData(id).SetRebarMaterial(type,grade);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRebarMaterial(PierIDType id,matRebar::Type* pType,matRebar::Grade* pGrade)
{
   GetPrivatePierData(id).GetRebarMaterial(pType,pGrade);
}

void CProjectAgentImp::SetConcrete(PierIDType id,const CConcreteMaterial& concrete)
{
   GetPrivatePierData(id).SetConcreteMaterial(concrete);
   Fire_OnProjectChanged();
}

const CConcreteMaterial& CProjectAgentImp::GetConcrete(PierIDType id)
{
   return GetPrivatePierData(id).GetConcreteMaterial();
}

void CProjectAgentImp::SetLowerXBeamDimensions(PierIDType id,Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w)
{
   GetPrivatePierData(id).SetLowerXBeamDimensions(h1,h2,h3,h4,x1,x2,w);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetLowerXBeamDimensions(PierIDType id,Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw)
{
   GetPrivatePierData(id).GetLowerXBeamDimensions(ph1,ph2,ph3,ph4,px1,px2,pw);
}

Float64 CProjectAgentImp::GetXBeamLeftOverhang(PierIDType id)
{
   return GetPrivatePierData(id).GetX3();
}

Float64 CProjectAgentImp::GetXBeamRightOverhang(PierIDType id)
{
   return GetPrivatePierData(id).GetX4();
}

Float64 CProjectAgentImp::GetXBeamWidth(PierIDType id)
{
   return GetPrivatePierData(id).GetW();
}

void CProjectAgentImp::SetColumnLayout(PierIDType id,IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s)
{
   GetPrivatePierData(id).SetColumnLayout(nColumns,refColumnDatum,refColumnIdx,refColumnOffset,x3,x4,s);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnLayout(PierIDType id,IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps)
{
   GetPrivatePierData(id).GetColumnLayout(pnColumns,prefColumnDatum,prefColumnIdx,prefColumnOffset,px3,px4,ps);
}

IndexType CProjectAgentImp::GetColumnCount(PierIDType id)
{
   return GetPrivatePierData(id).GetColumnCount();
}

Float64 CProjectAgentImp::GetColumnHeight(PierIDType id)
{
   return GetPrivatePierData(id).GetColumnHeight();
}

CColumnData::ColumnHeightMeasurementType CProjectAgentImp::GetColumnHeightMeasurementType(PierIDType id)
{
   return GetPrivatePierData(id).GetColumnHeightMeasure();
}

Float64 CProjectAgentImp::GetColumnSpacing(PierIDType id)
{
   return GetPrivatePierData(id).GetColumnSpacing();
}

void CProjectAgentImp::SetColumnProperties(PierIDType id,CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H)
{
   GetPrivatePierData(id).SetColumnProperties(shapeType,D1,D2,heightType,H);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnProperties(PierIDType id,CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH)
{
   GetPrivatePierData(id).GetColumnProperties(pshapeType,pD1,pD2,pheightType,pH);
}

Float64 CProjectAgentImp::GetXBeamLength(PierIDType id)
{
   return GetPrivatePierData(id).GetXBeamLength();
}

IndexType CProjectAgentImp::GetRebarRowCount(PierIDType id)
{
   return GetPrivatePierData(id).GetLongitudinalRebar().RebarRows.size();
}

void CProjectAgentImp::AddRebarRow(PierIDType id,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing)
{
   xbrLongitudinalRebarData::RebarRow row;
   row.Datum = datum;
   row.Cover = cover;
   row.BarSize = barSize;
   row.BarSpacing = spacing;
   row.NumberOfBars = nBars;
   GetPrivatePierData(id).GetLongitudinalRebar().RebarRows.push_back(row);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetRebarRow(PierIDType id,IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing)
{
   xbrLongitudinalRebarData::RebarRow row;
   row.Datum = datum;
   row.Cover = cover;
   row.BarSize = barSize;
   row.BarSpacing = spacing;
   row.NumberOfBars = nBars;
   GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx] = row;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetRebarRow(PierIDType id,IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,IndexType* pnBars,Float64* pSpacing)
{
   *pDatum   = GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx].Datum;
   *pCover   = GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx].Cover;
   *pBarSize = GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx].BarSize;
   *pnBars   = GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx].NumberOfBars;
   *pSpacing = GetPrivatePierData(id).GetLongitudinalRebar().RebarRows[rowIdx].BarSpacing;
}

void CProjectAgentImp::RemoveRebarRow(PierIDType id,IndexType rowIdx)
{
   GetPrivatePierData(id).GetLongitudinalRebar().RebarRows.erase(GetPrivatePierData(id).GetLongitudinalRebar().RebarRows.begin()+rowIdx);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::RemoveRebarRows(PierIDType id)
{
   GetPrivatePierData(id).GetLongitudinalRebar().RebarRows.clear();
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetLongitudinalRebar(PierIDType id,const xbrLongitudinalRebarData& rebar)
{
   GetPrivatePierData(id).SetLongitudinalRebar(rebar);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetLowerXBeamStirrups(PierIDType id,const xbrStirrupData& stirrups)
{
   GetPrivatePierData(id).SetLowerXBeamStirrups(stirrups);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetFullDepthStirrups(PierIDType id,const xbrStirrupData& stirrups)
{
   GetPrivatePierData(id).SetFullDepthStirrups(stirrups);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetConditionFactor(PierIDType id,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor)
{
   GetPrivatePierData(id).SetConditionFactorType(conditionFactorType);
   GetPrivatePierData(id).SetConditionFactor(conditionFactor);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetConditionFactor(PierIDType id,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor)
{
   *pConditionFactorType = GetPrivatePierData(id).GetConditionFactorType();
   *pConditionFactor = GetPrivatePierData(id).GetConditionFactor();
}

Float64 CProjectAgentImp::GetConditionFactor(PierIDType id)
{
   pgsTypes::ConditionFactorType cfType;
   Float64 CF;
   GetConditionFactor(id,&cfType,&CF);

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
// IBridgeDescriptionEventSink
HRESULT CProjectAgentImp::OnBridgeChanged(CBridgeChangedHint* pHint)
{
   UpdatePier();
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

xbrPierData& CProjectAgentImp::GetPrivatePierData(PierIDType id)
{
   std::map<PierIDType,xbrPierData>::const_iterator found(m_PierData.find(id));
   if ( found == m_PierData.end() )
   {
      xbrPierData pierData;
      pierData.SetID(id);
      m_PierData.insert(std::make_pair(id,pierData));
   }
   return m_PierData[id];
}

void CProjectAgentImp::UpdatePier()
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

   GET_IFACE(IBridgeDescription,pIBridgeDesc);
   Float64 alignment_offset = pIBridgeDesc->GetBridgeDescription()->GetAlignmentOffset();
   pierData.SetBridgeLineOffset(alignment_offset);

   GET_IFACE(IBridge,pBridge);
   Float64 pierStation = pBridge->GetPierStation(pierIdx);

   GET_IFACE(IRoadway,pRoadway);
   Float64 elevation = pRoadway->GetElevation(pierStation,0);
   pierData.SetDeckElevation(elevation);

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
   Float64 X1, X2, W;
   pPier->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1);
   pPier->GetXBeamDimensions(pgsTypes::pstRight,&H3,&H4,&X2);
   W = pPier->GetXBeamWidth();
   pierData.SetLowerXBeamDimensions(H1,H2,H3,H4,X1,X2,W);

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

   Float64 X3, X4;
   pPier->GetXBeamOverhangs(&X3,&X4);

   ColumnIndexType nColumns = pPier->GetColumnCount();

   Float64 S = (1 < nColumns ? pPier->GetColumnSpacing(0) : 0);
   
   pierData.SetColumnLayout(nColumns,refColMeasure,refColIdx,refColOffset,X3,X4,S);

   // Column Properties
   const CColumnData& columnData = pPier->GetColumnData(0);
   Float64 D1,D2;
   columnData.GetColumnDimensions(&D1,&D2);
   pierData.SetColumnProperties(columnData.GetColumnShape(),D1,D2,columnData.GetColumnHeightMeasurementType(),columnData.GetColumnHeight());

   // Materials
   pierData.SetConcreteMaterial(pPier->GetConcrete());

   // Bearing Lines and Bearing Locations
   if ( pierData.GetSuperstructureConnectionType() == xbrTypes::pctExpansion )
   {
      // two bearing lines
      pierData.SetBearingLineCount(2);
      m_vvBearingReactions.resize(2);

      GirderIndexType gdrIdx = 0;
      Float64 refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Back,pgsTypes::omtAlignment);

      std::vector<Float64> vBackSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Back,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData backBrgLine;
      backBrgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      backBrgLine.SetBearingCount(vBackSpacing.size()+1);
      backBrgLine.SetSpacing(vBackSpacing);
      pierData.SetBearingLineData(0,backBrgLine);

      m_vvBearingReactions[0].resize(backBrgLine.GetBearingCount());

      refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Ahead,pgsTypes::omtAlignment);
      std::vector<Float64> vAheadSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Ahead,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData aheadBrgLine;
      aheadBrgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      aheadBrgLine.SetBearingCount(vAheadSpacing.size()+1);
      aheadBrgLine.SetSpacing(vAheadSpacing);
      pierData.SetBearingLineData(1,aheadBrgLine);

      m_vvBearingReactions[1].resize(aheadBrgLine.GetBearingCount());
   }
   else
   {
      // one bearing line
      pierData.SetBearingLineCount(1);
      m_vvBearingReactions.resize(1);

      GirderIndexType gdrIdx = 0;
      Float64 refBrgOffset = pBridge->GetGirderOffset(gdrIdx,pierIdx,pgsTypes::Back,pgsTypes::omtAlignment);

      std::vector<Float64> vSpacing = pBridge->GetGirderSpacing(pierIdx,pgsTypes::Back,pgsTypes::AtPierLine,pgsTypes::AlongItem);
      xbrBearingLineData brgLine;
      brgLine.SetReferenceBearing(pgsTypes::omtAlignment,gdrIdx,refBrgOffset);
      brgLine.SetBearingCount(vSpacing.size()+1);
      brgLine.SetSpacing(vSpacing);
      pierData.SetBearingLineData(0,brgLine);

      m_vvBearingReactions[0].resize(brgLine.GetBearingCount());
   }

   Fire_OnProjectChanged();
}
