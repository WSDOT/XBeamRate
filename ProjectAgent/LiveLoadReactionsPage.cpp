///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2016  Washington State Department of Transportation
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

// LiveLoadReactionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LiveLoadReactionsPage.h"
#include "PierDlg.h"
#include <XBeamRateExt\XBeamRateUtilities.h>
#include <EAF\EAFDocument.h>
#include "..\Documentation\XBRate.hh"

void DDX_LiveLoadReactionsGrid(CDataExchange* pDX,CLiveLoadReactionGrid& grid,txnLiveLoadReactions& llData)
{
   if ( pDX->m_bSaveAndValidate )
   {
      grid.GetLiveLoadData(llData);
   }
   else
   {
      grid.SetLiveLoadData(llData);
   }
}


// CLiveLoadReactionsPage dialog

IMPLEMENT_DYNAMIC(CLiveLoadReactionsPage, CPropertyPage)

CLiveLoadReactionsPage::CLiveLoadReactionsPage()
	: CPropertyPage(CLiveLoadReactionsPage::IDD)
{

}

CLiveLoadReactionsPage::~CLiveLoadReactionsPage()
{
}

void CLiveLoadReactionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_Text(pDX,IDC_LF_INVENTORY,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthI_Inventory)]);
   DDX_Text(pDX,IDC_LF_OPERATING,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthI_Operating)]);
   DDX_Text(pDX,IDC_LF_LEGAL_ROUTINE,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthI_LegalRoutine)]);
   DDX_Text(pDX,IDC_LF_LEGAL_SPECIAL,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthI_LegalSpecial)]);
   DDX_Text(pDX,IDC_LF_PERMIT_ROUTINE,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthII_PermitRoutine)]);
   DDX_Text(pDX,IDC_LF_PERMIT_SPECIAL,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::StrengthII_PermitSpecial)]);
   DDX_Text(pDX,IDC_LF_PERMIT_ROUTINE_SERVICEI,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::ServiceI_PermitRoutine)]);
   DDX_Text(pDX,IDC_LF_PERMIT_SPECIAL_SERVICEI,pParent->m_PierData.m_gLL[GET_INDEX(pgsTypes::ServiceI_PermitSpecial)]);

   DDX_LiveLoadReactionsGrid(pDX,m_DesignGrid,        pParent->m_PierData.m_DesignLiveLoad);
   DDX_LiveLoadReactionsGrid(pDX,m_LegalRoutineGrid,  pParent->m_PierData.m_LegalRoutineLiveLoad);
   DDX_LiveLoadReactionsGrid(pDX,m_LegalSpecialGrid,  pParent->m_PierData.m_LegalSpecialLiveLoad);
   DDX_LiveLoadReactionsGrid(pDX,m_PermitRoutineGrid, pParent->m_PierData.m_PermitRoutineLiveLoad);
   DDX_LiveLoadReactionsGrid(pDX,m_PermitSpecialGrid, pParent->m_PierData.m_PermitSpecialLiveLoad);

   DDX_CBEnum(pDX,IDC_REACTION_APPLICATION,pParent->m_PierData.m_LiveLoadReactionApplication);
}


BEGIN_MESSAGE_MAP(CLiveLoadReactionsPage, CPropertyPage)
   ON_BN_CLICKED(IDC_DESIGN_ADD,OnDesignAdd)
   ON_BN_CLICKED(IDC_DESIGN_REMOVE,OnDesignRemove)
   ON_BN_CLICKED(IDC_LEGAL_ROUTINE_ADD,OnLegalRoutineAdd)
   ON_BN_CLICKED(IDC_LEGAL_ROUTINE_REMOVE,OnLegalRoutineRemove)
   ON_BN_CLICKED(IDC_LEGAL_SPECIAL_ADD,OnLegalSpecialAdd)
   ON_BN_CLICKED(IDC_LEGAL_SPECIAL_REMOVE,OnLegalSpecialRemove)
   ON_BN_CLICKED(IDC_PERMIT_ROUTINE_ADD,OnPermitRoutineAdd)
   ON_BN_CLICKED(IDC_PERMIT_ROUTINE_REMOVE,OnPermitRoutineRemove)
   ON_BN_CLICKED(IDC_PERMIT_SPECIAL_ADD,OnPermitSpecialAdd)
   ON_BN_CLICKED(IDC_PERMIT_SPECIAL_REMOVE,OnPermitSpecialRemove)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


// CLiveLoadReactionsPage message handlers

BOOL CLiveLoadReactionsPage::OnInitDialog()
{
   m_DesignGrid.SubclassDlgItem(IDC_DESIGN_GRID, this);
   m_DesignGrid.SetLoadRatingType(pgsTypes::lrDesign_Inventory);
   m_DesignGrid.CustomInit();

   m_LegalRoutineGrid.SubclassDlgItem(IDC_LEGAL_ROUTINE_GRID, this);
   m_LegalRoutineGrid.SetLoadRatingType(pgsTypes::lrLegal_Routine);
   m_LegalRoutineGrid.CustomInit();

   m_LegalSpecialGrid.SubclassDlgItem(IDC_LEGAL_SPECIAL_GRID, this);
   m_LegalSpecialGrid.SetLoadRatingType(pgsTypes::lrLegal_Special);
   m_LegalSpecialGrid.CustomInit();

   m_PermitRoutineGrid.SubclassDlgItem(IDC_PERMIT_ROUTINE_GRID, this);
   m_PermitRoutineGrid.SetLoadRatingType(pgsTypes::lrPermit_Routine);
   m_PermitRoutineGrid.CustomInit();

   m_PermitSpecialGrid.SubclassDlgItem(IDC_PERMIT_SPECIAL_GRID, this);
   m_PermitSpecialGrid.SetLoadRatingType(pgsTypes::lrPermit_Special);
   m_PermitSpecialGrid.CustomInit();

   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_REACTION_APPLICATION);
   int idx = pCB->AddString(_T("Apply reactions directly to cross beam"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::rlaCrossBeam);
   idx = pCB->AddString(_T("Apply reactions through bearings"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::rlaBearings);

   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CLiveLoadReactionsPage::OnDesignAdd()
{
   m_DesignGrid.AddVehicle();
}

void CLiveLoadReactionsPage::OnDesignRemove()
{
   m_DesignGrid.RemoveSelectedVehicles();
}

void CLiveLoadReactionsPage::OnLegalRoutineAdd()
{
   m_LegalRoutineGrid.AddVehicle();
}

void CLiveLoadReactionsPage::OnLegalRoutineRemove()
{
   m_LegalRoutineGrid.RemoveSelectedVehicles();
}

void CLiveLoadReactionsPage::OnLegalSpecialAdd()
{
   m_LegalSpecialGrid.AddVehicle();
}

void CLiveLoadReactionsPage::OnLegalSpecialRemove()
{
   m_LegalSpecialGrid.RemoveSelectedVehicles();
}

void CLiveLoadReactionsPage::OnPermitRoutineAdd()
{
   m_PermitRoutineGrid.AddVehicle();
}

void CLiveLoadReactionsPage::OnPermitRoutineRemove()
{
   m_PermitRoutineGrid.RemoveSelectedVehicles();
}

void CLiveLoadReactionsPage::OnPermitSpecialAdd()
{
   m_PermitSpecialGrid.AddVehicle();
}

void CLiveLoadReactionsPage::OnPermitSpecialRemove()
{
   m_PermitSpecialGrid.RemoveSelectedVehicles();
}

void CLiveLoadReactionsPage::OnHelp()
{
   EAFHelp(EAFGetDocument()->GetDocumentationSetName(), IDH_LIVE_LOAD);
}
