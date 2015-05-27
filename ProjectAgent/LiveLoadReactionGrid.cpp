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

// LiveLoadReactionGrid.cpp : implementation file
//

#include "stdafx.h"
#include "LiveLoadReactionGrid.h"
#include <EAF\EAFDisplayUnits.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GRID_IMPLEMENT_REGISTER(CLiveLoadReactionGrid, CS_DBLCLKS, 0, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CLiveLoadReactionGrid

CLiveLoadReactionGrid::CLiveLoadReactionGrid()
{
//   RegisterClass();
}

CLiveLoadReactionGrid::~CLiveLoadReactionGrid()
{
}

BEGIN_MESSAGE_MAP(CLiveLoadReactionGrid, CGXGridWnd)
	//{{AFX_MSG_MAP(CLiveLoadReactionGrid)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveLoadReactionGrid message handlers

void CLiveLoadReactionGrid::CustomInit()
{
// Initialize the grid. For CWnd based grids this call is // 
// essential. For view based grids this initialization is done 
// in OnInitialUpdate.
	Initialize( );

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   const int num_rows = 0;
   const int num_cols = 2;

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
   ROWCOL col = 1;
	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
         .SetValue(_T("Name"))
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetEnabled(FALSE)          // disables usage as current cell
		);

   CString cv;
   cv.Format(_T("LL+IM (%s)"),pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure.UnitTag().c_str());
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

void CLiveLoadReactionGrid::AddVehicle()
{
   ROWCOL nRows = GetRowCount();

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   InsertRows(nRows+1,1);
   SetRowStyle(GetRowCount());

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CLiveLoadReactionGrid::RemoveSelectedVehicles()
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

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CLiveLoadReactionGrid::GetLiveLoadData(txnLiveLoadData& llimData)
{
   llimData.m_LLIM.clear();
   ROWCOL nRows = GetRowCount();
   for ( ROWCOL row = 1; row <= nRows; row++ )
   {
      std::_tstring strName;
      Float64 llim;
      GetLiveLoadData(row,strName,llim);
      llimData.m_LLIM.push_back(std::make_pair(strName,llim));
   }
}

void CLiveLoadReactionGrid::SetLiveLoadData(const txnLiveLoadData& llimData)
{
   GetParam()->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   ROWCOL nRows = GetRowCount();
   if ( 0 < nRows )
   {
      RemoveRows(1,nRows);
   }

   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iter(llimData.m_LLIM.begin());
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator end(llimData.m_LLIM.end());
   for ( ; iter != end; iter++ )
   {
      AddLiveLoadData(iter->first,iter->second);
   }

   ResizeColWidthsToFit(CGXRange(0,0,GetRowCount(),GetColCount()));

   GetParam()->SetLockReadOnly(TRUE);
   GetParam()->EnableUndo(TRUE);
}

void CLiveLoadReactionGrid::SetRowStyle(ROWCOL row)
{
   ROWCOL col = 1;

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      );

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      );
}

void CLiveLoadReactionGrid::SetLiveLoadData(ROWCOL row,const std::_tstring& strName,Float64 llim)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetValue(strName.c_str())
      );

   Float64 value = ::ConvertFromSysUnits(llim,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetValue(value)
      );
}

void CLiveLoadReactionGrid::AddLiveLoadData(const std::_tstring& strName,Float64 llim)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();
   SetRowStyle(row);
   SetLiveLoadData(row,strName,llim);
}

void CLiveLoadReactionGrid::GetLiveLoadData(ROWCOL row,std::_tstring& strName,Float64& llim)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   strName = GetCellValue(row,1);
   llim = _tstof(GetCellValue(row,2));
   llim = ::ConvertToSysUnits(llim,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
}

CString CLiveLoadReactionGrid::GetCellValue(ROWCOL nRow, ROWCOL nCol)
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

BOOL CLiveLoadReactionGrid::OnEndEditing(ROWCOL nRow,ROWCOL nCol)
{
   ResizeColWidthsToFit(CGXRange(0,0,GetRowCount(),GetColCount()));
   return CGXGridWnd::OnEndEditing(nRow,nCol);
}
