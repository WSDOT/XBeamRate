

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
   m_ReactionLoadType = xbrTypes::rltConcentrated;
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

void CBearingLayoutGrid::CustomInit(xbrTypes::ReactionLoadType reactionLoadType)
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

   cv.Format(_T("W (%s)"),pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure.UnitTag().c_str());
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

   SetReactionLoadType(reactionLoadType);

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CBearingLayoutGrid::SetReactionLoadType(xbrTypes::ReactionLoadType reactionLoadType)
{
   m_ReactionLoadType = reactionLoadType;
   OnReactionLoadTypeChanged();
}

void CBearingLayoutGrid::AddBearing()
{
   ROWCOL nRows = GetRowCount();

   Float64 DC, DW, W, S;
   GetBearingData(nRows,&DC,&DW,&W,&S); // bearing data in the last row (we want DC and DW)

   Float64 Sdefault = ::ConvertToSysUnits(6.0,unitMeasure::Feet); // default spacing
   Float64 Wdefault = Sdefault;
   if ( 1 < nRows )
   {
      // get the spacing between the last two bearings
      Float64 DC1, DW1, W1, S1;
      GetBearingData(nRows-1,&DC1,&DW1,&W1,&S1);
      Wdefault = W1;
      Sdefault = S1;
   }

	GetParam( )->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   // set the default spacing in the last row
   SetBearingData(nRows,DC,DW,Wdefault,Sdefault);

   // add the new bearing (adds row to the grid)
   AddBearingRow(DC,DW,Wdefault,0);

   GetParam()->SetLockReadOnly(TRUE);
   GetParam( )->EnableUndo(TRUE);
}

void CBearingLayoutGrid::RemoveSelectedBearings()
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
   SetStyleRange(CGXRange(nRows,4), CGXStyle()
      .SetEnabled(FALSE)
      .SetReadOnly(TRUE)
      .SetInterior(::GetSysColor(COLOR_BTNFACE))
      .SetTextColor(::GetSysColor(COLOR_BTNFACE))
      .SetHorizontalAlignment(DT_RIGHT)
         );

   GetParam()->SetLockReadOnly(TRUE);
	GetParam( )->EnableUndo(TRUE);
}

void CBearingLayoutGrid::GetBearingData(xbrBearingLineData& brgLineData,std::vector<txnDeadLoadReaction>& deadLoadReactions)
{
   deadLoadReactions.clear();
   ROWCOL nRows = GetRowCount();
   brgLineData.SetBearingCount(nRows);
   for ( ROWCOL row = 1; row <= nRows; row++ )
   {
      Float64 DC, DW, W, S;
      GetBearingData(row,&DC,&DW,&W,&S);
      if ( row < nRows )
      {
         brgLineData.SetSpacing(row-1,S);
      }
      txnDeadLoadReaction reaction;
      reaction.m_DC = DC;
      reaction.m_DW = DW;
      reaction.m_W  = W;
      deadLoadReactions.push_back(reaction);
   }
}

void CBearingLayoutGrid::SetBearingData(xbrBearingLineData& brgLineData,std::vector<txnDeadLoadReaction>& deadLoadReactions)
{
   GetParam()->EnableUndo(FALSE);
   GetParam()->SetLockReadOnly(FALSE);

   ROWCOL nRows = GetRowCount();
   if ( 0 < nRows )
   {
      RemoveRows(1,nRows);
   }

   if ( brgLineData.GetBearingCount() == 0 )
   {
      // Always have one bearing
      AddBearingRow(0,0,0,0);
   }
   else
   {
      IndexType nBearings = brgLineData.GetBearingCount();
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 S = (brgIdx != nBearings-1 ? brgLineData.GetSpacing(brgIdx) : 0);
         Float64 DC = deadLoadReactions[brgIdx].m_DC;
         Float64 DW = deadLoadReactions[brgIdx].m_DW;
         Float64 W  = deadLoadReactions[brgIdx].m_W;
         AddBearingRow(DC,DW,W,S);
      }
   }

   GetParam()->SetLockReadOnly(TRUE);
   GetParam()->EnableUndo(TRUE);
}

IndexType CBearingLayoutGrid::GetBearingCount()
{
   return (IndexType)GetRowCount();
}

void CBearingLayoutGrid::SetBearingData(ROWCOL row,Float64 DC,Float64 DW,Float64 W,Float64 S)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   // DC
   Float64 value;
   if ( m_ReactionLoadType == xbrTypes::rltConcentrated )
   {
      value = ::ConvertFromSysUnits(DC,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   }
   else
   {
      value = ::ConvertFromSysUnits(DC,pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure);
   }

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // DW
   if ( m_ReactionLoadType == xbrTypes::rltConcentrated )
   {
      value = ::ConvertFromSysUnits(DW,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   }
   else
   {
      value = ::ConvertFromSysUnits(DW,pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure);
   }

   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // W
   value = ::ConvertFromSysUnits(W,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
   SetStyleRange(CGXRange(row,col++), CGXStyle()
      .SetEnabled(TRUE)
      .SetReadOnly(FALSE)
      .SetHorizontalAlignment(DT_RIGHT)
      .SetValue(value)
      );

   // Spacing
   // Set the value for the spacing to the next bearing and disable this cell
   // This assumes this bearing is in the last row and spacing to next isn't applicable
   value = ::ConvertFromSysUnits(S,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
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

void CBearingLayoutGrid::OnReactionLoadTypeChanged()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   CString strDC,strDW;
   if ( m_ReactionLoadType == xbrTypes::rltConcentrated )
   {
      strDC.Format(_T("DC (%s)"),pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure.UnitTag().c_str());
      strDW.Format(_T("DW (%s)"),pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure.UnitTag().c_str());
   }
   else
   {
      strDC.Format(_T("DC (%s)"),pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure.UnitTag().c_str());
      strDW.Format(_T("DW (%s)"),pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure.UnitTag().c_str());
   }

   ROWCOL col = 1;
   SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(strDC)
		);

	SetStyleRange(CGXRange(0,col++), CGXStyle()
         .SetWrapText(TRUE)
			.SetEnabled(FALSE)          // disables usage as current cell
         .SetHorizontalAlignment(DT_CENTER)
         .SetVerticalAlignment(DT_VCENTER)
			.SetValue(strDW)
		);

   // Hide/Show the "W" column
   BOOL bHide = (m_ReactionLoadType == xbrTypes::rltConcentrated ? TRUE : FALSE);
   HideCols(col,col++,bHide);
}

void CBearingLayoutGrid::AddBearingRow(Float64 DC,Float64 DW,Float64 W,Float64 S)
{
   InsertRows(GetRowCount()+1,1);
   ROWCOL row = GetRowCount();
   SetBearingData(row,DC,DW,W,S);
}

void CBearingLayoutGrid::GetBearingData(ROWCOL row,Float64* pDC,Float64* pDW,Float64* pW,Float64* pS)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   ROWCOL col = 1;

   Float64 DC = _tstof(GetCellValue(row,col++));
   if ( m_ReactionLoadType == xbrTypes::rltConcentrated )
   {
      *pDC = ::ConvertToSysUnits(DC,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   }
   else
   {
      *pDC = ::ConvertToSysUnits(DC,pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure);
   }

   Float64 DW = _tstof(GetCellValue(row,col++));
   if ( m_ReactionLoadType == xbrTypes::rltConcentrated )
   {
      *pDW = ::ConvertToSysUnits(DW,pDisplayUnits->GetGeneralForceUnit().UnitOfMeasure);
   }
   else
   {
      *pDW = ::ConvertToSysUnits(DW,pDisplayUnits->GetForcePerLengthUnit().UnitOfMeasure);
   }

   Float64 W = _tstof(GetCellValue(row,col++));
   if ( m_ReactionLoadType == xbrTypes::rltUniform )
   {
      *pW = ::ConvertToSysUnits(W,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
   }
   else
   {
      *pW = 0; // set W to zero for concentrated loads
   }

   Float64 S = _tstof(GetCellValue(row,col++));
   *pS = ::ConvertToSysUnits(S,pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure);
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
