///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
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

#include "resource.h"
#include "GraphController.h"

IMPLEMENT_DYNCREATE(CXBRGraphController,CEAFGraphControlWindow)

CXBRGraphController::CXBRGraphController()
{
}

BEGIN_MESSAGE_MAP(CXBRGraphController, CEAFGraphControlWindow)
	//{{AFX_MSG_MAP(CXBRGraphController)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXBRGraphController::OnInitDialog()
{
   FillLoadingList();

   CEAFGraphControlWindow::OnInitDialog();
   CheckRadioButton(IDC_MOMENT,IDC_SHEAR,IDC_MOMENT);
   return TRUE;
}

#ifdef _DEBUG
void CXBRGraphController::AssertValid() const
{
	CEAFGraphControlWindow::AssertValid();
}

void CXBRGraphController::Dump(CDumpContext& dc) const
{
	CEAFGraphControlWindow::Dump(dc);
}
#endif //_DEBUG

int CXBRGraphController::GetGraphType()
{
   if( GetCheckedRadioButton(IDC_MOMENT,IDC_SHEAR) == IDC_MOMENT )
      return MOMENT_GRAPH;
   else
      return SHEAR_GRAPH;
}

XBRProductForceType CXBRGraphController::GetLoading()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int curSel = plbLoading->GetCurSel();
   if ( curSel == LB_ERR )
   {
      return pftLowerXBeam;
   }

   return XBRProductForceType(plbLoading->GetItemData(curSel));
}

void CXBRGraphController::FillLoadingList()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int idx = plbLoading->AddString(_T("Lower Cross Beam Dead Load"));
   plbLoading->SetItemData(idx,(DWORD_PTR)pftLowerXBeam);

   idx = plbLoading->AddString(_T("Upper Cross Beam Dead Load"));
   plbLoading->SetItemData(idx,(DWORD_PTR)pftUpperXBeam);

   idx = plbLoading->AddString(_T("DC Reactions"));
   plbLoading->SetItemData(idx,(DWORD_PTR)pftDCReactions);

   idx = plbLoading->AddString(_T("DW Reactions"));
   plbLoading->SetItemData(idx,(DWORD_PTR)pftDWReactions);
}
