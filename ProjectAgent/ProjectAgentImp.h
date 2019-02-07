///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2019  Washington State Department of Transportation
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
#include <XBeamRateExt\XBeamRateUtilities.h>

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
   public IXBRExport,
   public IBridgeDescriptionEventSink,
   public IEventsSink
{  
public:
	CProjectAgentImp(); 
   virtual ~CProjectAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

   HRESULT SavePier(PierIndexType pierIdx,LPCTSTR lpszPathName);

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
   COM_INTERFACE_ENTRY_IID(IID_IXBRExport,IXBRExport)
   COM_INTERFACE_ENTRY(IBridgeDescriptionEventSink)
   COM_INTERFACE_ENTRY(IEventsSink)
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
	STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker) override;
   STDMETHOD(RegInterfaces)() override;
	STDMETHOD(Init)() override;
	STDMETHOD(Reset)() override;
	STDMETHOD(ShutDown)() override;
   STDMETHOD(Init2)() override;
   STDMETHOD(GetClassID)(CLSID* pCLSID) override;

// IAgentUIIntegration
public:
   STDMETHOD(IntegrateWithUI)(BOOL bIntegrate) override;

// IAgentPersist
public:
	STDMETHOD(Load)(/*[in]*/ IStructuredLoad* pStrLoad) override;
	STDMETHOD(Save)(/*[in]*/ IStructuredSave* pStrSave) override;

// IEAFCommandCallback
   // NOTE: this interface should be on CAgentCmdTarget, but it can't until
   // CAgentCmdTarget supports IUnknown. This is a COM interface
public:
   virtual BOOL OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo) override;
   virtual BOOL GetStatusBarMessageString(UINT nID, CString& rMessage) const override;
   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const override;

// IXBRProjectProperties
public:
   virtual LPCTSTR GetBridgeName() const override;
   virtual void SetBridgeName(LPCTSTR name) override;
   virtual LPCTSTR GetBridgeID() const override;
   virtual void SetBridgeID(LPCTSTR bid) override;
   virtual LPCTSTR GetJobNumber() const override;
   virtual void SetJobNumber(LPCTSTR jid) override;
   virtual LPCTSTR GetEngineer() const override;
   virtual void SetEngineer(LPCTSTR eng) override;
   virtual LPCTSTR GetCompany() const override;
   virtual void SetCompany(LPCTSTR company) override;
   virtual LPCTSTR GetComments() const override;
   virtual void SetComments(LPCTSTR comments) override;
   virtual void ShowProjectPropertiesOnNewProject(bool bShow) override;
   virtual bool ShowProjectPropertiesOnNewProject() override;
   virtual void PromptForProjectProperties() override;

// IXBRProject
public:
   virtual void SetPierData(const xbrPierData& pierData) override;
   virtual const xbrPierData& GetPierData(PierIDType pierID) const override;

   virtual xbrTypes::PierType GetPierType(PierIDType pierID) const override;
   virtual void SetPierType(PierIDType pierID,xbrTypes::PierType pierType) override;

   virtual void SetDeckElevation(PierIDType pierID,Float64 deckElevation) override;
   virtual Float64 GetDeckElevation(PierIDType pierID) const override;
   virtual void SetDeckThickness(PierIDType pierID,Float64 tDeck) override;
   virtual Float64 GetDeckThickness(PierIDType pierID) const override;
   virtual void SetCrownPointOffset(PierIDType pierID,Float64 cpo) override;
   virtual Float64 GetCrownPointOffset(PierIDType pierID) const override;
   virtual void SetBridgeLineOffset(PierIDType pierID,Float64 blo) override;
   virtual Float64 GetBridgeLineOffset(PierIDType pierID) const override;

   virtual void SetOrientation(PierIDType pierID,LPCTSTR strOrientation) override;
   virtual LPCTSTR GetOrientation(PierIDType pierID) const override;

   virtual pgsTypes::OffsetMeasurementType GetCurbLineDatum(PierIDType pierID) const override;
   virtual void SetCurbLineDatum(PierIDType pierID,pgsTypes::OffsetMeasurementType datumType) override;

   virtual void SetCurbLineOffset(PierIDType pierID,Float64 leftCLO,Float64 rightCLO) override;
   virtual void GetCurbLineOffset(PierIDType pierID,Float64* pLeftCLO,Float64* pRightCLO) const override;

   virtual void SetCrownSlopes(PierIDType pierID,Float64 sl,Float64 sr) override;
   virtual void GetCrownSlopes(PierIDType pierID,Float64* psl,Float64* psr) const override;

   virtual void GetDiaphragmDimensions(PierIDType pierID,Float64* pH,Float64* pW) const override;
   virtual void SetDiaphragmDimensions(PierIDType pierID,Float64 H,Float64 W) override;

   virtual IndexType GetBearingLineCount(PierIDType pierID) const override;
   virtual void SetBearingLineCount(PierIDType pierID,IndexType nBearingLines) override;

   virtual IndexType GetBearingCount(PierIDType pierID,IndexType brgLineIdx) const override;
   virtual void SetBearingCount(PierIDType pierID,IndexType brgLineIdx,IndexType nBearings) override;

   virtual Float64 GetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx) const override;
   virtual void SetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 spacing) override;

   virtual void SetBearingReactionType(PierIDType pierID,IndexType brgLineIdx,xbrTypes::ReactionLoadType brgReactionType) override;
   virtual xbrTypes::ReactionLoadType GetBearingReactionType(PierIDType pierID,IndexType brgLineIdx) const override;

   virtual void SetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW,Float64 CR,Float64 SH,Float64 PS,Float64 RE,Float64 W) override;
   virtual void GetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW,Float64* pCR,Float64* pSH,Float64* pPS,Float64* pRE,Float64* pW) const override;
   virtual Float64 GetBearingWidth(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx) const override;

   virtual void GetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum) const override;
   virtual void SetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum) override;

   virtual void SetReactionLoadApplicationType(PierIDType pierID,xbrTypes::ReactionLoadApplicationType applicationType) override;
   virtual xbrTypes::ReactionLoadApplicationType GetReactionLoadApplicationType(PierIDType pierID) const override;

   virtual IndexType GetLiveLoadReactionCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType) const override;
   virtual void SetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrLiveLoadReactionData>& vLLIM) override;
   virtual std::vector<xbrLiveLoadReactionData> GetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType) const override;
   virtual std::_tstring GetLiveLoadName(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const override;
   virtual Float64 GetLiveLoadReaction(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const override;
   virtual Float64 GetVehicleWeight(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx) const override;

   virtual void SetRebarMaterial(PierIDType pierID,matRebar::Type type,matRebar::Grade grade) override;
   virtual void GetRebarMaterial(PierIDType pierID,matRebar::Type* pType,matRebar::Grade* pGrade) const override;

   virtual void SetConcrete(PierIDType pierID,const CConcreteMaterial& concrete) override;
   virtual const CConcreteMaterial& GetConcrete(PierIDType pierID) const override;

   virtual void SetLowerXBeamDimensions(PierIDType pierID,Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 x3,Float64 x4,Float64 w) override;
   virtual void GetLowerXBeamDimensions(PierIDType pierID,Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* px3,Float64* px4,Float64* pw) const override;
   virtual Float64 GetXBeamLeftOverhang(PierIDType pierID) const override;
   virtual Float64 GetXBeamRightOverhang(PierIDType pierID) const override;
   virtual Float64 GetXBeamWidth(PierIDType pierID) const override;

   virtual void SetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset) override;
   virtual void GetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset) const override;
   virtual IndexType GetColumnCount(PierIDType pierID) const override;
   virtual Float64 GetColumnHeight(PierIDType pierID,ColumnIndexType colIdx) const override;
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType(PierIDType pierID,ColumnIndexType colIdx) const override;
   virtual Float64 GetColumnSpacing(PierIDType pierID,SpacingIndexType spaceIdx) const override;
   virtual pgsTypes::ColumnFixityType GetColumnFixity(PierIDType pierID,ColumnIndexType colIdx) const override;

   virtual void SetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H) override;
   virtual void GetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH) const override;

   virtual const xbrLongitudinalRebarData& GetLongitudinalRebar(PierIDType pierID) const override;
   virtual void SetLongitudinalRebar(PierIDType pierID,const xbrLongitudinalRebarData& rebar) override;
   virtual void SetLowerXBeamStirrups(PierIDType pierID,const xbrStirrupData& stirrups) override;
   virtual const xbrStirrupData& GetLowerXBeamStirrups(PierIDType pierID) const override;
   virtual void SetFullDepthStirrups(PierIDType pierID,const xbrStirrupData& stirrups) override;
   virtual const xbrStirrupData& GetFullDepthStirrups(PierIDType pierID) const override;
   
   virtual void SetFlexureResistanceFactors(Float64 phiC,Float64 phiT) override;
   virtual void GetFlexureResistanceFactors(Float64* phiC,Float64* phiT) const override;
   virtual void SetShearResistanceFactor(Float64 phi) override;
   virtual Float64 GetShearResistanceFactor() const override;

   virtual void SetSystemFactorFlexure(Float64 sysFactor) override;
   virtual Float64 GetSystemFactorFlexure() const override;
   
   virtual void SetSystemFactorShear(Float64 sysFactor) override;
   virtual Float64 GetSystemFactorShear() const override;

   virtual void SetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor) override;
   virtual void GetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor) const override;
   virtual Float64 GetConditionFactor(PierIDType pierID) const override;

   virtual void SetDCLoadFactor(pgsTypes::LimitState limitState,Float64 dc) override;
   virtual Float64 GetDCLoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetDWLoadFactor(pgsTypes::LimitState limitState,Float64 dw) override;
   virtual Float64 GetDWLoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetCRLoadFactor(pgsTypes::LimitState limitState,Float64 cr) override;
   virtual Float64 GetCRLoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetSHLoadFactor(pgsTypes::LimitState limitState,Float64 sh) override;
   virtual Float64 GetSHLoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetRELoadFactor(pgsTypes::LimitState limitState,Float64 re) override;
   virtual Float64 GetRELoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetPSLoadFactor(pgsTypes::LimitState limitState,Float64 ps) override;
   virtual Float64 GetPSLoadFactor(pgsTypes::LimitState limitState) const override;

   virtual void SetLiveLoadFactor(PierIDType pierID,pgsTypes::LimitState limitState,Float64 ll) override;
   virtual Float64 GetLiveLoadFactor(PierIDType pierID,pgsTypes::LimitState limitState,VehicleIndexType vehicleIdx) const override;

   virtual void SetMaxLiveLoadStepSize(Float64 stepSize) override;
   virtual Float64 GetMaxLiveLoadStepSize() const override;

   virtual void SetMaxLoadedLanes(IndexType nLanesMax) override;
   virtual IndexType GetMaxLoadedLanes() const override;

// IXBRRatingSpecification
public:
   virtual bool IsRatingEnabled(pgsTypes::LoadRatingType ratingType) const override;
   virtual void EnableRating(pgsTypes::LoadRatingType ratingType,bool bEnable) override;

   virtual void RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear) override;
   virtual bool RateForShear(pgsTypes::LoadRatingType ratingType) const override;

   // Evalute yield stress in reinforcement MBE 6A.5.4.2.2b
   virtual void CheckYieldStressLimit(bool bCheckYieldStress) override;
   virtual bool CheckYieldStressLimit() const override;

   // returns fraction of yield stress that reinforcement can be stressed to during
   // a permit load rating evaluation MBE 6A.5.4.2.2b
   virtual void SetYieldStressLimitCoefficient(Float64 x) override;
   virtual Float64 GetYieldStressLimitCoefficient() const override;

   virtual pgsTypes::AnalysisType GetAnalysisMethodForReactions() const override;
   virtual void SetAnalysisMethodForReactions(pgsTypes::AnalysisType analysisType) override;

   virtual xbrTypes::EmergencyRatingMethod GetEmergencyRatingMethod() const override;
   virtual void SetEmergencyRatingMethod(xbrTypes::EmergencyRatingMethod emergencyRatingMethod) override;
   virtual bool IsWSDOTEmergencyRating(pgsTypes::LoadRatingType ratingType) const override;

   virtual xbrTypes::PermitRatingMethod GetPermitRatingMethod() const override;
   virtual void SetPermitRatingMethod(xbrTypes::PermitRatingMethod permitRatingMethod) override;
   virtual bool IsWSDOTPermitRating(pgsTypes::LoadRatingType ratingType) const override;

// IXBRProjectEdit
public:
   virtual void EditPier(int nPage) override;
   virtual void EditOptions() override;

// IXBREvents
public:
   virtual void HoldEvents() override;
   virtual void FirePendingEvents() override;
   virtual void CancelPendingEvents() override;

// IBridgeDescriptionEventSink
public:
   virtual HRESULT OnBridgeChanged(CBridgeChangedHint* pHint) override;
   virtual HRESULT OnGirderFamilyChanged() override;
   virtual HRESULT OnGirderChanged(const CGirderKey& girderKey,Uint32 lHint) override;
   virtual HRESULT OnLiveLoadChanged() override;
   virtual HRESULT OnLiveLoadNameChanged(LPCTSTR strOldName,LPCTSTR strNewName) override;
   virtual HRESULT OnConstructionLoadChanged() override;

// IEventsSink
public:
   virtual HRESULT OnHoldEvents() override;
   virtual HRESULT OnFirePendingEvents() override;
   virtual HRESULT OnCancelPendingEvents() override;


// IXBRExport
public:
   virtual HRESULT Export(PierIndexType pierIdx) override;
   virtual HRESULT BatchExport() override;

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   CAgentCmdTarget m_CommandTarget;

   PierIDType m_SavePierID; // contains the ID for the pier data that is being saved
   bool m_bExportingModel; // set to true, when exporting a pier model from PGS

   DWORD m_dwBridgeDescCookie;
   DWORD m_dwEventsCookie;

   StatusGroupIDType m_XBeamRateStatusGroupID; // ID used to identify status items created by this agent
   StatusCallbackIDType m_scidBridgeInfo;
   StatusCallbackIDType m_scidBridgeWarn;
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
   xbrPierData& GetPrivatePierData(PierIDType pierID) const;
   mutable std::map<PierIDType,xbrPierData> m_PierData;

   Float64 m_SysFactorFlexure;
   Float64 m_SysFactorShear;
   Float64 m_PhiC, m_PhiT; // resistance factors for flexure (compress,tension controlled section)
   Float64 m_PhiV; // resistance factor for shear
   bool m_bRatingEnabled[pgsTypes::lrLoadRatingTypeCount]; // array index is pgsTypes::LoadRatingType
   bool m_bRateForShear[pgsTypes::lrLoadRatingTypeCount]; // array index is pgsTypes::LoadRatingType
   bool m_bCheckYieldStress;
   Float64 m_YieldStressCoefficient;

   Float64 m_MaxLLStepSize; // maximum step size for moving live load
   IndexType m_MaxLoadedLanes; // maximum number of loaded lanes to consider (INVALID_INDEX means to consider all lanes)

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

   mutable std::map<PierIDType,Float64> m_gLL[RATING_LIMIT_STATE_COUNT]; // use GET_INDEX macro to access the array
   // Bearing Reactions
   typedef struct BearingReactions
   {
      BearingReactions() { DC = 0, DW = 0; CR = 0; SH = 0; PS = 0; RE = 0; W = 0; }
      Float64 DC, DW, CR, SH, PS, RE;
      Float64 W; // width of reaction (only used of reaction type is rltUniform
   } BearingReactions;
   mutable std::map<PierIDType,std::vector<BearingReactions>> m_BearingReactions[2];
   std::vector<BearingReactions>& GetPrivateBearingReactions(PierIDType pierID,IndexType brgLineIdx) const;

   mutable std::map<PierIDType,xbrTypes::ReactionLoadType> m_BearingReactionType[2];
   xbrTypes::ReactionLoadType& GetPrivateBearingReactionType(PierIDType pierID,IndexType brgLineIdx) const;

   mutable std::map<PierIDType,std::vector<xbrLiveLoadReactionData>> m_LiveLoadReactions[pgsTypes::lrLoadRatingTypeCount]; // access with pgsTypes::LoadRatingType
   std::vector<xbrLiveLoadReactionData>& GetPrivateLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType) const;

   mutable std::map<PierIDType,xbrTypes::ReactionLoadApplicationType> m_ReactionApplication;
   xbrTypes::ReactionLoadApplicationType& GetPrivateReactionLoadApplication(PierIDType pierID) const;

   pgsTypes::AnalysisType m_AnalysisType; // use this analysis type when PGSuper is in Envelope model

   // Options
   xbrTypes::EmergencyRatingMethod m_EmergencyRatingMethod;
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
   PierIndexType GetPierIndex(PierIDType pierID) const;
   CGirderKey GetGirderKey(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx) const;
   bool UseUniformLoads(PierIDType pierID,IndexType brgLineIdx) const;

   GirderIndexType GetLongestGirderLine() const;

   void Invalidate();
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

