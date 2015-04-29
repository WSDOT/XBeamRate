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

// AnalysisAgentImp.h : Declaration of the CAnalysisAgentImp

// This agent is responsible for creating structural analysis models
// and providing analysis results

#pragma once

#include "resource.h"       // main symbols
#include <AnalysisAgent.h>
#include "AnalysisAgentCLSID.h"

#include <EAF\EAFInterfaceCache.h>

#include <IFace\Project.h>

#include <WBFLFem2d.h>

/////////////////////////////////////////////////////////////////////////////
// CAnalysisAgentImp
class ATL_NO_VTABLE CAnalysisAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CAnalysisAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CAnalysisAgentImp, &CLSID_AnalysisAgent>,
	//public IConnectionPointContainerImpl<CAnalysisAgentImp>, // needed if we implement a connection point
   //public CProxyIProjectEventSink<CAnalysisAgentImp>,// needed if we implement a connection point
   public IAgentEx,
   public IXBRProductForces,
   public IXBRAnalysisResults,
   public IXBRProjectEventSink
{  
public:
	CAnalysisAgentImp(); 
   virtual ~CAnalysisAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_ANALYSISAGENT)

BEGIN_COM_MAP(CAnalysisAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IXBRProductForces)
	COM_INTERFACE_ENTRY(IXBRAnalysisResults)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
	//COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)// needed if we implement a connection point
END_COM_MAP()

// needed if we implement a connection point
//BEGIN_CONNECTION_POINT_MAP(CAnalysisAgentImp)
   //CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
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

// IXBRProductForces
public:
   virtual const std::vector<LowerXBeamLoad>& GetLowerCrossBeamLoading();
   virtual Float64 GetUpperCrossBeamLoading();

// IXBRAnalysisResults
public:
   virtual Float64 GetMoment(XBRProductForceType pfType,const xbrPointOfInterest& poi);
   virtual sysSectionValue GetShear(XBRProductForceType pfType,const xbrPointOfInterest& poi);

// IXBRProjectEventSink
public:
   HRESULT OnProjectChanged();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   void Validate();
   void Invalidate();

   void ApplyDeadLoad();
   void ApplyLowerXBeamDeadLoad();
   void ApplyUpperXBeamDeadLoad();
   void ApplySuperstructureDeadLoadReactions();

   void ValidateLowerXBeamDeadLoad();

   void GetFemModelLocation(const xbrPointOfInterest& poi,MemberIDType* pMbrID,Float64* pMbrLocation);
   void GetCapBeamFemModelLocation(Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation);
   LoadCaseIDType GetLoadCaseID(XBRProductForceType pfType);

   DWORD m_dwProjectCookie;

   CComPtr<IFem2dModel> m_Model;

   struct CapBeamMember
   {
      Float64 Xs;
      Float64 Xe;
      MemberIDType mbrID;
   };
   std::vector<CapBeamMember> m_CapBeamMembers;

   std::vector<LowerXBeamLoad> m_LowerXBeamLoads;

   // key is the product model poi ID
   // value is the FEM model poi ID
   std::map<PoiIDType,PoiIDType> m_PoiMap;
};

OBJECT_ENTRY_AUTO(CLSID_AnalysisAgent, CAnalysisAgentImp)

