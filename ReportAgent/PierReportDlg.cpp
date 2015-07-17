
// SpanGirderReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PierReportDlg.h"

#include <IFace\Tools.h>
#include <IFace\Bridge.h>
#include <IFace\DocumentType.h>

#include <PgsExt\GirderLabel.h>
#include <MFCTools\CustomDDX.h>

#include <XBeamRateExt\XBeamRateUtilities.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <PgsExt\BridgeDescription2.h>

#include "XBeamRateReportSpecification.h"

//#include "HtmlHelp\HelpTopics.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// PierReportDlg dialog

IMPLEMENT_DYNAMIC(CPierReportDlg, CDialog)

CPierReportDlg::CPierReportDlg(IBroker* pBroker,const CReportDescription& rptDesc,boost::shared_ptr<CReportSpecification>& pRptSpec,CWnd* pParent)
: CDialog(CPierReportDlg::IDD, pParent), m_RptDesc(rptDesc), m_pInitRptSpec(pRptSpec)
{
   m_PierID = INVALID_ID;

   m_pBroker = pBroker;
}

CPierReportDlg::~CPierReportDlg()
{
}

void CPierReportDlg::DoDataExchange(CDataExchange* pDX)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ChList);

   DDX_CBItemData(pDX, IDC_PIER, m_PierID);

   if ( pDX->m_bSaveAndValidate )
   {
      m_ChapterList.clear();

      int cSelChapters = 0;
      int cChapters = m_ChList.GetCount();
      for ( int ch = 0; ch < cChapters; ch++ )
      {
         if ( m_ChList.GetCheck( ch ) == 1 )
         {
            cSelChapters++;

            CString strChapter;
            m_ChList.GetText(ch,strChapter);

            m_ChapterList.push_back(std::_tstring(strChapter));
         }
      }

      if ( cSelChapters == 0 )
      {
         pDX->PrepareCtrl(IDC_LIST);
         AfxMessageBox(IDS_E_NOCHAPTERS);
         pDX->Fail();
      }
   }
}


BEGIN_MESSAGE_MAP(CPierReportDlg, CDialog)
	ON_COMMAND(IDHELP, OnHelp)
END_MESSAGE_MAP()


// CPierReportDlg message handlers
void CPierReportDlg::UpdateChapterList()
{
   // Clear out the list box
   m_ChList.ResetContent();

   // Get the chapters in the report
   std::vector<CChapterInfo> chInfos = m_RptDesc.GetChapterInfo();

   // Populate the list box with the names of the chapters
   std::vector<CChapterInfo>::iterator iter;
   for ( iter = chInfos.begin(); iter != chInfos.end(); iter++ )
   {
      CChapterInfo chInfo = *iter;

      int idx = m_ChList.AddString( chInfo.Name.c_str() );
      if ( idx != LB_ERR ) // no error
      {
         m_ChList.SetCheck( idx, chInfo.Select ? 1 : 0 ); // turn the check on
         m_ChList.SetItemData( idx, chInfo.MaxLevel );
      }
   }

   // don't select any items in the chapter list
   m_ChList.SetCurSel(-1);
}

BOOL CPierReportDlg::OnInitDialog()
{
   CWnd* pwndTitle = GetDlgItem(IDC_REPORT_TITLE);
   pwndTitle->SetWindowText(m_RptDesc.GetReportName());

   if ( IsStandAlone())
   {
      CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_PIER);
      int idx = pCB->AddString(_T("INVALID_ID"));
      pCB->SetItemData(idx,(DWORD_PTR)INVALID_ID);
      m_PierID = INVALID_ID;

      // Hide the Pier combo box
      pCB->ShowWindow(SW_HIDE);
      
      // Hide the group box around the pier selector
      CWnd* pGroupBox = GetDlgItem(IDC_GROUP);
      pGroupBox->ShowWindow(SW_HIDE);

      // Move the list lable to where the group box was
      CRect rGroupBox;
      pGroupBox->GetWindowRect(&rGroupBox);
      LONG grpTop = rGroupBox.top;
      ScreenToClient(rGroupBox);

      CWnd* pChListLabel = GetDlgItem(IDC_LABEL);
      CRect rLabel;
      pChListLabel->GetWindowRect(&rLabel);
      pChListLabel->SetWindowPos(NULL,rGroupBox.left,rGroupBox.top,0,0,SWP_NOSIZE | SWP_NOZORDER);

      // Make the chapter list taller
      CWnd* pChList = GetDlgItem(IDC_LIST);
      CRect rList;
      pChList->GetWindowRect(&rList);
      LONG clearance = rList.top - rLabel.bottom; // clearance between ch list label and the ch list
      rList.top = grpTop + rLabel.Height() + clearance;

      ScreenToClient(rList);
      pChList->SetWindowPos(NULL,rList.left,rList.top,rList.Width(),rList.Height(),SWP_NOZORDER);

   }
   else
   {
      CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_PIER);

      GET_IFACE(IBridgeDescription,pIBridgeDesc);
      const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
      PierIndexType nPiers = pBridgeDesc->GetPierCount();
      for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
      {
         CString strPierLabel;
         strPierLabel.Format(_T("Pier %d"),LABEL_PIER(pierIdx));
         int idx = pCB->AddString(strPierLabel);

         PierIDType pierID = pBridgeDesc->GetPier(pierIdx)->GetID();
         pCB->SetItemData(idx,(DWORD_PTR)pierID);
      }

      m_PierID = pBridgeDesc->GetPier(1)->GetID();
   }

   CDialog::OnInitDialog();

   if ( m_pInitRptSpec )
   {
      InitFromRptSpec();
   }

   UpdateChapterList();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPierReportDlg::OnHelp() 
{
   //AFX_MANAGE_STATE(AfxGetAppModuleState());
   //::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_DIALOG_REPORT );
#pragma Reminder("UPDATE: Add Help Topic")
}

void CPierReportDlg::ClearChapterCheckMarks()
{
   int cChapters = m_ChList.GetCount();
   for ( int ch = 0; ch < cChapters; ch++ )
   {
      m_ChList.SetCheck(ch,0);
   }
}

void CPierReportDlg::InitFromRptSpec()
{
   boost::shared_ptr<CXBeamRateReportSpecification> pRptSpec = boost::dynamic_pointer_cast<CXBeamRateReportSpecification>(m_pInitRptSpec);
   m_PierID = pRptSpec->GetPierID();
   InitChapterListFromSpec();

   UpdateData(FALSE);
}

void CPierReportDlg::InitChapterListFromSpec()
{
   ClearChapterCheckMarks();
   std::vector<CChapterInfo> chInfo = m_pInitRptSpec->GetChapterInfo();
   std::vector<CChapterInfo>::iterator iter;
   for ( iter = chInfo.begin(); iter != chInfo.end(); iter++ )
   {
      CChapterInfo& ch = *iter;
      int cChapters = m_ChList.GetCount();
      for ( int idx = 0; idx < cChapters; idx++ )
      {
         CString strChapter;
         m_ChList.GetText(idx,strChapter);
         if ( strChapter == ch.Name.c_str() )
         {
            m_ChList.SetCheck(idx,1);
            break;
         }
      }
   }
}
