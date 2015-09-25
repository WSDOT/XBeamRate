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

#include <\ARP\PGSuper\Include\IFace\Project.h>

#include <PgsExt\ThreadManager.h>

#define FIRST_LIVELOAD_ID 10000

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
   public IXBRProjectEventSink,
   public IBridgeDescriptionEventSink
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
   COM_INTERFACE_ENTRY(IBridgeDescriptionEventSink)
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
   virtual const std::vector<LowerXBeamLoad>& GetLowerCrossBeamLoading(PierIDType pierID);
   virtual Float64 GetUpperCrossBeamLoading(PierIDType pierID);

// IXBRAnalysisResults
public:
   virtual Float64 GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi);
   virtual sysSectionValue GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi);
   virtual std::vector<Float64> GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi);
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi);

   virtual Float64 GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi);
   virtual sysSectionValue GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi);
   virtual std::vector<Float64> GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi);
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,VehicleIndexType* pMinVehicleIdx,VehicleIndexType* pMaxVehicleIdx);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,VehicleIndexType* pMinLeftVehicleIdx,VehicleIndexType* pMinRightVehicleIdx,VehicleIndexType* pMaxLeftVehicleIdx,VehicleIndexType* pMaxRightVehicleIdx);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<VehicleIndexType>* pvMinVehicleIdx,std::vector<VehicleIndexType>* pvMaxVehicleIdx);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<VehicleIndexType>* pvMinLeftVehicleIdx,std::vector<VehicleIndexType>* pvMinRightVehicleIdx,std::vector<VehicleIndexType>* pvMaxLeftVehicleIdx,std::vector<VehicleIndexType>* pvMaxRightVehicleIdx);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax);
   virtual void GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax);
   virtual void GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax);

// IXBRProjectEventSink
public:
   HRESULT OnProjectChanged();

// IBridgeDescriptionEventSink
public:
   virtual HRESULT OnBridgeChanged(CBridgeChangedHint* pHint);
   virtual HRESULT OnGirderFamilyChanged();
   virtual HRESULT OnGirderChanged(const CGirderKey& girderKey,Uint32 lHint);
   virtual HRESULT OnLiveLoadChanged();
   virtual HRESULT OnLiveLoadNameChanged(LPCTSTR strOldName,LPCTSTR strNewName);
   virtual HRESULT OnConstructionLoadChanged();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;
   StatusCallbackIDType m_scidBridgeError;

   void Invalidate();

   LoadCaseIDType GetLoadCaseID(xbrTypes::ProductForceType pfType);
   std::vector<xbrTypes::ProductForceType> GetLoads(xbrTypes::CombinedForceType lcType);

   DWORD m_dwProjectCookie;
   DWORD m_dwBridgeDescCookie;

   struct BeamMember
   {
      Float64 Xs; // start of member location
      Float64 Xe; // end of member location
      MemberIDType mbrID;
   };

   struct LiveLoadConfiguration
   {
      bool operator<(const LiveLoadConfiguration& other)const { return m_LoadCaseID < other.m_LoadCaseID;} 

      LoadCaseIDType m_LoadCaseID; // FEM load case id for this live load configuration
      IndexType m_nLoadedLanes; // number of loaded lanes for this configuration
      Float64 m_Xoffset; // offset from left curb line to the left edge of the left-most loaded lane
      std::vector<std::pair<Float64,Float64>> m_Loading; // first = load, second = load position
   };

   typedef struct ModelData
   {
      CComPtr<IFem2dModel> m_Model;
      
      std::vector<BeamMember> m_XBeamMembers; // these members make up the cap beam
      std::vector<BeamMember> m_SuperstructureMembers; // these are the members where the live load is applied
      
      std::vector<LowerXBeamLoad> m_LowerXBeamLoads;

      // key is the product model poi ID
      // value is the FEM model poi ID
      std::map<PoiIDType,PoiIDType> m_PoiMap;

      LoadCaseIDType m_NextLiveLoadCaseID;
      std::set<LiveLoadConfiguration> m_LiveLoadConfigurations;
      ModelData() { m_NextLiveLoadCaseID = FIRST_LIVELOAD_ID; }
   } ModelData;
   std::auto_ptr<std::map<PierIDType,ModelData>> m_pModelData;
   ModelData* GetModelData(PierIDType pierID);
   void BuildModel(PierIDType pierID);
   void GetFemModelLocation(ModelData* pModelData,const xbrPointOfInterest& poi,MemberIDType* pMbrID,Float64* pMbrLocation);
   void GetXBeamFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation);
   void GetSuperstructureFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation);

   void ApplyDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplyLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplyUpperXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplySuperstructureDeadLoadReactions(PierIDType pierID,ModelData* pModelData);
   void ValidateLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);

   void ApplyUnitLiveLoad(PierIDType pierID,ModelData* pModelData);
   void GetLanePositions(IndexType nTotalSteps,IndexType nLaneGaps,std::vector<std::vector<IndexType>>& vGapPositions);
   std::vector<std::pair<Float64,Float64>> ConfigureWheelLineLoads(Float64 skew,Float64 stepSize,Float64 wLoadedLane,std::vector<IndexType>& vGapPosition);
   LoadCaseIDType ApplyWheelLineLoadsToFemModel(ModelData* pModelData,Float64 Xoffset,std::vector<std::pair<Float64,Float64>>& vLoading);


   struct UnitLiveLoadResult
   {
      bool operator<(const UnitLiveLoadResult& other)const {return m_idPOI < other.m_idPOI;}
      PoiIDType m_idPOI;

      // moment
      LoadCaseIDType m_lcidMzMin;
      LoadCaseIDType m_lcidMzMax;
      Float64 m_MzMin;
      Float64 m_MzMax;

      // shear
      LoadCaseIDType m_lcidFyLeftMin, m_lcidFyRightMin;
      LoadCaseIDType m_lcidFyLeftMax, m_lcidFyRightMax;
      sysSectionValue m_FyMin;
      sysSectionValue m_FyMax;
   };
   std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>> m_pUnitLiveLoadResults;
   std::set<UnitLiveLoadResult>& GetUnitLiveLoadResults(PierIDType pierID);
   void ComputeUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);
   UnitLiveLoadResult GetUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);


#if defined _USE_MULTITHREADING
   CThreadManager m_ThreadManager;
#endif

   void InvalidateModels();
   static UINT DeleteModels(LPVOID pParam);

   void InvalidateResults();
   static UINT DeleteResults(LPVOID pParam);

};

OBJECT_ENTRY_AUTO(CLSID_AnalysisAgent, CAnalysisAgentImp)

