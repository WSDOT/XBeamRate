///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
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

// LongitudinalRebarGrid.cpp : implementation file
//

#pragma Reminder("Update all grids of PGSuper with the techniques for getting rebar information used here")
// rebar information is "discovered" rather than hard coded

#include "stdafx.h"
#include "resource.h"
#include "LongitudinalRebarGrid.h"
#include <EAF\EAFDisplayUnits.h>
#include <LRFD\RebarPool.h>

#include "ReinforcementPage.h"
#include "PierDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GRID_IMPLEMENT_REGISTER(CLongitudinalRebarGrid, CS_DBLCLKS, 0, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CLongitudinalRebarGrid

CLongitudinalRebarGrid::CLongitudinalRebarGrid()
{
//   RegisterClass();
}

CLongitudinalRebarGrid::~CLongitudinalRebarGrid()
{
}

BEGIN_MESSAGE_MAP(CLongitudinalRebarGrid, CGXGridWnd)
	//{{AFX_MSG_MAP(CLongitudinalRebarGrid)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLongitudinalRebarGrid message handlers

void CLongitudinalRebarGrid::CustomInit()
{
// Initialize the grid. For CWnd based grids this call is // 
// essential. For view based grids this initialization is done 
// in OnInitialUpdate.
	Initialize( );

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   SetMergeCellsMode(gxnMergeDelayEval);

   const int num_rows = 0;
   const int num_cols = 5;

	SetRowCount(num_rows);
	SetColCount(num_cols);

		// Turn off selecting whole columns when clicking on a column header
	GetParam()->EnableSelection((WORD) (GX_SELFULL & ~GX_SELCOL & ~GX_SELTABLE & ~GX_SELMULTIPLE & ~GX_SELSHIFT & ~GX_SELKEYBOARD));

   // no row moving
	GetParam()->EnableMoveRows(FALSE);

   // disable left side
	SetStyleRange(CGXRange(0,0,num_rows,0), CGXStyle()
			.SetControl(GX_IDS_CTRL_HEADER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);


   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

// set text along top row
   int col = 0;
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
         .SetValue(_T("Row"))
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);

   SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
         .SetValue(_T("Beam\nFace"))
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);

   CString cv;
   cv.Format(_T("Cover\n(%s)"),pDisplayUnits->GetComponentDimUnit().UnitOfMeasure.UnitTag().c_str());
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(_T("Bar\nSize"))
		);

	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
         .SetValue(_T("# of\nBars"))
		);

   cv.Format(_T("Spacing\n(%s)"),pDisplayUnits->GetComponentDimUnit().UnitOfMeasure.UnitTag().c_str());
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   // make it so that text fits correctly in header row
	ResizeRowHeightsToFit(CGXRange(0,0,0,num_cols));

   // don't allow users to resize grids
   GetParam( )->EnableTrackColWidth(0); 
   GetParam( )->EnableTrackRowHeight(0); 

	EnableIntelliMouse();
	SetFocus();

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CLongitudinalRebarGrid::AddBar()
{
   ROWCOL nRows = GetRowCount();

   txnLongutindalRebarData rebarData;

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   AddRebarRow(rebarData);

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CLongitudinalRebarGrid::RemoveSelectedBars()
{
	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   CDWordArray selRows;
   ROWCOL nSelRows = GetSelectedRows(selRows);
   for ( int r = nSelRows-1; r >= 0; r-- )
   {
      ROWCOL selRow = selRows[r];
      RemoveRows(selRow,selRow);
   }

   ROWCOL nRows = GetRowCount();
   SetStyleRange(CGXRange(nRows,3), CGXStyle()
      .SetEnabled(FALSE)
      .SetReadOnly(TRUE)
      .SetInterior(::GetSysColor(COLOR_BTNFACE))
      .SetTextColor(::GetSysColor(COLOR_BTNFACE))
      .SetHorizontalAlignment(DT_RIGHT)
         );

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CLongitudinalRebarGrid::GetRebarData(std::vector<txnLongutindalRebarData>& vRebarData)
{
   vRebarData.clear();
   ROWCOL nRows = GetRowCount();
   for ( ROWCOL row = 1; row <= nRows; row++ )
   {
      txnLongutindalRebarData rebarData;
      GetRebarData(row,rebarData);
      vRebarData.push_back(rebarData);
   }
}

void CLongitudinalRebarGrid::SetRebarData(const std::vector<txnLongutindalRebarData>& vRebarData)
{
   GetParam()->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   ROWCOL nRows = GetRowCount();
   if ( 0 < nRows )
   {
      RemoveRows(1,nRows);
   }

   BOOST_FOREACH(const txnLongutindalRebarData& rebarData, vRebarData)
   {
      AddRebarRow(rebarData);
   }

   GetParam()->SetLockReadOnly(TRUE);
   GetParam()->EnableUndo(TRUE);
}

void CLongitudinalRebarGrid::SetRebarData(ROWCOL row,const txnLongutindalRebarData& rebarData)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   CReinforcementPage* pParent = (CReinforcementPage*)GetParent();
   CPierDlg* pDlg = (CPierDlg*)pParent->GetParent();
   const txnEditPierData& pierData = pDlg->GetPierData();

   CString strBeamFaceChoiceList;
   if ( pierData.m_PierType == xbrTypes::pctExpansion )
   {
      strBeamFaceChoiceList = _T("Top\nBottom\n");
   }
   else
   {
      strBeamFaceChoiceList = _T("Top\nTop Lower XBeam\nBottom\n");
   }

   // Datum
   CString strDatum = (rebarData.datum == xbrTypes::Top ? _T("Top") : (rebarData.datum == xbrTypes::TopLowerXBeam ? _T("Top Lower XBeam") : _T("Bottom")));
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetControl(GX_IDS_CTRL_CBS_DROPDOWNLIST)
      .SetChoiceList(strBeamFaceChoiceList)
      .SetValue(strDatum)
      );

   // Cover
   Float64 value = ::ConvertFromSysUnits(rebarData.cover,pDisplayUnits->GetComponentDimUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // Bar Size
   matRebar::Grade grade = matRebar::Grade60;
   matRebar::Type type = matRebar::A615;
   CString strBarSizeChoiceList;
   lrfdRebarIter rebarIter(grade,type);
   for ( rebarIter.Begin(); rebarIter; rebarIter.Next() )
   {
      const matRebar* pRebar = rebarIter.GetCurrentRebar();
      strBarSizeChoiceList += pRebar->GetName().c_str();
      strBarSizeChoiceList += _T("\n");
   }

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetControl(GX_IDS_CTRL_CBS_DROPDOWNLIST)
      .SetChoiceList(strBarSizeChoiceList)
      .SetValue(lrfdRebarPool::GetBarSize(rebarData.barSize).c_str())
      );

   // Bar Count
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(rebarData.nBars)
      );

   // Spacing
   value = ::ConvertFromSysUnits(rebarData.spacing,pDisplayUnits->GetComponentDimUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   ResizeColWidthsToFit(CGXRange(0,0,GetRowCount(),GetColCount()));
}

void CLongitudinalRebarGrid::AddRebarRow(const txnLongutindalRebarData& rebarData)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();

   SetRebarData(row,rebarData);
}

void CLongitudinalRebarGrid::GetRebarData(ROWCOL row,txnLongutindalRebarData& rebarData)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   // Beam Face
   rebarData.datum = GetDatum(row,col++);

   // Cover
   Float64 cover = _tstof(GetCellValue(row,col++));
   rebarData.cover = ::ConvertToSysUnits(cover,pDisplayUnits->GetComponentDimUnit().UnitOfMeasure);

   // Bar Size
   rebarData.barSize = GetBarSize(row,col++);

   // # Bars
   Int16 nBars = _tstoi(GetCellValue(row,col++));
   rebarData.nBars = nBars;

   // Spacing;
   Float64 spacing = _tstof(GetCellValue(row,col++));
   rebarData.spacing = ::ConvertToSysUnits(spacing,pDisplayUnits->GetComponentDimUnit().UnitOfMeasure);
}

CString CLongitudinalRebarGrid::GetCellValue(ROWCOL nRow, ROWCOL nCol)
{
    if (IsCurrentCell(nRow, nCol) && IsActiveCurrentCell())
    {
        CString s;
        CGXControl* pControl = GetControl(nRow, nCol);
        pControl->GetValue(s);
        return s;
    }
    else
    {
        return GetValueRowCol(nRow, nCol);
    }
}

xbrTypes::LongitudinalRebarDatumType CLongitudinalRebarGrid::GetDatum(ROWCOL row,ROWCOL col)
{
   CString strDatum = GetCellValue(row,col);
   if ( strDatum == _T("Top") )
   {
      CReinforcementPage* pParent = (CReinforcementPage*)GetParent();
      CPierDlg* pDlg = (CPierDlg*)pParent->GetParent();
      const txnEditPierData& pierData = pDlg->GetPierData();
      if ( pierData.m_PierType == xbrTypes::pctExpansion )
      {
         return xbrTypes::TopLowerXBeam;
      }
      else
      {
         return xbrTypes::Top;
      }
   }
   else if ( strDatum == _T("Bottom") )
   {
      return xbrTypes::Bottom;
   }
   else
   {
      return xbrTypes::TopLowerXBeam;
   }
}

matRebar::Size CLongitudinalRebarGrid::GetBarSize(ROWCOL row,ROWCOL col)
{
   std::_tstring strBarSize = GetCellValue(row,col);
   matRebar::Grade grade = matRebar::Grade60;
   matRebar::Type type = matRebar::A615;
   lrfdRebarIter rebarIter(grade,type);
   for ( rebarIter.Begin(); rebarIter; rebarIter.Next() )
   {
      if ( rebarIter.GetCurrentRebar()->GetName() == strBarSize )
      {
         return rebarIter.GetCurrentRebar()->GetSize();
      }
   }

   ATLASSERT(false); // should never get here
   return matRebar::bs3;
}
