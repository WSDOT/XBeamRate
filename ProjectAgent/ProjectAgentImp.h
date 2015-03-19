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

using namespace XBR;

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
   public CProxyIProjectEventSink<CProjectAgentImp>,
   public IAgentEx,
   public IAgentUIIntegration,
   public IAgentPersist,
   public IEAFCommandCallback,
   public XBR::IProject
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
   COM_INTERFACE_ENTRY_IID(XBR::IID_IProject,XBR::IProject)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CProjectAgentImp)
   CONNECTION_POINT_ENTRY( IID_IProjectEventSink )
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

// IProject
public:
   virtual void SetProjectName(LPCTSTR strName);
   virtual LPCTSTR GetProjectName();

   virtual xbrTypes::TransverseDimensionMeasurementType GetTransverseDimensionsMeasurementType();
   virtual void SetTransverseDimensionsMeasurementType(xbrTypes::TransverseDimensionMeasurementType measurementType);
   virtual void SetDeckElevation(Float64 deckElevation);
   virtual Float64 GetDeckElevation();
   virtual void SetCrownPointOffset(Float64 cpo);
   virtual Float64 GetCrownPointOffset();
   virtual void SetBridgeLineOffset(Float64 blo);
   virtual Float64 GetBridgeLineOffset();

   virtual void SetOrientation(LPCTSTR strOrientation);
   virtual LPCTSTR GetOrientation();

   virtual IndexType GetBearingLineCount();
   virtual void SetBearingLineCount(IndexType nBearingLines);

   virtual IndexType GetBearingCount(IndexType brgLineIdx);
   virtual void SetBearingCount(IndexType brgLineIdx,IndexType nBearings);

   virtual Float64 GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx);
   virtual void SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 spacing);

   virtual void SetModE(Float64 Ec);
   virtual Float64 GetModE();

   virtual void SetXBeamDimensions(pgsTypes::PierSideType side,Float64 height,Float64 taperHeight,Float64 taperLength);
   virtual void GetXBeamDimensions(pgsTypes::PierSideType side,Float64* pHeight,Float64* pTaperHeight,Float64* pTaperLength);
   virtual void SetXBeamWidth(Float64 width);
   virtual Float64 GetXBeamWidth();
   virtual void SetXBeamOverhang(pgsTypes::PierSideType side,Float64 overhang);
   virtual void SetXBeamOverhangs(Float64 leftOverhang,Float64 rightOverhang);
   virtual Float64 GetXBeamOverhang(pgsTypes::PierSideType side);
   virtual void GetXBeamOverhangs(Float64* pLeftOverhang,Float64* pRightOverhang);

   virtual void SetColumns(IndexType nColumns,Float64 height,CColumnData::ColumnHeightMeasurementType measure,Float64 spacing);
   virtual IndexType GetColumnCount();
   virtual Float64 GetColumnHeight(IndexType colIdx);
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType();
   virtual xbrTypes::ColumnBaseType GetColumnBaseType(IndexType colIdx);
   virtual Float64 GetSpacing(IndexType spaceIdx);
   virtual void SetColumnShape(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2);
   virtual void GetColumnShape(CColumnData::ColumnShapeType* pShapeType,Float64* pD1,Float64* pD2);

   virtual void SetTransverseLocation(ColumnIndexType colIdx,Float64 offset,pgsTypes::OffsetMeasurementType measure);
   virtual void GetTransverseLocation(ColumnIndexType* pColIdx,Float64* pOffset,pgsTypes::OffsetMeasurementType* pMeasure);

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   CAgentCmdTarget m_CommandTarget;

   std::auto_ptr<XBeamRate> m_XBeamRateXML;

   friend CProxyIProjectEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();

   HRESULT ConvertToBaseUnits();
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

