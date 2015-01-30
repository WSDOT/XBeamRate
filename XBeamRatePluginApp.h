#ifndef INCLUDED_APP_H_
#define INCLUDED_APP_H_

#include <EAF\EAFDocTemplate.h>
class CXBeamRatePluginApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRatePluginApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
 	 virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXBeamRatePluginApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   HMENU m_hSharedMenu;
};

#endif // INCLUDED_APP_H_