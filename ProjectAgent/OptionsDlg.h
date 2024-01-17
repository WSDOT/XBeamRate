///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2024  Washington State Department of Transportation
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

#include <txnEditOptions.h>
#include "RatingOptionsPage.h"
#include "AnalysisOptionsPage.h"
#include "CapacityOptionsPage.h"


// COptionsDlg dialog

class COptionsDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);   // standard constructor
	virtual ~COptionsDlg();

   void SetOptions(const txnEditOptionsData& options);
   const txnEditOptionsData& GetOptions() const;

protected:
	DECLARE_MESSAGE_MAP()

   void Init();

   txnEditOptionsData m_Options;

   CRatingOptionsPage m_RatingOptions;
   CAnalysisOptionsPage m_AnalysisOptions;
   CCapacityOptionsPage m_CapacityOptions;

   friend CRatingOptionsPage;
   friend CAnalysisOptionsPage;
   friend CCapacityOptionsPage;
};
