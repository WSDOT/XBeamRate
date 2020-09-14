///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2020  Washington State Department of Transportation
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

#include <ReportManager\ChapterBuilder.h>
#include "XBeamRateChapterBuilder.h"

class xbrRatingArtifact;
class xbrPointOfInterest;

/*****************************************************************************
CLASS 
   CTestChapterBuilder

*****************************************************************************/

class CLoadRatingDetailsChapterBuilder : public CXBeamRateChapterBuilder
{
public:
   CLoadRatingDetailsChapterBuilder();
   virtual LPCTSTR GetName() const override;
   rptChapter* Build(CReportSpecification* pRptSpec,Uint16 level) const;
   virtual CChapterBuilder* Clone() const override;

private:
   // Prevent accidental copying and assignment
   CLoadRatingDetailsChapterBuilder(const CLoadRatingDetailsChapterBuilder&) = delete;
   CLoadRatingDetailsChapterBuilder& operator=(const CLoadRatingDetailsChapterBuilder&) = delete;

   void MomentRatingDetails(rptChapter* pChapter,IBroker* pBroker,PierIDType pierID,pgsTypes::LoadRatingType ratingType,bool bPositiveMoment,const xbrRatingArtifact* pRatingArtifact) const;
   void ShearRatingDetails(rptChapter* pChapter,IBroker* pBroker,PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrRatingArtifact* pRatingArtifact) const;
   void ReinforcementYieldingDetails(rptChapter* pChapter,IBroker* pBroker,PierIDType pierID,pgsTypes::LoadRatingType ratingType,bool bPositiveMoment,const xbrRatingArtifact* pRatingArtifact) const;
   void LoadPostingDetails(rptChapter* pChapter,IBroker* pBroker,const xbrRatingArtifact* pRatingArtifact) const;

   bool ReportAtThisPoi(const xbrPointOfInterest& poi,const xbrPointOfInterest& controllingPoi, bool IsNegativeMoment) const;

   mutable bool m_bReportEvenIncrements;
   mutable bool m_bReportNegativeMomentBetweenFOCs;
};
