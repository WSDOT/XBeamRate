#pragma once

#include <PgsExt\ColumnData.h>
#include <XBeamRateExt\PierData.h>

/*****************************************************************************
INTERFACE
   IXBRProjectProperties

   Interface to edit project properties.

DESCRIPTION
   Interface to edit project properties.
*****************************************************************************/
// {E27FD663-261C-40de-BFAD-01B03D98324D}
DEFINE_GUID(IID_IXBRProjectProperties, 
0xe27fd663, 0x261c, 0x40de, 0xbf, 0xad, 0x1, 0xb0, 0x3d, 0x98, 0x32, 0x4d);
interface IXBRProjectProperties : IUnknown
{
   virtual LPCTSTR GetBridgeName() const = 0;
   virtual void SetBridgeName(LPCTSTR name) = 0;
   virtual LPCTSTR GetBridgeID() const = 0;
   virtual void SetBridgeID(LPCTSTR bid) = 0;
   virtual PierIndexType GetPierIndex() = 0;
   virtual void SetPierIndex(PierIndexType pierIdx) = 0;
   virtual LPCTSTR GetJobNumber() const = 0;
   virtual void SetJobNumber(LPCTSTR jid) = 0;
   virtual LPCTSTR GetEngineer() const = 0;
   virtual void SetEngineer(LPCTSTR eng) = 0;
   virtual LPCTSTR GetCompany() const = 0;
   virtual void SetCompany(LPCTSTR company) = 0;
   virtual LPCTSTR GetComments() const = 0;
   virtual void SetComments(LPCTSTR comments) = 0;
};

/*****************************************************************************
INTERFACE
   IProjectPropertiesEventSink

   Callback interface for project properties.

DESCRIPTION
   Callback interface for project properties.
*****************************************************************************/
// {50C5E910-0941-4135-8603-A8D7192F19CB}
DEFINE_GUID(IID_IXBRProjectPropertiesEventSink, 
0x50c5e910, 0x941, 0x4135, 0x86, 0x3, 0xa8, 0xd7, 0x19, 0x2f, 0x19, 0xcb);
interface IXBRProjectPropertiesEventSink : IUnknown
{
   virtual HRESULT OnProjectPropertiesChanged() = 0;
};

/*****************************************************************************
INTERFACE
   IXBRProject

DESCRIPTION
   Interface for getting this application going.
*****************************************************************************/
// {2600A729-D7E6-44f6-9F9B-DF086FF9E53B}
DEFINE_GUID(IID_IXBRProject, 
0x2600a729, 0xd7e6, 0x44f6, 0x9f, 0x9b, 0xdf, 0x8, 0x6f, 0xf9, 0xe5, 0x3b);
interface IXBRProject : IUnknown
{
   virtual void SetPierData(const xbrPierData& pierData) = 0;
   virtual const xbrPierData& GetPierData() = 0;

   virtual xbrTypes::SuperstructureConnectionType GetPierType() = 0;
   virtual void SetPierType(xbrTypes::SuperstructureConnectionType pierType) = 0;

   // Elevation of the deck on the alignment at the CL Pier
   virtual void SetDeckElevation(Float64 deckElevation) = 0;
   virtual Float64 GetDeckElevation() = 0;

   // Distance from alignment to crown point.
   virtual void SetCrownPointOffset(Float64 cpo) = 0;
   virtual Float64 GetCrownPointOffset() = 0;

   // Distance form alignemnt to bridge line
   virtual void SetBridgeLineOffset(Float64 blo) = 0;
   virtual Float64 GetBridgeLineOffset() = 0;

   // Orientation of the pier
   virtual void SetOrientation(LPCTSTR strOrientation) = 0;
   virtual LPCTSTR GetOrientation() = 0;

   // Sets the basis for the curb lines
   virtual pgsTypes::OffsetMeasurementType GetCurbLineDatum() = 0;
   virtual void SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType) = 0;

   virtual void SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO) = 0;
   virtual void GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO) = 0;

   virtual void SetCrownSlopes(Float64 sl,Float64 sr) = 0;
   virtual void GetCrownSlopes(Float64* psl,Float64* psr) = 0;

   virtual void GetDiaphragmDimensions(Float64* pH,Float64* pW) = 0;
   virtual void SetDiaphragmDimensions(Float64 H,Float64 W) = 0;

   // Number of bearing lines at the pier. Valid values are 1 and 2.
   // Use 1 when girders are continuous (e.g. spliced girder, steel girders, etc)
   // Use 2 when simple span girders are made continuous (or are just simple spans)
   virtual IndexType GetBearingLineCount() = 0;
   virtual void SetBearingLineCount(IndexType nBearingLines) = 0;

   // Number of bearings on a bearing line. Some beam types, such as U-beams, use two
   // bearings and others, such as I-beams, use one bearing. This is the total number
   // of points of bearing along the bearing line
   virtual IndexType GetBearingCount(IndexType brgLineIdx) = 0;
   virtual void SetBearingCount(IndexType brgLineIdx,IndexType nBearings) = 0;

   // Spacing between the specified bearing and the bearing to its right
   virtual Float64 GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx) = 0;
   virtual void SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 spacing) = 0;

   // Bearing reactions
   virtual void SetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW) = 0;
   virtual void GetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW) = 0;

   // Reference bearing
   virtual void GetReferenceBearing(IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum) = 0;
   virtual void SetReferenceBearing(IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum) = 0;

   // Live Load Reactions per lane
   virtual IndexType GetLiveLoadReactionCount(pgsTypes::LoadRatingType ratingType) = 0;
   virtual void SetLiveLoadReactions(pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM) = 0;
   virtual std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(pgsTypes::LoadRatingType ratingType) = 0;
   virtual LPCTSTR GetLiveLoadName(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx) = 0;
   virtual Float64 GetLiveLoadReaction(pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx) = 0;

   // Material properties
   virtual void SetRebarMaterial(matRebar::Type type,matRebar::Grade grade) = 0;
   virtual void GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade) = 0;

   virtual void SetConcrete(const xbrConcreteMaterial& concrete) = 0;
   virtual const xbrConcreteMaterial& GetConcrete() = 0;

   virtual void SetLowerXBeamDimensions(Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w) = 0;
   virtual void GetLowerXBeamDimensions(Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw) = 0;
   virtual Float64 GetXBeamLeftOverhang() = 0;
   virtual Float64 GetXBeamRightOverhang() = 0;
   virtual Float64 GetXBeamWidth() = 0;

   virtual void SetColumnLayout(IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s) = 0;
   virtual void GetColumnLayout(IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps) = 0;
   virtual IndexType GetColumnCount() = 0;
   virtual Float64 GetColumnHeight() = 0;
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType() = 0;
   virtual Float64 GetColumnSpacing() = 0;

   virtual void SetColumnProperties(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H) = 0;
   virtual void GetColumnProperties(CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH) = 0;

   virtual Float64 GetXBeamLength() = 0;

   // Longitudinal Rebar
   virtual IndexType GetRebarRowCount() = 0;
   virtual void AddRebarRow(xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing) = 0;
   virtual void SetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,IndexType nBars,Float64 spacing) = 0;
   virtual void GetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,IndexType* pnBars,Float64* pSpacing) = 0;
   virtual void RemoveRebarRow(IndexType rowIdx) = 0;
   virtual void RemoveRebarRows() = 0;

   virtual void SetConditionFactor(pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor) = 0;
   virtual void GetConditionFactor(pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor) = 0;
   virtual Float64 GetConditionFactor() = 0;

   virtual void SetDCLoadFactor(Float64 dc) = 0;
   virtual Float64 GetDCLoadFactor() = 0;

   virtual void SetDWLoadFactor(Float64 dw) = 0;
   virtual Float64 GetDWLoadFactor() = 0;

   virtual void SetLiveLoadFactor(pgsTypes::LoadRatingType ratingType,Float64 ll) = 0;
   virtual Float64 GetLiveLoadFactor(pgsTypes::LoadRatingType ratingType) = 0;
};

/*****************************************************************************
INTERFACE
   IXBRProjectEventSink

DESCRIPTION
   Callback interface for changes to the project data
*****************************************************************************/
// {9DD03140-A788-4e46-A283-0B343956A619}
DEFINE_GUID(IID_IXBRProjectEventSink, 
0x9dd03140, 0xa788, 0x4e46, 0xa2, 0x83, 0xb, 0x34, 0x39, 0x56, 0xa6, 0x19);
interface IXBRProjectEventSink : IUnknown
{
   virtual HRESULT OnProjectChanged() = 0;
};

/*****************************************************************************
INTERFACE
   IXBRProjectEdit

DESCRIPTION
   Interface to enable programatic activation of the editing UI
*****************************************************************************/
// {BBA7F95B-A5DE-4c2a-BB68-19983F308767}
DEFINE_GUID(IID_IXBRProjectEdit, 
0xbba7f95b, 0xa5de, 0x4c2a, 0xbb, 0x68, 0x19, 0x98, 0x3f, 0x30, 0x87, 0x67);
interface IXBRProjectEdit : IUnknown
{
   virtual void EditPier(int nPage) = 0;
};


/*****************************************************************************
INTERFACE
   IEvents

   Interface to control events

DESCRIPTION
   Interface to control events
*****************************************************************************/
// {F0674DBA-E867-4692-B214-FDB23F04685B}
DEFINE_GUID(IID_IXBREvents, 
0xf0674dba, 0xe867, 0x4692, 0xb2, 0x14, 0xfd, 0xb2, 0x3f, 0x4, 0x68, 0x5b);
interface IXBREvents : IUnknown
{
   virtual void HoldEvents() = 0;
   virtual void FirePendingEvents() = 0;
   virtual void CancelPendingEvents() = 0;
};

/*****************************************************************************
INTERFACE
   IEventsSink

   Interface to control events

DESCRIPTION
   Interface to control events
*****************************************************************************/
// {46E15C3D-F822-4b97-A7E3-6ED0CE5FF37E}
DEFINE_GUID(IID_IXBREventsSink, 
0x46e15c3d, 0xf822, 0x4b97, 0xa7, 0xe3, 0x6e, 0xd0, 0xce, 0x5f, 0xf3, 0x7e);
interface IXBREventsSink : IUnknown
{
   virtual HRESULT OnHoldEvents() = 0;
   virtual HRESULT OnFirePendingEvents() = 0;
   virtual HRESULT OnCancelPendingEvents() = 0;
};
