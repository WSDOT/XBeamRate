#include "stdafx.h"
#include <XBeamRateExt\PierData.h>

xbrPierData::xbrPierData()
{
   m_ID = INVALID_ID;
   m_strSkew = _T("00 00 0.0 L");
   m_DeckElevation = 0;
   m_CrownPointOffset = 0;
   m_BridgeLineOffset = 0;
   m_ConnectionType = xbrTypes::pctIntegral;

   m_CurbLineDatum = pgsTypes::omtAlignment;
   m_LeftCurbOffset = ::ConvertToSysUnits(10.0,unitMeasure::Feet);
   m_RightCurbOffset = ::ConvertToSysUnits(10.0,unitMeasure::Feet);
   m_LeftCrownSlope = -0.02;
   m_RightCrownSlope = -0.02;
   m_H = ::ConvertToSysUnits(8,unitMeasure::Feet);
   m_W = ::ConvertToSysUnits(5,unitMeasure::Feet);

   m_H1 = ::ConvertToSysUnits(5,unitMeasure::Feet);
   m_H2 = ::ConvertToSysUnits(1,unitMeasure::Feet);
   m_H3 = ::ConvertToSysUnits(5,unitMeasure::Feet);
   m_H4 = ::ConvertToSysUnits(1,unitMeasure::Feet);
   m_X1 = ::ConvertToSysUnits(3,unitMeasure::Feet);
   m_X2 = ::ConvertToSysUnits(3,unitMeasure::Feet);
   m_XW = ::ConvertToSysUnits(5,unitMeasure::Feet);

   m_nColumns = 2;
   m_RefColumnIdx = 0;
   m_RefColumnOffset = -::ConvertToSysUnits(5,unitMeasure::Feet);
   m_RefColumnDatum = pgsTypes::omtAlignment;
   m_X3 = ::ConvertToSysUnits(5,unitMeasure::Feet);
   m_X4 = ::ConvertToSysUnits(5,unitMeasure::Feet);
   m_S = ::ConvertToSysUnits(10,unitMeasure::Feet);
   m_ColumnMeasurementType = CColumnData::chtHeight;
   m_ColumnHeight = ::ConvertToSysUnits(30,unitMeasure::Feet);
   m_ColumnShape = CColumnData::cstCircle;
   m_D1 = ::ConvertToSysUnits(3,unitMeasure::Feet);
   m_D2 = ::ConvertToSysUnits(3,unitMeasure::Feet);

   // Load Rating Condition
   m_ConditionFactorType = pgsTypes::cfGood;
   m_ConditionFactor = 1.0;

   // Materials
   m_RebarType = matRebar::A615;
   m_RebarGrade = matRebar::Grade60;

   // Rebar
   xbrLongitudinalRebarData::RebarRow row;
   row.Datum = xbrTypes::Bottom;
   row.BarSize = matRebar::bs11;
   row.BarSpacing = ::ConvertToSysUnits(6,unitMeasure::Inch);
   row.Cover = ::ConvertToSysUnits(2,unitMeasure::Inch);
   row.NumberOfBars = 5;
   m_LongitudinalRebar.RebarRows.push_back(row);

   // Bearings
   xbrBearingLineData bearingLine;
   bearingLine.SetReferenceBearing(pgsTypes::omtAlignment,0,-::ConvertToSysUnits(6,unitMeasure::Feet));
   bearingLine.SetBearingCount(3);
   bearingLine.SetSpacing(0,::ConvertToSysUnits(6,unitMeasure::Feet));
   bearingLine.SetSpacing(1,::ConvertToSysUnits(6,unitMeasure::Feet));
   m_vBearingLines.push_back(bearingLine);
}

xbrPierData::xbrPierData(const xbrPierData& other)
{
   MakeCopy(other);
}

xbrPierData::~xbrPierData()
{
}

xbrPierData& xbrPierData::operator= (const xbrPierData& rOther)
{
   if( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

void xbrPierData::SetID(PierIDType id)
{
   m_ID = id;
}

PierIDType xbrPierData::GetID() const
{
   return m_ID;
}

void xbrPierData::SetSkew(LPCTSTR strSkew)
{
   m_strSkew = strSkew;
}

LPCTSTR xbrPierData::GetSkew() const
{
   return m_strSkew.c_str();
}

void xbrPierData::SetDeckElevation(Float64 elev)
{
   m_DeckElevation = elev;
}

Float64 xbrPierData::GetDeckElevation() const
{
   return m_DeckElevation;
}

Float64& xbrPierData::GetDeckElevation()
{
   return m_DeckElevation;
}

void xbrPierData::SetCrownPointOffset(Float64 cpo)
{
   m_CrownPointOffset = cpo;
}

Float64 xbrPierData::GetCrownPointOffset() const
{
   return m_CrownPointOffset;
}

Float64& xbrPierData::GetCrownPointOffset()
{
   return m_CrownPointOffset;
}

void xbrPierData::SetBridgeLineOffset(Float64 blo)
{
   m_BridgeLineOffset = blo;
}

Float64 xbrPierData::GetBridgeLineOffset() const
{
   return m_BridgeLineOffset;
}

Float64& xbrPierData::GetBridgeLineOffset()
{
   return m_BridgeLineOffset;
}

void xbrPierData::SetSuperstructureConnectionType(xbrTypes::SuperstructureConnectionType type)
{
   m_ConnectionType = type;
}

xbrTypes::SuperstructureConnectionType xbrPierData::GetSuperstructureConnectionType() const
{
   return m_ConnectionType;
}

xbrTypes::SuperstructureConnectionType& xbrPierData::GetSuperstructureConnectionType()
{
   return m_ConnectionType;
}

pgsTypes::OffsetMeasurementType xbrPierData::GetCurbLineDatum() const
{
   return m_CurbLineDatum;
}

pgsTypes::OffsetMeasurementType& xbrPierData::GetCurbLineDatum()
{
   return m_CurbLineDatum;
}

void xbrPierData::SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType)
{
   m_CurbLineDatum = datumType;
}

void xbrPierData::SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO)
{
   m_LeftCurbOffset = leftCLO;
   m_RightCurbOffset = rightCLO;
}

void xbrPierData::GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO) const
{
   *pLeftCLO = m_LeftCurbOffset;
   *pRightCLO = m_RightCurbOffset;
}

Float64& xbrPierData::GetLeftCurbLineOffset()
{
   return m_LeftCurbOffset;
}

Float64& xbrPierData::GetRightCurbLineOffset()
{
   return m_RightCurbOffset;
}

void xbrPierData::SetCrownSlope(Float64 sLeft,Float64 sRight)
{
   m_LeftCrownSlope = sLeft;
   m_RightCrownSlope = sRight;
}

void xbrPierData::GetCrownSlope(Float64* psLeft,Float64* psRight) const
{
   *psLeft = m_LeftCrownSlope;
   *psRight = m_RightCrownSlope;
}

Float64& xbrPierData::GetLeftCrownSlope()
{
   return m_LeftCrownSlope;
}

Float64& xbrPierData::GetRightCrownSlope()
{
   return m_RightCrownSlope;
}

void xbrPierData::SetDiaphragmDimensions(Float64 h,Float64 w)
{
   m_H = h;
   m_W = w;
}

void xbrPierData::GetDiaphragmDimensions(Float64* ph,Float64* pw)
{
   *ph = m_H;
   *pw = m_W;
}

Float64& xbrPierData::GetDiaphragmHeight()
{
   return m_H;
}

Float64& xbrPierData::GetDiaphragmWidth()
{
   return m_W;
}

void xbrPierData::SetLowerXBeamDimensions(Float64 h1,Float64 h2,Float64 h3,Float64 h4,Float64 x1,Float64 x2,Float64 w)
{
   m_H1 = h1;
   m_H2 = h2;
   m_H3 = h3;
   m_H4 = h4;
   m_X1 = x1;
   m_X2 = x2;
   m_XW = w;
}

void xbrPierData::GetLowerXBeamDimensions(Float64* ph1,Float64* ph2,Float64* ph3,Float64* ph4,Float64* px1,Float64* px2,Float64* pw)
{
   *ph1 = m_H1;
   *ph2 = m_H2;
   *ph3 = m_H3;
   *ph4 = m_H4;
   *px1 = m_X1;
   *px2 = m_X2;
   *pw  = m_XW;
}

Float64& xbrPierData::GetH1()
{
   return m_H1;
}

Float64& xbrPierData::GetH2()
{
   return m_H2;
}

Float64& xbrPierData::GetH3()
{
   return m_H3;
}

Float64& xbrPierData::GetH4()
{
   return m_H4;
}

Float64& xbrPierData::GetX1()
{
   return m_X1;
}

Float64& xbrPierData::GetX2()
{
   return m_X2;
}

Float64& xbrPierData::GetW()
{
   return m_XW;
}

void xbrPierData::SetColumnLayout(IndexType nColumns,pgsTypes::OffsetMeasurementType refColumnDatum,IndexType refColumnIdx,Float64 refColumnOffset,Float64 x3,Float64 x4,Float64 s)
{
   m_nColumns        = nColumns;
   m_RefColumnDatum  = refColumnDatum;
   m_RefColumnIdx    = refColumnIdx;
   m_RefColumnOffset = refColumnOffset;
   m_X3              = x3;
   m_X4              = x4;
   m_S               = s;
}

void xbrPierData::GetColumnLayout(IndexType* pnColumns,pgsTypes::OffsetMeasurementType* prefColumnDatum,IndexType* prefColumnIdx,Float64* prefColumnOffset,Float64* px3,Float64* px4,Float64* ps)
{
   *pnColumns        = m_nColumns;
   *prefColumnDatum  = m_RefColumnDatum;
   *prefColumnIdx    = m_RefColumnIdx;
   *prefColumnOffset = m_RefColumnOffset;
   *px3              = m_X3;
   *px4              = m_X4;
   *ps               = m_S;
}

IndexType& xbrPierData::GetColumnCount()
{
   return m_nColumns;
}

pgsTypes::OffsetMeasurementType& xbrPierData::GetColumnLayoutDatum()
{
   return m_RefColumnDatum;
}

IndexType& xbrPierData::GetRefColumnIndex()
{
   return m_RefColumnIdx;
}

Float64& xbrPierData::GetRefColumnOffset()
{
   return m_RefColumnOffset;
}

Float64& xbrPierData::GetX3()
{
   return m_X3;
}

Float64& xbrPierData::GetX4()
{
   return m_X4;
}

Float64& xbrPierData::GetColumnSpacing()
{
   return m_S;
}

void xbrPierData::SetColumnProperties(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2,CColumnData::ColumnHeightMeasurementType heightType,Float64 H)
{
   m_ColumnShape = shapeType;
   m_D1 = D1;
   m_D2 = D2;
   m_ColumnMeasurementType = heightType;
   m_ColumnHeight = H;
}

void xbrPierData::GetColumnProperties(CColumnData::ColumnShapeType* pshapeType,Float64* pD1,Float64* pD2,CColumnData::ColumnHeightMeasurementType* pheightType,Float64* pH)
{
   *pshapeType = m_ColumnShape;
   *pD1 = m_D1;
   *pD2 = m_D2;
   *pheightType = m_ColumnMeasurementType;
   *pH = m_ColumnHeight;
}

CColumnData::ColumnShapeType& xbrPierData::GetColumnShape()
{
   return m_ColumnShape;
}

Float64& xbrPierData::GetD1()
{
   return m_D1;
}

Float64& xbrPierData::GetD2()
{
   return m_D2;
}

CColumnData::ColumnHeightMeasurementType& xbrPierData::GetColumnHeightMeasure()
{
   return m_ColumnMeasurementType;
}

Float64& xbrPierData::GetColumnHeight()
{
   return m_ColumnHeight;
}

pgsTypes::ConditionFactorType xbrPierData::GetConditionFactorType() const
{
   return m_ConditionFactorType;
}

pgsTypes::ConditionFactorType& xbrPierData::GetConditionFactorType()
{
   return m_ConditionFactorType;
}

void xbrPierData::SetConditionFactorType(pgsTypes::ConditionFactorType conditionFactorType)
{
   m_ConditionFactorType = conditionFactorType;
}

Float64 xbrPierData::GetConditionFactor() const
{
   return m_ConditionFactor;
}

Float64& xbrPierData::GetConditionFactor()
{
   return m_ConditionFactor;
}

void xbrPierData::SetConditionFactor(Float64 conditionFactor)
{
   m_ConditionFactor = conditionFactor;
}

void xbrPierData::SetRebarMaterial(matRebar::Type type,matRebar::Grade grade)
{
   m_RebarType = type;
   m_RebarGrade = grade;
}

void xbrPierData::GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade) const
{
   *pType = m_RebarType;
   *pGrade = m_RebarGrade;
}

matRebar::Type& xbrPierData::GetRebarType()
{
   return m_RebarType;
}

matRebar::Grade& xbrPierData::GetRebarGrade()
{
   return m_RebarGrade;
}

void xbrPierData::SetConcreteMaterial(const CConcreteMaterial& concrete)
{
   m_Concrete = concrete;
}

CConcreteMaterial& xbrPierData::GetConcreteMaterial()
{
   return m_Concrete;
}

const CConcreteMaterial& xbrPierData::GetConcreteMaterial() const
{
   return m_Concrete;
}

void xbrPierData::SetLongitudinalRebar(const xbrLongitudinalRebarData& rebarData)
{
   m_LongitudinalRebar = rebarData;
}

xbrLongitudinalRebarData& xbrPierData::GetLongitudinalRebar()
{
   return m_LongitudinalRebar;
}

const xbrLongitudinalRebarData& xbrPierData::GetLongitudinalRebar() const
{
   return m_LongitudinalRebar;
}

void xbrPierData::SetLowerXBeamStirrups(const xbrStirrupData& stirrupData)
{
   m_LowerXBeamStirrups = stirrupData;
}

xbrStirrupData& xbrPierData::GetLowerXBeamStirrups()
{
   return m_LowerXBeamStirrups;
}

const xbrStirrupData& xbrPierData::GetLowerXBeamStirrups() const
{
   return m_LowerXBeamStirrups;
}

void xbrPierData::SetFullDepthStirrups(const xbrStirrupData& stirrupData)
{
   m_FullDepthStirrups = stirrupData;
}

xbrStirrupData& xbrPierData::GetFullDepthStirrups()
{
   return m_FullDepthStirrups;
}

const xbrStirrupData& xbrPierData::GetFullDepthStirrups() const
{
   return m_FullDepthStirrups;
}

void xbrPierData::SetBearingLineCount(IndexType nBearingLines)
{
   ATLASSERT(1 <= nBearingLines && nBearingLines <= 2);
   m_vBearingLines.resize(nBearingLines);
}

IndexType xbrPierData::GetBearingLineCount() const
{
   return m_vBearingLines.size();
}

void xbrPierData::SetBearingLineData(IndexType brgLineIdx,const xbrBearingLineData& bearingLineData)
{
   ATLASSERT(brgLineIdx < m_vBearingLines.size());
   m_vBearingLines[brgLineIdx] = bearingLineData;
}

xbrBearingLineData& xbrPierData::GetBearingLineData(IndexType brgLineIdx)
{
   ATLASSERT(brgLineIdx < m_vBearingLines.size());
   return m_vBearingLines[brgLineIdx];
}

const xbrBearingLineData& xbrPierData::GetBearingLineData(IndexType brgLineIdx) const
{
   ATLASSERT(brgLineIdx < m_vBearingLines.size());
   return m_vBearingLines[brgLineIdx];
}
   
void xbrPierData::SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 s)
{
   m_vBearingLines[brgLineIdx].SetSpacing(brgIdx,s);
}

Float64 xbrPierData::GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx) const
{
   return m_vBearingLines[brgLineIdx].GetSpacing(brgIdx);
}

void xbrPierData::SetBearingCount(IndexType brgLineIdx,IndexType nBearings)
{
   m_vBearingLines[brgLineIdx].SetBearingCount(nBearings);
}

IndexType xbrPierData::GetBearingCount(IndexType brgLineIdx) const
{
   return m_vBearingLines[brgLineIdx].GetBearingCount();
}

Float64 xbrPierData::GetXBeamLength() const
{
   return m_X3 + (m_nColumns-1)*m_S + m_X4;
}

HRESULT xbrPierData::Save(IStructuredSave* pStrSave)
{
   pStrSave->BeginUnit(_T("Pier"),1.0);
   pStrSave->put_Property(_T("ID"),CComVariant(m_ID));

   pStrSave->BeginUnit(_T("Layout"),1.0);
      pStrSave->put_Property(_T("SuperstructureConnectionType"),CComVariant(m_ConnectionType));
      pStrSave->put_Property(_T("Skew"),CComVariant(m_strSkew.c_str()));

      pStrSave->put_Property(_T("DeckElevation"),CComVariant(m_DeckElevation));
      pStrSave->put_Property(_T("CrownPointOffset"),CComVariant(m_CrownPointOffset));
      pStrSave->put_Property(_T("BridgeLineOffset"),CComVariant(m_BridgeLineOffset));

      pStrSave->put_Property(_T("CurbLineDatum"),CComVariant(m_CurbLineDatum));
      pStrSave->put_Property(_T("LeftCurbOffset"),CComVariant(m_LeftCurbOffset));
      pStrSave->put_Property(_T("RightCurbOffset"),CComVariant(m_RightCurbOffset));

      pStrSave->put_Property(_T("LeftCrownSlope"),CComVariant(m_LeftCrownSlope));
      pStrSave->put_Property(_T("RightCrownSlope"),CComVariant(m_RightCrownSlope));
   pStrSave->EndUnit(); // Layout

   pStrSave->BeginUnit(_T("Diaphragm"),1.0);
      pStrSave->put_Property(_T("H"),CComVariant(m_H));
      pStrSave->put_Property(_T("W"),CComVariant(m_W));
   pStrSave->EndUnit(); // Diaphragm

   pStrSave->BeginUnit(_T("LowerCrossBeam"),1.0);
      pStrSave->put_Property(_T("H1"),CComVariant(m_H1));
      pStrSave->put_Property(_T("H2"),CComVariant(m_H2));
      pStrSave->put_Property(_T("H3"),CComVariant(m_H3));
      pStrSave->put_Property(_T("H4"),CComVariant(m_H4));
      pStrSave->put_Property(_T("X1"),CComVariant(m_X1));
      pStrSave->put_Property(_T("X2"),CComVariant(m_X2));
      pStrSave->put_Property(_T("W"), CComVariant(m_XW));
   pStrSave->EndUnit(); // LowerCrossBeam

   pStrSave->BeginUnit(_T("Columns"),1.0);
      pStrSave->put_Property(_T("ColumnCount"),CComVariant(m_nColumns));
      pStrSave->put_Property(_T("RefColumDatum"),CComVariant(m_RefColumnDatum));
      pStrSave->put_Property(_T("RefColumn"),CComVariant(m_RefColumnIdx));
      pStrSave->put_Property(_T("RefColumnOffset"),CComVariant(m_RefColumnOffset));
      pStrSave->put_Property(_T("X3"),CComVariant(m_X3));
      pStrSave->put_Property(_T("X4"),CComVariant(m_X4));
      pStrSave->put_Property(_T("S"),CComVariant(m_S));
      pStrSave->put_Property(_T("Shape"),CComVariant(m_ColumnShape));
      pStrSave->put_Property(_T("D1"),CComVariant(m_D1));
      pStrSave->put_Property(_T("D2"),CComVariant(m_D2));
      pStrSave->put_Property(_T("HeightType"),CComVariant(m_ColumnMeasurementType));
      pStrSave->put_Property(_T("Height"),CComVariant(m_ColumnHeight));
   pStrSave->EndUnit(); // Columns

   m_Concrete.Save(pStrSave,NULL);

   pStrSave->BeginUnit(_T("Reinforcement"),1.0);
      pStrSave->put_Property(_T("RebarType"),CComVariant(m_RebarType));
      pStrSave->put_Property(_T("RebarGrade"),CComVariant(m_RebarGrade));
      m_LongitudinalRebar.Save(pStrSave,NULL);
      m_LowerXBeamStirrups.Save(pStrSave);
      m_FullDepthStirrups.Save(pStrSave);
   pStrSave->EndUnit(); // Reinforcement

   pStrSave->BeginUnit(_T("Bearings"),1.0);
      BOOST_FOREACH(xbrBearingLineData& bearingLine,m_vBearingLines)
      {
         bearingLine.Save(pStrSave);
      }
   pStrSave->EndUnit(); // Bearings

   pStrSave->BeginUnit(_T("Condition"),1.0);
      pStrSave->put_Property(_T("ConditionFactorType"),CComVariant(m_ConditionFactorType));
      pStrSave->put_Property(_T("ConditionFactor"),CComVariant(m_ConditionFactor));
   pStrSave->EndUnit(); // Condition

   pStrSave->EndUnit(); // Pier
   return S_OK;
}

HRESULT xbrPierData::Load(IStructuredLoad* pStrLoad)
{
   USES_CONVERSION;
   CHRException hr;
   try
   {
      CComVariant var;
      hr = pStrLoad->BeginUnit(_T("Pier"));
      var.vt = VT_ID;
      hr = pStrLoad->get_Property(_T("ID"),&var);
      m_ID = VARIANT2ID(var);

      {
         hr = pStrLoad->BeginUnit(_T("Layout"));
         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("SuperstructureConnectionType"),&var);
         m_ConnectionType = (xbrTypes::SuperstructureConnectionType)(var.lVal);

         var.vt = VT_BSTR;
         hr = pStrLoad->get_Property(_T("Skew"),&var);
         m_strSkew = OLE2T(var.bstrVal);

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("DeckElevation"),&var);
         m_DeckElevation = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("CrownPointOffset"),&var);
         m_CrownPointOffset = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("BridgeLineOffset"),&var);
         m_BridgeLineOffset = var.dblVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("CurbLineDatum"),&var);
         m_CurbLineDatum = (pgsTypes::OffsetMeasurementType)(var.lVal);
         
         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("LeftCurbOffset"),&var);
         m_LeftCurbOffset = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("RightCurbOffset"),&var);
         m_RightCurbOffset = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("LeftCrownSlope"),&var);
         m_LeftCrownSlope = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("RightCrownSlope"),&var);
         m_RightCrownSlope = var.dblVal;

         hr = pStrLoad->EndUnit(); 
      }

      {
         hr = pStrLoad->BeginUnit(_T("Diaphragm"));

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("H"),&var);
         m_H = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("W"),&var);
         m_W = var.dblVal;

         hr = pStrLoad->EndUnit();
      }

      {
         hr = pStrLoad->BeginUnit(_T("LowerCrossBeam"));

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("H1"),&var);
         m_H1 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("H2"),&var);
         m_H2 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("H3"),&var);
         m_H3 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("H4"),&var);
         m_H4 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("X1"),&var);
         m_X1 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("X2"),&var);
         m_X2 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("W"),&var);
         m_XW = var.dblVal;

         hr = pStrLoad->EndUnit(); // LowerCrossBeam
      }

      {
         hr = pStrLoad->BeginUnit(_T("Columns"));

         var.vt = VT_INDEX;
         hr = pStrLoad->get_Property(_T("ColumnCount"),&var);
         m_nColumns = VARIANT2INDEX(var);

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("RefColumDatum"),&var);
         m_RefColumnDatum = (pgsTypes::OffsetMeasurementType)(var.lVal);

         var.vt = VT_INDEX;
         hr = pStrLoad->get_Property(_T("RefColumn"),&var);
         m_RefColumnIdx = VARIANT2INDEX(var);

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("RefColumnOffset"),&var);
         m_RefColumnOffset = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("X3"),&var);
         m_X3 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("X4"),&var);
         m_X4 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("S"),&var);
         m_S = var.dblVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("Shape"),&var);
         m_ColumnShape = (CColumnData::ColumnShapeType)var.lVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("D1"),&var);
         m_D1 = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("D2"),&var);
         m_D2 = var.dblVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("HeightType"),&var);
         m_ColumnMeasurementType = (CColumnData::ColumnHeightMeasurementType)var.lVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("Height"),&var);
         m_ColumnHeight = var.dblVal;

         hr = pStrLoad->EndUnit(); // Columns
      }

      {
         hr = m_Concrete.Load(pStrLoad,NULL);
      }

      {
         hr = pStrLoad->BeginUnit(_T("Reinforcement"));

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("RebarType"),&var);
         m_RebarType = (matRebar::Type)(var.lVal);

         hr = pStrLoad->get_Property(_T("RebarGrade"),&var);
         m_RebarGrade = (matRebar::Grade)(var.lVal);

         hr = m_LongitudinalRebar.Load(pStrLoad,NULL);
         hr = m_LowerXBeamStirrups.Load(pStrLoad);
         hr = m_FullDepthStirrups.Load(pStrLoad);
         hr = pStrLoad->EndUnit(); // Reinforcement
      }

      {
         hr = pStrLoad->BeginUnit(_T("Bearings"));
         m_vBearingLines.clear();
         xbrBearingLineData bearingLine;
         while ( SUCCEEDED(bearingLine.Load(pStrLoad)) )
         {
            m_vBearingLines.push_back(bearingLine);
         }
         hr = pStrLoad->EndUnit(); // Bearings
      }

      {
         hr = pStrLoad->BeginUnit(_T("Condition"));

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("ConditionFactorType"),&var);
         m_ConditionFactorType = (pgsTypes::ConditionFactorType)var.lVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("ConditionFactor"),&var);
         m_ConditionFactor = var.dblVal;

         hr = pStrLoad->EndUnit(); // Condition
      }

      hr = pStrLoad->EndUnit();
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }
   return S_OK;
}

void xbrPierData::MakeCopy(const xbrPierData& rOther)
{
   m_ID = rOther.m_ID;

   m_strSkew = rOther.m_strSkew;

   m_DeckElevation    = rOther.m_DeckElevation;
   m_CrownPointOffset = rOther.m_CrownPointOffset;
   m_BridgeLineOffset = rOther.m_BridgeLineOffset;


   m_ConnectionType  = rOther.m_ConnectionType;
   m_CurbLineDatum   = rOther.m_CurbLineDatum;
   m_LeftCurbOffset  = rOther.m_LeftCurbOffset;
   m_RightCurbOffset = rOther.m_RightCurbOffset;
   m_LeftCrownSlope  = rOther.m_LeftCrownSlope;
   m_RightCrownSlope = rOther.m_RightCrownSlope;

   m_H = rOther.m_H;
   m_W = rOther.m_W;

   m_H1 = rOther.m_H1;
   m_H2 = rOther.m_H2;
   m_H3 = rOther.m_H3;
   m_H4 = rOther.m_H4;
   m_X1 = rOther.m_X1;
   m_X2 = rOther.m_X2;
   m_XW = rOther.m_XW;

   m_nColumns        = rOther.m_nColumns;
   m_RefColumnDatum  = rOther.m_RefColumnDatum;
   m_RefColumnIdx    = rOther.m_RefColumnIdx;
   m_RefColumnOffset = rOther.m_RefColumnOffset;
   m_X3              = rOther.m_X3;
   m_X4              = rOther.m_X4;
   m_S               = rOther.m_S;

   m_ColumnShape = rOther.m_ColumnShape;
   m_D1 = rOther.m_D1;
   m_D2 = rOther.m_D2;
   m_ColumnMeasurementType = rOther.m_ColumnMeasurementType;
   m_ColumnHeight = rOther.m_ColumnHeight;

   m_RebarGrade = rOther.m_RebarGrade;
   m_RebarType = rOther.m_RebarType;

   m_Concrete = rOther.m_Concrete;

   m_ConditionFactorType = rOther.m_ConditionFactorType;
   m_ConditionFactor = rOther.m_ConditionFactor;

   m_LongitudinalRebar = rOther.m_LongitudinalRebar;
   m_LowerXBeamStirrups = rOther.m_LowerXBeamStirrups;
   m_FullDepthStirrups = rOther.m_FullDepthStirrups;

   m_vBearingLines = rOther.m_vBearingLines;
}

void xbrPierData::MakeAssignment(const xbrPierData& rOther)
{
   MakeCopy( rOther );
}
