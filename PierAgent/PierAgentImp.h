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
   virtual Float64 GetSkewAngle(PierIDType pierID);
   virtual IndexType GetBearingLineCount(PierIDType pierID);
   virtual IndexType GetBearingCount(PierIDType pierID,IndexType brgLineIdx);
   virtual Float64 GetBearingLocation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx);
   virtual Float64 GetBearingElevation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx);
   virtual IndexType GetColumnCount(PierIDType pierID);
   virtual Float64 GetColumnLocation(PierIDType pierID,IndexType colIdx);
   virtual Float64 GetColumnHeight(PierIDType pierID,IndexType colIdx);
   virtual Float64 GetTopColumnElevation(PierIDType pierID,IndexType colIdx);
   virtual Float64 GetBottomColumnElevation(PierIDType pierID,IndexType colIdx);
   virtual pgsTypes::ColumnFixityType GetColumnFixity(PierIDType pierID,IndexType colIdx);
   virtual Float64 GetMaxColumnHeight(PierIDType pierID);
   virtual Float64 GetXBeamLength(PierIDType pierID);
   virtual void GetUpperXBeamProfile(PierIDType pierID,IShape** ppShape);
   virtual void GetLowerXBeamProfile(PierIDType pierID,IShape** ppShape);
   virtual void GetTopSurface(PierIDType pierID,xbrTypes::Stage stage,IPoint2dCollection** ppPoints);
   virtual void GetBottomSurface(PierIDType pierID,xbrTypes::Stage stage,IPoint2dCollection** ppPoints);

   virtual Float64 GetCrownPointOffset(PierIDType pierID);
   virtual Float64 GetCrownPointLocation(PierIDType pierID);

   virtual Float64 GetElevation(PierIDType pierID,Float64 Xcl);

   virtual Float64 GetCurbToCurbWidth(PierIDType pierID);

   virtual Float64 ConvertCrossBeamToCurbLineCoordinate(PierIDType pierID,Float64 Xxb);
   virtual Float64 ConvertCurbLineToCrossBeamCoordinate(PierIDType pierID,Float64 Xcl);
   virtual Float64 ConvertPierToCrossBeamCoordinate(PierIDType pierID,Float64 Xpier);
   virtual Float64 ConvertCrossBeamToPierCoordinate(PierIDType pierID,Float64 Xxb);
   virtual Float64 ConvertPierToCurbLineCoordinate(PierIDType pierID,Float64 Xpier);
   virtual Float64 ConvertCurbLineToPierCoordinate(PierIDType pierID,Float64 Xcl);

// IXBRSectionProperties
public:
   virtual Float64 GetDepth(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetArea(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetIxx(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetIyy(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetStop(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual Float64 GetSbot(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi);
   virtual void GetXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape);
   virtual void GetXBeamShape(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,IShape** ppShape);

// IXBRMaterial
public:
   virtual Float64 GetXBeamDensity(PierIDType pierID);
   virtual Float64 GetXBeamEc(PierIDType pierID);
   virtual Float64 GetXBeamModulusOfRupture(PierIDType pierID);
   virtual Float64 GetColumnEc(PierIDType pierID,IndexType colIdx);
   virtual void GetRebarProperties(PierIDType pierID,Float64* pE,Float64* pFy,Float64* pFu);

// IXBRRebar
public:
   virtual void GetRebarSection(PierIDType pierIdx,xbrTypes::Stage stage,const xbrPointOfInterest& poi,IRebarSection** ppRebarSection);
   virtual IndexType GetRebarRowCount(PierIDType pierID);
   virtual IndexType GetRebarCount(PierIDType pierID,IndexType rowIdx);
   virtual void GetRebarProfile(PierIDType pierID,IndexType rowIdx,IPoint2dCollection** ppPoints);
   virtual Float64 GetDevLengthFactor(PierIDType pierID,const xbrPointOfInterest& poi,IRebarSectionItem* pRebarSectionItem);
   virtual Float64 GetRebarRowLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx);
   virtual void GetRebarLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx,IndexType barIdx,IPoint2d** ppPoint);

// IXBRStirrups
public:
   virtual ZoneIndexType GetStirrupZoneCount(PierIDType pierID,xbrTypes::Stage stage);
   virtual void GetStirrupZoneBoundary(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx,Float64* pXstart,Float64* pXend);
   virtual Float64 GetStirrupZoneSpacing(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx);
   virtual Float64 GetStirrupZoneReinforcement(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx);
   virtual IndexType GetStirrupCount(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx);

// IXBRPointOfInterest
public:
   virtual std::vector<xbrPointOfInterest> GetXBeamPointsOfInterest(PierIDType pierID,PoiAttributeType attrib);
   virtual std::vector<xbrPointOfInterest> GetColumnPointsOfInterest(PierIDType pierID,ColumnIndexType colIdx);
   virtual Float64 ConvertPoiToPierCoordinate(PierIDType pierID,const xbrPointOfInterest& poi);
   virtual xbrPointOfInterest ConvertPierCoordinateToPoi(PierIDType pierID,Float64 Xp);
   virtual xbrPointOfInterest GetNearestPointOfInterest(PierIDType pierID,const xbrPointOfInterest& poi);
   virtual xbrPointOfInterest GetNextPointOfInterest(PierIDType pierID,PoiIDType poiID);
   virtual xbrPointOfInterest GetPrevPointOfInterest(PierIDType pierID,PoiIDType poiID);

// IXBRProjectEventSink
public:
   HRESULT OnProjectChanged();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;
   DWORD m_dwProjectCookie;

   std::map<PierIDType,CComPtr<IPier>> m_PierModels;
   void ValidatePierModel(PierIDType pierID);
   void GetPierModel(PierIDType pierID,IPier** ppPierModel);

   void Invalidate();

   PoiIDType m_NextPoiID;
   void ValidatePointsOfInterest(PierIDType pierID);
   std::vector<xbrPointOfInterest>& GetPointsOfInterest(PierIDType pierID);
   std::map<PierIDType,std::vector<xbrPointOfInterest>> m_XBeamPoi;
   bool FindXBeamPoi(PierIDType pierID,Float64 Xxb,xbrPointOfInterest* pPoi);
   void GetCrownPoint(PierIDType pierID,IPoint2d** ppPoint);

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
   std::map<PierIDType,std::vector<StirrupZone>> m_StirrupZones[2]; // use xbrTypes::Stage to access array
   std::vector<StirrupZone>& GetStirrupZones(PierIDType pierID,xbrTypes::Stage stage);
   void ValidateStirrupZones(PierIDType pierID,xbrTypes::Stage stage);
   void ValidateStirrupZones(PierIDType pierID,const xbrStirrupData& stirrupData,std::vector<StirrupZone>* pvStirrupZones);
};

OBJECT_ENTRY_AUTO(CLSID_PierAgent, CPierAgentImp)

