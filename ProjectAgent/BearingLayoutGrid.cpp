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

// BridgeDescFramingGrid2.cpp : implementation file
//

// NOTE
// In this grid, when we talk about segment lengths, we don't mean the length of segments of the girder.
// Segment lengths in this context is the distance between segment supports measured along the alignment
// which is based on stationing.

#include "stdafx.h"
#include "BearingLayoutGrid.h"
#include <EAF\EAFDisplayUnits.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GRID_IMPLEMENT_REGISTER(CBearingLayoutGrid, CS_DBLCLKS, 0, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CBridgeDescFramingGrid

CBearingLayoutGrid::CBearingLayoutGrid()
{
//   RegisterClass();
}

CBearingLayoutGrid::~CBearingLayoutGrid()
{
}

BEGIN_MESSAGE_MAP(CBearingLayoutGrid, CGXGridWnd)
	//{{AFX_MSG_MAP(CBearingLayoutGrid)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBearingLayoutGrid message handlers

void CBearingLayoutGrid::CustomInit()
{
// Initialize the grid. For CWnd based grids this call is // 
// essential. For view based grids this initialization is done 
// in OnInitialUpdate.
	Initialize( );

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   SetMergeCellsMode(gxnMergeDelayEval);

   const int num_rows = 0;
   const int num_cols = 3;

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
         .SetValue(_T("Brg"))
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);

   CString cv;
   cv.Format(_T("DC (%s)"),pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure.UnitTag().c_str());
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   cv.Format(_T("DW (%s)"),pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure.UnitTag().c_str());
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   cv.Format(_T("S (%s)"),pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure.UnitTag().c_str());
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

void CBearingLayoutGrid::AddBearing()
{
   ROWCOL nRows = GetRowCount();

   txnBearingData brgData;
   GetBearingData(nRows,brgData); // bearing data in the last row

   Float64 S = ::ConvertToSysUnits(6.0,unitMeasure::Feet); // default spacing
   if ( 1 < nRows )
   {
      // get the spacing between the last two bearings
      txnBearingData brgData1;
      GetBearingData(nRows-1,brgData1);
      S = brgData1.m_S;
   }

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   // set the default spacing in the last row
   brgData.m_S = S;
   SetBearingData(nRows,brgData);

   // add the new bearing (adds row to the grid)
   brgData.m_S = 0; // no spacing for the last bearing
   AddBearingRow(brgData);

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CBearingLayoutGrid::RemoveSelectedBearings()
{
   AfxMessageBox(_T("Remove bearings"));
}

void CBearingLayoutGrid::GetBearingData(std::vector<txnBearingData>& vBrgData)
{
   vBrgData.clear();
   ROWCOL nRows = GetRowCount();
   for ( ROWCOL row = 1; row <= nRows; row++ )
   {
      txnBearingData brgData;
      GetBearingData(row,brgData);
      vBrgData.push_back(brgData);
   }
}

void CBearingLayoutGrid::SetBearingData(const std::vector<txnBearingData>& vBrgData)
{
   GetParam()->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   ROWCOL nRows = GetRowCount();
   if ( 0 < nRows )
   {
      RemoveRows(1,nRows);
   }

   if ( vBrgData.size() == 0 )
   {
      // Always have one bearing
      txnBearingData brgData;
      AddBearingRow(brgData);
   }
   else
   {
      BOOST_FOREACH(const txnBearingData& brgData, vBrgData)
      {
         AddBearingRow(brgData);
      }
   }

   GetParam()->SetLockReadOnly(TRUE);
   GetParam()->EnableUndo(TRUE);
}

void CBearingLayoutGrid::SetBearingData(ROWCOL row,const txnBearingData& brgData)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   // DC
   Float64 value = ::ConvertFromSysUnits(brgData.m_DC,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // DW
   value = ::ConvertFromSysUnits(brgData.m_DW,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // Spacing
   // Set the value for the spacing to the next bearing and disable this cell
   // This assumes this bearing is in the last row and spacing to next isn't applicable
   value = ::ConvertFromSysUnits(brgData.m_S,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col), CGXStyle()
      .SetEnabled(FALSE)
      .SetReadOnly(TRUE)
      .SetInterior(::GetSysColor(COLOR_BTNFACE))
      .SetTextColor(::GetSysColor(COLOR_BTNFACE))
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
         );

   if ( 1 < row )
   {
      // if this isn't the first row, enable the spacing in the previous row
      // because it obviously isn't the last row
      SetStyleRange(CGXRange(row-1,col),CGXStyle()
         .SetEnabled(TRUE)
         .SetReadOnly(FALSE)
         .SetInterior(::GetSysColor(COLOR_WINDOW))
        .SetTextColor(::GetSysColor(COLOR_WINDOWTEXT))
         );
   }

   col++;
}

void CBearingLayoutGrid::AddBearingRow(const txnBearingData& brgData)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();

   SetBearingData(row,brgData);
}

void CBearingLayoutGrid::GetBearingData(ROWCOL row,txnBearingData& brgData)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   Float64 DC = _tstof(GetCellValue(row,col++));
   brgData.m_DC = ::ConvertToSysUnits(DC,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);

   Float64 DW = _tstof(GetCellValue(row,col++));
   brgData.m_DW = ::ConvertToSysUnits(DW,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);

   Float64 S = _tstof(GetCellValue(row,col++));
   brgData.m_S = ::ConvertToSysUnits(S,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
}

CString CBearingLayoutGrid::GetCellValue(ROWCOL nRow, ROWCOL nCol)
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
