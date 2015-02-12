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

// ProjectAgentImp.h : Declaration of the CProjectAgentImp

// This agent provides everything related to the input data including the data itself,
// the UI for manipulating the data, and persistence of the data.

#pragma once

#include "resource.h"       // main symbols
#include <ProjectAgent.h>
#include "CPProjectAgent.h"
#include "ProjectAgentCLSID.h"
#include "AgentCmdTarget.h"

#include <EAF\EAFInterfaceCache.h>
#include <EAF\EAFUIIntegration.h>

#include "XBeamRate.hxx" // XML/C++ Binding Object

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
class ATL_NO_VTABLE CProjectAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CProjectAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CProjectAgentImp, &CLSID_ProjectAgent>,
	public IConnectionPointContainerImpl<CProjectAgentImp>,
   public CProxyIXBRProjectEventSink<CProjectAgentImp>,
   public IAgentEx,
   public IAgentUIIntegration,
   public IAgentPersist,
   public IEAFCommandCallback,
   public IXBRProject
{  
public:
	CProjectAgentImp(); 
   virtual ~CProjectAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_PROJECTAGENT)

BEGIN_COM_MAP(CProjectAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IAgentUIIntegration)
	COM_INTERFACE_ENTRY(IAgentPersist)
	COM_INTERFACE_ENTRY(IXBRProject)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CProjectAgentImp)
   CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
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

// IAgentUIIntegration
public:
   STDMETHOD(IntegrateWithUI)(BOOL bIntegrate);

// IAgentPersist
public:
	STDMETHOD(Load)(/*[in]*/ IStructuredLoad* pStrLoad);
	STDMETHOD(Save)(/*[in]*/ IStructuredSave* pStrSave);

// IEAFCommandCallback
   // NOTE: this interface should be on CAgentCmdTarget, but it can't until
   // CAgentCmdTarget supports IUnknown. This is a COM interface
public:
   virtual BOOL OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
   virtual BOOL GetStatusBarMessageString(UINT nID, CString& rMessage) const;
   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const;

// IXBRProject
public:
   virtual void SetProjectName(LPCTSTR strName);
   virtual LPCTSTR GetProjectName();

   virtual void SetOverhangs(Float64 left,Float64 right);
   virtual Float64 GetLeftOverhang();
   virtual Float64 GetRightOverhang();
   virtual void SetColumns(IndexType nColumns,Float64 height,Float64 spacing);
   virtual IndexType GetColumnCount();
   virtual Float64 GetColumnHeight(IndexType colIdx);
   virtual xbrTypes::ColumnBaseType GetColumnBaseType(IndexType colIdx);
   virtual Float64 GetSpacing(IndexType spaceIdx);

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   CAgentCmdTarget m_CommandTarget;

   std::auto_ptr<XBeamRate> m_XBeamRateXML;

   friend CProxyIXBRProjectEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

