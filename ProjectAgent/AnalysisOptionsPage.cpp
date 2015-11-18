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

// RatingOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AnalysisOptionsPage.h"
#include "OptionsDlg.h"

#include <EAF\EAFDisplayUnits.h>

#include "..\HtmlHelp\HelpTopics.hh"

// CAnalysisOptionsPage dialog

IMPLEMENT_DYNAMIC(CAnalysisOptionsPage, CPropertyPage)

CAnalysisOptionsPage::CAnalysisOptionsPage()
	: CPropertyPage(CAnalysisOptionsPage::IDD)
{

}

CAnalysisOptionsPage::~CAnalysisOptionsPage()
{
}

void CAnalysisOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   COptionsDlg* pParent = (COptionsDlg*)GetParent();

   DDX_UnitValueAndTag(pDX,IDC_LL_STEP,IDC_LL_STEP_UNIT,pParent->m_Options.m_MaxLLStepSize,pDisplayUnits->GetSpanLengthUnit());
   DDV_UnitValueGreaterThanLimit(pDX,IDC_LL_STEP,pParent->m_Options.m_MaxLLStepSize,0.0,pDisplayUnits->GetSpanLengthUnit());

   DDX_Keyword(pDX,IDC_MAX_LANES,_T("ALL"),pParent->m_Options.m_MaxLoadedLanes);
}


BEGIN_MESSAGE_MAP(CAnalysisOptionsPage, CPropertyPage)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


// CAnalysisOptionsPage message handlers

void CAnalysisOptionsPage::OnHelp() 
{
   ::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_OPTIONS_ANALYSIS );
}
