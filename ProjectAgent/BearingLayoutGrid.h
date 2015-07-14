
#pragma once

// BearingLayoutGrid.h : header file
//

#include "txnEditPier.h"


/////////////////////////////////////////////////////////////////////////////
// CBearingLayoutGrid window

class CBearingLayoutGrid : public CGXGridWnd
{
	GRID_DECLARE_REGISTER()
// Construction
public:
	CBearingLayoutGrid();
	virtual ~CBearingLayoutGrid();

// Attributes
public:

// Operations
public:
   void SetReactionLoadType(xbrTypes::ReactionLoadType reactionLoadType);

   void AddBearing();
   void RemoveSelectedBearings();

   void GetBearingData(xbrBearingLineData& brgLineData,std::vector<txnDeadLoadReaction>& deadLoadReactions);
   void SetBearingData(xbrBearingLineData& brgLineData,std::vector<txnDeadLoadReaction>& deadLoadReactions);

   IndexType GetBearingCount();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBridgeDescFramingGrid)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
public:
	//{{AFX_MSG(CBridgeDescFramingGrid)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // custom stuff for grid
   void CustomInit(xbrTypes::ReactionLoadType reactionLoadType);

private:
   xbrTypes::ReactionLoadType m_ReactionLoadType;
   void OnReactionLoadTypeChanged();

   void AddBearingRow(Float64 DC,Float64 DW,Float64 W,Float64 S);
   void SetBearingData(ROWCOL row,Float64 DC,Float64 DW,Float64 W,Float64 S);
   void GetBearingData(ROWCOL row,Float64* pDC,Float64* pDW,Float64* pW,Float64* pS);
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
