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

// PierAgentImp.h : Declaration of the CPierAgentImp

// This agent is responsible for creating structural analysis models
// and providing analysis results

#pragma once

#include "resource.h"       // main symbols
#include <PierAgent.h>
#include "PierAgentCLSID.h"

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>
#include <IFace\Pier.h>
#include <IFace\PointOfInterest.h>

/////////////////////////////////////////////////////////////////////////////
// CPierAgentImp
class ATL_NO_VTABLE CPierAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CPierAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CPierAgentImp, &CLSID_PierAgent>,
	//public IConnectionPointContainerImpl<CPierAgentImp>,
   //public CProxyIProjectEventSink<CPierAgentImp>,
   public IAgentEx,
   public IXBRPier,
   public IXBRSectionProperties,
   public IXBRMaterial,
   public IXBRPointOfInterest,
   public IXBRProjectEventSink
{  
public:
	CPierAgentImp(); 
   virtual ~CPierAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_PIERAGENT)

BEGIN_COM_MAP(CPierAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IXBRPier)
   COM_INTERFACE_ENTRY(IXBRSectionProperties)
   COM_INTERFACE_ENTRY(IXBRMaterial)
   COM_INTERFACE_ENTRY(IXBRPointOfInterest)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
	//COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CPierAgentImp)
//   CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
END_CONNECTION_POINT_MAP()

// IAgentEx
public:
	STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker);
   STDMETHOD(RegInterfaces)();
	STDMETHOD(Init)();
	STDMETHOD(Reset)();
	STDMETHOD(ShutDown)();
   STDMETHOD(Init2)();
   STDMETHOD(GetClassID)(CLSID* pCLSID);

// IXBRPier
public:
   virtual IndexType GetBearingLineCount();
   virtual IndexType GetBearingCount(IndexType brgLineIdx);
   virtual Float64 GetBearingLocation(IndexType brgLineIdx,IndexType brgIdx);

// IXBRSectionProperties
public:
   virtual Float64 GetDepth(xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetArea(xbrTypes::Stage stage,const xbrPointOfInterest& poi);

// IXBRMaterial
public:
   virtual Float64 GetXBeamDensity();

// IXBRPointOfInterest
public:
   virtual std::vector<xbrPointOfInterest> GetXBeamPointsOfInterest(PoiAttributeType attrib);
   virtual std::vector<xbrPointOfInterest> GetColumnPointsOfInterest(ColumnIndexType colIdx);

// IXBRProjectEventSink
public:
   HRESULT OnProjectChanged();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;
   DWORD m_dwProjectCookie;

   void Validate();
   void Invalidate();
   CComPtr<ITransversePierDescription> m_Pier;

   void ValidatePointsOfInterest();
   std::vector<xbrPointOfInterest> m_XBeamPoi;

   Float64 GetLeftBearingOffset(IndexType brgLineIdx);
   Float64 GetLeftColumnOffset();
   Float64 GetCrownPointLocation();
   Float64 GetSkewAngle();
};

OBJECT_ENTRY_AUTO(CLSID_PierAgent, CPierAgentImp)

