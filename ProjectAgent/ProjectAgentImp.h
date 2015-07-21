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
   virtual PierIndexType GetPierIndex();
   virtual void SetPierIndex(PierIndexType pierIdx);
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

   virtual IndexType GetLiveLoadReactionCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType);
   virtual void SetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM);
   virtual std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType);
   virtual LPCTSTR GetLiveLoadName(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx);
   virtual Float64 GetLiveLoadReaction(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx);

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

   virtual void SetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor);
   virtual void GetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor);
   virtual Float64 GetConditionFactor(PierIDType pierID);

   virtual void SetDCLoadFactor(Float64 dc);
   virtual Float64 GetDCLoadFactor();

   virtual void SetDWLoadFactor(Float64 dw);
   virtual Float64 GetDWLoadFactor();

   virtual void SetCRLoadFactor(Float64 cr);
   virtual Float64 GetCRLoadFactor();

   virtual void SetSHLoadFactor(Float64 sh);
   virtual Float64 GetSHLoadFactor();

   virtual void SetPSLoadFactor(Float64 ps);
   virtual Float64 GetPSLoadFactor();

   virtual void SetRELoadFactor(Float64 re);
   virtual Float64 GetRELoadFactor();

   virtual void SetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType,Float64 ll);
   virtual Float64 GetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType);

// IXBRRatingSpecification
public:
   //virtual bool IsRatingEnabled(pgsTypes::LoadRatingType ratingType) = 0;
   //virtual void EnableRating(pgsTypes::LoadRatingType ratingType,bool bEnable) = 0;

   //virtual std::_tstring GetRatingSpecification() = 0;
   //virtual void SetRatingSpecification(const std::_tstring& spec) = 0;

   //virtual void SetADTT(Int16 adtt) = 0;
   //virtual Int16 GetADTT() = 0; // < 0 = Unknown
   
   virtual void SetSystemFactorFlexure(Float64 sysFactor);
   virtual Float64 GetSystemFactorFlexure();
   
   virtual void SetSystemFactorShear(Float64 sysFactor);
   virtual Float64 GetSystemFactorShear();

   //virtual void SetDeadLoadFactor(pgsTypes::LimitState ls,Float64 gDC) = 0;
   //virtual Float64 GetDeadLoadFactor(pgsTypes::LimitState ls) = 0;

   //virtual void SetWearingSurfaceFactor(pgsTypes::LimitState ls,Float64 gDW) = 0;
   //virtual Float64 GetWearingSurfaceFactor(pgsTypes::LimitState ls) = 0;

   //virtual void SetLiveLoadFactor(pgsTypes::LimitState ls,Float64 gLL) = 0;
   //virtual Float64 GetLiveLoadFactor(pgsTypes::LimitState ls,bool bResolveIfDefault=false) = 0;
   //virtual Float64 GetLiveLoadFactor(pgsTypes::LimitState ls,pgsTypes::SpecialPermitType specialPermitType,Int16 adtt,const RatingLibraryEntry* pRatingEntry,bool bResolveIfDefault=false) = 0;

   //virtual void SetAllowableTensionCoefficient(pgsTypes::LoadRatingType ratingType,Float64 t) = 0;
   //virtual Float64 GetAllowableTensionCoefficient(pgsTypes::LoadRatingType ratingType) = 0;

   //virtual void RateForStress(pgsTypes::LoadRatingType ratingType,bool bRateForStress) = 0;
   //virtual bool RateForStress(pgsTypes::LoadRatingType ratingType) = 0;

   virtual void RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear);
   virtual bool RateForShear(pgsTypes::LoadRatingType ratingType);

   //// Per last paragraph in MBE 6A.4.4.2.1a, the lane load may be excluded and the 0.75 truck factor taken as 1.0
   //// for ADTT < 500 and in the Engineer's judgement it is warranted
   //virtual void ExcludeLegalLoadLaneLoading(bool bExclude) = 0;
   //virtual bool ExcludeLegalLoadLaneLoading() = 0;

   //// returns fraction of yield stress that reinforcement can be stressed to during
   //// a permit load rating evaluation MBE 6A.5.4.2.2b
   //virtual void SetYieldStressLimitCoefficient(Float64 x) = 0;
   //virtual Float64 GetYieldStressLimitCoefficient() = 0;

   //// Permit type for rating for special/limited crossing permit vehicle
   //virtual void SetSpecialPermitType(pgsTypes::SpecialPermitType type) = 0;
   //virtual pgsTypes::SpecialPermitType GetSpecialPermitType() = 0;

   virtual pgsTypes::AnalysisType GetAnalysisMethodForReactions();
   virtual void SetAnalysisMethodForReactions(pgsTypes::AnalysisType analysisType);

// IXBRProjectEdit
public:
   virtual void EditPier(int nPage);

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
   PierIndexType m_PierIdx;
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
   bool m_vbRateForShear[6]; // array index is pgsTypes::LoadRatingType

   // Load Factors
   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gCR;
   Float64 m_gSH;
   Float64 m_gPS;
   Float64 m_gRE;
   std::map<PierIDType,Float64> m_gLL[6]; // use pgsTypes::LoadRatingType to access array

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

   // Live Load Reactions
   class LiveLoadReaction
   {
   public:
      std::_tstring Name;
      Float64 LLIM;
      LiveLoadReaction() {;}
      LiveLoadReaction(LPCTSTR name,Float64 llim) : Name(name), LLIM(llim) {;}
      LiveLoadReaction(const LiveLoadReaction& other) {Name=other.Name; LLIM=other.LLIM;}
   };
   std::map<PierIDType,std::vector<LiveLoadReaction>> m_LiveLoadReactions[6]; // access with pgsTypes::LoadRatingType
   std::vector<CProjectAgentImp::LiveLoadReaction>& GetPrivateLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType);

   pgsTypes::AnalysisType m_AnalysisType; // use this analysis type when PGSuper is in Envelope model

   // Events
   int m_EventHoldCount;
   Uint32 m_PendingEvents;

   friend CProxyIXBRProjectPropertiesEventSink<CProjectAgentImp>;
   friend CProxyIXBRProjectEventSink<CProjectAgentImp>;
   friend CProxyIXBRRatingSpecificationEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();

   void UpdatePiers();
   void UpdatePierData(const CPierData2* pPier,xbrPierData& pierData);
   PierIndexType GetPierIndex(PierIDType pierID);
   CGirderKey GetGirderKey(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx);
   bool UseUniformLoads(PierIDType pierID,IndexType brgLineIdx);
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

