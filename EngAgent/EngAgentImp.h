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

// EngAgentImp.h : Declaration of the CEngAgentImp

// This agent is responsible for creating structural analysis models
// and providing analysis results

#pragma once

#include "resource.h"       // main symbols
#include <EngAgent.h>
#include "EngAgentCLSID.h"

#include <WBFLRCCapacity.h>

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>

/////////////////////////////////////////////////////////////////////////////
// CEngAgentImp
class ATL_NO_VTABLE CEngAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CEngAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CEngAgentImp, &CLSID_EngAgent>,
   public IAgentEx,
   public IXBRLoadRating,
   public IXBRProjectEventSink
{  
public:
	CEngAgentImp(); 
   virtual ~CEngAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_ENGAGENT)

BEGIN_COM_MAP(CEngAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
	COM_INTERFACE_ENTRY(IXBRLoadRating)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
	//COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

//BEGIN_CONNECTION_POINT_MAP(CEngAgentImp)
//   CONNECTION_POINT_ENTRY( IID_IProjectEventSink )
//END_CONNECTION_POINT_MAP()

// IAgentEx
public:
	STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker);
   STDMETHOD(RegInterfaces)();
	STDMETHOD(Init)();
	STDMETHOD(Reset)();
	STDMETHOD(ShutDown)();
   STDMETHOD(Init2)();
   STDMETHOD(GetClassID)(CLSID* pCLSID);

// IXBRLoadRating
public:
   virtual Float64 GetMomentCapacity(const xbrPointOfInterest& poi,bool bPositiveMoment);
   virtual Float64 GetShearCapacity(const xbrPointOfInterest& poi);
   virtual Float64 GetRatingFactor();

// IXBRProjectEventSink
public:
   virtual HRESULT OnProjectChanged();

private:
   DECLARE_EAF_AGENT_DATA;

   DWORD m_dwProjectCookie;

   typedef struct MomentCapacityDetails
   {
      CComPtr<IRCBeam2> rcBeam;
      CComPtr<IRCSolutionEx> solution;
      Float64 dt; // distance from compression face to extreme tensile reinforcement
      Float64 phi; // capacity reduction factor
      Float64 Mn; // nominal capacity
      Float64 Mr; // nominal resistance (phi*Mn)
   } MomentCapacityDetails;

   std::map<IDType,MomentCapacityDetails> m_PositiveMomentCapacity;
   std::map<IDType,MomentCapacityDetails> m_NegativeMomentCapacity;

   MomentCapacityDetails GetMomentCapacityDetails(const xbrPointOfInterest& poi,bool bPositiveMoment);
   MomentCapacityDetails ComputeMomentCapacity(const xbrPointOfInterest& poi,bool bPositiveMoment);
};

OBJECT_ENTRY_AUTO(CLSID_EngAgent, CEngAgentImp)

