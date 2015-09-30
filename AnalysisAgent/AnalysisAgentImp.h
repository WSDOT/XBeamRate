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

   virtual IndexType GetLiveLoadConfigurationCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType);
   virtual IndexType GetLoadedLaneCount(PierIDType pierID,IndexType liveLoadConfigIdx);
   virtual WheelLineConfiguration GetLiveLoadConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType liveLoadConfigIdx,IndexType permitLaneIdx);

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

   virtual Float64 GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi);
   virtual sysSectionValue GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi);
   virtual std::vector<Float64> GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi);
   virtual std::vector<sysSectionValue> GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,Float64* pMpermit,Float64* pMlegal);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMpermit,std::vector<Float64>* pvMlegal);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,Float64* pMpermit,Float64* pMlegal);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMpermit,std::vector<Float64>* pvMlegal);

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

   struct LaneConfiguration
   {
      Float64 Xleft; // location of left wheel line
      Float64 Xright; // location of right wheel line
      LoadCaseIDType m_SingleLaneLoadCaseID; // load case ID when this lane configuration is used for the single loaded lane case
   };

   struct LiveLoadConfiguration
   {
      bool operator<(const LiveLoadConfiguration& other)const { return m_LoadCaseID < other.m_LoadCaseID;} 

      LoadCaseIDType m_LoadCaseID; // FEM load case id for this live load configuration
      IndexType m_nLoadedLanes; // number of loaded lanes for this configuration
      
      std::vector<LaneConfiguration> m_LaneConfiguration; // each element in the vector is the configuration for a loaded lane
      // e.g. at index 1 in the vector is the configuration for loaded lane 1 (zero-based index, of course)
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

      LoadCaseIDType m_LastSingleLaneLoadCaseID; // Load case ID of the last load case where a single lane is loaded
      LoadCaseIDType m_NextLiveLoadCaseID;
      std::set<LiveLoadConfiguration> m_LiveLoadConfigurations;

      std::map<IndexType,LoadCaseIDType> m_SingleLaneLoadCaseIDs;
      // index is the step index that positions the single lane
      // load case ID is the FEM load case for a single load in that position

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
   std::vector<Float64> ConfigureWheelLineLoads(Float64 skew,Float64 stepSize,Float64 wLoadedLane,std::vector<IndexType>& vGapPosition);
   LoadCaseIDType ApplyWheelLineLoadsToFemModel(ModelData* pModelData,Float64 Xoffset,std::vector<Float64>& vLoadPositions);


   struct UnitLiveLoadResult
   {
      // unit load results are moment/shear for a unit lane reaction in each loaded lane
      // multplied by the multiple presence factor.
      // to get actual moment/shear for a lane reaction case, simply scale the stored values
      // by a single lane reaction.
      bool operator<(const UnitLiveLoadResult& other)const {return m_idPOI < other.m_idPOI;}
      PoiIDType m_idPOI;

      // controlling moment for one loaded lane
      LoadCaseIDType m_lcidMzMin_SingleLane; // controlling FEM load case for min moment
      LoadCaseIDType m_lcidMzMax_SingleLane; // controlling FEM load case for max moment
      Float64 m_MzMin_SingleLane;
      Float64 m_MzMax_SingleLane;

      // controlling moment for multiple loaded lanes
      LoadCaseIDType m_lcidMzMin; // controlling FEM load case for min moment
      LoadCaseIDType m_lcidMzMax; // controlling FEM load case for max moment
      Float64 m_MzMin;
      Float64 m_MzMax;

      // controlling shear for one loaded lane
      LoadCaseIDType m_lcidFyLeftMin_SingleLane, m_lcidFyRightMin_SingleLane; // controlling FEM load cases for min shear
      LoadCaseIDType m_lcidFyLeftMax_SingleLane, m_lcidFyRightMax_SingleLane; // controlling FEM load cases for max shear
      sysSectionValue m_FyMin_SingleLane;
      sysSectionValue m_FyMax_SingleLane;

      // controlling shear for multiple loaded lanes
      LoadCaseIDType m_lcidFyLeftMin, m_lcidFyRightMin; // controlling FEM load cases for min shear
      LoadCaseIDType m_lcidFyLeftMax, m_lcidFyRightMax; // controlling FEM load cases for max shear
      sysSectionValue m_FyMin;
      sysSectionValue m_FyMax;
   };
   std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>> m_pUnitLiveLoadResults;
   std::set<UnitLiveLoadResult>& GetUnitLiveLoadResults(PierIDType pierID);
   void ComputeUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);
   UnitLiveLoadResult GetUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);

   WheelLineConfiguration GetWheelLineConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const LiveLoadConfiguration& llConfig,IndexType permitLaneIdx);

   Float64 GetMaxLegalReaction(PierIDType pierID);
   LiveLoadConfiguration& GetLiveLoadConfiguration(ModelData* pModelData,LoadCaseIDType lcid);


#if defined _USE_MULTITHREADING
   CThreadManager m_ThreadManager;
#endif

   void InvalidateModels();
   static UINT DeleteModels(LPVOID pParam);

   void InvalidateResults();
   static UINT DeleteResults(LPVOID pParam);

};

OBJECT_ENTRY_AUTO(CLSID_AnalysisAgent, CAnalysisAgentImp)

