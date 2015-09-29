///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
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

#include "stdafx.h"

#include "resource.h"
#include "LiveLoadGraphController.h"
#include "LiveLoadGraphBuilder.h"

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>

#include <PgsExt\GirderLabel.h>
#include <PgsExt\BridgeDescription2.h>
#include <EAF\EAFUtilities.h>
#include <XBeamRateExt\XBeamRateUtilities.h>

IMPLEMENT_DYNCREATE(CXBRLiveLoadGraphController,CEAFGraphControlWindow)

CXBRLiveLoadGraphController::CXBRLiveLoadGraphController()
{
}

BEGIN_MESSAGE_MAP(CXBRLiveLoadGraphController, CEAFGraphControlWindow)
	//{{AFX_MSG_MAP(CXBRLiveLoadGraphController)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXBRLiveLoadGraphController::OnInitDialog()
{
   FillPierList();
   FillLoadingList();
   
   CEAFGraphControlWindow::OnInitDialog();
   CheckRadioButton(IDC_MOMENT,IDC_SHEAR,IDC_MOMENT);

   return TRUE;
}

#ifdef _DEBUG
void CXBRLiveLoadGraphController::AssertValid() const
{
	CEAFGraphControlWindow::AssertValid();
}

void CXBRLiveLoadGraphController::Dump(CDumpContext& dc) const
{
	CEAFGraphControlWindow::Dump(dc);
}
#endif //_DEBUG

std::vector<IndexType> CXBRLiveLoadGraphController::GetSelectedLiveLoadConfigurations()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int nSel = plbLoading->GetSelCount();
   CArray<int,int> selectedItems;
   selectedItems.SetSize(nSel);
   plbLoading->GetSelItems(nSel,selectedItems.GetData());

   std::vector<IndexType> vLiveLoadConfigs;
   for ( int i = 0; i < nSel; i++ )
   {
      int idx = selectedItems.GetAt(i);
      IndexType llConfigIdx = (IndexType)plbLoading->GetItemData(idx);
      vLiveLoadConfigs.push_back(llConfigIdx);
   }

   return vLiveLoadConfigs;
}

ActionType CXBRLiveLoadGraphController::GetActionType()
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

PierIDType CXBRLiveLoadGraphController::GetPierID()
{
   CComboBox* pcbPier = (CComboBox*)GetDlgItem(IDC_PIERS);
   int curSel = pcbPier->GetCurSel();
   PierIDType pierID = (PierIDType)(pcbPier->GetItemData(curSel));
   return pierID;
}

pgsTypes::LoadRatingType CXBRLiveLoadGraphController::GetLoadRatingType()
{
   CComboBox* pcbRatingType = (CComboBox*)GetDlgItem(IDC_RATING_TYPE);
   int curSel = pcbRatingType->GetCurSel();
   pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)(pcbRatingType->GetItemData(curSel));
   return ratingType;
}

VehicleIndexType CXBRLiveLoadGraphController::GetVehicleIndex()
{
   CComboBox* pcbVehicle = (CComboBox*)GetDlgItem(IDC_VEHICLE);
   int curSel = pcbVehicle->GetCurSel();
   VehicleIndexType vehicleIdx = (VehicleIndexType)(pcbVehicle->GetItemData(curSel));
   return vehicleIdx;
}

BOOL CALLBACK EnableMyChildWindow(HWND hwnd,LPARAM lParam)
{
   ::EnableWindow(hwnd,(int)lParam);
   return TRUE;
}

void CXBRLiveLoadGraphController::EnableControls(BOOL bEnable)
{
   EnumChildWindows(GetSafeHwnd(),EnableMyChildWindow,bEnable);
}

void CXBRLiveLoadGraphController::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   FillPierList();
   FillRatingType();
   FillVehicleType();
}

void CXBRLiveLoadGraphController::FillPierList()
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
      for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ ) // skip abutments (pierIdx = 0 & nPiers-1)
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

void CXBRLiveLoadGraphController::FillRatingType()
{
   CComboBox* pcbRatingType = (CComboBox*)GetDlgItem(IDC_RATING_TYPE);
   for ( int i = 0; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      int idx = pcbRatingType->AddString(GetLiveLoadTypeName(ratingType));
      pcbRatingType->SetItemData(idx,(DWORD_PTR)ratingType);
   }
   pcbRatingType->SetCurSel(0);
}

void CXBRLiveLoadGraphController::FillVehicleType()
{
   CComboBox* pcbVehicle = (CComboBox*)GetDlgItem(IDC_VEHICLE);
   int curSel = pcbVehicle->GetCurSel();

   pcbVehicle->ResetContent();

   PierIDType pierID = GetPierID();
   pgsTypes::LoadRatingType ratingType = GetLoadRatingType();

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRProject,pProject);

   VehicleIndexType nLiveLoads = pProject->GetLiveLoadReactionCount(pierID,ratingType);
   for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nLiveLoads; vehicleIdx++ )
   {
      std::_tstring strLiveLoad = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);
      if ( strLiveLoad != _T("No Live Load Defined") )
      {
         int idx = pcbVehicle->AddString(strLiveLoad.c_str());
         pcbVehicle->SetItemData(idx,(DWORD_PTR)vehicleIdx);
      }
   }

   curSel = pcbVehicle->SetCurSel(curSel);
   if ( curSel == CB_ERR )
   {
      pcbVehicle->SetCurSel(0);
   }
}

void CXBRLiveLoadGraphController::SelectAll()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   plbLoading->SelItemRange(TRUE,0,plbLoading->GetCount());
}

void CXBRLiveLoadGraphController::Next()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int nSel = plbLoading->GetSelCount();
   CArray<int,int> selectedItems;
   selectedItems.SetSize(nSel);
   plbLoading->GetSelItems(nSel,selectedItems.GetData());

   int curSel = selectedItems.GetAt(selectedItems.GetCount()-1);
   curSel++;
   plbLoading->SetSel(-1,FALSE);
   plbLoading->SetSel(curSel);
}

void CXBRLiveLoadGraphController::Prev()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   int nSel = plbLoading->GetSelCount();
   CArray<int,int> selectedItems;
   selectedItems.SetSize(nSel);
   plbLoading->GetSelItems(nSel,selectedItems.GetData());

   int curSel = selectedItems.GetAt(0);
   curSel--;
   plbLoading->SetSel(-1,FALSE);
   plbLoading->SetSel(curSel);
}

void CXBRLiveLoadGraphController::RatingTypeChanged()
{
   FillVehicleType();
   FillLoadingList();
}

void CXBRLiveLoadGraphController::FillLoadingList()
{
   CListBox* plbLoading = (CListBox*)GetDlgItem(IDC_LOADING);
   plbLoading->ResetContent();

   PierIDType pierID = GetPierID();

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);

   pgsTypes::LoadRatingType ratingType = GetLoadRatingType();
   IndexType nLiveLoadConfigs = pProductForces->GetLiveLoadConfigurationCount(pierID,ratingType);
   for ( IndexType llConfigIdx = 0; llConfigIdx < nLiveLoadConfigs; llConfigIdx++ )
   {
      CString str;
      str.Format(_T("%d"),(llConfigIdx+1));
      int idx = plbLoading->AddString(str);
      plbLoading->SetItemData(idx,llConfigIdx);
   }

   plbLoading->SetCurSel(0);
}