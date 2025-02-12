///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2025  Washington State Department of Transportation
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

#pragma once

// LongitudinalRebarGrid.h : header file
//

#include <XBeamRateExt\LongitudinalRebarData.h>


/////////////////////////////////////////////////////////////////////////////
// CLongitudinalRebarGrid window

class CLongitudinalRebarGrid : public CGXGridWnd
{
	GRID_DECLARE_REGISTER()
// Construction
public:
	CLongitudinalRebarGrid();
	virtual ~CLongitudinalRebarGrid();

// Attributes
public:

// Operations
public:
   void AddBar();
   void RemoveSelectedBars();

   void GetRebarData(xbrLongitudinalRebarData& rebarData);
   void SetRebarData(const xbrLongitudinalRebarData& rebarData);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLongitudinalRebarGrid)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
public:
	//{{AFX_MSG(CLongitudinalRebarGrid)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // custom stuff for grid
   void CustomInit();

private:
   virtual void OnModifyCell(ROWCOL nRow,ROWCOL nCol) override;

   CString GetDatumOptions();
   CString GetDatum(xbrTypes::LongitudinalRebarDatumType datum);
   void AddRebarRow(const xbrLongitudinalRebarData::RebarRow& rebarData);
   bool SetRebarData(ROWCOL row,const xbrLongitudinalRebarData::RebarRow& rebarData);
   void GetRebarData(ROWCOL row,xbrLongitudinalRebarData::RebarRow& rebarData);
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);
   xbrTypes::LongitudinalRebarDatumType GetDatum(ROWCOL row,ROWCOL col);
   WBFL::Materials::Rebar::Size GetBarSize(ROWCOL row,ROWCOL col);
   LPCTSTR GetRebarLayoutType(xbrTypes::LongitudinalRebarLayoutType layoutType);
   xbrTypes::LongitudinalRebarLayoutType GetRebarLayoutType(LPCTSTR lpszLayoutType);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
