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
   public IAgentEx,
   public IAgentUIIntegration,
   public IAgentPersist,
   public IEAFCommandCallback,
   public IXBRProjectProperties,
   public IXBRProject,
   public IXBRRatingSpecification,
   public IXBRProjectEdit,
   public IXBREvents
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
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CProjectAgentImp)
   CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
   CONNECTION_POINT_ENTRY( IID_IXBRProjectPropertiesEventSink )
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
   virtual const xbrPierData& GetPierData();

   virtual xbrTypes::SuperstructureConnectionType GetPierType();
   virtual void SetPierType(xbrTypes::SuperstructureConnectionType pierType);

   virtual void SetDeckElevation(Float64 deckElevation);
   virtual Float64 GetDeckElevation();
   virtual void SetCrownPointOffset(Float64 cpo);
   virtual Float64 GetCrownPointOffset();
   virtual void SetBridgeLineOffset(Float64 blo);
   virtual Float64 GetBridgeLineOffset();

   virtual void SetOrientation(LPCTSTR strOrientation);
   virtual LPCTSTR GetOrientation();

   virtual pgsTypes::OffsetMeasurementType GetCurbLineDatum();
   virtual void SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType);

   virtual void SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO);
   virtual void GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO);

   virtual void SetCrownSlopes(Float64 sl,Float64 sr);
   virtual void GetCrownSlopes(Float64* psl,Float64* psr);

   virtual void GetDiaphragmDimensions(Float64* pH,Float64* pW);
   virtual void SetDiaphragmDimensions(Float64 H,Float64 W);

   virtual IndexType GetBearingLineCount();
   virtual void SetBearingLineCount(IndexType nBearingLines);

   virtual IndexType GetBearingCount(IndexType brgLineIdx);
   virtual void SetBearingCount(IndexType brgLineIdx,IndexType nBearings);

   virtual Float64 GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx);
   virtual void SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 spacing);

   virtual void SetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW);
   virtual void GetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW);

   virtual void GetReferenceBearing(IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum);
   virtual void SetReferenceBearing(IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum);

   virtual IndexType GetLiveLoadReactionCount(pgsTypes::LoadRatingType ratingType);
   virtual void SetLiveLoadReactions(pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM);
   virtual std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(pgsTypes::LoadRatingType ratingType);
   virtual LPCTSTR GetLiveLoadName(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx);
   virtual Float64 GetLiveLoadReaction(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx);

   virtual void SetRebarMaterial(matRebar::Type type,matRebar::Grade grade);
   virtual void GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade);

   virtual void SetConcrete(const CConcreteMaterial& concrete);
   virtual const CConcreteMaterial& GetConcrete();

   virtual void SetLowerXBeamDimensions(Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w);
   virtual void GetLowerXBeamDimensions(Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw);
   virtual Float64 GetXBeamLeftOverhang();
   virtual Float64 GetXBeamRightOverhang();
   virtual Float64 GetXBeamWidth();

   virtual void SetColumnLayout(IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s);
   virtual void GetColumnLayout(IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps);
   virtual IndexType GetColumnCount();
   virtual Float64 GetColumnHeight();
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType();
   virtual Float64 GetColumnSpacing();

   virtual void SetColumnProperties(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H);
   virtual void GetColumnProperties(CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH);

   virtual Float64 GetXBeamLength();

   virtual IndexType GetRebarRowCount();
   virtual void AddRebarRow(xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing);
   virtual void SetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing);
   virtual void GetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,IndexType* pnBars,Float64* pSpacing);
   virtual void RemoveRebarRow(IndexType rowIdx);
   virtual void RemoveRebarRows();

   virtual void SetConditionFactor(pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor);
   virtual void GetConditionFactor(pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor);
   virtual Float64 GetConditionFactor();

   virtual void SetDCLoadFactor(Float64 dc);
   virtual Float64 GetDCLoadFactor();

   virtual void SetDWLoadFactor(Float64 dw);
   virtual Float64 GetDWLoadFactor();

   virtual void SetLiveLoadFactor(pgsTypes::LoadRatingType ratingType,Float64 ll);
   virtual Float64 GetLiveLoadFactor(pgsTypes::LoadRatingType ratingType);

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

// IXBRProjectEdit
public:
   virtual void EditPier(int nPage);

// IXBREvents
public:
   virtual void HoldEvents();
   virtual void FirePendingEvents();
   virtual void CancelPendingEvents();

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   CAgentCmdTarget m_CommandTarget;

   // Project Properties
   CString m_strBridgeName;
   CString m_strBridgeId;
   PierIndexType m_PierIdx;
   CString m_strJobNumber;
   CString m_strEngineer;
   CString m_strCompany;
   CString m_strComments;

   // The raw data for this project
   xbrPierData m_PierData; // eventually this will be a vector supporting multiple piers

   Float64 m_SysFactorFlexure;
   Float64 m_SysFactorShear;
   bool m_vbRateForShear[6]; // array index is pgsTypes::LoadRatingType

   // Load Factors
   Float64 m_gDC;
   Float64 m_gDW;
   Float64 m_gLL[6]; // use pgsTypes::LoadRatingType to access array

   // Bearing Reactions
   typedef struct
   {
      Float64 DC, DW;
   } BearingReactions;
   std::vector<std::vector<BearingReactions>> m_vvBearingReactions;
   // outer vector is for number of bearing lines
   // inner vector is for number of bearings on a bearing line
   // m_vvBearingReactions[brgLineIdx][brgIdx].DC/DW

   // Live Load Reactions
   class LiveLoadReaction
   {
   public:
      std::_tstring Name;
      Float64 LLIM;
      LiveLoadReaction() {;}
      LiveLoadReaction(LPCTSTR name,Float64 llim) : Name(name), LLIM(llim) {;}
   };
   std::vector<LiveLoadReaction> m_vLiveLoadReactions[6]; // access with pgsTypes::LoadRatingType

   // Events
   int m_EventHoldCount;
   Uint32 m_PendingEvents;

   friend CProxyIXBRProjectPropertiesEventSink<CProjectAgentImp>;
   friend CProxyIXBRProjectEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

