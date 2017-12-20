///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2018  Washington State Department of Transportation
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
#include <XBeamRateReportSpecification.h>

#include <IFace\VersionInfo.h>
#include <IFace\Project.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <EAF\EAFDocument.h>

#include <PgsExt\PierData2.h>
#include <PgsExt\GirderLabel.h>

#include <MFCTools\AutoRegistry.h>

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

bool CXBeamRateTitlePageBuilder::NeedsUpdate(CReportHint* pHint,std::shared_ptr<CReportSpecification>& pRptSpec)
{
   // don't let the title page control whether or not a report needs updating
   return false;
}

rptChapter* CXBeamRateTitlePageBuilder::Build(std::shared_ptr<CReportSpecification>& pRptSpec)
{
   std::shared_ptr<CXBeamRateReportSpecification> pXBRRptSpec = std::dynamic_pointer_cast<CXBeamRateReportSpecification,CReportSpecification>(pRptSpec);

   // Create a title page for the report
   rptChapter* pTitlePage = new rptChapter;

   rptParagraph* pPara = new rptParagraph;
   pPara->SetStyleName(rptStyleManager::GetReportTitleStyle());
   *pTitlePage << pPara;

   std::_tstring title = GetReportTitle();

   *pPara << title.c_str();

   if ( pXBRRptSpec->GetPierID() != INVALID_INDEX )
   {
      GET_IFACE(IBridgeDescription,pIBridgeDesc);

      const CPierData2* pPier = pIBridgeDesc->FindPier(pXBRRptSpec->GetPierID());
      ATLASSERT(pPier);

      *pPara << rptNewLine;
      *pPara << _T("for Pier ") << LABEL_PIER(pPier->GetIndex()) << rptNewLine;
   }


   pPara = new rptParagraph;
   pPara->SetStyleName(rptStyleManager::GetReportTitleStyle());
   *pTitlePage << pPara;
#if defined _WIN64
   *pPara << _T("XBRate") << Super(symbol(TRADEMARK)) << _T(" (x64)") << rptNewLine;
#else
   *pPara << _T("XBRate") << Super(symbol(TRADEMARK)) << _T(" (x86)") << rptNewLine;
#endif

   pPara = new rptParagraph(rptStyleManager::GetCopyrightStyle());
   *pTitlePage << pPara;
   *pPara << _T("Copyright ") << symbol(COPYRIGHT) << _T(" ") << sysDate().Year() << _T(", WSDOT, All Rights Reserved") << rptNewLine;

   pPara = new rptParagraph;
   pPara->SetStyleName(rptStyleManager::GetReportSubtitleStyle());
   *pTitlePage << pPara;
   GET_IFACE(IXBRVersionInfo,pVerInfo);
   *pPara << pVerInfo->GetVersionString() << rptNewLine;

   // Title page art image.
   // Can't use rptStyleManager method because it is for PGSuper/PGSplice.
   // We will just do it here locally.
   CAutoRegistry ar(_T("XBeamRate"));
   CWinApp* pApp = (CWinApp*)AfxGetApp();
   std::_tstring strImageName(pApp->GetProfileString(_T("Settings"),_T("ReportCoverImage"),_T("xbr_title_page_art.gif")));
   std::_tstring strImage;
   if ( strImageName == _T("xbr_title_page_art.gif") )
   {
      strImage = std::_tstring(rptStyleManager::GetImagePath()) + strImageName;
   }
   else
   {
      strImage = strImageName;
   }

   WIN32_FIND_DATA file_find_data;
   HANDLE hFind;
   hFind = FindFirstFile(strImage.c_str(),&file_find_data);
   if ( hFind != INVALID_HANDLE_VALUE )
   {
      *pPara << rptRcImage(strImage) << rptNewLine;
   }
   *pPara << rptNewLine << rptNewLine;

   ////////////////////////////
   // If this is stand alone, use the IXBRProjectProperties, otherwise
   // use the IProjectProperties and report in PGSuper format
   ////////////////////////////

   GET_IFACE(IEAFDocument,pDocument);
   GET_IFACE(IXBRProjectProperties,pProps);
   rptParagraph* pPara3 = new rptParagraph( rptStyleManager::GetHeadingStyle() );
   *pTitlePage << pPara3;

   rptRcTable* pTbl = rptStyleManager::CreateTableNoHeading(2,_T("Project Properties"));

   pTbl->SetColumnStyle(0,rptStyleManager::GetTableCellStyle( CB_NONE | CJ_LEFT ) );
   pTbl->SetColumnStyle(1,rptStyleManager::GetTableCellStyle( CB_NONE | CJ_LEFT ) );
   pTbl->SetStripeRowColumnStyle(0,rptStyleManager::GetTableStripeRowCellStyle( CB_NONE | CJ_LEFT ) );
   pTbl->SetStripeRowColumnStyle(1,rptStyleManager::GetTableStripeRowCellStyle( CB_NONE | CJ_LEFT ) );

   *pPara3 << rptNewLine << rptNewLine << rptNewLine;

   *pPara3 << pTbl;
   (*pTbl)(0,0) << _T("Bridge Name");
   (*pTbl)(0,1) << pProps->GetBridgeName();
   (*pTbl)(1,0) << _T("Bridge ID");
   (*pTbl)(1,1) << pProps->GetBridgeID();
   (*pTbl)(2,0) << _T("Company");
   (*pTbl)(2,1) << pProps->GetCompany();
   (*pTbl)(3,0) << _T("Engineer");
   (*pTbl)(3,1) << pProps->GetEngineer();
   (*pTbl)(4,0) << _T("Job Number");
   (*pTbl)(4,1) << pProps->GetJobNumber();
   (*pTbl)(5,0) << _T("Comments");
   (*pTbl)(5,1) << pProps->GetComments();
   (*pTbl)(6,0) << _T("File");
   (*pTbl)(6,1) << pDocument->GetFilePath();


   // Throw in a page break
   pPara = new rptParagraph;
   *pTitlePage << pPara;
   *pPara << rptNewPage;

   return pTitlePage;
}
