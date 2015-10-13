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
#include "MomentCapacityDetailsChapterBuilder.h"
#include <EAF\EAFDisplayUnits.h>

#include <IFace\LoadRating.h>
#include <IFace\PointOfInterest.h>

#include "XBeamRateReportSpecification.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   CMomentCapacitDetailsChapterBuilder
****************************************************************************/
CMomentCapacityDetailsChapterBuilder::CMomentCapacityDetailsChapterBuilder()
{
}

//======================== OPERATORS  =======================================
//======================== OPERATIONS =======================================
LPCTSTR CMomentCapacityDetailsChapterBuilder::GetName() const
{
   return TEXT("Moment Capacity Details");
}

rptChapter* CMomentCapacityDetailsChapterBuilder::Build(CReportSpecification* pRptSpec,Uint16 level) const
{
   CXBeamRateReportSpecification* pXBRRptSpec = dynamic_cast<CXBeamRateReportSpecification*>(pRptSpec);

   // This report does not use the passd span and girder parameters
   rptChapter* pChapter = CXBeamRateChapterBuilder::Build(pRptSpec,level);

   CComPtr<IBroker> pBroker;
   pXBRRptSpec->GetBroker(&pBroker);

   PierIDType pierID = pXBRRptSpec->GetPierID();

   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   INIT_UV_PROTOTYPE( rptLengthUnitValue, location, pDisplayUnits->GetSpanLengthUnit(), false );
   INIT_UV_PROTOTYPE( rptLengthUnitValue, dim, pDisplayUnits->GetComponentDimUnit(), false );
   INIT_UV_PROTOTYPE( rptMomentUnitValue, moment, pDisplayUnits->GetMomentUnit(), false );

   rptParagraph* pPara = new rptParagraph;
   *pChapter << pPara;

   for ( int i = 0; i < 2; i++ )
   {
      bool bPositiveMoment = (i == 0 ? true : false);

      CString strTitle;
      strTitle.Format(_T("%s Moment"),(bPositiveMoment ? _T("Positive") : _T("Negative")));

      rptRcTable* pTable = pgsReportStyleHolder::CreateDefaultTable(5,strTitle);
      *pPara << pTable << rptNewLine;

#pragma Reminder("WORKING HERE")
      // Need alpha 1 in moment capacity details
      // Need beta 1 in moment capacity details
      // Need moment capacity equation
      // Need to report something for the reinforcement
      // Need equation graphics

      ColumnIndexType col = 0;
      (*pTable)(0,col++) << COLHDR(_T("Location"), rptLengthUnitTag, pDisplayUnits->GetSpanLengthUnit());
      (*pTable)(0,col++) << COLHDR(_T("c"), rptLengthUnitTag, pDisplayUnits->GetComponentDimUnit());
      (*pTable)(0,col++) << symbol(phi);
      (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("n")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());
      (*pTable)(0,col++) << COLHDR(Sub2(_T("M"),_T("r")), rptMomentUnitTag, pDisplayUnits->GetMomentUnit());

      GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
      std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID);

      GET_IFACE2(pBroker,IXBRMomentCapacity,pMomentCapacity);
      RowIndexType row = pTable->GetNumberOfHeaderRows();
      BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
      {
         col = 0;

         const MomentCapacityDetails& mcd = pMomentCapacity->GetMomentCapacityDetails(pierID,xbrTypes::Stage2,poi,bPositiveMoment);

         Float64 c;
         mcd.solution->get_NeutralAxisDepth(&c);

         (*pTable)(row,col++) << location.SetValue(poi.GetDistFromStart());
         (*pTable)(row,col++) << dim.SetValue(c);
         (*pTable)(row,col++) << mcd.phi;
         (*pTable)(row,col++) << moment.SetValue(mcd.Mn);
         (*pTable)(row,col++) << moment.SetValue(mcd.Mr);

         row++;
      }
   }

   return pChapter;
}

CChapterBuilder* CMomentCapacityDetailsChapterBuilder::Clone() const
{
   return new CMomentCapacityDetailsChapterBuilder;
}
