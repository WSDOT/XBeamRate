///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2022  Washington State Department of Transportation
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

#include <XBeamRateExt\StirrupData.h>


/////////////////////////////////////////////////////////////////////////////
// CStirrupGrid window

class CStirrupGrid : public CGXGridWnd
{
	GRID_DECLARE_REGISTER()
// Construction
public:
	CStirrupGrid();
	virtual ~CStirrupGrid();

// Attributes
public:

// Operations
public:
   void SetSymmetry(bool isSymmetrical);

   void AddZone();
   void RemoveSelectedZones();

   bool GetStirrupData(xbrStirrupData& stirrups);
   void SetStirrupData(const xbrStirrupData& stirrups);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStirrupGrid)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CStirrupGrid)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // custom stuff for grid
   void CustomInit(LPCTSTR lpszGridName);

   BOOL OnLButtonHitRowCol(ROWCOL nHitRow,ROWCOL nHitCol,ROWCOL nDragRow,ROWCOL nDragCol,CPoint point,UINT flags,WORD nHitState);
   BOOL OnLButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);

private:
   void AddZoneData(const xbrStirrupData::StirrupZone& zoneData);
   void SetZoneData(ROWCOL row,const xbrStirrupData::StirrupZone& zoneData);
   bool GetZoneData(ROWCOL row,xbrStirrupData::StirrupZone& zoneData);
   void UpdateLastZoneLength();
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);

   // set up styles for interior rows
   WBFL::Materials::Rebar::Size GetBarSize(ROWCOL row,ROWCOL col);
   bool m_IsSymmetrical;
   CString m_strGridName;
   CString m_strBarSizeChoiceList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
