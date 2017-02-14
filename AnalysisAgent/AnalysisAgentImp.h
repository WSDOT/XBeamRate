///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2017  Washington State Department of Transportation
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

#if defined _USE_MULTITHREADING
#include <PgsExt\ThreadManager.h>
#endif

#define FIRST_LIVELOAD_ID 10000

#define MODEL_INIT_NONE    -1
#define MODEL_INIT_TOPOLOGY 0
#define MODEL_INIT_LOADS    1
#define MODEL_INIT_ALL      MODEL_INIT_LOADS

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
   virtual std::vector<Float64> GetWheelLineLocations(PierIDType pierID);
   virtual WheelLineConfiguration GetLiveLoadConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType liveLoadConfigIdx,IndexType permitLaneIdx);
   virtual void GetGoverningMomentLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvMin,std::vector<IndexType>* pvMax);
   virtual void GetGoverningShearLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvLLConfig);

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
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,sysSectionValue* pVpermit,sysSectionValue* pVlegal);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvVpermit,std::vector<sysSectionValue>* pvVlegal);

   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration*/);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*,WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration*/);
   virtual void GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration*/);
   virtual void GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration*/);

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

   void Invalidate(bool bCreateNewDataStructures = true);

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
      // locations are measured from the left curb line
      Float64 Xleft;  // location of left wheel line
      Float64 Xright; // location of right wheel line
   };

   struct LiveLoadConfiguration
   {
      // a live load configuration consists of one or more live load cases
      // each live load case models a single lane (two wheel lines)

      std::vector<LoadCaseIDType> m_LoadCases; // FEM load case IDs that make up the loads for this configuration.
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

      LoadCaseIDType m_NextLiveLoadCaseID; // ID of the next live load case

      // this vector contains all of the live load configurations
      // each configuration represents the same number of loaded lanes,
      // the position of the loaded lanes with respect to one another, 
      // and the position of the loaded lanes transversely on the cross beam
      std::vector<LiveLoadConfiguration> m_LiveLoadConfigurations;

      IndexType m_LastSingleLaneLLConfigIdx; // Index of the last live load configuration where a single lane is loaded
                                             // This is needed because for MBE permit ratings, only the one loaded lane case is needed
                                             // This is the index of the last entry in m_LiveLoadConfigruations that is for a single loaded lane case

      // this map keeps track of the live load cases
      // the key is the location of the center of the lane
      std::map<Float64,LoadCaseIDType,Float64_less> m_LiveLoadCases;

      // this map keeps track of the lane configurations corresponding
      // to each live load case.
      std::map<LoadCaseIDType,LaneConfiguration> m_LaneConfigurations;

      int m_InitLevel;

      ModelData() { m_InitLevel = MODEL_INIT_NONE; 
                    m_NextLiveLoadCaseID = FIRST_LIVELOAD_ID; }
   } ModelData;
   std::auto_ptr<std::map<PierIDType,ModelData>> m_pModelData;
   ModelData* GetModelData(PierIDType pierID,int level = MODEL_INIT_ALL);
   void BuildModel(PierIDType pierID,int level);
   void GetFemModelLocation(ModelData* pModelData,const xbrPointOfInterest& poi,MemberIDType* pMbrID,Float64* pMbrLocation);
   void GetXBeamFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation);
   void GetSuperstructureFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation);

   void ApplyDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplyLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplyUpperXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);
   void ApplySuperstructureDeadLoadReactions(PierIDType pierID,ModelData* pModelData);
   void ValidateLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData);

   void ComputeLiveLoadLocations(PierIDType pierID,ModelData* pModelData);
   void GetLaneGapConfiguration(IndexType nTotalSteps,IndexType nLaneGaps,std::vector<std::vector<IndexType>>& vGapPositions);
   std::vector<Float64> GetWheelLinePositions(Float64 skew,Float64 stepSize,Float64 wLoadedLane,std::vector<IndexType>& vGapPosition);
   std::vector<LoadCaseIDType> InitializeWheelLineLoads(ModelData* pModelData,Float64 Xoffset,std::vector<Float64>& vLoadPositions);
   void ApplyWheelLineLoadsToFemModel(ModelData* pModelData);


   typedef struct UnitLiveLoadResult
   {
      // unit load results are moment/shear for a unit lane reaction in each loaded lane
      // multplied by the multiple presence factor.
      // to get actual moment/shear for a lane reaction case, simply scale the stored values
      // by a single lane reaction.
      bool operator<(const UnitLiveLoadResult& other)const {return m_idPOI < other.m_idPOI;}
      PoiIDType m_idPOI;

      // controlling moment for one loaded lane
      IndexType m_llConfigIdx_MzMin_SingleLane; // live load configuration for the controlling min moment
      IndexType m_llConfigIdx_MzMax_SingleLane; // live load configuration for the controlling max moment
      Float64 m_MzMin_SingleLane;
      Float64 m_MzMax_SingleLane;

      // controlling moment for multiple loaded lanes
      IndexType m_llConfigIdx_MzMin; // live load configuration for the controlling min moment
      IndexType m_llConfigIdx_MzMax; // live load configuration for the controlling max moment
      Float64 m_MzMin;
      Float64 m_MzMax;

      // Live load configuration index for the configurations that cause the N max/min moments
      // Used only when permit rating factors are computed by the WSDOT method.
      // WSDOT permit rating factor method requires that multiple combinations
      // of permit load in one lane and legal load in all remaining lanes be
      // evaluated. In theory, all loading conditions should be evaluated, however
      // this is considerably time-consuming. Instead, the rating factor
      // will be computed based the N load cases that cause the max/min
      // moments at this section.
      std::vector<IndexType> m_MzMaxLiveLoadConfigs;
      std::vector<IndexType> m_MzMinLiveLoadConfigs;

      // controlling shear for one loaded lane
      IndexType m_llConfigIdx_FyLeftMin_SingleLane, m_llConfigIdx_FyRightMin_SingleLane; // live load configuration for the controlling min shear
      IndexType m_llConfigIdx_FyLeftMax_SingleLane, m_llConfigIdx_FyRightMax_SingleLane; // live load configuration for the controlling max shear
      sysSectionValue m_FyMin_SingleLane;
      sysSectionValue m_FyMax_SingleLane;

      // controlling shear for multiple loaded lanes
      IndexType m_llConfigIdx_FyLeftMin, m_llConfigIdx_FyRightMin; // live load configuration for the controlling min shear
      IndexType m_llConfigIdx_FyLeftMax, m_llConfigIdx_FyRightMax; // live load configuration for the controlling max shear
      sysSectionValue m_FyMin;
      sysSectionValue m_FyMax;

      // Live load configuration index for the configurations that cause the N max shears
      // Max shear is based on the magnitude of the shear.
      // Used only when permit rating factors are computed by the WSDOT method.
      // WSDOT permit rating factor method requires that multiple combinations
      // of permit load in one lane and legal load in all remaining lanes be
      // evaluated. In theory, all loading conditions should be evaluated, however
      // this is considerably time-consuming. Instead, the rating factor
      // will be computed based the N load cases that cause the max
      // shears at this section.
      std::vector<IndexType> m_FyLiveLoadConfigs;
   } UnitLiveLoadResult;
   std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>> m_pUnitLiveLoadResults;
   std::set<UnitLiveLoadResult>& GetUnitLiveLoadResults(PierIDType pierID);
   void ComputeUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);
   UnitLiveLoadResult& GetUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi);

   Float64 GetMaxLegalReaction(PierIDType pierID);
   std::vector<Float64> GetWheelLineLocations(ModelData* pModelData);


#if defined _USE_MULTITHREADING
   CThreadManager m_ThreadManager;
#endif

   void InvalidateModels(bool bCreateNewDataStructures = true);
   static UINT DeleteModels(LPVOID pParam);

   void InvalidateResults(bool bCreateNewDataStructures = true);
   static UINT DeleteResults(LPVOID pParam);

};

OBJECT_ENTRY_AUTO(CLSID_AnalysisAgent, CAnalysisAgentImp)

