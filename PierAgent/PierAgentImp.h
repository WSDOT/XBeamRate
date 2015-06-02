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
   public IXBRRebar,
   public IXBRStirrups,
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
   COM_INTERFACE_ENTRY(IXBRRebar)
   COM_INTERFACE_ENTRY(IXBRStirrups)
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
   virtual IndexType GetColumnCount();
   virtual Float64 GetColumnLocation(IndexType colIdx);
   virtual Float64 GetColumnHeight(IndexType colIdx);
   virtual Float64 GetTopColumnElevation(IndexType colIdx);
   virtual Float64 GetBottomColumnElevation(IndexType colIdx);
   virtual Float64 GetMaxColumnHeight();
   virtual void GetUpperXBeamPoints(IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBC,IPoint2d** ppBR);
   virtual void GetLowerXBeamPoints(IPoint2d** ppTL,IPoint2d** ppTC,IPoint2d** ppTR,IPoint2d** ppBL,IPoint2d** ppBL2,IPoint2d** ppBR2,IPoint2d** ppBR);
   virtual void GetUpperXBeamProfile(IShape** ppShape);
   virtual void GetLowerXBeamProfile(IShape** ppShape);

   virtual Float64 GetElevation(Float64 distFromLeftEdge);

// IXBRSectionProperties
public:
   virtual Float64 GetDepth(xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetArea(xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetIxx(xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetIyy(xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual void GetUpperXBeamShape(const xbrPointOfInterest& poi,IShape** ppShape);
   virtual void GetLowerXBeamShape(const xbrPointOfInterest& poi,IShape** ppShape);

// IXBRMaterial
public:
   virtual Float64 GetXBeamDensity();
   virtual Float64 GetXBeamEc();
   virtual Float64 GetColumnEc(IndexType colIdx);

// IXBRRebar
public:
   virtual IndexType GetRebarRowCount();
   virtual IndexType GetRebarCount(IndexType rowIdx);
   virtual void GetRebarProfile(IndexType rowIdx,IPoint2dCollection** ppPoints);
   virtual Float64 GetRebarRowLocation(const xbrPointOfInterest& poi,IndexType rowIdx);
   virtual void GetRebarLocation(const xbrPointOfInterest& poi,IndexType rowIdx,IndexType barIdx,IPoint2d** ppPoint);

// IXBRStirrups
public:
   virtual IndexType GetStirrupZoneCount(xbrTypes::Stage stage);
   virtual void GetStirrupZoneBoundary(xbrTypes::Stage stage,ZoneIndexType zoneIdx,Float64* pXstart,Float64* pXend);
   virtual Float64 GetStirrupZoneSpacing(xbrTypes::Stage stage,ZoneIndexType zoneIdx);
   virtual IndexType GetStirrupCount(xbrTypes::Stage stage,ZoneIndexType zoneIdx);

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
   Float64 GetLeftEdgeLocation();
   Float64 GetCrownPointLocation();
   Float64 GetSkewAngle();

   typedef struct
   {
      Float64 Xstart;
      Float64 Xend;
      Float64 Length;
      matRebar::Size BarSize;
      Float64 S;
      Float64 Av_over_S;
      Float64 nLegs;
      IndexType nStirrups;
   } StirrupZone;
   std::vector<StirrupZone> m_StirrupZones[2]; // use xbrTypes::Stage to access array
   void ValidateStirrupZones();
   void ValidateStirrupZones(const xbrStirrupData& stirrupData,std::vector<StirrupZone>* pvStirrupZones);
   bool m_bStirrupZonesValid;
};

OBJECT_ENTRY_AUTO(CLSID_PierAgent, CPierAgentImp)

