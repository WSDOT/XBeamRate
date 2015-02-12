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

#include "stdafx.h"
#include <XBeamRateTitlePageBuilder.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CXBeamRateTitlePageBuilder::CXBeamRateTitlePageBuilder(IBroker* pBroker,LPCTSTR strTitle) :
CTitlePageBuilder(strTitle),
m_pBroker(pBroker)
{
}

CXBeamRateTitlePageBuilder::CXBeamRateTitlePageBuilder(const CXBeamRateTitlePageBuilder& other) :
CTitlePageBuilder(other),
m_pBroker(other.m_pBroker)
{
}

CXBeamRateTitlePageBuilder::~CXBeamRateTitlePageBuilder(void)
{
}

CTitlePageBuilder* CXBeamRateTitlePageBuilder::Clone() const
{
   return new CXBeamRateTitlePageBuilder(*this);
}

bool CXBeamRateTitlePageBuilder::NeedsUpdate(CReportHint* pHint,boost::shared_ptr<CReportSpecification>& pRptSpec)
{
   // don't let the title page control whether or not a report needs updating
   return false;
}

rptChapter* CXBeamRateTitlePageBuilder::Build(boost::shared_ptr<CReportSpecification>& pRptSpec)
{
   // Create a title page for the report
   rptChapter* pTitlePage = new rptChapter;

   rptParagraph* pPara = new rptParagraph;
   //pPara->SetStyleName(pgsReportStyleHolder::GetReportTitleStyle());
   *pTitlePage << pPara;

   std::_tstring title = GetReportTitle();

   *pPara << title.c_str();

   // Throw in a page break
   pPara = new rptParagraph;
   *pTitlePage << pPara;
   *pPara << rptNewPage;

   return pTitlePage;
}
