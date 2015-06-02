
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

   void GetStirrupData(xbrStirrupData& stirrups);
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
   void CustomInit();

private:
   void AddZoneData(const xbrStirrupData::StirrupZone& zoneData);
   void SetZoneData(ROWCOL row,const xbrStirrupData::StirrupZone& zoneData);
   void GetZoneData(ROWCOL row,xbrStirrupData::StirrupZone& zoneData);
   CString GetCellValue(ROWCOL nRow, ROWCOL nCol);

   // set up styles for interior rows
   matRebar::Size GetBarSize(ROWCOL row,ROWCOL col);
   bool m_IsSymmetrical;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
