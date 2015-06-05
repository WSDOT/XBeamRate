#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <PgsExt\ColumnData.h>
#include <XBeamRateExt\LongitudinalRebarData.h>
#include <XBeamRateExt\StirrupData.h>
#include <XBeamRateExt\BearingLineData.h>
#include <PgsExt\ConcreteMaterial.h>

class XBREXTCLASS xbrPierData
{
public:
   xbrPierData();
   xbrPierData(const xbrPierData& rOther);
   virtual ~xbrPierData();

   xbrPierData& operator = (const xbrPierData& rOther);

   void SetID(PierIDType id);
   PierIDType GetID() const;

   // Sets/Get the pier skew angle as an input string (e.g. "15 01 14 L")
   void SetSkew(LPCTSTR strSkew);
   LPCTSTR GetSkew() const;

   // Set/Get the deck elevation at the alignment line
   void SetDeckElevation(Float64 elev);
   Float64 GetDeckElevation() const;
   Float64& GetDeckElevation();

   // Set/Get the offset from the alignment to the crown point
   void SetCrownPointOffset(Float64 cpo);
   Float64 GetCrownPointOffset() const;
   Float64& GetCrownPointOffset();

   // Set/Get the offset from the alignment to the bridge line
   void SetBridgeLineOffset(Float64 blo);
   Float64 GetBridgeLineOffset() const;
   Float64& GetBridgeLineOffset();

   // Defines the general superstructure connectivity at this pier
   void SetSuperstructureConnectionType(xbrTypes::SuperstructureConnectionType type);
   xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType() const;
   xbrTypes::SuperstructureConnectionType& GetSuperstructureConnectionType();

   // Defines the datum for measuring the curb line locations
   pgsTypes::OffsetMeasurementType GetCurbLineDatum() const;
   pgsTypes::OffsetMeasurementType& GetCurbLineDatum();
   void SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType);

   // Location of curb lines measured from the curb line datum
   void SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO);
   void GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO) const;
   Float64& GetLeftCurbLineOffset();
   Float64& GetRightCurbLineOffset();

   // Set/Get the crown slope on either side of the crown point
   // Slope is measured in the plane of the pier
   void SetCrownSlope(Float64 sLeft,Float64 sRight);
   void GetCrownSlope(Float64* psLeft,Float64* psRight) const;
   Float64& GetLeftCrownSlope();
   Float64& GetRightCrownSlope();

   // Set/Get the diaphragm dimensions
   // w = width of the diaphragm. at expansion piers, w is the sum of the left and right diaphragm
   // h = height of the diaphragm from the top of deck to the top of the lower cross beam
   void SetDiaphragmDimensions(Float64 h,Float64 w);
   void GetDiaphragmDimensions(Float64* ph,Float64* pw);
   Float64& GetDiaphragmHeight();
   Float64& GetDiaphragmWidth();

   void SetLowerXBeamDimensions(Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w);
   void GetLowerXBeamDimensions(Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw);
   Float64& GetH1();
   Float64& GetH2();
   Float64& GetH3();
   Float64& GetH4();
   Float64& GetX1();
   Float64& GetX2();
   Float64& GetW();

   void SetColumnLayout(IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s);
   void GetColumnLayout(IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps);
   IndexType& GetColumnCount();
   pgsTypes::OffsetMeasurementType& GetColumnLayoutDatum();
   IndexType& GetRefColumnIndex();
   Float64& GetRefColumnOffset();
   Float64& GetX3();
   Float64& GetX4();
   Float64& GetColumnSpacing();

   void SetColumnProperties(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H);
   void GetColumnProperties(CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH);
   CColumnData::ColumnShapeType& GetColumnShape();
   Float64& GetD1();
   Float64& GetD2();
   CColumnData::ColumnHeightMeasurementType& GetColumnHeightMeasure();
   Float64& GetColumnHeight();
   
   pgsTypes::ConditionFactorType GetConditionFactorType() const;
   pgsTypes::ConditionFactorType& GetConditionFactorType();
   void SetConditionFactorType(pgsTypes::ConditionFactorType conditionFactorType);

   Float64 GetConditionFactor() const;
   Float64& GetConditionFactor();
   void SetConditionFactor(Float64 conditionFactor);

   // Material
   void SetRebarMaterial(matRebar::Type type,matRebar::Grade grade);
   void GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade) const;
   matRebar::Type& GetRebarType();
   matRebar::Grade& GetRebarGrade();

   void SetConcreteMaterial(const CConcreteMaterial& concrete);
   CConcreteMaterial& GetConcreteMaterial();
   const CConcreteMaterial& GetConcreteMaterial() const;

   void SetLongitudinalRebar(const xbrLongitudinalRebarData& rebarData);
   xbrLongitudinalRebarData& GetLongitudinalRebar();
   const xbrLongitudinalRebarData& GetLongitudinalRebar() const;

   void SetLowerXBeamStirrups(const xbrStirrupData& stirrupData);
   xbrStirrupData& GetLowerXBeamStirrups();
   const xbrStirrupData& GetLowerXBeamStirrups() const;

   void SetFullDepthStirrups(const xbrStirrupData& stirrupData);
   xbrStirrupData& GetFullDepthStirrups();
   const xbrStirrupData& GetFullDepthStirrups() const;

   void SetBearingLineCount(IndexType nBearingLines);
   IndexType GetBearingLineCount() const;

   void SetBearingLineData(IndexType brgLineIdx,const xbrBearingLineData& bearingLineData);
   xbrBearingLineData& GetBearingLineData(IndexType brgLineIdx);
   const xbrBearingLineData& GetBearingLineData(IndexType brgLineIdx) const;

   void SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 s);
   Float64 GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx) const;
   
   void SetBearingCount(IndexType brgLineIdx,IndexType nBearings);
   IndexType GetBearingCount(IndexType brgLineIdx) const;

   Float64 GetXBeamLength() const;

   HRESULT Load(IStructuredLoad* pStrLoad);
	HRESULT Save(IStructuredSave* pStrSave);

protected:
   void MakeCopy(const xbrPierData& rOther);
   virtual void MakeAssignment(const xbrPierData& rOther);

protected:
   PierIDType m_ID;
   std::_tstring m_strSkew;
   Float64 m_DeckElevation; // elevation of the top of deck at the alignment line
   Float64 m_CrownPointOffset; // offset from alignment to crown point
   Float64 m_BridgeLineOffset; // offset from alignment to bridge line
   xbrTypes::SuperstructureConnectionType m_ConnectionType;
   pgsTypes::OffsetMeasurementType m_CurbLineDatum;
   Float64 m_LeftCurbOffset, m_RightCurbOffset; 
   Float64 m_LeftCrownSlope, m_RightCrownSlope;
   Float64 m_H, m_W; // diaphragm dimensions

   // Lower X-Beam Dimensions
   Float64 m_H1, m_H2, m_H3, m_H4;
   Float64 m_X1, m_X2;
   Float64 m_XW;

   // Column Layout
   IndexType m_nColumns;
   IndexType m_RefColumnIdx;
   Float64 m_RefColumnOffset;
   pgsTypes::OffsetMeasurementType m_RefColumnDatum;
   Float64 m_X3, m_X4;
   Float64 m_S;
   CColumnData::ColumnHeightMeasurementType m_ColumnMeasurementType;
   Float64 m_ColumnHeight; // height or bottom elevation, depending on m_ColumnMeasurementType
   CColumnData::ColumnShapeType m_ColumnShape;
   Float64 m_D1; // Radius if circle, width in plane of pier if rectangular
   Float64 m_D2; // Ignored if circle, thickness normal to plane of pier if rectangular

   // Load Rating Condition
   pgsTypes::ConditionFactorType m_ConditionFactorType;
   Float64 m_ConditionFactor;

   // Materials
   matRebar::Type m_RebarType;
   matRebar::Grade m_RebarGrade;
   CConcreteMaterial m_Concrete;

   // Rebar
   xbrLongitudinalRebarData m_LongitudinalRebar;
   xbrStirrupData m_LowerXBeamStirrups;
   xbrStirrupData m_FullDepthStirrups;

   // Bearings
   std::vector<xbrBearingLineData> m_vBearingLines;
};
