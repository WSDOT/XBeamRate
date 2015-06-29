#include "stdafx.h"
#include "resource.h"
#include <XBeamRateChildFrame.h>
#include <XBeamRateView.h>

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>
#include <IFace\Pier.h>
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
   m_CutLocation = 0;
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
void CXBeamRateChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if (bAddToTitle)
   {
      PierIndexType pierIdx = GetPierIndex();
      CString strTitle;
      if ( pierIdx == INVALID_INDEX )
      {
         strTitle = _T("Pier View");
      }
      else
      {
         strTitle.Format(_T("Pier View - Pier %d"),LABEL_PIER(pierIdx));
      }

      // set our title
      SetWindowText(strTitle);
   }
   else
   {
      CEAFChildFrame::OnUpdateFrameTitle(bAddToTitle);
   }
}

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

   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   HICON hIcon = AfxGetApp()->LoadIcon(IDI_PIERVIEW);
   SetIcon(hIcon,TRUE);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   if ( pBroker != NULL )
   {
      // we only get a broker at this point if we a PGSuper/PGSplice plugin

      // add the control bar
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

      OnPierChanged();
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

// iCutLocation
#pragma Reminder("WORKING HERE - implment iCutLocation")
Float64 CXBeamRateChildFrame::GetCurrentCutLocation()
{
   return m_CutLocation;
}

void CXBeamRateChildFrame::CutAt(Float64 Xgl)
{
   m_CutLocation = ::ForceIntoRange(m_Xmin,Xgl,m_Xmax);

   CXBeamRateView* pView = (CXBeamRateView*)GetActiveView();
   pView->OnUpdate(0,HINT_SECTION_CUT_MOVED,0);
}

void CXBeamRateChildFrame::CutAtNext()
{
   Float64 delta = (m_Xmax - m_Xmin)/10;
   CutAt(m_CutLocation + delta);
}

void CXBeamRateChildFrame::CutAtPrev()
{
   Float64 delta = (m_Xmax - m_Xmin)/10;
   CutAt(m_CutLocation - delta);
}

void CXBeamRateChildFrame::ShowCutDlg()
{
   AfxMessageBox(_T("CutDlg"));
}

Float64 CXBeamRateChildFrame::GetMinCutLocation()
{
   return m_Xmin;
}

Float64 CXBeamRateChildFrame::GetMaxCutLocation()
{
   return m_Xmax;
}

void CXBeamRateChildFrame::UpdateSectionCutExtents()
{
   PierIDType pierID = GetPierID();

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRProject,pProject);
   Float64 Lxb = pProject->GetXBeamLength(pierID);

   GET_IFACE2(pBroker,IXBRPier,pPier);
   m_Xmin = pPier->GetPierCoordinate(pierID,0);
   m_Xmax = pPier->GetPierCoordinate(pierID,Lxb);

   m_CutLocation = ::ForceIntoRange(m_Xmin,m_CutLocation,m_Xmax);
}

void CXBeamRateChildFrame::OnPierChanged()
{
   UpdateSectionCutExtents();
   m_CutLocation = 0.5*(m_Xmin + m_Xmax);

   CComboBox* pcbPiers = (CComboBox*)m_ControlBar.GetDlgItem(IDC_PIERS);
   int curSel = pcbPiers->GetCurSel();
   PierIDType pierID = (PierIDType)pcbPiers->GetItemData(curSel);

   CView* pView = GetActiveView();
   if ( pView )
   {
      pView->OnInitialUpdate();
   }

   OnUpdateFrameTitle(TRUE);
}
