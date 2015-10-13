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
#include <XBeamRateExt\PierData.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
class ATL_NO_VTABLE CProjectAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CProjectAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CProjectAgentImp, &CLSID_ProjectAgent>,
	public IConnectionPointContainerImpl<CProjectAgentImp>,
   public CProxyIXBRProjectPropertiesEventSink<CProjectAgentImp>,
   public CProxyIXBRProjectEventSink<CProjectAgentImp>,
   public CProxyIXBREventsEventSink<CProjectAgentImp>,
   public CProxyIXBRRatingSpecificationEventSink<CProjectAgentImp>,
   public IAgentEx,
   public IAgentUIIntegration,
   public IAgentPersist,
   public IEAFCommandCallback,
   public IXBRProjectProperties,
   public IXBRProject,
   public IXBRRatingSpecification,
   public IXBRProjectEdit,
   public IXBREvents,
   public IBridgeDescriptionEventSink
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
   COM_INTERFACE_ENTRY_IID(IID_IXBRProjectProperties,IXBRProjectProperties)
   COM_INTERFACE_ENTRY_IID(IID_IXBRProject,IXBRProject)
   COM_INTERFACE_ENTRY_IID(IID_IXBRRatingSpecification,IXBRRatingSpecification)
   COM_INTERFACE_ENTRY_IID(IID_IXBRProjectEdit,IXBRProjectEdit)
   COM_INTERFACE_ENTRY_IID(IID_IXBREvents,IXBREvents)
   COM_INTERFACE_ENTRY(IBridgeDescriptionEventSink)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CProjectAgentImp)
   CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
   CONNECTION_POINT_ENTRY( IID_IXBRProjectPropertiesEventSink )
   CONNECTION_POINT_ENTRY( IID_IXBRRatingSpecificationEventSink )
   CONNECTION_POINT_ENTRY( IID_IXBREventsSink )
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

// IXBRProjectProperties
public:
   virtual LPCTSTR GetBridgeName() const;
   virtual void SetBridgeName(LPCTSTR name);
   virtual LPCTSTR GetBridgeID() const;
   virtual void SetBridgeID(LPCTSTR bid);
   virtual LPCTSTR GetJobNumber() const;
   virtual void SetJobNumber(LPCTSTR jid);
   virtual LPCTSTR GetEngineer() const;
   virtual void SetEngineer(LPCTSTR eng);
   virtual LPCTSTR GetCompany() const;
   virtual void SetCompany(LPCTSTR company);
   virtual LPCTSTR GetComments() const;
   virtual void SetComments(LPCTSTR comments);

// IXBRProject
public:
   virtual void SetPierData(const xbrPierData& pierData);
   virtual const xbrPierData& GetPierData(PierIDType pierID);

   virtual xbrTypes::SuperstructureConnectionType GetPierType(PierIDType pierID);
   virtual void SetPierType(PierIDType pierID,xbrTypes::SuperstructureConnectionType pierType);

   virtual void SetDeckElevation(PierIDType pierID,Float64 deckElevation);
   virtual Float64 GetDeckElevation(PierIDType pierID);
   virtual void SetDeckThickness(PierIDType pierID,Float64 tDeck);
   virtual Float64 GetDeckThickness(PierIDType pierID);
   virtual void SetCrownPointOffset(PierIDType pierID,Float64 cpo);
   virtual Float64 GetCrownPointOffset(PierIDType pierID);
   virtual void SetBridgeLineOffset(PierIDType pierID,Float64 blo);
   virtual Float64 GetBridgeLineOffset(PierIDType pierID);

   virtual void SetOrientation(PierIDType pierID,LPCTSTR strOrientation);
   virtual LPCTSTR GetOrientation(PierIDType pierID);

   virtual pgsTypes::OffsetMeasurementType GetCurbLineDatum(PierIDType pierID);
   virtual void SetCurbLineDatum(PierIDType pierID,pgsTypes::OffsetMeasurementType datumType);

   virtual void SetCurbLineOffset(PierIDType pierID,Float64 leftCLO,Float64 rightCLO);
   virtual void GetCurbLineOffset(PierIDType pierID,Float64* pLeftCLO,Float64* pRightCLO);

   virtual void SetCrownSlopes(PierIDType pierID,Float64 sl,Float64 sr);
   virtual void GetCrownSlopes(PierIDType pierID,Float64* psl,Float64* psr);

   virtual void GetDiaphragmDimensions(PierIDType pierID,Float64* pH,Float64* pW);
   virtual void SetDiaphragmDimensions(PierIDType pierID,Float64 H,Float64 W);

   virtual IndexType GetBearingLineCount(PierIDType pierID);
   virtual void SetBearingLineCount(PierIDType pierID,IndexType nBearingLines);

   virtual IndexType GetBearingCount(PierIDType pierID,IndexType brgLineIdx);
   virtual void SetBearingCount(PierIDType pierID,IndexType brgLineIdx,IndexType nBearings);

   virtual Float64 GetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx);
   virtual void SetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 spacing);

   virtual void SetBearingReactionType(PierIDType pierID,IndexType brgLineIdx,xbrTypes::ReactionLoadType brgReactionType);
   virtual xbrTypes::ReactionLoadType GetBearingReactionType(PierIDType pierID,IndexType brgLineIdx);

   virtual void SetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW,Float64 CR,Float64 SH,Float64 PS,Float64 RE,Float64 W);
   virtual void GetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW,Float64* pCR,Float64* pSH,Float64* pPS,Float64* pRE,Float64* pW);

   virtual void GetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum);
   virtual void SetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum);

   virtual void SetReactionLoadApplicationType(PierIDType pierID,xbrTypes::ReactionLoadApplicationType applicationType);
   virtual xbrTypes::ReactionLoadApplicationType GetReactionLoadApplicationType(PierIDType pierID);

   virtual IndexType GetLiveLoadReactionCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType);
   virtual void SetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrLiveLoadReactionData>& vLLIM);
   virtual std::vector<xbrLiveLoadReactionData> GetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType);
   virtual std::_tstring GetLiveLoadName(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);
   virtual Float64 GetLiveLoadReaction(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);
   virtual Float64 GetVehicleWeight(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx);

   virtual void SetRebarMaterial(PierIDType pierID,matRebar::Type type,matRebar::Grade grade);
   virtual void GetRebarMaterial(PierIDType pierID,matRebar::Type* pType,matRebar::Grade* pGrade);

   virtual void SetConcrete(PierIDType pierID,const CConcreteMaterial& concrete);
   virtual const CConcreteMaterial& GetConcrete(PierIDType pierID);

   virtual void SetLowerXBeamDimensions(PierIDType pierID,Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 x3,Float64 x4,Float64 w);
   virtual void GetLowerXBeamDimensions(PierIDType pierID,Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* px3,Float64* px4,Float64* pw);
   virtual Float64 GetXBeamLeftOverhang(PierIDType pierID);
   virtual Float64 GetXBeamRightOverhang(PierIDType pierID);
   virtual Float64 GetXBeamWidth(PierIDType pierID);

   virtual void SetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset);
   virtual void GetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset);
   virtual IndexType GetColumnCount(PierIDType pierID);
   virtual Float64 GetColumnHeight(PierIDType pierID,ColumnIndexType colIdx);
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType(PierIDType pierID,ColumnIndexType colIdx);
   virtual Float64 GetColumnSpacing(PierIDType pierID,SpacingIndexType spaceIdx);
   virtual pgsTypes::ColumnFixityType GetColumnFixity(PierIDType pierID,ColumnIndexType colIdx);

   virtual void SetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H);
   virtual void GetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH);

   virtual const xbrLongitudinalRebarData& GetLongitudinalRebar(PierIDType pierID);
   virtual void SetLongitudinalRebar(PierIDType pierID,const xbrLongitudinalRebarData& rebar);
   virtual void SetLowerXBeamStirrups(PierIDType pierID,const xbrStirrupData& stirrups);
   virtual const xbrStirrupData& GetLowerXBeamStirrups(PierIDType pierID);
   virtual void SetFullDepthStirrups(PierIDType pierID,const xbrStirrupData& stirrups);
   virtual const xbrStirrupData& GetFullDepthStirrups(PierIDType pierID);
   
   virtual void SetFlexureResistanceFactors(Float64 phiC,Float64 phiT);
   virtual void GetFlexureResistanceFactors(Float64* phiC,Float64* phiT);
   virtual void SetShearResistanceFactor(Float64 phi);
   virtual Float64 GetShearResistanceFactor();

   virtual void SetSystemFactorFlexure(Float64 sysFactor);
   virtual Float64 GetSystemFactorFlexure();
   
   virtual void SetSystemFactorShear(Float64 sysFactor);
   virtual Float64 GetSystemFactorShear();

   virtual void SetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor);
   virtual void GetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor);
   virtual Float64 GetConditionFactor(PierIDType pierID);

   virtual void SetDCLoadFactor(pgsTypes::LimitState limitState,Float64 dc);
   virtual Float64 GetDCLoadFactor(pgsTypes::LimitState limitState);

   virtual void SetDWLoadFactor(pgsTypes::LimitState limitState,Float64 dw);
   virtual Float64 GetDWLoadFactor(pgsTypes::LimitState limitState);

   virtual void SetCRLoadFactor(pgsTypes::LimitState limitState,Float64 cr);
   virtual Float64 GetCRLoadFactor(pgsTypes::LimitState limitState);

   virtual void SetSHLoadFactor(pgsTypes::LimitState limitState,Float64 sh);
   virtual Float64 GetSHLoadFactor(pgsTypes::LimitState limitState);

   virtual void SetRELoadFactor(pgsTypes::LimitState limitState,Float64 re);
   virtual Float64 GetRELoadFactor(pgsTypes::LimitState limitState);

   virtual void SetPSLoadFactor(pgsTypes::LimitState limitState,Float64 ps);
   virtual Float64 GetPSLoadFactor(pgsTypes::LimitState limitState);

   virtual void SetLiveLoadFactor(PierIDType pierID,pgsTypes::LimitState limitState,Float64 ll);
   virtual Float64 GetLiveLoadFactor(PierIDType pierID,pgsTypes::LimitState limitState,VehicleIndexType vehicleIdx);

// IXBRRatingSpecification
public:
   virtual bool IsRatingEnabled(pgsTypes::LoadRatingType ratingType);
   virtual void EnableRating(pgsTypes::LoadRatingType ratingType,bool bEnable);

   //virtual std::_tstring GetRatingSpecification() = 0;
   //virtual void SetRatingSpecification(const std::_tstring& spec) = 0;

   //virtual void SetADTT(Int16 adtt) = 0;
   //virtual Int16 GetADTT() = 0; // < 0 = Unknown

   virtual void RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear);
   virtual bool RateForShear(pgsTypes::LoadRatingType ratingType);

   //// Per last paragraph in MBE 6A.4.4.2.1a, the lane load may be excluded and the 0.75 truck factor taken as 1.0
   //// for ADTT < 500 and in the Engineer's judgement it is warranted
   //virtual void ExcludeLegalLoadLaneLoading(bool bExclude) = 0;
   //virtual bool ExcludeLegalLoadLaneLoading() = 0;

   // Evalute yield stress in reinforcement MBE 6A.5.4.2.2b
   virtual void CheckYieldStressLimit(bool bCheckYieldStress);
   virtual bool CheckYieldStressLimit();

   // returns fraction of yield stress that reinforcement can be stressed to during
   // a permit load rating evaluation MBE 6A.5.4.2.2b
   virtual void SetYieldStressLimitCoefficient(Float64 x);
   virtual Float64 GetYieldStressLimitCoefficient();

   //// Permit type for rating for special/limited crossing permit vehicle
   //virtual void SetSpecialPermitType(pgsTypes::SpecialPermitType type) = 0;
   //virtual pgsTypes::SpecialPermitType GetSpecialPermitType() = 0;

   virtual pgsTypes::AnalysisType GetAnalysisMethodForReactions();
   virtual void SetAnalysisMethodForReactions(pgsTypes::AnalysisType analysisType);

   virtual xbrTypes::PermitRatingMethod GetPermitRatingMethod();
   virtual void SetPermitRatingMethod(xbrTypes::PermitRatingMethod permitRatingMethod);

// IXBRProjectEdit
public:
   virtual void EditPier(int nPage);
   virtual void EditOptions();

// IXBREvents
public:
   virtual void HoldEvents();
   virtual void FirePendingEvents();
   virtual void CancelPendingEvents();

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

   CAgentCmdTarget m_CommandTarget;

   DWORD m_dwBridgeDescCookie;
   StatusGroupIDType m_XBeamRateStatusGroupID; // ID used to identify status items created by this agent
   StatusCallbackIDType m_scidBridgeError;

   // Project Properties
   CString m_strBridgeName;
   CString m_strBridgeId;
   CString m_strJobNumber;
   CString m_strEngineer;
   CString m_strCompany;
   CString m_strComments;

   // The raw data for this project
   // The key to this map is the pier ID. INVALID_ID
   // will be the key for the stand alone case
   xbrPierData& GetPrivatePierData(PierIDType pierID);
   std::map<PierIDType,xbrPierData> m_PierData;

   Float64 m_SysFactorFlexure;
   Float64 m_SysFactorShear;
   Float64 m_PhiC, m_PhiT; // resistance factors for flexure (compress,tension controlled section)
   Float64 m_PhiV; // resistance factor for shear
   bool m_bRatingEnabled[6]; // array index is pgsTypes::LoadRatingType
   bool m_bRateForShear[6]; // array index is pgsTypes::LoadRatingType
   bool m_bCheckYieldStress;
   Float64 m_YieldStressCoefficient;

   // Load Factors
   Float64 m_gDC_StrengthI;
   Float64 m_gDW_StrengthI;
   Float64 m_gCR_StrengthI;
   Float64 m_gSH_StrengthI;
   Float64 m_gPS_StrengthI;
   Float64 m_gRE_StrengthI;

   Float64 m_gDC_StrengthII;
   Float64 m_gDW_StrengthII;
   Float64 m_gCR_StrengthII;
   Float64 m_gSH_StrengthII;
   Float64 m_gPS_StrengthII;
   Float64 m_gRE_StrengthII;

   Float64 m_gDC_ServiceI;
   Float64 m_gDW_ServiceI;
   Float64 m_gCR_ServiceI;
   Float64 m_gSH_ServiceI;
   Float64 m_gPS_ServiceI;
   Float64 m_gRE_ServiceI;

   std::map<PierIDType,Float64> m_gLL[8]; // use GET_INDEX macro to access the array
   // Bearing Reactions
   typedef struct BearingReactions
   {
      BearingReactions() { DC = 0, DW = 0; CR = 0; SH = 0; PS = 0; RE = 0; W = 0; }
      Float64 DC, DW, CR, SH, PS, RE;
      Float64 W; // width of reaction (only used of reaction type is rltUniform
   } BearingReactions;
   std::map<PierIDType,std::vector<BearingReactions>> m_BearingReactions[2];
   std::vector<BearingReactions>& GetPrivateBearingReactions(PierIDType pierID,IndexType brgLineIdx);

   std::map<PierIDType,xbrTypes::ReactionLoadType> m_BearingReactionType[2];
   xbrTypes::ReactionLoadType& GetPrivateBearingReactionType(PierIDType pierID,IndexType brgLineIdx);

   std::map<PierIDType,std::vector<xbrLiveLoadReactionData>> m_LiveLoadReactions[6]; // access with pgsTypes::LoadRatingType
   std::vector<xbrLiveLoadReactionData>& GetPrivateLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType);

   std::map<PierIDType,xbrTypes::ReactionLoadApplicationType> m_ReactionApplication;
   xbrTypes::ReactionLoadApplicationType& GetPrivateReactionLoadApplication(PierIDType pierID);

   pgsTypes::AnalysisType m_AnalysisType; // use this analysis type when PGSuper is in Envelope model

   // Options
   xbrTypes::PermitRatingMethod m_PermitRatingMethod;

   // Events
   int m_EventHoldCount;
   Uint32 m_PendingEvents;

   friend CProxyIXBRProjectPropertiesEventSink<CProjectAgentImp>;
   friend CProxyIXBRProjectEventSink<CProjectAgentImp>;
   friend CProxyIXBRRatingSpecificationEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();

   void CreateToolbars();
   void RemoveToolbars();

   void UpdatePiers();
   void UpdatePierData(const CPierData2* pPier,xbrPierData& pierData);
   PierIndexType GetPierIndex(PierIDType pierID);
   CGirderKey GetGirderKey(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx);
   bool UseUniformLoads(PierIDType pierID,IndexType brgLineIdx);

   GirderIndexType GetLongestGirderLine();
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

