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
#include "RatingOptionsPage.h"
#include "OptionsDlg.h"

#include <txnEditOptions.h>


// CRatingOptionsPage dialog

IMPLEMENT_DYNAMIC(CRatingOptionsPage, CPropertyPage)

CRatingOptionsPage::CRatingOptionsPage()
	: CPropertyPage(CRatingOptionsPage::IDD)
{

}

CRatingOptionsPage::~CRatingOptionsPage()
{
}

void CRatingOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

   COptionsDlg* pParent = (COptionsDlg*)GetParent();

   DDX_Check_Bool(pDX,IDC_DESIGN_RATING,pParent->m_Options.m_bDesignRating);
   DDX_Check_Bool(pDX,IDC_DESIGN_SHEAR,pParent->m_Options.m_bDesignRateForShear);

   DDX_Check_Bool(pDX,IDC_LEGAL_RATING,pParent->m_Options.m_bLegalRating);
   DDX_Check_Bool(pDX,IDC_LEGAL_SHEAR,pParent->m_Options.m_bLegalRateForShear);

   DDX_Check_Bool(pDX,IDC_PERMIT_RATING,pParent->m_Options.m_bPermitRating);
   DDX_Check_Bool(pDX,IDC_PERMIT_SHEAR,pParent->m_Options.m_bPermitRateForShear);
   DDX_Check_Bool(pDX,IDC_PERMIT_YIELD_STRESS,pParent->m_Options.m_bCheckYieldStress);
   DDX_Text(pDX,IDC_ALLOWABLE_STRESS,pParent->m_Options.m_YieldStressCoefficient);
   DDX_CBEnum(pDX,IDC_PERMIT_FACTORS,pParent->m_Options.m_PermitRatingMethod);

   DDX_Text(pDX,IDC_SYSTEM_FACTOR_FLEXURE,pParent->m_Options.m_SystemFactorFlexure);
   DDX_Text(pDX,IDC_SYSTEM_FACTOR_SHEAR,pParent->m_Options.m_SystemFactorShear);
}


BEGIN_MESSAGE_MAP(CRatingOptionsPage, CPropertyPage)
END_MESSAGE_MAP()


// COptionsDlg message handlers


BOOL CRatingOptionsPage::OnInitDialog()
{
   FillPermitFactorList();

   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CRatingOptionsPage::FillPermitFactorList()
{
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_PERMIT_FACTORS);
   pCB->ResetContent();

   int idx = pCB->AddString(_T("AASHTO MBE Equation 6A.4.2.1-1"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmAASHTO);

   idx = pCB->AddString(_T("WSDOT BDM Equation 13.1.1A-2"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmWSDOT);
}
