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

#pragma once

// LiveLoadReactionGrid.h : header file
//

#include "txnEditPier.h"


/////////////////////////////////////////////////////////////////////////////
// CLiveLoadReactionGrid window

class CLiveLoadReactionGrid : public CGXGridWnd
{
	GRID_DECLARE_REGISTER()
// Construction
public:
	CLiveLoadReactionGrid();
	virtual ~CLiveLoadReactionGrid();

// Attributes
public:

// Operations
public:
   void AddVehicle();
   void RemoveSelectedVehicles();

   void GetLiveLoadData(txnLiveLoadReactions& llimData);
   void SetLiveLoadData(const txnLiveLoadReactions& llimData);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveLoadReactionGrid)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
public:
	//{{AFX_MSG(CLiveLoadReactionGrid)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   // custom stuff for grid
   void CustomInit();

private:
   virtual BOOL OnEndEditing(ROWCOL nRow,ROWCOL nCol);

   void SetRowStyle(ROWCOL row);

   void AddLiveLoadData(const std::_tstring& strName,Float64 llim);
   void SetLiveLoadData(ROWCOL row,const std::_tstring& strName,Float64 llim);
   void GetLiveLoadData(ROWCOL row,std::_tstring& strName,Float64& llim);
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
