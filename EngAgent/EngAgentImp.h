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

#include <EAF\EAFInterfaceCache.h>

/////////////////////////////////////////////////////////////////////////////
// CEngAgentImp
class ATL_NO_VTABLE CEngAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CEngAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CEngAgentImp, &CLSID_EngAgent>,
	public IConnectionPointContainerImpl<CEngAgentImp>,
   //public CProxyIXBRProjectEventSink<CEngAgentImp>,
   public IAgentEx,
   public ILoadRating
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
	COM_INTERFACE_ENTRY(ILoadRating)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CEngAgentImp)
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

// ILoadRating
public:
   virtual Float64 GetRatingFactor();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;
};

OBJECT_ENTRY_AUTO(CLSID_EngAgent, CEngAgentImp)

