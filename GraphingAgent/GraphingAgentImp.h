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

// GraphingAgentImp.h : Declaration of the CGraphingAgentImp

// This agent is responsible for creating structural analysis models
// and providing analysis results

#pragma once

#include "resource.h"       // main symbols
#include <GraphingAgent.h>
#include "GraphingAgentCLSID.h"

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>

/////////////////////////////////////////////////////////////////////////////
// CGraphingAgentImp
class ATL_NO_VTABLE CGraphingAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CGraphingAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CGraphingAgentImp, &CLSID_GraphingAgent>,
	public IConnectionPointContainerImpl<CGraphingAgentImp>,
   //public CProxyIProjectEventSink<CGraphingAgentImp>,
   public IAgentEx,
   public IXBRProjectEventSink
{  
public:
	CGraphingAgentImp(); 
   virtual ~CGraphingAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_GRAPHINGAGENT)

BEGIN_COM_MAP(CGraphingAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CGraphingAgentImp)
//   CONNECTION_POINT_ENTRY( IID_IProjectEventSink )
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

// IXBRProjectEventSink
public:
   HRESULT OnProjectChanged();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   DWORD m_dwProjectCookie;

   HRESULT InitGraphBuilders();
};

OBJECT_ENTRY_AUTO(CLSID_GraphingAgent, CGraphingAgentImp)

