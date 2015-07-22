#pragma once

#include <PgsExt\ColumnData.h>
#include <XBeamRateExt\PierData.h>

#include <boost\shared_ptr.hpp>

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
   PierIDType pierID is the pier identifier. For stand alone use INVALID_ID
*****************************************************************************/
// {2600A729-D7E6-44f6-9F9B-DF086FF9E53B}
DEFINE_GUID(IID_IXBRProject, 
0x2600a729, 0xd7e6, 0x44f6, 0x9f, 0x9b, 0xdf, 0x8, 0x6f, 0xf9, 0xe5, 0x3b);
interface IXBRProject : IUnknown
{
   virtual void SetPierData(const xbrPierData& pierData) = 0;
   virtual const xbrPierData& GetPierData(PierIDType pierID) = 0;

   virtual xbrTypes::SuperstructureConnectionType GetPierType(PierIDType pierID) = 0;
   virtual void SetPierType(PierIDType pierID,xbrTypes::SuperstructureConnectionType pierType) = 0;

   // Elevation of the deck on the alignment at the CL Pier
   virtual void SetDeckElevation(PierIDType pierID,Float64 deckElevation) = 0;
   virtual Float64 GetDeckElevation(PierIDType pierID) = 0;

   // Gross thickness of the deck at the CL Pier
   virtual void SetDeckThickness(PierIDType pierID,Float64 tDeck) = 0;
   virtual Float64 GetDeckThickness(PierIDType pierID) = 0;

   // Distance from alignment to crown point.
   virtual void SetCrownPointOffset(PierIDType pierID,Float64 cpo) = 0;
   virtual Float64 GetCrownPointOffset(PierIDType pierID) = 0;

   // Distance form alignemnt to bridge line
   virtual void SetBridgeLineOffset(PierIDType pierID,Float64 blo) = 0;
   virtual Float64 GetBridgeLineOffset(PierIDType pierID) = 0;

   // Orientation of the pier
   virtual void SetOrientation(PierIDType pierID,LPCTSTR strOrientation) = 0;
   virtual LPCTSTR GetOrientation(PierIDType pierID) = 0;

   // Sets the basis for the curb lines
   virtual pgsTypes::OffsetMeasurementType GetCurbLineDatum(PierIDType pierID) = 0;
   virtual void SetCurbLineDatum(PierIDType pierID,pgsTypes::OffsetMeasurementType datumType) = 0;

   virtual void SetCurbLineOffset(PierIDType pierID,Float64 leftCLO,Float64 rightCLO) = 0;
   virtual void GetCurbLineOffset(PierIDType pierID,Float64* pLeftCLO,Float64* pRightCLO) = 0;

   virtual void SetCrownSlopes(PierIDType pierID,Float64 sl,Float64 sr) = 0;
   virtual void GetCrownSlopes(PierIDType pierID,Float64* psl,Float64* psr) = 0;

   virtual void GetDiaphragmDimensions(PierIDType pierID,Float64* pH,Float64* pW) = 0;
   virtual void SetDiaphragmDimensions(PierIDType pierID,Float64 H,Float64 W) = 0;

   // Number of bearing lines at the pier. Valid values are 1 and 2.
   // Use 1 when girders are continuous (e.g. spliced girder, steel girders, etc)
   // Use 2 when simple span girders are made continuous (or are just simple spans)
   virtual IndexType GetBearingLineCount(PierIDType pierID) = 0;
   virtual void SetBearingLineCount(PierIDType pierID,IndexType nBearingLines) = 0;

   // Number of bearings on a bearing line. Some beam types, such as U-beams, use two
   // bearings and others, such as I-beams, use one bearing. This is the total number
   // of points of bearing along the bearing line
   virtual IndexType GetBearingCount(PierIDType pierID,IndexType brgLineIdx) = 0;
   virtual void SetBearingCount(PierIDType pierID,IndexType brgLineIdx,IndexType nBearings) = 0;

   // Spacing between the specified bearing and the bearing to its right
   virtual Float64 GetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx) = 0;
   virtual void SetBearingSpacing(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 spacing) = 0;

   // Bearing reactions
   virtual void SetBearingReactionType(PierIDType pierID,IndexType brgLineIdx,xbrTypes::ReactionLoadType brgReactionType) = 0;
   virtual xbrTypes::ReactionLoadType GetBearingReactionType(PierIDType pierID,IndexType brgLineIdx) = 0;
   virtual void SetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW,Float64 CR,Float64 SH,Float64 PS,Float64 RE,Float64 W) = 0;
   virtual void GetBearingReactions(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW,Float64* pCR,Float64* pSH,Float64* pPS,Float64* pRE,Float64* pW) = 0;

   // Reference bearing
   virtual void GetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum) = 0;
   virtual void SetReferenceBearing(PierIDType pierID,IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum) = 0;

   // Live Load Reactions per lane
   virtual IndexType GetLiveLoadReactionCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType) = 0;
   virtual void SetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM) = 0;
   virtual std::vector<std::pair<std::_tstring,Float64>> GetLiveLoadReactions(PierIDType pierID,pgsTypes::LoadRatingType ratingType) = 0;
   virtual LPCTSTR GetLiveLoadName(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx) = 0;
   virtual Float64 GetLiveLoadReaction(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx) = 0;

   // Material properties
   virtual void SetRebarMaterial(PierIDType pierID,matRebar::Type type,matRebar::Grade grade) = 0;
   virtual void GetRebarMaterial(PierIDType pierID,matRebar::Type* pType,matRebar::Grade* pGrade) = 0;

   virtual void SetConcrete(PierIDType pierID,const CConcreteMaterial& concrete) = 0;
   virtual const CConcreteMaterial& GetConcrete(PierIDType pierID) = 0;

   virtual void SetLowerXBeamDimensions(PierIDType pierID,Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 x3,Float64 x4,Float64 w) = 0;
   virtual void GetLowerXBeamDimensions(PierIDType pierID,Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* px3,Float64* px4,Float64* pw) = 0;

   virtual Float64 GetXBeamLeftOverhang(PierIDType pierID) = 0;
   virtual Float64 GetXBeamRightOverhang(PierIDType pierID) = 0;
   virtual Float64 GetXBeamWidth(PierIDType pierID) = 0;

   virtual void SetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset) = 0;
   virtual void GetRefColumnLocation(PierIDType pierID,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset) = 0;
   virtual IndexType GetColumnCount(PierIDType pierID) = 0;
   virtual Float64 GetColumnHeight(PierIDType pierID,ColumnIndexType colIdx) = 0;
   virtual CColumnData::ColumnHeightMeasurementType GetColumnHeightMeasurementType(PierIDType pierID,ColumnIndexType colIdx) = 0;
   virtual Float64 GetColumnSpacing(PierIDType pierID,SpacingIndexType spaceIdx) = 0;
   virtual pgsTypes::ColumnFixityType GetColumnFixity(PierIDType pierID,ColumnIndexType colIdx) = 0;

   virtual void SetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H) = 0;
   virtual void GetColumnProperties(PierIDType pierID,ColumnIndexType colIdx,CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH) = 0;

   // Longitudinal Rebar
   virtual const xbrLongitudinalRebarData& GetLongitudinalRebar(PierIDType pierID) = 0;
   virtual void SetLongitudinalRebar(PierIDType pierID,const xbrLongitudinalRebarData& rebar) = 0;

   // Stirrups
   virtual void SetLowerXBeamStirrups(PierIDType pierID,const xbrStirrupData& stirrups) = 0;
   virtual const xbrStirrupData& GetLowerXBeamStirrups(PierIDType pierID) = 0;
   virtual void SetFullDepthStirrups(PierIDType pierID,const xbrStirrupData& stirrups) = 0;
   virtual const xbrStirrupData& GetFullDepthStirrups(PierIDType pierID) = 0;


   virtual void SetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor) = 0;
   virtual void GetConditionFactor(PierIDType pierID,pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor) = 0;
   virtual Float64 GetConditionFactor(PierIDType pierID) = 0;

   virtual void SetDCLoadFactor(Float64 dc) = 0;
   virtual Float64 GetDCLoadFactor() = 0;

   virtual void SetDWLoadFactor(Float64 dw) = 0;
   virtual Float64 GetDWLoadFactor() = 0;

   virtual void SetCRLoadFactor(Float64 cr) = 0;
   virtual Float64 GetCRLoadFactor() = 0;

   virtual void SetSHLoadFactor(Float64 sh) = 0;
   virtual Float64 GetSHLoadFactor() = 0;

   virtual void SetPSLoadFactor(Float64 ps) = 0;
   virtual Float64 GetPSLoadFactor() = 0;

   virtual void SetRELoadFactor(Float64 re) = 0;
   virtual Float64 GetRELoadFactor() = 0;

   virtual void SetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType,Float64 ll) = 0;
   virtual Float64 GetLiveLoadFactor(PierIDType pierID,pgsTypes::LoadRatingType ratingType) = 0;
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
   IXBREvents

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
   IXBREventsSink

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

/*****************************************************************************
INTERFACE
   IXBRUIEvents

   Interface to control events in the user interface

DESCRIPTION
   Interface to control events in the user interface
*****************************************************************************/
DEFINE_GUID(IID_IXBRUIEvents, 
0xb2734352, 0xc900, 0x4c92, 0x9b, 0x68, 0x94, 0x84, 0xfc, 0x58, 0x1a, 0x1b);
interface IXBRUIEvents : IUnknown
{
   virtual void HoldEvents(bool bHold=true) = 0;
   virtual void FirePendingEvents() = 0;
   virtual void CancelPendingEvents() = 0;
   virtual void FireEvent(CView* pSender = NULL,LPARAM lHint = 0,boost::shared_ptr<CObject> pHint = boost::shared_ptr<CObject>()) = 0;
};
