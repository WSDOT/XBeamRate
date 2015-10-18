///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
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

#include "StdAfx.h"
#include "LoadingDetailsChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\Project.h>
#include <IFace\AnalysisResults.h>

#include "XBeamRateReportSpecification.h"

#include <XBeamRateExt\LongitudinalRebarData.h>
#include <XBeamRateExt\StirrupData.h>

#include <PgsExt\GirderLabel.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void write_limit_state_details(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);
void write_dead_load_details(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID);

/****************************************************************************
CLASS
   CLoadingDetailsChapterBuilder
****************************************************************************/
CLoadingDetailsChapterBuilder::CLoadingDetailsChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CLoadingDetailsChapterBuilder::GetName() const
{
   return TEXT("Loading Details");
}

rptChapter* CLoadingDetailsChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   USES_CONVERSION;

   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   write_limit_state_details(pBroker,pDisplayUnits,pChapter,pierID);
   write_dead_load_details(pBroker,pDisplayUnits,pChapter,pierID);

   return pChapter;
}


CChapterBuilder* CLoadingDetailsChapterBuilder::Clone() const
{
   return new CLoadingDetailsChapterBuilder;
}

void write_limit_state_details(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   GET_IFACE2(pBroker,IXBRProject,pProject);
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Limit States") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(8);
   pTable->SetColumnStyle(0,pgsReportStyleHolder::GetTableCellStyle(CB_NONE | CJ_LEFT));
   pTable->SetStripeRowColumnStyle(0,pgsReportStyleHolder::GetTableStripeRowCellStyle(CB_NONE | CJ_LEFT));
   *pPara << pTable << rptNewLine;

   ColumnIndexType col = 0;
   (*pTable)(0,col++) << _T("Limit State");
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("DC"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("DW"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("CR"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("SH"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("RE"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("PS"));
   (*pTable)(0,col++) << Sub2(symbol(gamma),_T("LL"));

   pgsTypes::LimitState limitStates[] = { pgsTypes::StrengthI_Inventory,
                                          pgsTypes::StrengthI_Operating,
                                          pgsTypes::StrengthI_LegalRoutine,
                                          pgsTypes::StrengthI_LegalSpecial,
                                          pgsTypes::StrengthII_PermitRoutine,
                                          pgsTypes::ServiceI_PermitRoutine,
                                          pgsTypes::StrengthII_PermitSpecial,
                                          pgsTypes::ServiceI_PermitSpecial };
   
   RowIndexType row = pTable->GetNumberOfHeaderRows();
   IndexType nLimitStates = sizeof(limitStates)/sizeof(pgsTypes::LimitState);
   for ( IndexType limitStateIdx = 0; limitStateIdx < nLimitStates; limitStateIdx++, row++ )
   {
      col = 0;

      pgsTypes::LimitState limitState = limitStates[limitStateIdx];
      Float64 DC = pProject->GetDCLoadFactor(limitState);
      Float64 DW = pProject->GetDWLoadFactor(limitState);
      Float64 CR = pProject->GetCRLoadFactor(limitState);
      Float64 SH = pProject->GetSHLoadFactor(limitState);
      Float64 RE = pProject->GetRELoadFactor(limitState);
      Float64 PS = pProject->GetPSLoadFactor(limitState);

      pgsTypes::LoadRatingType ratingType = ::RatingTypeFromLimitState(limitState);

      CString strLimitState = ::GetLimitStateName(limitState);

      Float64 LL = pProject->GetLiveLoadFactor(pierID,limitState,INVALID_INDEX);

      (*pTable)(row,col++) << strLimitState;
      (*pTable)(row,col++) << DC;
      (*pTable)(row,col++) << DW;
      (*pTable)(row,col++) << CR;
      (*pTable)(row,col++) << SH;
      (*pTable)(row,col++) << RE;
      (*pTable)(row,col++) << PS;
      (*pTable)(row,col++) << LL;
   }
}

void write_dead_load_details(IBroker* pBroker,IEAFDisplayUnits* pDisplayUnits,rptChapter* pChapter,PierIDType pierID)
{
   rptParagraph* pPara = new rptParagraph(pgsReportStyleHolder::GetHeadingStyle());
   *pChapter << pPara;
   *pPara << _T("Cross Beam Dead Load") << rptNewLine;

   pPara = new rptParagraph;
   *pChapter << pPara;

   INIT_UV_PROTOTYPE( rptLengthUnitValue, length, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptForcePerLengthUnitValue, fpl, pDisplayUnits->GetForcePerLengthUnit(), false );

   rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(4,_T("Lower Cross Beam"));
   *pPara << pTable << rptNewLine;

   ColumnIndexType col = 0;
   (*pTable)(0,col++) << COLHDR(Sub2(_T("X"),_T("start")), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(Sub2(_T("X"),_T("end")), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
   (*pTable)(0,col++) << COLHDR(Sub2(_T("w"),_T("start")), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());
   (*pTable)(0,col++) << COLHDR(Sub2(_T("w"),_T("end")), rptForcePerLengthUnitTag, pDisplayUnits->GetForcePerLengthUnit());

   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
   const std::vector<LowerXBeamLoad>& vLoads = pProductForces->GetLowerCrossBeamLoading(pierID);
   RowIndexType row = pTable->GetNumberOfHeaderRows();
   BOOST_FOREACH(const LowerXBeamLoad& load,vLoads)
   {
      col = 0;

      (*pTable)(row,col++) << length.SetValue(load.Xs);
      (*pTable)(row,col++) << length.SetValue(load.Xe);
      (*pTable)(row,col++) << fpl.SetValue(load.Ws);
      (*pTable)(row,col++) << fpl.SetValue(load.We);

      row++;
   }

   fpl.ShowUnitTag(true);
   pPara = new rptParagraph;
   *pChapter << pPara;
   *pPara << _T("Upper Cross Beam Diaphragm, w = ") << fpl.SetValue(pProductForces->GetUpperCrossBeamLoading(pierID));
}
