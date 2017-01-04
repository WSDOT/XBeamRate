///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2017  Washington State Department of Transportation
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


// CLoadRatingOptionsPage dialog

class CLoadRatingOptionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLoadRatingOptionsPage)

public:
	CLoadRatingOptionsPage();
	virtual ~CLoadRatingOptionsPage();

// Dialog Data
	enum { IDD = IDD_RATING_OPTIONS };
   virtual BOOL OnInitDialog();

   pgsTypes::AnalysisType m_AnalysisType;
   xbrTypes::PermitRatingMethod m_PermitRatingMethod;
   Float64 m_MaxLLStepSize;
   IndexType m_MaxLoadedLanes;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   void FillAnalysisModeComboBox();
   void FillPermitRatingMethodComboBox();

   afx_msg void OnHelp();

	DECLARE_MESSAGE_MAP()
};
