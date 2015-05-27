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

#include "XBeamRate.hxx" // XML/C++ Binding Object

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
class ATL_NO_VTABLE CProjectAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CProjectAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CProjectAgentImp, &CLSID_ProjectAgent>,
	public IConnectionPointContainerImpl<CProjectAgentImp>,
   public CProxyIXBRProjectEventSink<CProjectAgentImp>,
   public IAgentEx,
   public IAgentUIIntegration,
   public IAgentPersist,
   public IEAFCommandCallback,
   public IXBRProject,
   public IXBRRatingSpecification,
   public IXBRProjectEdit
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
   COM_INTERFACE_ENTRY_IID(IID_IXBRProject,IXBRProject)
   COM_INTERFACE_ENTRY_IID(IID_IXBRRatingSpecification,IXBRRatingSpecification)
   COM_INTERFACE_ENTRY_IID(IID_IXBRProjectEdit,IXBRProjectEdit)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CProjectAgentImp)
   CONNECTION_POINT_ENTRY( IID_IXBRProjectEventSink )
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

// IXBRProject
public:
   virtual void SetProjectName(LPCTSTR strName);
   virtual LPCTSTR GetProjectName();

   virtual xbrTypes::PierConnectionType GetPierType();
   virtual void SetPierType(xbrTypes::PierConnectionType pierType);

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

   virtual IndexType GetLiveLoadReactionCount(pgsTypes::LiveLoadType liveLoadType);
   virtual void SetLiveLoadReactions(pgsTypes::LiveLoadType liveLoadType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM);
   virtual std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(pgsTypes::LiveLoadType liveLoadType);

   virtual void SetModE(Float64 Ec);
   virtual Float64 GetModE();
   virtual void SetFc(Float64 fc);
   virtual Float64 GetFc();

   virtual void SetXBeamDimensions(pgsTypes::PierSideType side,Float64 height,Float64 taperHeight,Float64 taperLength);
   virtual void GetXBeamDimensions(pgsTypes::PierSideType side,Float64* pHeight,Float64* pTaperHeight,Float64* pTaperLength);
   virtual void SetXBeamWidth(Float64 width);
   virtual Float64 GetXBeamWidth();
   virtual void SetXBeamOverhang(pgsTypes::PierSideType side,Float64 overhang);
   virtual void SetXBeamOverhangs(Float64 leftOverhang,Float64 rightOverhang);
   virtual Float64 GetXBeamOverhang(pgsTypes::PierSideType side);
   virtual void GetXBeamOverhangs(Float64* pLeftOverhang,Float64* pRightOverhang);

   virtual void SetColumns(IndexType nColumns,Float64 height,CColumnData::ColumnHeightMeasurementType measure,Float64 spacing);
   virtual IndexType GetColumnCount();
   virtual Float64 GetColumnHeight(IndexType colIdx);
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType();
   virtual xbrTypes::ColumnBaseType GetColumnBaseType(IndexType colIdx);
   virtual Float64 GetSpacing(IndexType spaceIdx);
   virtual void SetColumnShape(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2);
   virtual void GetColumnShape(CColumnData::ColumnShapeType* pShapeType,Float64* pD1,Float64* pD2);

   virtual void SetTransverseLocation(ColumnIndexType colIdx,Float64 offset,pgsTypes::OffsetMeasurementType measure);
   virtual void GetTransverseLocation(ColumnIndexType* pColIdx,Float64* pOffset,pgsTypes::OffsetMeasurementType* pMeasure);

   virtual Float64 GetXBeamLength();

   virtual IndexType GetRebarRowCount();
   virtual void AddRebarRow(xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,Int16 nBars,Float64 spacing);
   virtual void SetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,Int16 nBars,Float64 spacing);
   virtual void GetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,Int16* pnBars,Float64* pSpacing);
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

   //virtual void RateForShear(pgsTypes::LoadRatingType ratingType,bool bRateForShear) = 0;
   //virtual bool RateForShear(pgsTypes::LoadRatingType ratingType) = 0;

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

#ifdef _DEBUG
   bool AssertValid() const;
#endif//

private:
   DECLARE_EAF_AGENT_DATA;

   CAgentCmdTarget m_CommandTarget;

   std::auto_ptr<XBeamRate::XBeamRate> m_XBeamRateXML;

   friend CProxyIXBRProjectEventSink<CProjectAgentImp>;

   void CreateMenus();
   void RemoveMenus();

   HRESULT ConvertToBaseUnits();

   void SetLiveLoadReactions(const std::vector<std::pair<std::_tstring,Float64>>& vLLIM,XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions);
   std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(const XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions);
};

OBJECT_ENTRY_AUTO(CLSID_ProjectAgent, CProjectAgentImp)

