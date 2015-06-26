
#pragma once

// ColumnLayoutGrid.h : header file
//

#include "txnEditPier.h"


/////////////////////////////////////////////////////////////////////////////
// CColumnLayoutGrid window

class CColumnLayoutGrid : public CGXGridWnd
{
	GRID_DECLARE_REGISTER()
// Construction
public:
	CColumnLayoutGrid();
	virtual ~CColumnLayoutGrid();

// Attributes
public:

// Operations
public:
   void AddColumn();
   void RemoveSelectedColumns();

   void GetColumnData(xbrPierData& pier);
   void SetColumnData(const xbrPierData& pier);

   void SetHeightMeasurementType(CColumnData::ColumnHeightMeasurementType measure);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColumnLayoutGrid)
	//}}AFX_VIRTUAL

// Implementation
public:
   void CustomInit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColumnLayoutGrid)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   
   virtual void OnClickedButtonRowCol(ROWCOL nRow,ROWCOL nCol);
   virtual void OnModifyCell(ROWCOL nRow,ROWCOL nCol);

private:
   void AddColumn(const CColumnData& column,Float64 S);
   void SetColumnData(ROWCOL row,const CColumnData& column,Float64 S);
   void GetColumnData(ROWCOL row,CColumnData* pColumn,Float64* pS);
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
