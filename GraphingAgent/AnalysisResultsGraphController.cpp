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

#include "stdafx.h"

#include "resource.h"
#include "AnalysisResultsGraphController.h"
#include "AnalysisResultsGraphBuilder.h"

#include <IFace\XBeamRateAgent.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>

#include <PgsExt\GirderLabel.h>
#include <PgsExt\BridgeDescription2.h>
#include <EAF\EAFUtilities.h>
#include <XBeamRateExt\XBeamRateUtilities.h>

IMPLEMENT_DYNCREATE(CXBRAnalysisResultsGraphController,CEAFGraphControlWindow)

CXBRAnalysisResultsGraphController::CXBRAnalysisResultsGraphController()
{
}

BEGIN_MESSAGE_MAP(CXBRAnalysisResultsGraphController, CEAFGraphControlWindow)
	//{{AFX_MSG_MAP(CXBRAnalysisResultsGraphController)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXBRAnalysisResultsGraphController::OnInitDialog()
{
   FillPierList();
   FillLoadingList();
   
   CEAFGraphControlWindow::OnInitDialog();
   CheckRadioButton(IDC_MOMENT,IDC_SHEAR,IDC_MOMENT);

   return TRUE;
}

#ifdef _DEBUG
void CXBRAnalysisResultsGraphController::AssertValid() const
{
	CEAFGraphControlWindow::AssertValid();
}

void CXBRAnalysisResultsGraphController::Dump(CDumpContext& dc) const
{
	CEAFGraphControlWindow::Dump(dc);
}
#endif //_DEBUG

CXBRAnalysisResultsGraphDefinitions CXBRAnalysisResultsGraphController::GetSelectedGraphDefinitions()
{
   CXBRAnalysisResultsGraphBuilder* pGraphBuilder = (CXBRAnalysisResultsGraphBuilder*)GetGraphBuilder();
   const CXBRAnalysisResultsGraphDefinitions& graphDefinitions = pGraphBuilder->GetGraphDefinitions();

   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int nSel = plbLoading->GetSelCount();
   CArray<int,int> selectedItems;
   selectedItems.SetSize(nSel);
   plbLoading->GetSelItems(nSel,selectedItems.GetData());

   CXBRAnalysisResultsGraphDefinitions selectedGraphDefinitions;
   for ( int i = 0; i < nSel; i++ )
   {
      int idx = selectedItems.GetAt(i);
      IDType graphID = (IDType)plbLoading->GetItemData(idx);
      const CXBRAnalysisResultsGraphDefinition& graphDef(graphDefinitions.FindGraphDefinition(graphID));
      selectedGraphDefinitions.AddGraphDefinition(graphDef);
   }

   return selectedGraphDefinitions;
}

ActionType CXBRAnalysisResultsGraphController::GetActionType()
{
#pragma Reminder("UPDATE: need to add load rating graph action")
   int graphType = GetCheckedRadioButton(IDC_MOMENT,IDC_SHEAR);
   if(  graphType == IDC_MOMENT )
   {
      return actionMoment;
   }
   else
   {
      return actionShear;
   }
}

PierIDType CXBRAnalysisResultsGraphController::GetPierID()
{
   CComboBox* pcbPier = (CComboBox*)GetDlgItem(IDC_PIERS);
   int curSel = pcbPier->GetCurSel();
   PierIDType pierID = (PierIDType)(pcbPier->GetItemData(curSel));
   return pierID;
}

BOOL CALLBACK EnableChildWindow(HWND hwnd,LPARAM lParam)
{
   ::EnableWindow(hwnd,(int)lParam);
   return TRUE;
}

void CXBRAnalysisResultsGraphController::EnableControls(BOOL bEnable)
{
   EnumChildWindows(GetSafeHwnd(),EnableChildWindow,bEnable);
}

void CXBRAnalysisResultsGraphController::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   FillPierList();

   CXBRAnalysisResultsGraphBuilder* pGraphBuilder = (CXBRAnalysisResultsGraphBuilder*)GetGraphBuilder();
   pGraphBuilder->UpdateGraphDefinitions();
}

void CXBRAnalysisResultsGraphController::FillPierList()
{
   CComboBox* pcbPiers = (CComboBox*)GetDlgItem(IDC_PIERS);
   int curSel = pcbPiers->GetCurSel();
   pcbPiers->ResetContent();

   if ( IsStandAlone() )
   {
      int idx = pcbPiers->AddString(_T("Stand Alone Mode")); // put a dummy pier in the combo box
      pcbPiers->SetItemData(idx,(DWORD_PTR)INVALID_ID);
      // so when we get the pier index there is something to return
      pcbPiers->ShowWindow(SW_HIDE);
   }
   else
   {
      // We are extending PGSuper/PGSplice
      // Fill the combo box with piers
      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);

      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
      PierIndexType nPiers = pBridgeDesc->GetPierCount();
      for ( PierIndexType pierIdx = 0; pierIdx < nPiers; pierIdx++ )
      {
         // Can only load rate piers with a physical description
         const CPierData2* pPierData = pBridgeDesc->GetPier(pierIdx);
         if ( pPierData->GetPierModelType() == pgsTypes::pmtPhysical )
         {
            PierIDType pierID = pPierData->GetID();
            CString strPier;
            strPier.Format(_T("Pier %d"),LABEL_PIER(pierIdx));
            int idx = pcbPiers->AddString(strPier);
            pcbPiers->SetItemData(idx,(DWORD_PTR)pierID);
         }
      }
   }

   pcbPiers->SetCurSel(curSel == CB_ERR ? 0 : curSel);
}

void CXBRAnalysisResultsGraphController::FillLoadingList()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
#pragma Reminder("WORKING HERE - need to capture current selection and attempt to reselect")
   plbLoading->ResetContent();

   CXBRAnalysisResultsGraphBuilder* pGraphBuilder = (CXBRAnalysisResultsGraphBuilder*)GetGraphBuilder();
   const CXBRAnalysisResultsGraphDefinitions& graphDefinitions = pGraphBuilder->GetGraphDefinitions();
   IndexType nGraphs = graphDefinitions.GetGraphDefinitionCount();
   for ( IndexType graphIdx = 0; graphIdx < nGraphs; graphIdx++ )
   {
      const CXBRAnalysisResultsGraphDefinition& graphDefinition(graphDefinitions.GetGraphDefinition(graphIdx));
      
      int idx = plbLoading->AddString(graphDefinition.m_Name.c_str());
      plbLoading->SetItemData(idx,graphDefinition.m_ID);
   }

   plbLoading->SetCurSel(0);
}
