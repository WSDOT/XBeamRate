///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2015  Washington State Department of Transportation
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

#include "StdAfx.h"
#include "PierDescriptionDetailsChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\Project.h>
#include <IFace\Pier.h>

#include "XBeamRateReportSpecification.h"

#include <XBeamRateExt\LongitudinalRebarData.h>
#include <XBeamRateExt\StirrupData.h>

#include <PgsExt\GirderLabel.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void write_superstructure_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_substructure_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_concrete_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_longitudinal_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_transverse_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID,xbrTypes::Stage stage);
void write_bearing_layout_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_live_load_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);

/****************************************************************************
CLASS
   CPierDescriptionDetailsChapterBuilder
****************************************************************************/
CPierDescriptionDetailsChapterBuilder::CPierDescriptionDetailsChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CPierDescriptionDetailsChapterBuilder::GetName() const
{
   return TEXT("Pier Description Details");
}

rptChapter* CPierDescriptionDetailsChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   USES_CONVERSION;

   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   write_superstructure_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_substructure_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_concrete_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_reinforcement_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_longitudinal_reinforcement_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_transverse_reinforcement_data(pBroker,pDisplayUnits,pChapter,pierID,xbrTypes::Stage1);
   write_transverse_reinforcement_data(pBroker,pDisplayUnits,pChapter,pierID,xbrTypes::Stage2);
   write_bearing_layout_data(pBroker,pDisplayUnits,pChapter,pierID);
   write_live_load_data(pBroker,pDisplayUnits,pChapter,pierID);

   return pChapter;
}


CChapterBuilder* CPierDescriptionDetailsChapterBuilder::Clone() const
{
   return new CPierDescriptionDetailsChapterBuilder;
}

void write_superstructure_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   INIT_UV_PROTOTYPE( rptLengthUnitValue, elevation, pDisplayUnits->GetAlignmentLengthUnit(), true );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), true );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), true );

   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Superstructure Properties") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   CString strDatumType[] = {_T("Alignment"), _T("BridgeLine") };
   CString strPierTypes[] = {_T("Continuous"),_T("Integral"),_T("Expansion")};

   CString strImageName;
   strImageName.Format(_T("XBR_Superstructure_%s_%s.png"),strPierTypes[pProject->GetPierType(pierID)],strDatumType[pProject->GetCurbLineDatum(pierID)]);
   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + std::_tstring(strImageName)) << rptNewLine << rptNewLine;

   *pPara << _T("Horizonal dimensions are normal to the alignment, except for W which is normal to the plane of the pier.") << rptNewLine;

   rptRcTable* pTable = pgsReportStyleHolder::CreateTableNoHeading(2);
   pTable->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   pTable->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));
   *pPara << pTable << rptNewLine;



   RowIndexType row = 0;
   (*pTable)(row,0) << _T("Pier Type");
   (*pTable)(row,1) << strPierTypes[pProject->GetPierType(pierID)];
   row++;

   (*pTable)(row,0) << _T("Curb line measured from");
   (*pTable)(row,1) << (pProject->GetCurbLineDatum(pierID) == pgsTypes::omtAlignment ? _T("Alignment") : _T("BridgeLink"));
   row++;

   (*pTable)(row,0) << _T("Skew");
   (*pTable)(row,1) << pProject->GetOrientation(pierID);
   row++;

   (*pTable)(row,0) << _T("Deck elevation at alignment");
   (*pTable)(row,1) << elevation.SetValue(pProject->GetDeckElevation(pierID));
   row++;

   (*pTable)(row,0) << _T("Gross Slab Depth (TS)");
   (*pTable)(row,1) << dim.SetValue(pProject->GetDeckThickness(pierID));
   row++;

   (*pTable)(row,0) << _T("Crown Point Offset (CPO)");
   (*pTable)(row,1) << RPT_OFFSET(pProject->GetCrownPointOffset(pierID),length);
   row++;

   (*pTable)(row,0) << _T("Bridge Line Offset (BLO)");
   (*pTable)(row,1) << RPT_OFFSET(pProject->GetBridgeLineOffset(pierID),length);
   row++;

   Float64 LCO, RCO;
   pProject->GetCurbLineOffset(pierID,&LCO,&RCO);
   (*pTable)(row,0) << _T("Left Curb Offset (LCO)");
   (*pTable)(row,1) << RPT_OFFSET(LCO,length);
   row++;

   (*pTable)(row,0) << _T("Right Curb Offset (RCO)");
   (*pTable)(row,1) << RPT_OFFSET(RCO,length);
   row++;

   std::_tstring strSlopeTag = pDisplayUnits->GetAlignmentLengthUnit().UnitOfMeasure.UnitTag();
   Float64 SL, SR;
   pProject->GetCrownSlopes(pierID,&SL,&SR);
   (*pTable)(row,0) << _T("Left Crown Slope (SL)");
   (*pTable)(row,1) << SL << _T(" ") << strSlopeTag << _T("/") << strSlopeTag;
   row++;
   (*pTable)(row,0) << _T("Right Crown Slope (SR)");
   (*pTable)(row,1) << SR << _T(" ") << strSlopeTag << _T("/") << strSlopeTag;
   row++;

   Float64 W,H;
   pProject->GetDiaphragmDimensions(pierID,&H,&W);
   (*pTable)(row,0) << _T("Diaphragm Width (W)");
   (*pTable)(row,1) << length.SetValue(W);
   row++;
   (*pTable)(row,0) << _T("Diaphragm Height (H)");
   (*pTable)(row,1) << length.SetValue(H);
   row++;
}

void write_substructure_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   INIT_UV_PROTOTYPE( rptLengthUnitValue, elevation, pDisplayUnits->GetAlignmentLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), false );

   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Substructure Properties") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("Lower_XBeam_Dimensions.png")) << rptNewLine << rptNewLine;

   *pPara << _T("Horizonal dimensions are in the plane of the pier.") << rptNewLine;

   rptRcTable* pTable = pgsReportStyleHolder::CreateTableNoHeading(4);
   pTable->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   pTable->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));
   pTable->SetColumnStyle(2,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   pTable->SetStripeRowColumnStyle(2,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));
   *pPara << pTable << rptNewLine;

   Float64 H1,H2,H3,H4,X1,X2,X3,X4,X5,X6,W;
   pProject->GetLowerXBeamDimensions(pierID,&H1,&H2,&H3,&H4,&X1,&X2,&X3,&X4,&W);
   X5 = pProject->GetXBeamLeftOverhang(pierID);
   X6 = pProject->GetXBeamRightOverhang(pierID);

   (*pTable)(0,0) << _T("H1 = ") << length.SetValue(H1);
   (*pTable)(0,1) << _T("H2 = ") << length.SetValue(H2);
   (*pTable)(0,2) << _T("H3 = ") << length.SetValue(H3);
   (*pTable)(0,3) << _T("H4 = ") << length.SetValue(H4);

   (*pTable)(1,0) << _T("X1 = ") << length.SetValue(X1);
   (*pTable)(1,1) << _T("X2 = ") << length.SetValue(X2);
   (*pTable)(1,2) << _T("X3 = ") << length.SetValue(X3);
   (*pTable)(1,3) << _T("X4 = ") << length.SetValue(X4);

   (*pTable)(2,0) << _T("X5 = ") << length.SetValue(X5);
   (*pTable)(2,1) << _T("X5 = ") << length.SetValue(X6);
   (*pTable)(2,2) << _T("");
   (*pTable)(2,3) << _T("W = ") << length.SetValue(W);

   pTable = pgsReportStyleHolder::CreateDefaultTable(9,_T("Column Properties"));
   *pPara << pTable << rptNewLine;
   ColumnIndexType col = 0;
   (*pTable)(0,col++) << _T("Col");
   (*pTable)(0,col++) << COLHDR(_T("Top") << rptNewLine << _T("Elev"), rptLengthUnitTag, pDisplayUnits->GetAlignmentLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Bottom") << rptNewLine << _T("Elev"), rptLengthUnitTag, pDisplayUnits->GetAlignmentLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Height"), rptLengthUnitTag, pDisplayUnits->GetAlignmentLengthUnit());
   (*pTable)(0,col++) << _T("Fixity");
   (*pTable)(0,col++) << _T("Shape");
   (*pTable)(0,col++) << COLHDR(_T("Diameter") << rptNewLine << _T("Width"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Depth"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Spacing"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());

   GET_IFACE2(pBroker,IXBRPier,pPier);

   CString strFixity[] = {_T("Fixed"), _T("Pinned") };
   CString strShape[] = {_T("Circle"), _T("Rectangle") };


   RowIndexType row = pTable->GetNumberOfHeaderRows();
   ColumnIndexType nColumns = pProject->GetColumnCount(pierID);
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++, row++ )
   {
      col = 0;

      (*pTable)(row,col++) << (colIdx+1);
      (*pTable)(row,col++) << elevation.SetValue(pPier->GetTopColumnElevation(pierID,colIdx));
      (*pTable)(row,col++) << elevation.SetValue(pPier->GetBottomColumnElevation(pierID,colIdx));
      (*pTable)(row,col++) << length.SetValue(pProject->GetColumnHeight(pierID,colIdx));
      (*pTable)(row,col++) << strFixity[pProject->GetColumnFixity(pierID,colIdx)];

      CColumnData::ColumnShapeType shapeType;
      CColumnData::ColumnHeightMeasurementType measureType;
      Float64 D1, D2, H;
      pProject->GetColumnProperties(pierID,colIdx,&shapeType,&D1,&D2,&measureType,&H);
      (*pTable)(row,col++) << strShape[shapeType];

      if ( shapeType == CColumnData::cstCircle )
      {
         (*pTable)(row,col++) << length.SetValue(D1);
         (*pTable)(row,col++) << _T("");
      }
      else
      {
         (*pTable)(row,col++) << length.SetValue(D1);
         (*pTable)(row,col++) << length.SetValue(D2);
      }

      if ( colIdx < nColumns-1 )
      {
         (*pTable)(row,col++) << length.SetValue(pProject->GetColumnSpacing(pierID,colIdx));
      }
      else
      {
         (*pTable)(row,col++) << _T("");
      }
   }

   pgsTypes::OffsetMeasurementType columnDatum;
   ColumnIndexType refColumnIdx;
   Float64 refColumnOffset;
   pProject->GetRefColumnLocation(pierID,&columnDatum,&refColumnIdx,&refColumnOffset);
   CString strDatumType[] = {_T("Alignment"), _T("BridgeLine") };

   length.ShowUnitTag(true);

   *pPara << _T("Column ") << LABEL_COLUMN(refColumnIdx) << _T(" is located ") << RPT_OFFSET(refColumnOffset,length) << _T("from the ") << strDatumType[columnDatum] << rptNewLine;
}

void write_concrete_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   bool bK1 = (lrfdVersionMgr::ThirdEditionWith2005Interims <= lrfdVersionMgr::GetVersion());

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(bK1 ? 13 : 7,_T("Concrete Properties"));
   pTable->SetColumnStyle(0, pgsReportStyleHolder::GetTableCellStyle( CB_NONE | CJ_LEFT) );
   pTable->SetStripeRowColumnStyle(0, pgsReportStyleHolder::GetTableStripeRowCellStyle( CB_NONE | CJ_LEFT) );
   pTable->SetColumnStyle(1, pgsReportStyleHolder::GetTableCellStyle( CB_NONE | CJ_LEFT) );
   pTable->SetStripeRowColumnStyle(1, pgsReportStyleHolder::GetTableStripeRowCellStyle( CB_NONE | CJ_LEFT) );

   *pPara << pTable << rptNewLine;

   ColumnIndexType col = 0;
   RowIndexType row = 0;
   (*pTable)(row,col++) << _T("Type");
   (*pTable)(row,col++) << COLHDR(RPT_FC,  rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*pTable)(row,col++) << COLHDR(RPT_EC,  rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   (*pTable)(row,col++) << COLHDR(Sub2(symbol(gamma),_T("w")), rptDensityUnitTag, pDisplayUnits->GetDensityUnit() );
   (*pTable)(row,col++) << COLHDR(Sub2(symbol(gamma),_T("s")), rptDensityUnitTag, pDisplayUnits->GetDensityUnit() );
   (*pTable)(row,col++) << COLHDR(Sub2(_T("D"),_T("agg")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit() );
   (*pTable)(row,col++) << COLHDR(RPT_STRESS(_T("ct")),  rptStressUnitTag, pDisplayUnits->GetStressUnit() );
   if ( bK1 )
   {
      pTable->SetNumberOfHeaderRows(2);
      for ( int i = 0; i < 7; i++ )
      {
         pTable->SetRowSpan(0,i,2); 
         pTable->SetRowSpan(1,i,SKIP_CELL);
      }

      pTable->SetColumnSpan(0,7,2);
      pTable->SetColumnSpan(0,8,SKIP_CELL);
      (*pTable)(0,10) << Sub2(_T("E"),_T("c"));
      (*pTable)(1,10) << Sub2(_T("K"),_T("1"));
      (*pTable)(1,11) << Sub2(_T("K"),_T("2"));

      pTable->SetColumnSpan(0,9,2);
      pTable->SetColumnSpan(0,10,SKIP_CELL);
      (*pTable)(0,12) << _T("Creep");
      (*pTable)(1,12) << Sub2(_T("K"),_T("1"));
      (*pTable)(1,13) << Sub2(_T("K"),_T("2"));

      pTable->SetColumnSpan(0,11,2);
      pTable->SetColumnSpan(0,12,SKIP_CELL);
      (*pTable)(0,14) << _T("Shrinkage");
      (*pTable)(1,14) << Sub2(_T("K"),_T("1"));
      (*pTable)(1,15) << Sub2(_T("K"),_T("2"));
   }


   row = pTable->GetNumberOfHeaderRows();

   GET_IFACE2(pBroker,IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);

   INIT_UV_PROTOTYPE( rptLengthUnitValue,  cmpdim,  pDisplayUnits->GetComponentDimUnit(), false );
   INIT_UV_PROTOTYPE( rptStressUnitValue,  stress,  pDisplayUnits->GetStressUnit(),       false );
   INIT_UV_PROTOTYPE( rptDensityUnitValue, density, pDisplayUnits->GetDensityUnit(),      false );
   INIT_UV_PROTOTYPE( rptStressUnitValue,  modE,    pDisplayUnits->GetModEUnit(),         false );

   col = 0;

   (*pTable)(row,col++) << matConcrete::GetTypeName( (matConcrete::Type)concrete.Type, true );

   GET_IFACE2(pBroker,IXBRMaterial,pMaterial);
   Float64 Ec = pMaterial->GetXBeamEc(pierID);

   (*pTable)(row,col++) << stress.SetValue( concrete.Fc );
   (*pTable)(row,col++) << modE.SetValue( Ec );
   (*pTable)(row,col++) << density.SetValue( concrete.WeightDensity );

   if ( concrete.bUserEc )
   {
      (*pTable)(row,col++) << RPT_NA;
   }
   else
   {
      (*pTable)(row,col++) << density.SetValue( concrete.StrengthDensity );
   }

   (*pTable)(row,col++) << cmpdim.SetValue( concrete.MaxAggregateSize );

   if ( concrete.bHasFct )
   {
      (*pTable)(row,col++) << stress.SetValue( concrete.Fct );
   }
   else
   {
      (*pTable)(row,col++) << RPT_NA;
   }


   if (bK1)
   {
      (*pTable)(row,col++) << concrete.EcK1;
      (*pTable)(row,col++) << concrete.EcK2;
      (*pTable)(row,col++) << concrete.CreepK1;
      (*pTable)(row,col++) << concrete.CreepK2;
      (*pTable)(row,col++) << concrete.ShrinkageK1;
      (*pTable)(row,col++) << concrete.ShrinkageK2;
   }
}

void write_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   GET_IFACE2(pBroker,IXBRProject,pProject);
   matRebar::Type type;
   matRebar::Grade grade;
   pProject->GetRebarMaterial(pierID,&type,&grade);

   std::_tstring strName = lrfdRebarPool::GetMaterialName(type,grade);

   GET_IFACE2(pBroker,IXBRMaterial,pMaterial);
   Float64 E, fy, fu;
   pMaterial->GetRebarProperties(pierID,&E,&fy,&fu);

   INIT_UV_PROTOTYPE( rptStressUnitValue,  modE,    pDisplayUnits->GetModEUnit(),         false );
   INIT_UV_PROTOTYPE( rptStressUnitValue,  stress,  pDisplayUnits->GetStressUnit(),       false );

   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(3,_T("Reinforcement Properties"));
   *pPara << pTable << rptNewLine;

   (*pTable)(0,0) << _T("Name");
   (*pTable)(1,0) << strName;

   (*pTable)(0,1) << COLHDR(RPT_ES,rptStressUnitTag,pDisplayUnits->GetModEUnit());
   (*pTable)(1,1) << modE.SetValue(E);

   (*pTable)(0,2) << COLHDR(RPT_FY,rptStressUnitTag,pDisplayUnits->GetStressUnit());
   (*pTable)(1,2) << stress.SetValue(fy);
}

void write_longitudinal_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Longitudinal Reinforcement") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(11);
   *pPara << pTable << rptNewLine;

   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), false );

   ColumnIndexType col = 0;
   (*pTable)(0,col++) << _T("Row");
   (*pTable)(0,col++) << _T("Face");
   (*pTable)(0,col++) << _T("Datum");
   (*pTable)(0,col++) << COLHDR(_T("Start"),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Length"),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(_T("Cover"),rptLengthUnitTag,pDisplayUnits->GetComponentDimUnit());
   (*pTable)(0,col++) << _T("Bar") << rptNewLine << _T("Size");
   (*pTable)(0,col++) << _T("#") << rptNewLine << _T("Bars");
   (*pTable)(0,col++) << COLHDR(_T("Spacing"),rptLengthUnitTag,pDisplayUnits->GetComponentDimUnit());
   (*pTable)(0,col++) << _T("Start") << rptNewLine << _T("Hook");
   (*pTable)(0,col++) << _T("End") << rptNewLine << _T("Hook");

   GET_IFACE2(pBroker,IXBRProject,pProject);
   const xbrLongitudinalRebarData& rebarData = pProject->GetLongitudinalRebar(pierID);

   CString strFace[] = { _T("Top"), _T("Top Lower XBeam"), _T("Bottom") };
   CString strDatum[] = { _T("Left End"), _T("Right End"), _T("Full Length") };


   RowIndexType row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(const xbrLongitudinalRebarData::RebarRow& rebarRow, rebarData.RebarRows)
   {
      col = 0;

      (*pTable)(row,col++) << row;
      (*pTable)(row,col++) << strFace[rebarRow.Datum];
      (*pTable)(row,col++) << strDatum[rebarRow.LayoutType];

      if ( rebarRow.LayoutType == xbrTypes::blFullLength )
      {
         (*pTable)(row,col++) << _T("");
         (*pTable)(row,col++) << _T("");
      }
      else
      {
         (*pTable)(row,col++) << length.SetValue(rebarRow.Start);
         (*pTable)(row,col++) << length.SetValue(rebarRow.Length);
      }

      (*pTable)(row,col++) << dim.SetValue(rebarRow.Cover);
      (*pTable)(row,col++) << lrfdRebarPool::GetBarSize(rebarRow.BarSize);
      (*pTable)(row,col++) << rebarRow.NumberOfBars;
      (*pTable)(row,col++) << dim.SetValue(rebarRow.BarSpacing);

      if ( rebarRow.bHookStart )
      {
         (*pTable)(row,col++) << _T("Yes");
      }
      else
      {
         (*pTable)(row,col++) << _T("No");
      }

      if ( rebarRow.bHookEnd )
      {
         (*pTable)(row,col++) << _T("Yes");
      }
      else
      {
         (*pTable)(row,col++) << _T("No");
      }

      row++;
   }
}

void write_transverse_reinforcement_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID,xbrTypes::Stage stage)
{
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Stirrups");
   if ( stage == xbrTypes::Stage1 )
   {
      *pPara << _T(" - Lower Cross Beam") << rptNewLine;
   }
   else
   {
      *pPara << _T(" - Full Depth Cross Beam") << rptNewLine;
   }

   GET_IFACE2(pBroker,IXBRProject,pProject);
   const xbrStirrupData* pStirrups;
   if ( stage == xbrTypes::Stage1 )
   {
      pStirrups = &(pProject->GetLowerXBeamStirrups(pierID));
   }
   else
   {
      pStirrups = &(pProject->GetFullDepthStirrups(pierID));
   }

   if ( pStirrups->Symmetric )
   {
      pPara = new rptParagraph;
      *pChapter << pPara;

      *pPara << _T("Stirrups are symmetric about cneterline of cross beam") << rptNewLine;
   }

   pPara = new rptParagraph;
   *pChapter << pPara;

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(5);
   *pPara << pTable << rptNewLine;

   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), false );

   ColumnIndexType col = 0;
   (*pTable)(0,col++) << _T("Zone");
   (*pTable)(0,col++) << COLHDR(_T("Zone") << rptNewLine << _T("Length"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << _T("Bar") << rptNewLine << _T("Size");
   (*pTable)(0,col++) << COLHDR(_T("Spacing"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pTable)(0,col++) << _T("# of") << rptNewLine << _T("Legs");


   RowIndexType row = pTable->GetNumberOfHeaderRows();
   IndexType zoneIdx = 0;
   BOOST_FOREACH(const xbrStirrupData::StirrupZone& stirrupZone, pStirrups->Zones)
   {
      col = 0;

      (*pTable)(row,col++) << (zoneIdx+1);
      if ( zoneIdx == pStirrups->Zones.size()-1 )
      {
         if ( pStirrups->Symmetric )
         {
            (*pTable)(row,col++) << _T("to center");
         }
         else
         {
            (*pTable)(row,col++) << _T("to end");
         }
      }
      else
      {
         (*pTable)(row,col++) << length.SetValue(stirrupZone.Length);
      }
      (*pTable)(row,col++) << lrfdRebarPool::GetBarSize(stirrupZone.BarSize);
      (*pTable)(row,col++) << dim.SetValue(stirrupZone.BarSpacing);
      (*pTable)(row,col++) << stirrupZone.nBars;

      row++;
      zoneIdx++;
   }
      
}

void write_bearing_layout_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Bearing Layout") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptForceUnitValue, force, pDisplayUnits->GetGeneralForceUnit(), false );
   INIT_UV_PROTOTYPE( rptForcePerLengthUnitValue, fpl, pDisplayUnits->GetForcePerLengthUnit(), false );

   GET_IFACE2(pBroker,IXBRProject,pProject);
   IndexType nBearingLines = pProject->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      ColumnIndexType nColumns = 8;
      xbrTypes::ReactionLoadType bearingReactionType = pProject->GetBearingReactionType(pierID,brgLineIdx);
      if ( bearingReactionType == xbrTypes::rltUniform )
      {
         nColumns++;
      }

      rptRcTable* pTable;
      if ( nBearingLines == 1 )
      {
         pTable = pgsReportStyleHolder::CreateDefaultTable(nColumns);
      }
      else
      {
         CString strTitle;
         strTitle.Format(_T("%s Bearing Line"),(brgLineIdx == 0 ? _T("Back") : _T("Ahead")));
         pTable = pgsReportStyleHolder::CreateDefaultTable(nColumns,strTitle);
      }
      *pPara << pTable << rptNewLine;


      ColumnIndexType col = 0;
      (*pTable)(0,col++) << _T("Bearing");
      if ( bearingReactionType == xbrTypes::rltUniform )
      {
         (*pTable)(0,col++) << COLHDR(_T("DC"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("DW"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("CR"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("SH"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("PS"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("RE"), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
         (*pTable)(0,col++) << COLHDR(_T("Width"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit() );
      }
      else
      {
         (*pTable)(0,col++) << COLHDR(_T("DC"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
         (*pTable)(0,col++) << COLHDR(_T("DW"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
         (*pTable)(0,col++) << COLHDR(_T("CR"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
         (*pTable)(0,col++) << COLHDR(_T("SH"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
         (*pTable)(0,col++) << COLHDR(_T("PS"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
         (*pTable)(0,col++) << COLHDR(_T("RE"), rptForceUnitTag, pDisplayUnits->GetGeneralForceUnit());
      }

      (*pTable)(0,col++) << COLHDR(_T("Spacing"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit() );

      RowIndexType row = pTable->GetNumberOfHeaderRows();

      length.ShowUnitTag(false);

      IndexType nBearings = pProject->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++, row++ )
      {
         col = 0;
         (*pTable)(row,col++) << (brgIdx+1);

   
         Float64 DC, DW, CR, SH, PS, RE, W;
         pProject->GetBearingReactions(pierID,brgLineIdx,brgIdx,&DC,&DW,&CR,&SH,&PS,&RE,&W);

         if ( bearingReactionType == xbrTypes::rltUniform )
         {
            (*pTable)(row,col++) << fpl.SetValue(DC);
            (*pTable)(row,col++) << fpl.SetValue(DW);
            (*pTable)(row,col++) << fpl.SetValue(CR);
            (*pTable)(row,col++) << fpl.SetValue(SH);
            (*pTable)(row,col++) << fpl.SetValue(PS);
            (*pTable)(row,col++) << fpl.SetValue(RE);
            (*pTable)(row,col++) << length.SetValue(W);
         }
         else
         {
            (*pTable)(row,col++) << force.SetValue(DC);
            (*pTable)(row,col++) << force.SetValue(DW);
            (*pTable)(row,col++) << force.SetValue(CR);
            (*pTable)(row,col++) << force.SetValue(SH);
            (*pTable)(row,col++) << force.SetValue(PS);
            (*pTable)(row,col++) << force.SetValue(RE);
         }

         if ( brgIdx < nBearings-1 )
         {
            Float64 S = pProject->GetBearingSpacing(pierID,brgLineIdx,brgIdx);
            (*pTable)(row,col++) << length.SetValue(S);
         }
         else
         {
            (*pTable)(row,col++) << _T("");
         }
      }

      length.ShowUnitTag(true);
      IndexType refBrgIdx;
      Float64 refBrgOffset;
      pgsTypes::OffsetMeasurementType refBearingDatum;
      pProject->GetReferenceBearing(pierID,brgLineIdx,&refBrgIdx,&refBrgOffset,&refBearingDatum);
      *pPara << _T("Bearing ") << (refBrgIdx+1) << _T(" is located ") << RPT_OFFSET(refBrgOffset,length) << _T(" of the ");
      if ( refBearingDatum == pgsTypes::omtAlignment )
      {
         *pPara << _T("Alignment") << rptNewLine;
      }
      else
      {
         *pPara << _T("Bridge Line") << rptNewLine;
      }
   }
}

void write_live_load_data(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
}
