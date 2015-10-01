///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
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

// EngAgentImp.cpp : Implementation of CEngAgentImp
#include "stdafx.h"
#include "EngAgent.h"
#include "EngAgentImp.h"

#include <XBeamRateExt\PointOfInterest.h>
#include <XBeamRateExt\XBeamRateUtilities.h>
#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>
#include <IFace\Pier.h>
#include <IFace\AnalysisResults.h>

#include <Units\SysUnits.h>

#include "LoadRater.h"

#include <WBFLGenericBridge.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEngAgentImp
CEngAgentImp::CEngAgentImp()
{
   m_pBroker = 0;
}

CEngAgentImp::~CEngAgentImp()
{
}

HRESULT CEngAgentImp::FinalConstruct()
{
   return S_OK;
}

void CEngAgentImp::FinalRelease()
{
}

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CEngAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CEngAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface( IID_IXBRMomentCapacity, this );
   pBrokerInit->RegInterface( IID_IXBRShearCapacity,  this );
   pBrokerInit->RegInterface( IID_IXBRArtifact,       this );

   return S_OK;
};

STDMETHODIMP CEngAgentImp::Init()
{
   //EAF_AGENT_INIT;

   //
   // Attach to connection points for interfaces this agent depends on
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CEngAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CEngAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CEngAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_EngAgent;
   return S_OK;
}

STDMETHODIMP CEngAgentImp::ShutDown()
{
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint(IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the connection point

   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IXBRMomentCapacity
Float64 CEngAgentImp::GetMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   const MomentCapacityDetails& capacityDetails = GetMomentCapacityDetails(pierID,stage,poi,bPositiveMoment);
   return capacityDetails.Mr;
}

const MomentCapacityDetails& CEngAgentImp::GetMomentCapacityDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   std::map<IDType,MomentCapacityDetails>* pCapacity = (bPositiveMoment ? &m_PositiveMomentCapacity[stage] : &m_NegativeMomentCapacity[stage]);
   std::map<IDType,MomentCapacityDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   MomentCapacityDetails capacityDetails = ComputeMomentCapacity(pierID,stage,poi,bPositiveMoment);
   ATLASSERT(poi.GetID() != INVALID_ID);

   std::pair<std::map<IDType,MomentCapacityDetails>::iterator,bool> result = pCapacity->insert(std::make_pair(poi.GetID(),capacityDetails));
   ATLASSERT(result.second == true);

   std::map<IDType,MomentCapacityDetails>::iterator iter = result.first;
   MomentCapacityDetails& details = iter->second;
   return details;
}

Float64 CEngAgentImp::GetCrackingMoment(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   const CrackingMomentDetails& McrDetails = GetCrackingMomentDetails(pierID,stage,poi,bPositiveMoment);
   Float64 Mcr = McrDetails.Mcr;

   bool bAfter2002 = ( lrfdVersionMgr::SecondEditionWith2003Interims <= lrfdVersionMgr::GetVersion() ? true : false );
   bool bBefore2012 = ( lrfdVersionMgr::GetVersion() <  lrfdVersionMgr::SixthEdition2012 ? true : false );
   if ( bAfter2002 && bBefore2012 )
   {
      Mcr = (bPositiveMoment ? Max(McrDetails.Mcr,McrDetails.McrLimit) : Min(McrDetails.Mcr,McrDetails.McrLimit));
   }
   else
   {
      Mcr = McrDetails.Mcr;
   }

   return Mcr;
}

const CrackingMomentDetails& CEngAgentImp::GetCrackingMomentDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   std::map<IDType,CrackingMomentDetails>* pCapacity = (bPositiveMoment ? &m_PositiveCrackingMoment[stage] : &m_NegativeCrackingMoment[stage]);
   std::map<IDType,CrackingMomentDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   CrackingMomentDetails McrDetails = ComputeCrackingMoment(pierID,stage,poi,bPositiveMoment);
   ATLASSERT(poi.GetID() != INVALID_ID);

   std::pair<std::map<IDType,CrackingMomentDetails>::iterator,bool> result = pCapacity->insert(std::make_pair(poi.GetID(),McrDetails));
   ATLASSERT(result.second == true);

   std::map<IDType,CrackingMomentDetails>::iterator iter = result.first;
   CrackingMomentDetails& details = iter->second;
   return details;
}

Float64 CEngAgentImp::GetMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   const MinMomentCapacityDetails& MminDetails = GetMinMomentCapacityDetails(pierID,limitState,stage,poi,bPositiveMoment);
   return MminDetails.MrMin;
}

const MinMomentCapacityDetails& CEngAgentImp::GetMinMomentCapacityDetails(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   ATLASSERT(::IsRatingLimitState(limitState));// must be a load rating limit state
   std::map<IDType,MinMomentCapacityDetails>* pCapacity = (bPositiveMoment ? &m_PositiveMinMomentCapacity[stage][GET_INDEX(limitState)] : &m_NegativeMinMomentCapacity[stage][GET_INDEX(limitState)]);
   std::map<IDType,MinMomentCapacityDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   MinMomentCapacityDetails MminDetails = ComputeMinMomentCapacity(pierID,limitState,stage,poi,bPositiveMoment);
   ATLASSERT(poi.GetID() != INVALID_ID);

   std::pair<std::map<IDType,MinMomentCapacityDetails>::iterator,bool> result = pCapacity->insert(std::make_pair(poi.GetID(),MminDetails));
   ATLASSERT(result.second == true);

   std::map<IDType,MinMomentCapacityDetails>::iterator iter = result.first;
   MinMomentCapacityDetails& details = iter->second;
   return details;
}

MinMomentCapacityDetails CEngAgentImp::GetMinMomentCapacityDetails(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx)
{
   GET_IFACE(IProgress, pProgress);
   CEAFAutoProgress ap(pProgress);
   pProgress->UpdateMessage(_T("Computing minimum moment capacity"));

   return ComputeMinMomentCapacity(pierID,limitState,stage,poi,bPositiveMoment,vehicleIdx,llConfigIdx,permitLaneIdx);
}

//////////////////////////////////////////////////////////////////////
// IXBRShearCapacity
Float64 CEngAgentImp::GetShearCapacity(PierIDType pierID,const xbrPointOfInterest& poi)
{
   // LRFD 5.8.3.4.1
   Float64 beta = 2.0;
   Float64 theta = M_PI/4; // 45 deg

   GET_IFACE(IXBRProject,pProject);

   matRebar::Type type;
   matRebar::Grade grade;
   pProject->GetRebarMaterial(pierID,&type,&grade);
   Float64 fy = matRebar::GetYieldStrength(type,grade);


   Float64 dv1 = GetDv(pierID,xbrTypes::Stage1,poi);
   Float64 dv2 = GetDv(pierID,xbrTypes::Stage2,poi);
   Float64 Av_over_S1 = GetAverageAvOverS(pierID,xbrTypes::Stage1,poi,theta);
   Float64 Av_over_S2 = GetAverageAvOverS(pierID,xbrTypes::Stage2,poi,theta);

   // if non-integralpier, dv2 is zero so dv1 will be the max, 
   // otherwise dv2 will be the max
   Float64 dv = Max(dv1,dv2);

   // Also need to account for x-beam type (integral, continuous, expansion... only integral has upper diaphragm)
   Float64 fc = pProject->GetConcrete(pierID).Fc;
   Float64 bv = pProject->GetXBeamWidth(pierID);
   Float64 fc_us = ::ConvertFromSysUnits(fc,unitMeasure::KSI);
   Float64 Vc_us = 0.0316*beta*sqrt(fc_us)*bv*dv;
   Float64 Vc = ::ConvertToSysUnits(Vc_us,unitMeasure::KSI);
   Float64 Vs1 = Av_over_S1*fy*dv1/(tan(theta)); // lower x-beam reinforcement capacity
   Float64 Vs2 = Av_over_S2*fy*dv2/(tan(theta)); // full x-beam reinforcement capacity
   Float64 Vs = Vs1 + Vs2; // total capacity due to reinforcement

   Float64 Vn1 = Vc + Vs;
   Float64 Vn2 = 0.25*fc*bv*dv;

   return Min(Vn1,Vn2);
}

//////////////////////////////////////////////////////////////////////
// IXBRArtifactCapacity
const xbrRatingArtifact* CEngAgentImp::GetXBeamRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx)
{
   RatingArtifacts& ratingArtifacts = GetPrivateRatingArtifacts(pierID,ratingType,vehicleIdx);
   std::map<VehicleIndexType,xbrRatingArtifact>::iterator found = ratingArtifacts.find(vehicleIdx);
   if ( found == ratingArtifacts.end() )
   {
      CreateRatingArtifact(pierID,ratingType,vehicleIdx);
      found = ratingArtifacts.find(vehicleIdx);
   }

   if ( found == ratingArtifacts.end() )
   {
      return NULL;
   }
   else
   {
      return &(found->second);
   }
}

//////////////////////////////////////////////////
HRESULT CEngAgentImp::OnProjectChanged()
{
   for ( int i = 0; i < 2; i++ )
   {
      m_PositiveMomentCapacity[i].clear();
      m_NegativeMomentCapacity[i].clear();

      m_PositiveCrackingMoment[i].clear();
      m_NegativeCrackingMoment[i].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_Inventory)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_Inventory)].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_Operating)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_Operating)].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_LegalRoutine)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_LegalRoutine)].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_LegalSpecial)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthI_LegalSpecial)].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthII_PermitRoutine)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthII_PermitRoutine)].clear();

      m_PositiveMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthII_PermitSpecial)].clear();
      m_NegativeMinMomentCapacity[i][GET_INDEX(pgsTypes::StrengthII_PermitSpecial)].clear();
   }

   for ( int i = 0; i < 6; i++ )
   {
      m_RatingArtifacts[i].clear();
   }

   return S_OK;
}

//////////////////////////////////////////////////
MomentCapacityDetails CEngAgentImp::ComputeMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   CComPtr<IRCBeam2> rcBeam;
   HRESULT hr = rcBeam.CoCreateInstance(CLSID_RCBeam2);
   ATLASSERT(SUCCEEDED(hr));

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 h = pSectProps->GetDepth(pierID,stage,poi);
   rcBeam->put_h(h);
   rcBeam->put_hf(0);

   GET_IFACE(IXBRProject,pProject);
   Float64 w = pProject->GetXBeamWidth(pierID);
   rcBeam->put_b(w);
   rcBeam->put_bw(w);

   xbrTypes::SuperstructureConnectionType connectionType = pProject->GetPierType(pierID);
   Float64 d;
   pProject->GetDiaphragmDimensions(pierID,&d,&w);

   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   rcBeam->put_FcSlab(concrete.Fc);
   rcBeam->put_FcBeam(concrete.Fc);

#pragma Reminder("WORKING HERE - need rebar material for rc model")
   // rebar could be grade 60, 80, 100, etc

   Float64 dt = 0; // location of the extreme tension steel

   GET_IFACE(IXBRRebar,pRebar);
   IndexType nRows = pRebar->GetRebarRowCount(pierID);
   for ( IndexType rowIdx = 0; rowIdx < nRows; rowIdx++ )
   {
      CComPtr<IRebarSection> rebarSection;
      pRebar->GetRebarSection(pierID,stage,poi,&rebarSection);

      CComPtr<IEnumRebarSectionItem> enumRebar;
      rebarSection->get__EnumRebarSectionItem(&enumRebar);

      CComPtr<IRebarSectionItem> rebarSectionItem;
      while ( enumRebar->Next(1,&rebarSectionItem,NULL) != S_FALSE )
      {
         CComPtr<IPoint2d> pntRebar;
         rebarSectionItem->get_Location(&pntRebar);

         Float64 Ybar;
         pntRebar->get_Y(&Ybar);
         ATLASSERT(Ybar < 0); // bars are in section coordinates so they should be below the X-axis

         Ybar *= -1; // Ybar is now measured from the top down with Y being positive downwards

         // if continous or expansion pier, the upper cross beam doesn't contribute
         // to capacity. but the rebar are measured from the top down of the entire
         // section. deduct the height of the upper cross beam to get the depth of
         // the rebar relative to the top of the lower cross beam
         if ( connectionType != xbrTypes::pctIntegral )
         {
            Ybar -= d;
         }

         if ( !bPositiveMoment )
         {
            Ybar = h - Ybar; // Ybar is measured from the bottom up for negative moment
         }

         dt = Max(dt,Ybar);

         CComPtr<IRebar> rebar;
         rebarSectionItem->get_Rebar(&rebar);

         Float64 As;
         rebar->get_NominalArea(&As);

         Float64 devFactor = pRebar->GetDevLengthFactor(pierID,poi,rebarSectionItem);
         ATLASSERT(::InRange(0.0,devFactor,1.0));
         rcBeam->AddRebarLayer(Ybar,As,devFactor);

         rebarSectionItem.Release();
      }
   }


   CComPtr<IRCSolver2> solver;
   hr = solver.CoCreateInstance(CLSID_LRFDSolver2);
   ATLASSERT(SUCCEEDED(hr));

   CComPtr<IRCSolutionEx> solution;
   hr = solver->Solve(rcBeam,&solution); 
   ATLASSERT(SUCCEEDED(hr));

   Float64 Mn;
   solution->get_Mn(&Mn);
   if ( !bPositiveMoment )
   {
      Mn *= -1;
   }

   Float64 dc = 0;
   Float64 de = 0;
   Float64 phi = 0;
   if ( !IsZero(Mn) )
   {
      Float64 c;
      solution->get_NeutralAxisDepth(&c);

      matRebar::Type rebarType;
      matRebar::Grade rebarGrade;
      pProject->GetRebarMaterial(pierID,&rebarType,&rebarGrade);

      Float64 et = (dt - c)*0.003/c;
      Float64 ecl = lrfdRebar::GetCompressionControlledStrainLimit(rebarGrade);
      Float64 etl = lrfdRebar::GetTensionControlledStrainLimit(rebarGrade);
      phi = 0.75 + 0.15*(et - ecl)/(etl-ecl);
      phi = ::ForceIntoRange(0.75,phi,0.9);

      Float64 Cweb, Cflange, Yweb, Yflange;
      solution->get_Cweb(&Cweb);
      solution->get_Cflange(&Cflange);
      solution->get_Yweb(&Yweb);
      solution->get_Yflange(&Yflange);
      dc = (Cweb*Yweb + Cflange*Yflange)/(Cweb + Cflange);

      Float64 T;
      solution->get_T(&T);
      Float64 MomentArm = fabs(Mn/T);
      de = dc + MomentArm;
   }

   MomentCapacityDetails capacityDetails;
   //capacityDetails.rcBeam = rcBeam;
   capacityDetails.solution = solution;
   capacityDetails.dc = dc;
   capacityDetails.de = de;
   capacityDetails.dt = dt;
   capacityDetails.phi = phi;
   capacityDetails.Mn = Mn;
   capacityDetails.Mr = phi*Mn;

   return capacityDetails;
}

void CEngAgentImp::GetCrackingMomentFactors(PierIDType pierID,Float64* pG1,Float64* pG2,Float64* pG3)
{
   // gamma factors from LRFD 5.7.3.3.2 (LRFD 6th Edition, 2012)
   if ( lrfdVersionMgr::SixthEdition2012 <= lrfdVersionMgr::GetVersion() )
   {
      *pG1 = 1.6; // all other concrete structures (not-segmental)
      *pG2 = 1.1; // bonded strand/tendon

      GET_IFACE(IXBRMaterial,pMaterial);
      Float64 E,fy,fu;
      pMaterial->GetRebarProperties(pierID,&E,&fy,&fu);
      *pG3 = fy/fu;
   }
   else
   {
      *pG1 = 1.0;
      *pG2 = 1.0;
      *pG3 = 1.0;
   }
}

CrackingMomentDetails CEngAgentImp::ComputeCrackingMoment(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   CrackingMomentDetails McrDetails;

   McrDetails.McrLimit; // Limiting cracking moment ... per 2nd Edition + 2003 interims (changed in 2005 interims)

   GET_IFACE(IXBRMaterial,pMaterial);
   Float64 fr = pMaterial->GetXBeamModulusOfRupture(pierID);

   Float64 fcpe = 0; // There is no prestressing in the cross beams

   // gamma factors from LRFD 5.7.3.3.2 (LRFD 6th Edition, 2012)
   Float64 g1, g2, g3;
   GetCrackingMomentFactors(pierID,&g1,&g2,&g3);

   GET_IFACE(IXBRAnalysisResults,pAnalysisResults);
   Float64 Mdnc = pAnalysisResults->GetMoment(pierID,xbrTypes::pftLowerXBeam,poi);

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 Sc  = (bPositiveMoment ? pSectProps->GetSbot(pierID,stage,poi)            : pSectProps->GetStop(pierID,stage,poi));
   Float64 Snc = (bPositiveMoment ? pSectProps->GetSbot(pierID,xbrTypes::Stage1,poi) : pSectProps->GetStop(pierID,xbrTypes::Stage1,poi));

   Float64 Mcr = g3*((g1*fr + g2*fcpe)*Sc - Mdnc*(Sc/Snc-1));

   if ( lrfdVersionMgr::SecondEditionWith2003Interims <= lrfdVersionMgr::GetVersion() )
   {
      Float64 McrLimit = Sc*fr;
      McrDetails.McrLimit = McrLimit;
   }

   McrDetails.Snc = Snc;
   McrDetails.Sc  = Sc;
   McrDetails.Mdnc = Mdnc;
   McrDetails.fr = fr;
   McrDetails.fcpe = fcpe;
   McrDetails.g1 = g1;
   McrDetails.g2 = g2;
   McrDetails.g3 = g3;

   McrDetails.Mcr = Mcr;

   return McrDetails;
}

MinMomentCapacityDetails CEngAgentImp::ComputeMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   Float64 Mr;     // Nominal resistance (phi*Mn)
   Float64 Mcr;    // Cracking moment
   Float64 MrMin;  // Minimum nominal resistance - Min(MrMin1,MrMin2)
   Float64 MrMin1; // 1.2Mcr
   Float64 MrMin2; // 1.33Mu
   Float64 Mu;

   const MomentCapacityDetails& MnDetails  = GetMomentCapacityDetails(pierID,stage,poi,bPositiveMoment);
   const CrackingMomentDetails& McrDetails = GetCrackingMomentDetails(pierID,stage,poi,bPositiveMoment);

   bool bAfter2002  = ( lrfdVersionMgr::SecondEditionWith2003Interims <= lrfdVersionMgr::GetVersion() ? true : false );
   bool bBefore2012 = ( lrfdVersionMgr::GetVersion() <  lrfdVersionMgr::SixthEdition2012 ? true : false );
   if ( bAfter2002 && bBefore2012 )
   {
      Mcr = (bPositiveMoment ? Max(McrDetails.Mcr,McrDetails.McrLimit) : Min(McrDetails.Mcr,McrDetails.McrLimit));
   }
   else
   {
      Mcr = McrDetails.Mcr;
   }

   Mr = MnDetails.phi * MnDetails.Mn;

   GET_IFACE(IXBRAnalysisResults,pAnalysisResults);

   Float64 MuMin, MuMax;
   pAnalysisResults->GetMoment(pierID,limitState,poi,&MuMin,&MuMax);
   Mu = (bPositiveMoment ? MuMax : MuMin);

   if ( lrfdVersionMgr::SixthEdition2012 <= lrfdVersionMgr::GetVersion() )
   {
      MrMin1 = Mcr;
   }
   else
   {
      MrMin1 = 1.20*Mcr;
   }

   MrMin2 = 1.33*Mu;

   MrMin = (bPositiveMoment ? Min(MrMin1,MrMin2) : Max(MrMin1,MrMin2));

   MinMomentCapacityDetails MminDetails;
   MminDetails.Mr     = Mr;
   MminDetails.Mcr    = Mcr;
   MminDetails.MrMin  = MrMin;
   MminDetails.MrMin1 = MrMin1;
   MminDetails.MrMin2 = MrMin2;
   MminDetails.Mu     = Mu;

   return MminDetails;
}

MinMomentCapacityDetails CEngAgentImp::ComputeMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx)
{
   Float64 Mr;     // Nominal resistance (phi*Mn)
   Float64 Mcr;    // Cracking moment
   Float64 MrMin;  // Minimum nominal resistance - Min(MrMin1,MrMin2)
   Float64 MrMin1; // 1.2Mcr
   Float64 MrMin2; // 1.33Mu
   Float64 Mu;

   const MomentCapacityDetails& MnDetails  = GetMomentCapacityDetails(pierID,stage,poi,bPositiveMoment);
   const CrackingMomentDetails& McrDetails = GetCrackingMomentDetails(pierID,stage,poi,bPositiveMoment);

   bool bAfter2002  = ( lrfdVersionMgr::SecondEditionWith2003Interims <= lrfdVersionMgr::GetVersion() ? true : false );
   bool bBefore2012 = ( lrfdVersionMgr::GetVersion() <  lrfdVersionMgr::SixthEdition2012 ? true : false );
   if ( bAfter2002 && bBefore2012 )
   {
      Mcr = (bPositiveMoment ? Max(McrDetails.Mcr,McrDetails.McrLimit) : Min(McrDetails.Mcr,McrDetails.McrLimit));
   }
   else
   {
      Mcr = McrDetails.Mcr;
   }

   Mr = MnDetails.phi * MnDetails.Mn;

   GET_IFACE(IXBRAnalysisResults,pAnalysisResults);

   Float64 Mdc = pAnalysisResults->GetMoment(pierID,xbrTypes::lcDC,poi);
   Float64 Mdw = pAnalysisResults->GetMoment(pierID,xbrTypes::lcDW,poi);
   Float64 Mcreep = pAnalysisResults->GetMoment(pierID,xbrTypes::lcCR,poi);
   Float64 Msh = pAnalysisResults->GetMoment(pierID,xbrTypes::lcSH,poi);
   Float64 Mre = pAnalysisResults->GetMoment(pierID,xbrTypes::lcRE,poi);
   Float64 Mps = pAnalysisResults->GetMoment(pierID,xbrTypes::lcPS,poi);

   pgsTypes::LoadRatingType ratingType = ::RatingTypeFromLimitState(limitState);

   Float64 Mpermit, Mlegal;
   pAnalysisResults->GetMoment(pierID,ratingType,vehicleIdx,llConfigIdx,permitLaneIdx,poi,&Mpermit,&Mlegal);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor(limitState);
   Float64 gDW = pProject->GetDWLoadFactor(limitState);
   Float64 gCR = pProject->GetCRLoadFactor(limitState);
   Float64 gSH = pProject->GetSHLoadFactor(limitState);
   Float64 gRE = pProject->GetRELoadFactor(limitState);
   Float64 gPS = pProject->GetPSLoadFactor(limitState);
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,limitState,vehicleIdx);

   Mu = gDC*Mdc + gDW*Mdw + gCR*Mcreep + gSH*Msh + gRE*Mre + gPS*Mps + gLL*(Mpermit + Mlegal);

   if ( lrfdVersionMgr::SixthEdition2012 <= lrfdVersionMgr::GetVersion() )
   {
      MrMin1 = Mcr;
   }
   else
   {
      MrMin1 = 1.20*Mcr;
   }

   MrMin2 = 1.33*Mu;

   MrMin = (bPositiveMoment ? Min(MrMin1,MrMin2) : Max(MrMin1,MrMin2));

   MinMomentCapacityDetails MminDetails;
   MminDetails.Mr     = Mr;
   MminDetails.Mcr    = Mcr;
   MminDetails.MrMin  = MrMin;
   MminDetails.MrMin1 = MrMin1;
   MminDetails.MrMin2 = MrMin2;
   MminDetails.Mu     = Mu;

   return MminDetails;
}

Float64 CEngAgentImp::GetDv(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( pProject->GetPierType(pierID) != xbrTypes::pctIntegral && stage == xbrTypes::Stage2 )
   {
      // there isn't stage 2 for non-integral cross beams
      return 0;
   }

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 h = pSectProps->GetDepth(pierID,stage,poi);
   MomentCapacityDetails posCapacityDetails = GetMomentCapacityDetails(pierID,stage,poi,true);
   MomentCapacityDetails negCapacityDetails = GetMomentCapacityDetails(pierID,stage,poi,false);
   Float64 posMomentArm = posCapacityDetails.de - posCapacityDetails.dc;
   Float64 posDv = Max(posMomentArm,0.9*posCapacityDetails.de,0.72*h);
   Float64 negMomentArm = negCapacityDetails.de - negCapacityDetails.dc;
   Float64 negDv = Max(negMomentArm,0.9*negCapacityDetails.de,0.72*h);
   Float64 dv = Min(posDv,negDv);
   return dv;
}

Float64 CEngAgentImp::GetAverageAvOverS(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,Float64 theta)
{
   GET_IFACE(IXBRProject,pProject);
   if ( pProject->GetPierType(pierID) != xbrTypes::pctIntegral && stage == xbrTypes::Stage2 )
   {
      // there isn't stage 2 for non-integral cross beams
      return 0;
   }

   GET_IFACE(IXBRPier,pPier);
   Float64 L = pPier->GetXBeamLength(pierID);

   // Get start/end of the shear failur plane at the poi
   Float64 dv = GetDv(pierID,stage,poi);
   Float64 sfpStart = Max(poi.GetDistFromStart() - dv/(2*tan(theta)),0.0);
   Float64 sfpEnd   = Min(poi.GetDistFromStart() + dv/(2*tan(theta)),L);

   Float64 Avg_Av_over_S = 0;
   GET_IFACE(IXBRStirrups,pStirrups);
   ZoneIndexType nZones = pStirrups->GetStirrupZoneCount(pierID,stage);
   for ( ZoneIndexType zoneIdx = 0; zoneIdx < nZones; zoneIdx++ )
   {
      Float64 szStart, szEnd;
      pStirrups->GetStirrupZoneBoundary(pierID,stage,zoneIdx,&szStart,&szEnd);

      if ( szEnd <= sfpStart )
      {
         // The shear failure plane starts after this zone ends
         // This zone does not contribute stirrups to average Av/S
         // Continue with the next zone
         continue;
      }

      if ( sfpEnd <= szStart )
      {
         // The shear failure plane ends before this zone starts
         // This zone, and all zones that come after it, does not contribute stirrups to average Av/S
         // Break here so we don't have to process the remaining zones
         break;
      }

      Float64 start = Max(szStart,sfpStart,0.0);
      Float64 end   = Min(szEnd,  sfpEnd,  L);
      Float64 Av_over_S = pStirrups->GetStirrupZoneReinforcement(pierID,stage,zoneIdx);

      Avg_Av_over_S += Av_over_S*(end-start);
   }

   // average Av/S over the length of the shear failure plane
   // (if we are near the ends, it can be less than dv)
   Float64 Lsfp = sfpEnd - sfpStart;
   ATLASSERT(::IsLE(Lsfp,dv/tan(theta)));
   Avg_Av_over_S /= Lsfp;

   return Avg_Av_over_S;
}

CEngAgentImp::RatingArtifacts& CEngAgentImp::GetPrivateRatingArtifacts(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx)
{
   std::map<PierIDType,RatingArtifacts>::iterator found = m_RatingArtifacts[ratingType].find(pierID);
   if ( found == m_RatingArtifacts[ratingType].end() )
   {
      RatingArtifacts artifacts;
      std::pair<std::map<PierIDType,RatingArtifacts>::iterator,bool> result = m_RatingArtifacts[ratingType].insert(std::make_pair(pierID,artifacts));
      ATLASSERT(result.second == true);
      found = result.first;
   }
   return found->second;
}

void CEngAgentImp::CreateRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx)
{
   GET_IFACE(IProgress, pProgress);
   CEAFAutoProgress ap(pProgress);
   pProgress->UpdateMessage(_T("Load rating cross beam"));

   RatingArtifacts& ratingArtifacts = GetPrivateRatingArtifacts(pierID,ratingType,vehicleIdx);
   xbrLoadRater loadRater(m_pBroker);
   xbrRatingArtifact artifact = loadRater.RateXBeam(pierID,ratingType,vehicleIdx);
   std::pair<std::map<VehicleIndexType,xbrRatingArtifact>::iterator,bool> result = ratingArtifacts.insert(std::make_pair(vehicleIdx,artifact));
   ATLASSERT(result.second == true);
}

