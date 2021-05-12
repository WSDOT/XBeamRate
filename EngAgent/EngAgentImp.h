///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2021  Washington State Department of Transportation
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

#include <WBFLRCCapacity.h>

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>

#if defined _USE_MULTITHREADING
#include <PgsExt\ThreadManager.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CEngAgentImp
class ATL_NO_VTABLE CEngAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CEngAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CEngAgentImp, &CLSID_EngAgent>,
   public IAgentEx,
   public IXBRMomentCapacity,
   public IXBRShearCapacity,
   public IXBRCrackedSection,
   public IXBRArtifact,
   public IXBRProjectEventSink
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
	COM_INTERFACE_ENTRY(IXBRMomentCapacity)
	COM_INTERFACE_ENTRY(IXBRShearCapacity)
   COM_INTERFACE_ENTRY(IXBRCrackedSection)
   COM_INTERFACE_ENTRY(IXBRArtifact)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
	//COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

//BEGIN_CONNECTION_POINT_MAP(CEngAgentImp)
//   CONNECTION_POINT_ENTRY( IID_IProjectEventSink )
//END_CONNECTION_POINT_MAP()

// IAgentEx
public:
	STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker) override;
   STDMETHOD(RegInterfaces)() override;
	STDMETHOD(Init)() override;
	STDMETHOD(Reset)() override;
	STDMETHOD(ShutDown)() override;
   STDMETHOD(Init2)() override;
   STDMETHOD(GetClassID)(CLSID* pCLSID) override;

// IXBRMomentCapacity
public:
   virtual Float64 GetMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;
   virtual const MomentCapacityDetails& GetMomentCapacityDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;

   virtual Float64 GetCrackingMoment(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;
   virtual const CrackingMomentDetails& GetCrackingMomentDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;

   virtual Float64 GetMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;
   virtual const MinMomentCapacityDetails& GetMinMomentCapacityDetails(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const override;
   virtual MinMomentCapacityDetails GetMinMomentCapacityDetails(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx) const override;

// IXBRCrackedSection
public:
   virtual Float64 GetIcrack(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,xbrTypes::LoadType loadType) const override;
   virtual const CrackedSectionDetails& GetCrackedSectionDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,xbrTypes::LoadType loadType) const override;

// IXBRShearCapacity
public:
   virtual Float64 GetShearCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const override;
   virtual const ShearCapacityDetails& GetShearCapacityDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const override;
   virtual const AvOverSDetails& GetAverageAvOverSDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const override;
   virtual Float64 GetDv(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const override;
   virtual const DvDetails& GetDvDetails(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const override;

// IXBRArtifact
public:
   virtual const xbrRatingArtifact* GetXBeamRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const override;

// IXBRProjectEventSink
public:
   virtual HRESULT OnProjectChanged() override;

private:
   DECLARE_EAF_AGENT_DATA;

   DWORD m_dwProjectCookie;


   // PierID is not used to store results because the POI ID is sufficent
   // POI IDs are not duplicated between piers
   std::unique_ptr<std::map<IDType,MomentCapacityDetails>> m_pPositiveMomentCapacity[2]; // key = POI ID, array index = xbrTypes::Stage
   std::unique_ptr<std::map<IDType,MomentCapacityDetails>> m_pNegativeMomentCapacity[2];

   std::unique_ptr<std::map<IDType,CrackingMomentDetails>> m_pPositiveCrackingMoment[2]; // key = POI ID, array index = xbrTypes::Stage
   std::unique_ptr<std::map<IDType,CrackingMomentDetails>> m_pNegativeCrackingMoment[2];

   std::unique_ptr<std::map<IDType,MinMomentCapacityDetails>> m_pPositiveMinMomentCapacity[2][pgsTypes::lrLoadRatingTypeCount]; // key = POI ID, array index = xbrTypes::Stage, second array index is based on limit state type.use GET_INDEX(limitState) macro
   std::unique_ptr<std::map<IDType,MinMomentCapacityDetails>> m_pNegativeMinMomentCapacity[2][pgsTypes::lrLoadRatingTypeCount];

   std::unique_ptr<std::map<IDType,CrackedSectionDetails>> m_pPositiveMomentCrackedSection[2][2]; // key = POI ID, array index = [xbrTypes::Stage][xbrTypes::LoadType]
   std::unique_ptr<std::map<IDType,CrackedSectionDetails>> m_pNegativeMomentCrackedSection[2][2];

   std::unique_ptr<std::map<IDType,ShearCapacityDetails>> m_pShearCapacity[2];
   std::unique_ptr<std::map<IDType,AvOverSDetails>> m_pShearFailurePlane[2];

   std::unique_ptr<std::map<IDType,DvDetails>> m_pDvDetails[2];

   MomentCapacityDetails ComputeMomentCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const;
   CrackingMomentDetails ComputeCrackingMoment(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const;
   MinMomentCapacityDetails ComputeMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment) const;
   MinMomentCapacityDetails ComputeMinMomentCapacity(PierIDType pierID,pgsTypes::LimitState limitState,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx) const;
   void GetCrackingMomentFactors(PierIDType pierID,Float64* pG1,Float64* pG2,Float64* pG3) const;
   CrackedSectionDetails ComputeCrackedSectionProperties(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,xbrTypes::LoadType loadType) const;
   void BuildMomentCapacityModel(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,bool bPositiveMoment,IRCBeam2** ppModel,Float64* pdt) const;

   DvDetails ComputeDv(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const;
   ShearCapacityDetails ComputeShearCapacity(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi) const;
   AvOverSDetails ComputeAverageAvOverS(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,Float64 theta) const;


   // rating artifacts for vehicleIdx == INVALID_INDEX are the governing artifacts for a load rating type
   typedef std::map<VehicleIndexType,xbrRatingArtifact> RatingArtifacts;
   std::unique_ptr<std::map<PierIDType,RatingArtifacts>> m_pRatingArtifacts[pgsTypes::lrLoadRatingTypeCount]; // array index is pgsTypes::LoadRatingType
   RatingArtifacts& GetPrivateRatingArtifacts(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const;
   void CreateRatingArtifact(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const;

   CComPtr<IRCSolver2> m_MomentCapacitySolver;
   CComPtr<IRCCrackedSectionSolver2> m_CrackedSectionSolver;


#if defined _USE_MULTITHREADING
   CThreadManager m_ThreadManager;
#endif

   void Invalidate(bool bCreateNewDataStructures = true);
   struct DataStructures
   {
      std::map<IDType,MomentCapacityDetails>* m_pPositiveMomentCapacity[2];
      std::map<IDType,MomentCapacityDetails>* m_pNegativeMomentCapacity[2];

      std::map<IDType,CrackingMomentDetails>* m_pPositiveCrackingMoment[2];
      std::map<IDType,CrackingMomentDetails>* m_pNegativeCrackingMoment[2];

      std::map<IDType,MinMomentCapacityDetails>* m_pPositiveMinMomentCapacity[2][pgsTypes::lrLoadRatingTypeCount];
      std::map<IDType,MinMomentCapacityDetails>* m_pNegativeMinMomentCapacity[2][pgsTypes::lrLoadRatingTypeCount];

      std::map<IDType,CrackedSectionDetails>* m_pPositiveMomentCrackedSection[2][2];
      std::map<IDType,CrackedSectionDetails>* m_pNegativeMomentCrackedSection[2][2];

      std::map<PierIDType,RatingArtifacts>* m_pRatingArtifacts[pgsTypes::lrLoadRatingTypeCount];

      std::map<IDType,ShearCapacityDetails>* m_pShearCapacity[2];
      std::map<IDType,AvOverSDetails>* m_pShearFailurePlane[2];

      std::map<IDType,DvDetails>* m_pDvDetails[2];
   };
   static UINT DeleteDataStructures(LPVOID pParam);
   void CreateDataStructures();
};

OBJECT_ENTRY_AUTO(CLSID_EngAgent, CEngAgentImp)

