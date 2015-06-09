#include "stdafx.h"
#include "resource.h"
#include <XBeamRateChildFrame.h>

#include <IFace\XBeamRateAgent.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <IFace\Selection.h>

#include <PgsExt\GirderLabel.h>
#include <PgsExt\BridgeDescription2.h>

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
   ON_CBN_SELCHANGE(IDC_PIERS,OnPierChanged)
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

PierIDType CXBeamRateChildFrame::GetPierID()
{
   if ( m_ControlBar.GetSafeHwnd() == 0 )
   {
      return INVALID_ID;
   }

   CComboBox* pcb = (CComboBox*)m_ControlBar.GetDlgItem(IDC_PIERS);
   int curSel = pcb->GetCurSel();
   if ( curSel == CB_ERR )
   {
      return INVALID_ID;
   }

   return (PierIDType)(pcb->GetItemData(curSel));
}

PierIndexType CXBeamRateChildFrame::GetPierIndex()
{
   PierIDType pierID = GetPierID();
   if ( pierID == INVALID_ID )
   {
      return INVALID_INDEX;
   }

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
   const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
   const CPierData2* pPier = pBridgeDesc->FindPier(pierID);
   ATLASSERT(pPier);
   return pPier->GetIndex();
}

BOOL CXBeamRateChildFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext)
{
   if ( !CEAFChildFrame::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext) )
      return FALSE;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   if ( pBroker != NULL )
   {
      // we only get a broker at this point if we a stand alone app

      // we are a PGSuper/PGSplice plugin... add the control bar
      if ( !m_ControlBar.Create(this,IDD_PIER_VIEW_CONTROLS,CBRS_TOP,100) )
      {
         return FALSE;
      }

      GET_IFACE2(pBroker,ISelection,pSelection);
      PierIndexType selPierIdx = pSelection->GetSelectedPier();

      CComboBox* pcbPiers = (CComboBox*)m_ControlBar.GetDlgItem(IDC_PIERS);
      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
      PierIndexType nPiers = pBridgeDesc->GetPierCount();
      for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
      {
         CString strPierLabel;
         strPierLabel.Format(_T("Pier %d"),LABEL_PIER(pierIdx));
         int idx = pcbPiers->AddString(strPierLabel);

         PierIDType pierID = pBridgeDesc->GetPier(pierIdx)->GetID();
         pcbPiers->SetItemData(idx,(DWORD_PTR)pierID);

         if ( pierIdx == selPierIdx )
         {
            pcbPiers->SetCurSel(idx);
         }
      }

      if ( selPierIdx == INVALID_INDEX || selPierIdx == 0 || selPierIdx == nPiers-1 )
      {
         pcbPiers->SetCurSel(0);
      }
     

   }

   return TRUE;
}

BOOL CXBeamRateChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
   // CEAFChildFrame::LoadFrame is going to load resources based on nIDResource
   // We have to set the module context to this module so the load happens
   // correctly
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !CEAFChildFrame::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext) )
      return FALSE;

   return TRUE;
}

BOOL CXBeamRateChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
   // force this window to be maximized (not sure why WS_VISIBLE is required)
   cs.style |= WS_MAXIMIZE | WS_VISIBLE;
	
	return CEAFChildFrame::PreCreateWindow(cs);
}

void CXBeamRateChildFrame::OnPierChanged()
{
   CComboBox* pcbPiers = (CComboBox*)m_ControlBar.GetDlgItem(IDC_PIERS);
   int curSel = pcbPiers->GetCurSel();
   PierIndexType pierIdx = (PierIndexType)pcbPiers->GetItemData(curSel);
   CView* pView = GetActiveView();
   if ( pView )
   {
      pView->OnInitialUpdate();
   }
}
