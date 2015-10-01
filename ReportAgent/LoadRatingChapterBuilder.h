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


#pragma once;
#include "XBeamRateChapterBuilder.h"


/*****************************************************************************
CLASS 
   CLoadRatingChapterBuilder

   Reports load rating outcome.


DESCRIPTION
   Reports load rating outcome.

COPYRIGHT
   Copyright � 1997-2009
   Washington State Department Of Transportation
   All Rights Reserved

LOG
   rab : 12.07.2009 : Created file
*****************************************************************************/

class CLoadRatingChapterBuilder : public CXBeamRateChapterBuilder
{
public:
   CLoadRatingChapterBuilder();

   //------------------------------------------------------------------------
   virtual LPCTSTR GetName() const;

   //------------------------------------------------------------------------
   virtual rptChapter* Build(CReportSpecification* pRptSpec,Uint16 level) const;

   //------------------------------------------------------------------------
   virtual CChapterBuilder* Clone() const;

private:
   // Prevent accidental copying and assignment
   CLoadRatingChapterBuilder(const CLoadRatingChapterBuilder&);
   CLoadRatingChapterBuilder& operator=(const CLoadRatingChapterBuilder&);
};