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
#include "ShearCapacityDetailsChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\LoadRating.h>
#include <IFace\PointOfInterest.h>
#include <IFace\Pier.h>
#include <IFace\Project.h>

#include "XBeamRateReportSpecification.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   CShearCapacityDetailsChapterBuilder
****************************************************************************/
CShearCapacityDetailsChapterBuilder::CShearCapacityDetailsChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CShearCapacityDetailsChapterBuilder::GetName() const
{
   return TEXT("Shear Capacity Details");
}

rptChapter* CShearCapacityDetailsChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IXBRProject,pProject);
   xbrTypes::PierType pierType = pProject->GetPierType(pierID);

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptXBRPointOfInterest, location, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dist, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), false );
   INIT_UV_PROTOTYPE( rptForceUnitValue, shear, pDisplayUnits->GetShearUnit(), false );
   INIT_UV_PROTOTYPE( rptStressUnitValue, stress, pDisplayUnits->GetStressUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, avs, pDisplayUnits->GetAvOverSUnit(), false );
   INIT_UV_PROTOTYPE( rptAngleUnitValue, angle, pDisplayUnits->GetAngleUnit(), false );

   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   *pPara << _T("Shear resistance computed by LRFD 5.8.3.4.1 - Simplified Procedure for Nonprestressed Sections") << rptNewLine;

   pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   if ( pierType == xbrTypes::pctIntegral )
   {
      *pPara << _T("Effective Shear Dimension - Lower Cross Beam - LRFD 5.8.2.9") << rptNewLine;
   }
   else
   {
      *pPara << _T("Effective Shear Dimension - LRFD 5.8.2.9") << rptNewLine;
   }

   pPara = new rptParagraph;
   *pChapter << pPara;
   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("EffectiveShearDepth.png")) << rptNewLine;
   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeam_dv.png")) << rptNewLine;

   rptRcTable* pDvTable1 = pgsReportStyleHolder::CreateDefaultTable(10);
   *pPara << pDvTable1 << rptNewLine;

   if ( pierType == xbrTypes::pctIntegral )
   {
      pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
      *pChapter << pPara;
      *pPara << _T("Effective Shear Dimension - Full Depth Cross Beam - LRFD 5.8.2.9") << rptNewLine;
   }

   rptRcTable* pDvTable2 = NULL;
   if ( pierType == xbrTypes::pctIntegral )
   {
      pPara = new rptParagraph;
      *pChapter << pPara;
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("EffectiveShearDepth.png")) << rptNewLine;
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeam_dv.png")) << rptNewLine;

      pDvTable2 = pgsReportStyleHolder::CreateDefaultTable(10);
      *pPara << pDvTable2 << rptNewLine;
   }

   rptRcTable* pAvSTable1 = NULL;
   rptRcTable* pAvSTable2 = NULL;
   if ( lrfrVersionMgr::SecondEditionWith2015Interims <= lrfrVersionMgr::GetVersion() )
   {
      pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
      *pChapter << pPara;
      if ( pierType == xbrTypes::pctIntegral )
      {
         *pPara << _T("Average shear reinforcement - Lower Cross Beam - MBE 6A.5.8") << rptNewLine;
      }
      else
      {
         *pPara << _T("Average shear reinforcement - MBE 6A.5.8") << rptNewLine;
      }

      pPara = new rptParagraph;
      *pChapter << pPara;
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeam_AvOverS.png"),rptRcImage::Middle) << _T(" based on MBE Eqn 6A.5.8-1") << rptNewLine;
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("Lshear_failure_plane.png"),rptRcImage::Middle) << rptNewLine;

      pAvSTable1 = pgsReportStyleHolder::CreateDefaultTable(4);
      *pPara << pAvSTable1 << rptNewLine;

      if ( pierType == xbrTypes::pctIntegral )
      {
         pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
         *pChapter << pPara;

         *pPara << _T("Average shear reinforcement - Upper Cross Beam - MBE 6A.5.8") << rptNewLine;
         pPara = new rptParagraph;
         *pChapter << pPara;
         *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeam_AvOverS.png"),rptRcImage::Middle) << _T(" based on MBE Eqn 6A.5.8-1") << rptNewLine;
         *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("Lshear_failure_plane.png"),rptRcImage::Middle) << rptNewLine;

         pAvSTable2 = pgsReportStyleHolder::CreateDefaultTable(4);
         *pPara << pAvSTable2 << rptNewLine;
      }
   }

   pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Shear Resistance Provided By Tensile Stress in the Concrete") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;
   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeamVc.png"),rptRcImage::Middle) << _T(" LRFD Eqn 5.8.3.3-3") << rptNewLine;

   rptRcTable* pVcTable = pgsReportStyleHolder::CreateDefaultTable(6);
   *pPara << pVcTable << rptNewLine;


   pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Shear Resistance Provided By Shear Reinforcement") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;
   if ( pierType == xbrTypes::pctIntegral )
   {
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeamVsIntegral.png"),rptRcImage::Middle) << _T(" based on LRFD Eqn C5.8.3.3-1") << rptNewLine;
   }
   else
   {
      *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeamVs.png"),rptRcImage::Middle) << _T(" based on LRFD Eqn C5.8.3.3-1") << rptNewLine;
   }

   rptRcTable* pVsTable = pgsReportStyleHolder::CreateDefaultTable((pierType == xbrTypes::pctIntegral ? 8 : 6));
   *pPara << pVsTable << rptNewLine;


   pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Nominal Shear Resistance") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;
   *pPara << rptRcImage(pgsReportStyleHolder::GetImagePath() + _T("XBeamVn.png"),rptRcImage::Middle) << _T(" LRFD Eqns 5.8.3.3-1 and 5.8.3.3-2") << rptNewLine;

   rptRcTable* pVnTable = pgsReportStyleHolder::CreateDefaultTable(11);
   *pPara << pVnTable << rptNewLine;

   ColumnIndexType DvTableCol = 0;
   pDvTable1->SetNumberOfHeaderRows(2);

   pDvTable1->SetRowSpan(0,DvTableCol,2);
   (*pDvTable1)(0,DvTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   pDvTable1->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

   pDvTable1->SetRowSpan(0,DvTableCol,2);
   (*pDvTable1)(0,DvTableCol++) << COLHDR(_T("h"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   pDvTable1->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

   pDvTable1->SetRowSpan(0,DvTableCol,2);
   (*pDvTable1)(0,DvTableCol++) << COLHDR(_T("0.72h"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   pDvTable1->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

   pDvTable1->SetColumnSpan(0,DvTableCol,3);
   (*pDvTable1)(0,DvTableCol) << _T("Positive Moment");
   pDvTable1->SetColumnSpan(0,DvTableCol+1,SKIP_CELL);
   pDvTable1->SetColumnSpan(0,DvTableCol+2,SKIP_CELL);
   (*pDvTable1)(1,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pDvTable1)(1,DvTableCol++) << COLHDR(_T("0.9") << Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pDvTable1)(1,DvTableCol++) << COLHDR(_T("Moment Arm"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());

   pDvTable1->SetColumnSpan(0,DvTableCol,3);
   (*pDvTable1)(0,DvTableCol) << _T("Negative Moment");
   pDvTable1->SetColumnSpan(0,DvTableCol+1,SKIP_CELL);
   pDvTable1->SetColumnSpan(0,DvTableCol+2,SKIP_CELL);
   (*pDvTable1)(1,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pDvTable1)(1,DvTableCol++) << COLHDR(_T("0.9") << Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pDvTable1)(1,DvTableCol++) << COLHDR(_T("Moment Arm"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());

   pDvTable1->SetRowSpan(0,DvTableCol,2);
   (*pDvTable1)(0,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   pDvTable1->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

   //
   if ( pierType == xbrTypes::pctIntegral )
   {
      DvTableCol = 0;
      pDvTable2->SetNumberOfHeaderRows(2);

      pDvTable2->SetRowSpan(0,DvTableCol,2);
      (*pDvTable2)(0,DvTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      pDvTable2->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

      pDvTable2->SetRowSpan(0,DvTableCol,2);
      (*pDvTable2)(0,DvTableCol++) << COLHDR(_T("h"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      pDvTable2->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

      pDvTable2->SetRowSpan(0,DvTableCol,2);
      (*pDvTable2)(0,DvTableCol++) << COLHDR(_T("0.72h"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      pDvTable2->SetRowSpan(1,DvTableCol-1,SKIP_CELL);

      pDvTable2->SetColumnSpan(0,DvTableCol,3);
      (*pDvTable2)(0,DvTableCol) << _T("Positive Moment");
      pDvTable2->SetColumnSpan(0,DvTableCol+1,SKIP_CELL);
      pDvTable2->SetColumnSpan(0,DvTableCol+2,SKIP_CELL);
      (*pDvTable2)(1,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pDvTable2)(1,DvTableCol++) << COLHDR(_T("0.9") << Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pDvTable2)(1,DvTableCol++) << COLHDR(_T("Moment Arm"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());

      pDvTable2->SetColumnSpan(0,DvTableCol,3);
      (*pDvTable2)(0,DvTableCol) << _T("Negative Moment");
      pDvTable2->SetColumnSpan(0,DvTableCol+1,SKIP_CELL);
      pDvTable2->SetColumnSpan(0,DvTableCol+2,SKIP_CELL);
      (*pDvTable2)(1,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pDvTable2)(1,DvTableCol++) << COLHDR(_T("0.9") << Sub2(_T("d"),_T("e")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pDvTable2)(1,DvTableCol++) << COLHDR(_T("Moment Arm"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());

      pDvTable2->SetRowSpan(0,DvTableCol,2);
      (*pDvTable2)(0,DvTableCol++) << COLHDR(Sub2(_T("d"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      pDvTable2->SetRowSpan(1,DvTableCol-1,SKIP_CELL);
   }

   ColumnIndexType AvSTableCol = 0;
   if ( lrfrVersionMgr::SecondEditionWith2015Interims <= lrfrVersionMgr::GetVersion() )
   {
      (*pAvSTable1)(0,AvSTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      (*pAvSTable1)(0,AvSTableCol++) << _T("Stirrup Zones");
      (*pAvSTable1)(0,AvSTableCol++) << COLHDR(Sub2(_T("L"),_T("shear failure plane")), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      (*pAvSTable1)(0,AvSTableCol++) << COLHDR(_T("Avg. ") << Sub2(_T("A"),_T("v")) << _T("/s"), rptLengthUnitTag, pDisplayUnits->GetAvOverSUnit());

      if ( pierType == xbrTypes::pctIntegral )
      {
         AvSTableCol = 0;
         (*pAvSTable2)(0,AvSTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
         (*pAvSTable2)(0,AvSTableCol++) << _T("Stirrup Zones");
         (*pAvSTable2)(0,AvSTableCol++) << COLHDR(Sub2(_T("L"),_T("shear failure plane")), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
         (*pAvSTable2)(0,AvSTableCol++) << COLHDR(_T("Avg. ") << Sub2(_T("A"),_T("v")) << _T("/s"), rptLengthUnitTag, pDisplayUnits->GetAvOverSUnit());
      }
   }

   ColumnIndexType VcTableCol = 0;
   (*pVcTable)(0,VcTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pVcTable)(0,VcTableCol++) << symbol(beta);
   (*pVcTable)(0,VcTableCol++) << COLHDR(RPT_FC,rptStressUnitTag,pDisplayUnits->GetStressUnit());
   (*pVcTable)(0,VcTableCol++) << COLHDR(Sub2(_T("b"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pVcTable)(0,VcTableCol++) << COLHDR(Sub2(_T("d"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pVcTable)(0,VcTableCol++) << COLHDR(Sub2(_T("V"),_T("c")), rptForceUnitTag, pDisplayUnits->GetShearUnit());

   ColumnIndexType VsTableCol = 0;
   (*pVsTable)(0,VsTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   if ( pierType == xbrTypes::pctIntegral )
   {
      (*pVsTable)(0,VsTableCol++) << COLHDR(_T("(") << Sub2(_T("A"),_T("v")) << _T("/S)") << Sub(_T("1")), rptLengthUnitTag, pDisplayUnits->GetAvOverSUnit());
      (*pVsTable)(0,VsTableCol++) << COLHDR(Sub2(_T("d"),_T("v1")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pVsTable)(0,VsTableCol++) << COLHDR(_T("(") << Sub2(_T("A"),_T("v")) << _T("/S)") << Sub(_T("2")), rptLengthUnitTag, pDisplayUnits->GetAvOverSUnit());
      (*pVsTable)(0,VsTableCol++) << COLHDR(Sub2(_T("d"),_T("v2")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   }
   else
   {
      (*pVsTable)(0,VsTableCol++) << COLHDR(Sub2(_T("A"),_T("v")) << _T("/S"), rptLengthUnitTag, pDisplayUnits->GetAvOverSUnit());
      (*pVsTable)(0,VsTableCol++) << COLHDR(Sub2(_T("d"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   }
   (*pVsTable)(0,VsTableCol++) << COLHDR(RPT_FY, rptStressUnitTag, pDisplayUnits->GetStressUnit());
   (*pVsTable)(0,VsTableCol++) << COLHDR(symbol(theta), rptAngleUnitTag, pDisplayUnits->GetAngleUnit());
   (*pVsTable)(0,VsTableCol++) << COLHDR(Sub2(_T("V"),_T("s")), rptForceUnitTag, pDisplayUnits->GetShearUnit());


   ColumnIndexType VnTableCol = 0;
   (*pVnTable)(0,VnTableCol++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("c")), rptForceUnitTag, pDisplayUnits->GetShearUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("s")), rptForceUnitTag, pDisplayUnits->GetShearUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("n1")), rptForceUnitTag, pDisplayUnits->GetShearUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(RPT_FC,rptStressUnitTag,pDisplayUnits->GetStressUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("b"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("d"),_T("v")), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("n2")), rptForceUnitTag, pDisplayUnits->GetShearUnit());
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("n")), rptForceUnitTag, pDisplayUnits->GetShearUnit());
   (*pVnTable)(0,VnTableCol++) << symbol(phi);
   (*pVnTable)(0,VnTableCol++) << COLHDR(Sub2(_T("V"),_T("r")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());

   GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetRatingPointsOfInterest(pierID);

   GET_IFACE2(pBroker,IXBRShearCapacity,pShearCapacity);

   GET_IFACE2(pBroker,IXBRMaterial,pMaterial);
   Float64 fc = pMaterial->GetXBeamFc(pierID);

   Float64 Es, fy, fu;
   pMaterial->GetRebarProperties(pierID,&Es,&fy,&fu);

   RowIndexType row = pVcTable->GetNumberOfHeaderRows();
   RowIndexType DvTableRow = pDvTable1->GetNumberOfHeaderRows();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      DvTableCol = 0;
      AvSTableCol = 0;
      VcTableCol = 0;
      VsTableCol = 0;
      VnTableCol = 0;

      const DvDetails& dvDetails1 = pShearCapacity->GetDvDetails(pierID,(pierType == xbrTypes::pctIntegral ? xbrTypes::Stage1 : xbrTypes::Stage2),poi);

      (*pDvTable1)(DvTableRow,DvTableCol++) << location.SetValue(poi);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.h);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(0.72*dvDetails1.h);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.de[0]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(0.90*dvDetails1.de[0]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.MomentArm[0]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.de[1]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(0.90*dvDetails1.de[1]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.MomentArm[1]);
      (*pDvTable1)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails1.dv);

      if ( pierType == xbrTypes::pctIntegral )
      {
         DvTableCol = 0;
         const DvDetails& dvDetails2 = pShearCapacity->GetDvDetails(pierID,xbrTypes::Stage2,poi);
         (*pDvTable2)(DvTableRow,DvTableCol++) << location.SetValue(poi);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.h);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(0.72*dvDetails2.h);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.de[0]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(0.90*dvDetails2.de[0]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.MomentArm[0]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.de[1]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(0.90*dvDetails2.de[1]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.MomentArm[1]);
         (*pDvTable2)(DvTableRow,DvTableCol++) << dim.SetValue(dvDetails2.dv);
      }

      if ( lrfrVersionMgr::SecondEditionWith2015Interims <= lrfrVersionMgr::GetVersion() )
      {
         const AvOverSDetails& avsDetails = pShearCapacity->GetAverageAvOverSDetails(pierID,xbrTypes::Stage1,poi);
         (*pAvSTable1)(row,AvSTableCol++) << location.SetValue(poi);

         rptRcTable* pAvTable = pgsReportStyleHolder::CreateDefaultTable(4);
         (*pAvSTable1)(row,AvSTableCol++) << pAvTable;

         (*pAvSTable1)(row,AvSTableCol++) << dist.SetValue(avsDetails.ShearFailurePlaneLength);
         (*pAvSTable1)(row,AvSTableCol++) << avs.SetValue(avsDetails.AvgAvOverS);

         (*pAvTable)(0,0) << _T("");
         (*pAvTable)(0,1) << COLHDR(Sub2(_T("x"),_T("start")),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
         (*pAvTable)(0,2) << COLHDR(Sub2(_T("x"),_T("end")),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
         (*pAvTable)(0,3) << COLHDR(Sub2(_T("A"),_T("v")) << _T("/s"),rptLengthUnitTag,pDisplayUnits->GetAvOverSUnit());
         RowIndexType avRow = pAvTable->GetNumberOfHeaderRows();
         IndexType ZoneIdx = 1;
         BOOST_FOREACH(const AvOverSZone& zone,avsDetails.Zones)
         {
            (*pAvTable)(avRow,0) << ZoneIdx;
            (*pAvTable)(avRow,1) << dist.SetValue(zone.Start);
            (*pAvTable)(avRow,2) << dist.SetValue(zone.End);
            (*pAvTable)(avRow,3) << avs.SetValue(zone.AvOverS);
            avRow++;
            ZoneIdx++;
         }

         if ( pierType == xbrTypes::pctIntegral )
         {
            AvSTableCol = 0;
            const AvOverSDetails& avsDetails = pShearCapacity->GetAverageAvOverSDetails(pierID,xbrTypes::Stage2,poi);
            (*pAvSTable2)(row,AvSTableCol++) << location.SetValue(poi);

            rptRcTable* pAvTable = pgsReportStyleHolder::CreateDefaultTable(4);
            (*pAvSTable2)(row,AvSTableCol++) << pAvTable;

            (*pAvSTable2)(row,AvSTableCol++) << dist.SetValue(avsDetails.ShearFailurePlaneLength);
            (*pAvSTable2)(row,AvSTableCol++) << avs.SetValue(avsDetails.AvgAvOverS);

            (*pAvTable)(0,0) << _T("");
            (*pAvTable)(0,1) << COLHDR(Sub2(_T("x"),_T("start")),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
            (*pAvTable)(0,2) << COLHDR(Sub2(_T("x"),_T("end")),rptLengthUnitTag,pDisplayUnits->GetSpanLengthUnit());
            (*pAvTable)(0,3) << COLHDR(Sub2(_T("A"),_T("v")) << _T("/s"),rptLengthUnitTag,pDisplayUnits->GetAvOverSUnit());
            RowIndexType avRow = pAvTable->GetNumberOfHeaderRows();
            IndexType ZoneIdx = 1;
            BOOST_FOREACH(const AvOverSZone& zone,avsDetails.Zones)
            {
               (*pAvTable)(avRow,0) << ZoneIdx;
               (*pAvTable)(avRow,1) << dist.SetValue(zone.Start);
               (*pAvTable)(avRow,2) << dist.SetValue(zone.End);
               (*pAvTable)(avRow,3) << avs.SetValue(zone.AvOverS);
               avRow++;
               ZoneIdx++;
            }
         }
      }


      const ShearCapacityDetails& scd = pShearCapacity->GetShearCapacityDetails(pierID,xbrTypes::Stage2,poi);
      
      (*pVcTable)(row,VcTableCol++) << location.SetValue(poi);
      (*pVcTable)(row,VcTableCol++) << scd.beta;
      (*pVcTable)(row,VcTableCol++) << stress.SetValue(fc);
      (*pVcTable)(row,VcTableCol++) << dim.SetValue(scd.bv);
      (*pVcTable)(row,VcTableCol++) << dim.SetValue(scd.dv);
      (*pVcTable)(row,VcTableCol++) << shear.SetValue(scd.Vc);

      (*pVsTable)(row,VsTableCol++) << location.SetValue(poi);
      if ( pierType == xbrTypes::pctIntegral )
      {
         (*pVsTable)(row,VsTableCol++) << avs.SetValue(scd.Av_over_S1);
         (*pVsTable)(row,VsTableCol++) << dim.SetValue(scd.dv1);
         (*pVsTable)(row,VsTableCol++) << avs.SetValue(scd.Av_over_S2);
         (*pVsTable)(row,VsTableCol++) << dim.SetValue(scd.dv2);
      }
      else
      {
         (*pVsTable)(row,VsTableCol++) << avs.SetValue(scd.Av_over_S1);
         (*pVsTable)(row,VsTableCol++) << dim.SetValue(scd.dv1);
      }
      (*pVsTable)(row,VsTableCol++) << stress.SetValue(fy);
      (*pVsTable)(row,VsTableCol++) << angle.SetValue(scd.theta);
      (*pVsTable)(row,VsTableCol++) << shear.SetValue(scd.Vs);

      (*pVnTable)(row,VnTableCol++) << location.SetValue(poi);
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vc);
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vs);
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vn1);
      (*pVnTable)(row,VnTableCol++) << stress.SetValue(fc);
      (*pVnTable)(row,VnTableCol++) << dim.SetValue(scd.bv);
      (*pVnTable)(row,VnTableCol++) << dim.SetValue(scd.dv);
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vn2);
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vn);
      (*pVnTable)(row,VnTableCol++) << scd.phi;
      (*pVnTable)(row,VnTableCol++) << shear.SetValue(scd.Vr);

      row++;
      DvTableRow++;
   }

   return pChapter;
}

CChapterBuilder* CShearCapacityDetailsChapterBuilder::Clone() const
{
   return new CShearCapacityDetailsChapterBuilder;
}
