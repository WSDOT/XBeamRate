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
#include "GraphBuilder.h"

#include <IFace\XBeamRateAgent.h>
#include <\ARP\PGSuper\Include\IFace\Bridge.h>

#include <PgsExt\GirderLabel.h>
#include <EAF\EAFUtilities.h>

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

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   CComboBox* pcbPiers = (CComboBox*)GetDlgItem(IDC_PIERS);
   CComPtr<IXBeamRateAgent> pXBR;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pXBR);
   if ( SUCCEEDED(hr) )
   {
      // We are extending PGSuper/PGSplice
      // Fill the combo box with piers
      GET_IFACE2(pBroker,IBridge,pBridge);
      PierIndexType nPiers = pBridge->GetPierCount();
      for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
      {
         CString strPier;
         strPier.Format(_T("Pier %d"),LABEL_PIER(pierIdx));
         int idx = pcbPiers->AddString(strPier);
         pcbPiers->SetItemData(idx,(DWORD_PTR)pierIdx);
      }
   }
   else
   {
      int idx = pcbPiers->AddString(_T("Pier 2")); // put a dummy pier in the combo box
      pcbPiers->SetItemData(idx,(DWORD_PTR)1);
      // so when we get the pier index there is something to return
      pcbPiers->ShowWindow(SW_HIDE);
   }
   pcbPiers->SetCurSel(0);

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

CGraphDefinitions CXBRGraphController::GetSelectedGraphDefinitions()
{
   CXBRGraphBuilder* pGraphBuilder = (CXBRGraphBuilder*)GetGraphBuilder();
   const CGraphDefinitions& graphDefinitions = pGraphBuilder->GetGraphDefinitions();

   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int nSel = plbLoading->GetSelCount();
   CArray<int,int> selectedItems;
   selectedItems.SetSize(nSel);
   plbLoading->GetSelItems(nSel,selectedItems.GetData());

   CGraphDefinitions selectedGraphDefinitions;
   for ( int i = 0; i < nSel; i++ )
   {
      int idx = selectedItems.GetAt(i);
      IDType graphID = (IDType)plbLoading->GetItemData(idx);
      const CGraphDefinition& graphDef(graphDefinitions.FindGraphDefinition(graphID));
      selectedGraphDefinitions.AddGraphDefinition(graphDef);
   }

   return selectedGraphDefinitions;
}

ActionType CXBRGraphController::GetActionType()
{
#pragma Reminder("UPDATE: need to add load rating graph action")
   int graphType = GetCheckedRadioButton(IDC_MOMENT,IDC_SHEAR);
   if(  graphType == IDC_MOMENT )
      return actionMoment;
   else
      return actionShear;
}

PierIndexType CXBRGraphController::GetPier()
{
   CComboBox* pcbPier = (CComboBox*)GetDlgItem(IDC_PIERS);
   int curSel = pcbPier->GetCurSel();
   PierIndexType pierIdx = (PierIndexType)(pcbPier->GetItemData(curSel));
   return pierIdx;
}

void CXBRGraphController::FillLoadingList()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);

   CXBRGraphBuilder* pGraphBuilder = (CXBRGraphBuilder*)GetGraphBuilder();
   const CGraphDefinitions& graphDefinitions = pGraphBuilder->GetGraphDefinitions();
   IndexType nGraphs = graphDefinitions.GetGraphDefinitionCount();
   for ( IndexType graphIdx = 0; graphIdx < nGraphs; graphIdx++ )
   {
      const CGraphDefinition& graphDefinition(graphDefinitions.GetGraphDefinition(graphIdx));
      
      int idx = plbLoading->AddString(graphDefinition.m_Name.c_str());
      plbLoading->SetItemData(idx,graphDefinition.m_ID);
   }

   plbLoading->SetCurSel(0);
}
