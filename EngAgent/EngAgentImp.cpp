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

// EngAgentImp.cpp : Implementation of CEngAgentImp
#include "stdafx.h"
#include "EngAgent.h"
#include "EngAgentImp.h"

#include <XBeamRateExt\PointOfInterest.h>
#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>
#include <IFace\Pier.h>

#include <Units\SysUnits.h>

#include "LoadRater.h"

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
Float64 CEngAgentImp::GetMomentCapacity(xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   MomentCapacityDetails capacityDetails = GetMomentCapacityDetails(stage,poi,bPositiveMoment);
   return capacityDetails.Mr;
}

//////////////////////////////////////////////////////////////////////
// IXBRShearCapacity
Float64 CEngAgentImp::GetShearCapacity(const xbrPointOfInterest& poi)
{
   // LRFD 5.8.3.4.1
   Float64 beta = 2.0;
   Float64 theta = M_PI/4; // 45 deg

   GET_IFACE(IXBRProject,pProject);
   GET_IFACE(IXBRSectionProperties,pSectProps);

   // LRFD 5.8.2.9
   // dv = Distance between C and T, but not less than Max(0.9de and 0.72h)
   Float64 dv1 = 0;
   Float64 dv2 = 0;
   Float64 Av_over_S1 = 0;
   Float64 Av_over_S2 = 0;

   Float64 h1 = pSectProps->GetDepth(xbrTypes::Stage1,poi);
   MomentCapacityDetails posCapacityDetails1 = GetMomentCapacityDetails(xbrTypes::Stage1,poi,true);
   MomentCapacityDetails negCapacityDetails1 = GetMomentCapacityDetails(xbrTypes::Stage1,poi,false);
   Float64 posMomentArm1 = posCapacityDetails1.de - posCapacityDetails1.dc;
   Float64 posDv1 = Max(posMomentArm1,0.9*posCapacityDetails1.de,0.72*h1);
   Float64 negMomentArm1 = negCapacityDetails1.de - negCapacityDetails1.dc;
   Float64 negDv1 = Max(negMomentArm1,0.9*negCapacityDetails1.de,0.72*h1);
   dv1 = Min(posDv1,negDv1);
   // Av_over_S1 = ?; // need to do the computation (need to do the averaging over the 45deg crack width)

   if ( pProject->GetPierType() == xbrTypes::pctIntegral )
   {
      Float64 h2 = pSectProps->GetDepth(xbrTypes::Stage2,poi);
      MomentCapacityDetails posCapacityDetails2 = GetMomentCapacityDetails(xbrTypes::Stage2,poi,true);
      MomentCapacityDetails negCapacityDetails2 = GetMomentCapacityDetails(xbrTypes::Stage2,poi,false);
      Float64 posMomentArm2 = posCapacityDetails2.de - posCapacityDetails2.dc;
      Float64 posDv2 = Max(posMomentArm2,0.9*posCapacityDetails2.de,0.72*h2);
      Float64 negMomentArm2 = negCapacityDetails2.de - negCapacityDetails2.dc;
      Float64 negDv2 = Max(negMomentArm2,0.9*negCapacityDetails2.de,0.72*h2);
      dv2 = Min(posDv2,negDv2);
      // Av_over_S = ?; // need to do the computation (need to do the averaging over the 45deg crack width)
   }

#pragma Reminder("WORKING HERE: Need to finish shear capacity- need stirrup information")
   // Also need to account for x-beam type (integral, continuous, expansion... only integral has upper diaphragm)
   Float64 fc = pProject->GetConcrete().Fc;
   Float64 fy = 0;
   Float64 bv = pProject->GetXBeamWidth();
   Float64 fc_us = ::ConvertFromSysUnits(fc,unitMeasure::KSI);
   Float64 Vc_us = 0.0316*beta*sqrt(fc_us)*bv*Max(dv1,dv2);
   Float64 Vc = ::ConvertToSysUnits(Vc_us,unitMeasure::KSI);
   Float64 Vs1 = Av_over_S1*fy*dv1/(tan(theta)); // lower x-beam reinforcement capacity
   Float64 Vs2 = Av_over_S2*fy*dv2/(tan(theta)); // full x-beam reinforcement capacity
   Float64 Vs = Vs1 + Vs2; // total capacity due to reinforcement

   Float64 Vn1 = Vc + Vs;
   Float64 Vn2 = 0.25*fc*bv*dv2;

   return Min(Vn1,Vn2);
}

//////////////////////////////////////////////////////////////////////
// IXBRArtifactCapacity
const xbrRatingArtifact* CEngAgentImp::GetXBeamRatingArtifact(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex)
{
   std::map<VehicleIndexType,xbrRatingArtifact>::iterator found = m_RatingArtifacts[ratingType].find(vehicleIndex);
   if ( found == m_RatingArtifacts[ratingType].end() )
   {
      CreateRatingArtifact(ratingType,vehicleIndex);
      found = m_RatingArtifacts[ratingType].find(vehicleIndex);
   }

   if ( found == m_RatingArtifacts[ratingType].end() )
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
   m_PositiveMomentCapacity[xbrTypes::Stage1].clear();
   m_PositiveMomentCapacity[xbrTypes::Stage2].clear();

   m_NegativeMomentCapacity[xbrTypes::Stage1].clear();
   m_NegativeMomentCapacity[xbrTypes::Stage2].clear();
   return S_OK;
}

//////////////////////////////////////////////////
CEngAgentImp::MomentCapacityDetails CEngAgentImp::GetMomentCapacityDetails(xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   std::map<IDType,MomentCapacityDetails>* pCapacity = (bPositiveMoment ? &m_PositiveMomentCapacity[stage] : &m_NegativeMomentCapacity[stage]);
   std::map<IDType,MomentCapacityDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   MomentCapacityDetails capacityDetails = ComputeMomentCapacity(stage,poi,bPositiveMoment);
   if ( poi.GetID() != INVALID_ID )
   {
      std::pair<std::map<IDType,MomentCapacityDetails>::iterator,bool> result = pCapacity->insert(std::make_pair(poi.GetID(),capacityDetails));
      ATLASSERT(result.second == true);
   }

   return capacityDetails;
}

CEngAgentImp::MomentCapacityDetails CEngAgentImp::ComputeMomentCapacity(xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   CComPtr<IRCBeam2> rcBeam;
   HRESULT hr = rcBeam.CoCreateInstance(CLSID_RCBeam2);
   ATLASSERT(SUCCEEDED(hr));

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 h = pSectProps->GetDepth(stage,poi);
   rcBeam->put_h(h);
   rcBeam->put_hf(0);

   GET_IFACE(IXBRProject,pProject);
   Float64 w = pProject->GetXBeamWidth();
   rcBeam->put_b(w);
   rcBeam->put_bw(w);

   xbrTypes::SuperstructureConnectionType connectionType = pProject->GetPierType();

   const xbrConcreteMaterial& concrete = pProject->GetConcrete();
   rcBeam->put_FcSlab(concrete.Fc);
   rcBeam->put_FcBeam(concrete.Fc);

   Float64 dt = 0; // location of the extreme tension steel

   GET_IFACE(IXBRRebar,pRebar);
   IndexType nRows = pRebar->GetRebarRowCount();
   for ( IndexType rowIdx = 0; rowIdx < nRows; rowIdx++ )
   {
      xbrTypes::LongitudinalRebarDatumType datum;
      matRebar::Size barSize;
      IndexType nBars;
      Float64 cover;
      Float64 spacing;
      pProject->GetRebarRow(rowIdx,&datum,&cover,&barSize,&nBars,&spacing);

      if ( datum == xbrTypes::Top && (connectionType == xbrTypes::pctIntegral && stage == xbrTypes::Stage1) )
      {
         // top rebar in integral piers is not included in stage 1 capacity
         continue;
      }

      Float64 Ybar = pRebar->GetRebarRowLocation(poi,rowIdx);
      if ( !bPositiveMoment )
      {
         Ybar = h - Ybar;
      }

      dt = Max(dt,Ybar);


      lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();
      matRebar::Type barType;
      matRebar::Grade barGrade;
      pProject->GetRebarMaterial(&barType,&barGrade);
      const matRebar* pRebar = pRebarPool->GetRebar(barType,barGrade,barSize);

      Float64 as = pRebar->GetNominalArea();
      Float64 As = nBars*as;

      Float64 devFactor = 1.0; // assuming fully developed
      rcBeam->AddRebarLayer(Ybar,As,devFactor);
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

   // Still need phi-factor
   Float64 c;
   solution->get_NeutralAxisDepth(&c);

   Float64 et = (dt - c)*0.003/c;
   Float64 ecl = 0.002;
   Float64 etl = 0.005;
   Float64 phi = 0.75 + 0.15*(et - ecl)/(etl-ecl);
   phi = ::ForceIntoRange(0.75,phi,0.9);

   Float64 Cweb, Cflange, Yweb, Yflange;
   solution->get_Cweb(&Cweb);
   solution->get_Cflange(&Cflange);
   solution->get_Yweb(&Yweb);
   solution->get_Yflange(&Yflange);
   Float64 dc = (Cweb*Yweb + Cflange*Yflange)/(Cweb + Cflange);

   Float64 T;
   solution->get_T(&T);
   Float64 MomentArm = fabs(Mn/T);
   Float64 de = dc + MomentArm;

   MomentCapacityDetails capacityDetails;
   capacityDetails.rcBeam = rcBeam;
   capacityDetails.solution = solution;
   capacityDetails.dc = dc;
   capacityDetails.de = de;
   capacityDetails.dt = dt;
   capacityDetails.phi = phi;
   capacityDetails.Mn = Mn;
   capacityDetails.Mr = phi*Mn;

   return capacityDetails;
}

void CEngAgentImp::CreateRatingArtifact(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex)
{
   xbrLoadRater loadRater(m_pBroker);
   xbrRatingArtifact artifact = loadRater.RateXBeam(ratingType,vehicleIndex);
   std::pair<std::map<VehicleIndexType,xbrRatingArtifact>::iterator,bool> result = m_RatingArtifacts[ratingType].insert(std::make_pair(vehicleIndex,artifact));
   ATLASSERT(result.second == true);
}
