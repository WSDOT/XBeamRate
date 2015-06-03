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

   matRebar::Type type;
   matRebar::Grade grade;
   pProject->GetRebarMaterial(&type,&grade);
   Float64 fy = matRebar::GetYieldStrength(type,grade);


   Float64 dv1 = GetDv(xbrTypes::Stage1,poi);
   Float64 dv2 = GetDv(xbrTypes::Stage2,poi);
   Float64 Av_over_S1 = GetAverageAvOverS(xbrTypes::Stage1,poi,theta);
   Float64 Av_over_S2 = GetAverageAvOverS(xbrTypes::Stage2,poi,theta);

   // Also need to account for x-beam type (integral, continuous, expansion... only integral has upper diaphragm)
   Float64 fc = pProject->GetConcrete().Fc;
   Float64 bv = pProject->GetXBeamWidth();
   Float64 fc_us = ::ConvertFromSysUnits(fc,unitMeasure::KSI);
   Float64 Vc_us = 0.0316*beta*sqrt(fc_us)*bv*Max(dv1,dv2); // if non-integral, dv2 is zero so dv1 will be max, otherwise dv2 should be max
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

Float64 CEngAgentImp::GetDv(xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   GET_IFACE(IXBRProject,pProject);
   if ( pProject->GetPierType() != xbrTypes::pctIntegral && stage == xbrTypes::Stage2 )
   {
      // there isn't stage 2 for non-integral cross beams
      return 0;
   }

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 h = pSectProps->GetDepth(stage,poi);
   MomentCapacityDetails posCapacityDetails = GetMomentCapacityDetails(stage,poi,true);
   MomentCapacityDetails negCapacityDetails = GetMomentCapacityDetails(stage,poi,false);
   Float64 posMomentArm = posCapacityDetails.de - posCapacityDetails.dc;
   Float64 posDv = Max(posMomentArm,0.9*posCapacityDetails.de,0.72*h);
   Float64 negMomentArm = negCapacityDetails.de - negCapacityDetails.dc;
   Float64 negDv = Max(negMomentArm,0.9*negCapacityDetails.de,0.72*h);
   Float64 dv = Min(posDv,negDv);
   return dv;
}

Float64 CEngAgentImp::GetAverageAvOverS(xbrTypes::Stage stage,const xbrPointOfInterest& poi,Float64 theta)
{
   GET_IFACE(IXBRProject,pProject);
   if ( pProject->GetPierType() != xbrTypes::pctIntegral && stage == xbrTypes::Stage2 )
   {
      // there isn't stage 2 for non-integral cross beams
      return 0;
   }

   Float64 L = pProject->GetXBeamLength();

   // Get start/end of the shear failur plane at the poi
   Float64 dv = GetDv(stage,poi);
   Float64 sfpStart = poi.GetDistFromStart() - dv/tan(theta);
   Float64 sfpEnd   = poi.GetDistFromStart() + dv/tan(theta);

   Float64 Avg_Av_over_S = 0;
   GET_IFACE(IXBRStirrups,pStirrups);
   ZoneIndexType nZones = pStirrups->GetStirrupZoneCount(stage);
   for ( ZoneIndexType zoneIdx = 0; zoneIdx < nZones; zoneIdx++ )
   {
      Float64 szStart, szEnd;
      pStirrups->GetStirrupZoneBoundary(stage,zoneIdx,&szStart,&szEnd);

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
      Float64 Av_over_S = pStirrups->GetStirrupZoneReinforcement(stage,zoneIdx);

      Avg_Av_over_S += Av_over_S*(end-start);
   }

   Avg_Av_over_S /= (dv/tan(theta));

   return Avg_Av_over_S;
}

void CEngAgentImp::CreateRatingArtifact(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex)
{
   xbrLoadRater loadRater(m_pBroker);
   xbrRatingArtifact artifact = loadRater.RateXBeam(ratingType,vehicleIndex);
   std::pair<std::map<VehicleIndexType,xbrRatingArtifact>::iterator,bool> result = m_RatingArtifacts[ratingType].insert(std::make_pair(vehicleIndex,artifact));
   ATLASSERT(result.second == true);
}

