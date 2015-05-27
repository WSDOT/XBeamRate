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
#include <IFace\Pier.h>

#include <Units\SysUnits.h>

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

   pBrokerInit->RegInterface( IID_IXBRLoadRating,    this );

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
// IXBRLoadRating
Float64 CEngAgentImp::GetMomentCapacity(const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   MomentCapacityDetails capacityDetails = GetMomentCapacityDetails(poi,bPositiveMoment);
   return capacityDetails.Mr;
}

Float64 CEngAgentImp::GetShearCapacity(const xbrPointOfInterest& poi)
{
   // LRFD 5.8.3.4.1
   Float64 beta = 2.0;
   Float64 theta = M_PI/4; // 45 deg

   GET_IFACE(IXBRProject,pProject);

   // LRFD 5.8.2.9
   // dv = Distance between C and T, but not less than Max(0.9de and 0.72h)

   Float64 fc = pProject->GetFc();
   Float64 fy = 0;
   Float64 bv = pProject->GetXBeamWidth();;
   Float64 dv = 0;
   Float64 Av = 0;
   Float64 s = 1;
   Float64 fc_us = ::ConvertFromSysUnits(fc,unitMeasure::KSI);
   Float64 Vc_us = 0.0316*beta*sqrt(fc_us)*bv*dv;
   Float64 Vc = ::ConvertToSysUnits(Vc_us,unitMeasure::KSI);
   Float64 Vs = Av*fy*dv/(s*tan(theta));

   Float64 Vn1 = Vc + Vs;
   Float64 Vn2 = 0.25*fc*bv*dv;

   return Min(Vn1,Vn2);
}

Float64 CEngAgentImp::GetRatingFactor()
{
   return 1.2;
}

//////////////////////////////////////////////////
HRESULT CEngAgentImp::OnProjectChanged()
{
   m_PositiveMomentCapacity.clear();
   m_NegativeMomentCapacity.clear();
   return S_OK;
}

//////////////////////////////////////////////////
CEngAgentImp::MomentCapacityDetails CEngAgentImp::GetMomentCapacityDetails(const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   std::map<IDType,MomentCapacityDetails>* pCapacity = (bPositiveMoment ? &m_PositiveMomentCapacity : &m_NegativeMomentCapacity);
   std::map<IDType,MomentCapacityDetails>::iterator found(pCapacity->find(poi.GetID()));
   if ( found != pCapacity->end() )
   {
      return found->second;
   }

   MomentCapacityDetails capacityDetails = ComputeMomentCapacity(poi,bPositiveMoment);
   if ( poi.GetID() != INVALID_ID )
   {
      pCapacity->insert(std::make_pair(poi.GetID(),capacityDetails));
   }

   return capacityDetails;
}

CEngAgentImp::MomentCapacityDetails CEngAgentImp::ComputeMomentCapacity(const xbrPointOfInterest& poi,bool bPositiveMoment)
{
   CComPtr<IRCBeam2> rcBeam;
   HRESULT hr = rcBeam.CoCreateInstance(CLSID_RCBeam2);
   ATLASSERT(SUCCEEDED(hr));

   GET_IFACE(IXBRSectionProperties,pSectProps);
   Float64 h = pSectProps->GetDepth(xbrTypes::Stage2,poi);
   rcBeam->put_h(h);
   rcBeam->put_hf(0);

   GET_IFACE(IXBRProject,pProject);
   Float64 w = pProject->GetXBeamWidth();
   rcBeam->put_b(w);
   rcBeam->put_bw(w);

   Float64 fc = pProject->GetFc();
   rcBeam->put_FcSlab(fc);
   rcBeam->put_FcBeam(fc);

   Float64 dt = 0; // location of the extreme tension steel

   GET_IFACE(IXBRRebar,pRebar);
   IndexType nRows = pRebar->GetRebarRowCount();
   for ( IndexType rowIdx = 0; rowIdx < nRows; rowIdx++ )
   {
      Float64 Ybar = -pRebar->GetRebarRowLocation(poi.GetDistFromStart(),rowIdx);
      if ( !bPositiveMoment )
      {
         Ybar = h - Ybar;
      }

      dt = Max(dt,Ybar);

      xbrTypes::LongitudinalRebarDatumType datum;
      matRebar::Size barSize;
      Int16 nBars;
      Float64 cover;
      Float64 spacing;
      pProject->GetRebarRow(rowIdx,&datum,&cover,&barSize,&nBars,&spacing);

      lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();
      matRebar::Type barType = matRebar::A615;
      matRebar::Grade barGrade = matRebar::Grade60;
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

   MomentCapacityDetails capacityDetails;
   capacityDetails.rcBeam = rcBeam;
   capacityDetails.solution = solution;
   capacityDetails.dt = dt;
   capacityDetails.phi = phi;
   capacityDetails.Mn = Mn;
   capacityDetails.Mr = phi*Mn;

   return capacityDetails;
}
