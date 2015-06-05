// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <initguid.h>

#include <EAF\EAFDisplayUnits.h>
#include <IFace\XBeamRateAgent.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt
// See main.cpp for the implementation of this class
//

class CXBeamRateExt : public CWinApp
{
public:
	CXBeamRateExt();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateExt)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXBeamRateExt)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt

BEGIN_MESSAGE_MAP(CXBeamRateExt, CWinApp)
	//{{AFX_MSG_MAP(CXBeamRateExt)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt construction

CXBeamRateExt::CXBeamRateExt()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXBeamRateExt object

CXBeamRateExt theApp;

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateExt initialization

BOOL CXBeamRateExt::InitInstance()
{
   CWinApp::InitInstance();
   ::GXInit();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

int CXBeamRateExt::ExitInstance() 
{
   ::GXTerminate();
   return CWinApp::ExitInstance();
}
