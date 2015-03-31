#include "stdafx.h"
#include <XBeamRateChildFrame.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateChildFrame

IMPLEMENT_DYNCREATE(CXBeamRateChildFrame,CEAFChildFrame)

CXBeamRateChildFrame::CXBeamRateChildFrame()
{
}

CXBeamRateChildFrame::~CXBeamRateChildFrame()
{
}
#ifdef _DEBUG
void CXBeamRateChildFrame::AssertValid() const
{
   // Make sure the module state is that for the CWinApp class
   // before diving down into MFC (if the module state isn't
   // correct, all sorts of asserts will fire)   
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   CEAFChildFrame::AssertValid();
}

void CXBeamRateChildFrame::Dump(CDumpContext& dc) const
{
   CEAFChildFrame::Dump(dc);
}
#endif


BEGIN_MESSAGE_MAP(CXBeamRateChildFrame, CEAFChildFrame)
	//{{AFX_MSG_MAP(CXBeamRateChildFrame)
	//}}AFX_MSG_MAP
   ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateChildFrame message handlers
//void CXBeamRateChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
//{
//	if (bAddToTitle && (GetStyle() & FWS_ADDTOTITLE) == 0)
//   {
//      // By turning off the default MFC-defined FWS_ADDTOTITLE style,
//      // the framework will use first string in the document template
//      // STRINGTABLE resource instead of the document name. We want
//      // to append a view count to the end of the window title.  
//		TCHAR szText[256+_MAX_PATH];
//      CString window_text;
//      CString window_title;
//      GetWindowText(window_text);
//
//      // Look for the last :
//      // The text to the left of the last : is the window title
//      int idx = window_text.ReverseFind(':');
//      if (idx != -1) // -1 meams : was not found
//         window_title = window_text.Left(idx);
//      else
//         window_title = window_text;
//
//		lstrcpy(szText,window_title);
////		if (m_nWindow > 0)
////			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
//
//		// set title if changed, but don't remove completely
//		AfxSetWindowText(m_hWnd, szText);
//   }
//   else
//   {
//      CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);
//   }
//}

BOOL CXBeamRateChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
   // force this window to be maximized (not sure why WS_VISIBLE is required)
   cs.style |= WS_MAXIMIZE | WS_VISIBLE;
	
	return CEAFChildFrame::PreCreateWindow(cs);
}
