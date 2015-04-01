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
#include "TestGraphController.h"

IMPLEMENT_DYNCREATE(CTestGraphController,CEAFGraphControlWindow)

CTestGraphController::CTestGraphController()
{
}

BEGIN_MESSAGE_MAP(CTestGraphController, CEAFGraphControlWindow)
	//{{AFX_MSG_MAP(CTestGraphController)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BUTTON1,&CTestGraphController::OnButton)
END_MESSAGE_MAP()

BOOL CTestGraphController::OnInitDialog()
{
   CEAFGraphControlWindow::OnInitDialog();
   CheckRadioButton(IDC_MOMENT,IDC_SHEAR,IDC_MOMENT);
   return TRUE;
}

void CTestGraphController::OnButton()
{
   AfxMessageBox(_T("OnButton in CTestGraphController"));
}

#ifdef _DEBUG
void CTestGraphController::AssertValid() const
{
	CEAFGraphControlWindow::AssertValid();
}

void CTestGraphController::Dump(CDumpContext& dc) const
{
	CEAFGraphControlWindow::Dump(dc);
}
#endif //_DEBUG

int CTestGraphController::GetGraphType()
{
   if( GetCheckedRadioButton(IDC_MOMENT,IDC_SHEAR) == IDC_MOMENT )
      return MOMENT_GRAPH;
   else
      return SHEAR_GRAPH;
}
