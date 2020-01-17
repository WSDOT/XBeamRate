///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2020  Washington State Department of Transportation
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


#pragma once

#include <ReportManager\ReportBuilder.h>

/*****************************************************************************
CLASS 
   CXBeamRateReportBuilder

   We need to over-ride the report building process so we are creating
   a custom version of the report builder
*****************************************************************************/

class CXBeamRateReportBuilder : public CReportBuilder
{
public:
	CXBeamRateReportBuilder(LPCTSTR strName,bool bHidden = false,bool bIncludeTimingChapter=false);
   virtual std::shared_ptr<rptReport> CreateReport(std::shared_ptr<CReportSpecification>& pRptSpec) override;
};
