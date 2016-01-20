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

// PierLayoutPat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProjectAgent.h"
#include "PierLayoutPage.h"
#include "PierDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>

#include <PgsExt\GirderLabel.h>
#include "..\HtmlHelp\HelpTopics.hh"

// CPierLayoutPage dialog


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DDX_ColumnGrid(CDataExchange* pDX,CColumnLayoutGrid& grid,xbrPierData& pier)
{
   if ( pDX->m_bSaveAndValidate )
   {
      grid.GetColumnData(pier);
   }
   else
   {
      grid.SetColumnData(pier);
   }
}

void DDV_ColumnGrid(CDataExchange* pDX,CColumnLayoutGrid& grid)
{
   if ( pDX->m_bSaveAndValidate )
   {
      if ( grid.GetRowCount() == 0 )
      {
         AfxMessageBox(_T("The pier must have at least one column"),MB_OK | MB_ICONEXCLAMATION);
         pDX->Fail();
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPierLayoutPage property page

IMPLEMENT_DYNCREATE(CPierLayoutPage, CPropertyPage)

CPierLayoutPage::CPierLayoutPage() : CPropertyPage(CPierLayoutPage::IDD)
{
	//{{AFX_DATA_INIT(CPierLayoutPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPierLayoutPage::~CPierLayoutPage()
{
}

void CPierLayoutPage::DoDataExchange(CDataExchange* pDX)
{
  
   CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPierConnectionsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_MetaFileStatic(pDX, IDC_PIER_LAYOUT, m_LayoutPicture,_T("PIERLAYOUT"), _T("Metafile") );

   // Transverse location of the pier
   DDX_CBIndex(pDX,IDC_REFCOLUMN,pParent->m_PierData.m_PierData.GetRefColumnIndex());
   DDX_OffsetAndTag(pDX,IDC_REFCOLUMN_OFFSET,IDC_REFCOLUMN_OFFSET_UNIT,pParent->m_PierData.m_PierData.GetRefColumnOffset(), pDisplayUnits->GetSpanLengthUnit() );
   DDX_CBItemData(pDX,IDC_REFCOLUMN_MEASUREMENT,pParent->m_PierData.m_PierData.GetColumnLayoutDatum());

   DDX_UnitValueAndTag(pDX,IDC_H1,IDC_H1_UNIT,pParent->m_PierData.m_PierData.GetH1(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_H2,IDC_H2_UNIT,pParent->m_PierData.m_PierData.GetH2(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_X1,IDC_X1_UNIT,pParent->m_PierData.m_PierData.GetX1(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_X2,IDC_X2_UNIT,pParent->m_PierData.m_PierData.GetX2(),pDisplayUnits->GetSpanLengthUnit() );

   DDX_UnitValueAndTag(pDX,IDC_H3,IDC_H3_UNIT,pParent->m_PierData.m_PierData.GetH3(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_H4,IDC_H4_UNIT,pParent->m_PierData.m_PierData.GetH4(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_X3,IDC_X3_UNIT,pParent->m_PierData.m_PierData.GetX3(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_X4,IDC_X4_UNIT,pParent->m_PierData.m_PierData.GetX4(),pDisplayUnits->GetSpanLengthUnit() );

   DDX_UnitValueAndTag(pDX,IDC_W,IDC_W_UNIT,pParent->m_PierData.m_PierData.GetW(),pDisplayUnits->GetSpanLengthUnit() );

   DDX_UnitValueAndTag(pDX,IDC_X5,IDC_X5_UNIT,pParent->m_PierData.m_PierData.GetX5(),pDisplayUnits->GetSpanLengthUnit() );
   DDX_UnitValueAndTag(pDX,IDC_X6,IDC_X6_UNIT,pParent->m_PierData.m_PierData.GetX6(),pDisplayUnits->GetSpanLengthUnit() );

   DDV_ColumnGrid(pDX,m_ColumnLayoutGrid);
   DDX_ColumnGrid(pDX,m_ColumnLayoutGrid,pParent->m_PierData.m_PierData);

   if ( pDX->m_bSaveAndValidate )
   {
      CColumnData::ColumnHeightMeasurementType measure;
      DDX_CBItemData(pDX,IDC_HEIGHT_MEASURE,measure);
      ColumnIndexType nColumns = pParent->m_PierData.m_PierData.GetColumnCount();
      for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
      {
         pParent->m_PierData.m_PierData.GetColumnData(colIdx).SetColumnHeightMeasurementType(measure);
      }
   }
   else
   {
      CColumnData::ColumnHeightMeasurementType measure = pParent->m_PierData.m_PierData.GetColumnData(0).GetColumnHeightMeasurementType();
      DDX_CBItemData(pDX,IDC_HEIGHT_MEASURE,measure);
   }

   DDX_CBEnum(pDX, IDC_CONDITION_FACTOR_TYPE, pParent->m_PierData.m_PierData.GetConditionFactorType());
   DDX_Text(pDX,   IDC_CONDITION_FACTOR,      pParent->m_PierData.m_PierData.GetConditionFactor());

   if ( pDX->m_bSaveAndValidate )
   {
#pragma Reminder("WOKRING HERE - need to validate overall pier geometry")
      // maybe do this on the parent property page...
      // XBeam needs to be long enough to support all girders
      // XBeam needs to be as wide as columns(? not necessarily)
   }
}

BEGIN_MESSAGE_MAP(CPierLayoutPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPierLayoutPage)
	ON_COMMAND(ID_HELP, OnHelp)
   ON_CBN_SELCHANGE(IDC_CONDITION_FACTOR_TYPE, OnConditionFactorTypeChanged)
   ON_CBN_SELCHANGE(IDC_HEIGHT_MEASURE, OnHeightMeasureChanged)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_ADD_COLUMN, &CPierLayoutPage::OnAddColumn)
   ON_BN_CLICKED(IDC_REMOVE_COLUMN, &CPierLayoutPage::OnRemoveColumns)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPierLayoutPage message handlers

BOOL CPierLayoutPage::OnInitDialog() 
{
   m_ColumnLayoutGrid.SubclassDlgItem(IDC_COLUMN_GRID, this);
   m_ColumnLayoutGrid.CustomInit();

   CPierDlg* pParent = (CPierDlg*)GetParent();
   FillRefColumnComboBox(pParent->m_PierData.m_PierData.GetColumnCount());
   FillTransverseLocationComboBox();
   FillHeightMeasureComboBox();

   CComboBox* pcbConditionFactor = (CComboBox*)GetDlgItem(IDC_CONDITION_FACTOR_TYPE);
   pcbConditionFactor->AddString(_T("Good or Satisfactory (Structure condition rating 6 or higher)"));
   pcbConditionFactor->AddString(_T("Fair (Structure condition rating of 5)"));
   pcbConditionFactor->AddString(_T("Poor (Structure condition rating 4 or lower)"));
   pcbConditionFactor->AddString(_T("Other"));
   pcbConditionFactor->SetCurSel(0);

   CPropertyPage::OnInitDialog();

   OnConditionFactorTypeChanged();
   OnHeightMeasureChanged();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPierLayoutPage::OnConditionFactorTypeChanged()
{
   CEdit* pEdit = (CEdit*)GetDlgItem(IDC_CONDITION_FACTOR);
   CComboBox* pcbConditionFactor = (CComboBox*)GetDlgItem(IDC_CONDITION_FACTOR_TYPE);

   int idx = pcbConditionFactor->GetCurSel();
   switch(idx)
   {
   case 0:
      pEdit->EnableWindow(FALSE);
      pEdit->SetWindowText(_T("1.00"));
      break;
   case 1:
      pEdit->EnableWindow(FALSE);
      pEdit->SetWindowText(_T("0.95"));
      break;
   case 2:
      pEdit->EnableWindow(FALSE);
      pEdit->SetWindowText(_T("0.85"));
      break;
   case 3:
      pEdit->EnableWindow(TRUE);
      break;
   }
}

void CPierLayoutPage::FillTransverseLocationComboBox()
{
   CComboBox* pcbMeasure = (CComboBox*)GetDlgItem(IDC_REFCOLUMN_MEASUREMENT);
   pcbMeasure->ResetContent();
   int idx = pcbMeasure->AddString(_T("from the Alignment"));
   pcbMeasure->SetItemData(idx,(DWORD_PTR)pgsTypes::omtAlignment);
   idx = pcbMeasure->AddString(_T("from the Bridgeline"));
   pcbMeasure->SetItemData(idx,(DWORD_PTR)pgsTypes::omtBridge);
}

void CPierLayoutPage::FillRefColumnComboBox(ColumnIndexType nColumns)
{
   CComboBox* pcbRefColumn = (CComboBox*)GetDlgItem(IDC_REFCOLUMN);
   int curSel = pcbRefColumn->GetCurSel();
   pcbRefColumn->ResetContent();
   if ( nColumns == INVALID_INDEX )
   {
      nColumns = (ColumnIndexType)m_ColumnLayoutGrid.GetRowCount();
   }

   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      CString strLabel;
      strLabel.Format(_T("Column %d"),LABEL_COLUMN(colIdx));
      pcbRefColumn->AddString(strLabel);
   }

   if ( pcbRefColumn->SetCurSel(curSel) == CB_ERR )
   {
      pcbRefColumn->SetCurSel(0);
   }
}

void CPierLayoutPage::FillHeightMeasureComboBox()
{
   CComboBox* pcbHeightMeasure = (CComboBox*)GetDlgItem(IDC_HEIGHT_MEASURE);
   pcbHeightMeasure->ResetContent();
   int idx = pcbHeightMeasure->AddString(_T("Height (H)"));
   pcbHeightMeasure->SetItemData(idx,(DWORD_PTR)CColumnData::chtHeight);
   idx = pcbHeightMeasure->AddString(_T("Bottom Elevation"));
   pcbHeightMeasure->SetItemData(idx,(DWORD_PTR)CColumnData::chtBottomElevation);
}

void CPierLayoutPage::OnHeightMeasureChanged()
{
   CComboBox* pcbHeightMeasure = (CComboBox*)GetDlgItem(IDC_HEIGHT_MEASURE);
   int curSel = pcbHeightMeasure->GetCurSel();
   CColumnData::ColumnHeightMeasurementType measure = (CColumnData::ColumnHeightMeasurementType)(pcbHeightMeasure->GetItemData(curSel));
   m_ColumnLayoutGrid.SetHeightMeasurementType(measure);
}

void CPierLayoutPage::OnHelp() 
{
   ::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_SUBSTRUCTURE );
}

void CPierLayoutPage::OnAddColumn()
{
   m_ColumnLayoutGrid.AddColumn();
   FillRefColumnComboBox();
}

void CPierLayoutPage::OnRemoveColumns()
{
   m_ColumnLayoutGrid.RemoveSelectedColumns();
   FillRefColumnComboBox();
}
