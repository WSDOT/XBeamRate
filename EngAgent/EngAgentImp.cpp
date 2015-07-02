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
   MomentCapacityDetails capacityDetails = GetMomentCapacityDetails(pierID,stage,poi,bPositiveMoment);
   return capacityDetails.Mr;
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
const xbrRatingArtifact* CEngAgentImp::GetXBeamRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex)
{
#pragma Reminder("UPDATE: need rating artifacts for each pier")
   std::map<VehicleIndexType,xbrRatingArtifact>::iterator found = m_RatingArtifacts[ratingType].find(vehicleIndex);
   if ( found == m_RatingArtifacts[ratingType].end() )
   {
      CreateRatingArtifact(pierID,ratingType,vehicleIndex);
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
CEngAgentImp::MomentCapacityDetails CEngAgentImp::GetMomentCapacityDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   std::map<IDType,MomentCapacityDetails>* pCapacity = (bPositiveMoment ? &m_PositiveMomentCapacity[stage] : &m_NegativeMomentCapacity[stage]);
   std::map<IDType,MomentCapacityDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   MomentCapacityDetails capacityDetails = ComputeMomentCapacity(pierID,stage,poi,bPositiveMoment);
   if ( poi.GetID() != INVALID_ID )
   {
      std::pair<std::map<IDType,MomentCapacityDetails>::iterator,bool> result = pCapacity->insert(std::make_pair(poi.GetID(),capacityDetails));
      ATLASSERT(result.second == true);
   }

   return capacityDetails;
}

CEngAgentImp::MomentCapacityDetails CEngAgentImp::ComputeMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment)
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
   if ( connectionType != xbrTypes::pctIntegral )
   {
      Float64 d,w;
      pProject->GetDiaphragmDimensions(pierID,&d,&w);
      h += d;
   }

   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   rcBeam->put_FcSlab(concrete.Fc);
   rcBeam->put_FcBeam(concrete.Fc);

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

         if ( !bPositiveMoment )
         {
            Ybar = h - Ybar; // Ybar is measured from the bottom up for negative moment
         }

         dt = Max(dt,Ybar);

         CComPtr<IRebar> rebar;
         rebarSectionItem->get_Rebar(&rebar);

         Float64 As;
         rebar->get_NominalArea(&As);

#pragma Reminder("WORKING HERE - need to adjust bar area for development")
         Float64 devFactor = 1.0; // assuming fully developed
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

   // Still need phi-factor
   Float64 c;
   solution->get_NeutralAxisDepth(&c);

   matRebar::Type rebarType;
   matRebar::Grade rebarGrade;
   pProject->GetRebarMaterial(pierID,&rebarType,&rebarGrade);

   Float64 et = (dt - c)*0.003/c;
   Float64 ecl = lrfdRebar::GetCompressionControlledStrainLimit(rebarGrade);
   Float64 etl = lrfdRebar::GetTensionControlledStrainLimit(rebarGrade);
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

void CEngAgentImp::CreateRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIndex)
{
#pragma Reminder("UPDATE: need to have rating artifacts by pier")
   xbrLoadRater loadRater(m_pBroker);
   xbrRatingArtifact artifact = loadRater.RateXBeam(pierID,ratingType,vehicleIndex);
   std::pair<std::map<VehicleIndexType,xbrRatingArtifact>::iterator,bool> result = m_RatingArtifacts[ratingType].insert(std::make_pair(vehicleIndex,artifact));
   ATLASSERT(result.second == true);
}

