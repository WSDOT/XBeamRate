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
   const int num_cols = 4;

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

// set text along top row
   int col = 0;
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
         .SetValue(_T("Brg"))
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);

   CString cv = _T("Spacing");
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   cv = _T("DC");
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   cv = _T("DW");
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(cv)
		);

   cv = _T("LL+IM");
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

void CBearingLayoutGrid::GetBearingData(std::vector<txnBearingLineData>& vBrgData)
{
   vBrgData.clear();
   ROWCOL nRows = GetRowCount();
   if ( nRows == 1 )
   {
      txnBearingLineData brgData;
      brgData.m_S = 0;
      GetBearingData(nRows,brgData);
      vBrgData.push_back(brgData);
   }
   else
   {
      for ( ROWCOL row = 0; row < nRows; row += 2 )
      {
         txnBearingLineData brgData;
         GetBearingData(row,brgData);
         GetSpacingData(row+1,brgData);
         vBrgData.push_back(brgData);
      }
   }
}

void CBearingLayoutGrid::SetBearingData(const std::vector<txnBearingLineData>& vBrgData)
{
   GetParam()->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   std::vector<txnBearingLineData>::const_iterator iter(vBrgData.begin());
   std::vector<txnBearingLineData>::const_iterator end(vBrgData.end());
   for ( ; iter != end; iter++ )
   {
      const txnBearingLineData& brgData(*iter);
      AddBearingRow(brgData);
      if ( iter != end-1 )
      {
         AddSpacingRow(brgData);
      }
   }

   GetParam()->SetLockReadOnly(TRUE);
   GetParam()->EnableUndo(TRUE);
}

void CBearingLayoutGrid::AddBearingRow(const txnBearingLineData& brgData)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   // Spacing
   SetStyleRange(CGXRange(row,1), CGXStyle()
            .SetEnabled(FALSE)
            .SetReadOnly(TRUE)
            .SetInterior( ::GetSysColor(COLOR_BTNFACE) )
            .SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) )
         );

   // DC
   Float64 value = ::ConvertFromSysUnits(brgData.m_DC,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,2), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // DW
   value = ::ConvertFromSysUnits(brgData.m_DW,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,3), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // LLIM
   value = ::ConvertFromSysUnits(brgData.m_LLIM,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,4), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );
}

void CBearingLayoutGrid::AddSpacingRow(const txnBearingLineData& brgData)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   // Spacing
   Float64 value = ::ConvertFromSysUnits(brgData.m_S,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,1), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
         );

   // DC
   SetStyleRange(CGXRange(row,2), CGXStyle()
            .SetEnabled(FALSE)
            .SetReadOnly(TRUE)
            .SetInterior( ::GetSysColor(COLOR_BTNFACE) )
            .SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) )
      );

   // DW
   SetStyleRange(CGXRange(row,3), CGXStyle()
            .SetEnabled(FALSE)
            .SetReadOnly(TRUE)
            .SetInterior( ::GetSysColor(COLOR_BTNFACE) )
            .SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) )
      );

   // LLIM
   SetStyleRange(CGXRange(row,4), CGXStyle()
            .SetEnabled(FALSE)
            .SetReadOnly(TRUE)
            .SetInterior( ::GetSysColor(COLOR_BTNFACE) )
            .SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) )
      );
}

void CBearingLayoutGrid::GetBearingData(ROWCOL row,txnBearingLineData& brgData)
{
}

void CBearingLayoutGrid::GetSpacingData(ROWCOL row,txnBearingLineData& brgData)
{
}
