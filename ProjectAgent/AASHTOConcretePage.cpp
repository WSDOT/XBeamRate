
// AASHTOConcretePage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ConcreteDetailsDlg.h"
#include "AASHTOConcretePage.h"
#include <EAF\EAFApp.h>
//#include "HtmlHelp\HelpTopics.hh"
//#include <System\Tokenizer.h>
//#include "CopyConcreteEntry.h"
//#include <Lrfd\Lrfd.h>
//#include <EAF\EAFDisplayUnits.h>
//#include <IFace\Bridge.h>
//
//#include <PGSuperColors.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAASHTOConcretePage dialog


CAASHTOConcretePage::CAASHTOConcretePage() : CPropertyPage(CAASHTOConcretePage::IDD)
{
	//{{AFX_DATA_INIT(CAASHTOConcretePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAASHTOConcretePage::DoDataExchange(CDataExchange* pDX)
{
   CEAFApp* pApp = EAFGetApp();
   const unitmgtIndirectMeasure* pDisplayUnits = pApp->GetDisplayUnits();

   CPropertyPage::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CAASHTOConcretePage)
   //}}AFX_DATA_MAP
   DDX_Check_Bool(pDX, IDC_HAS_AGG_STRENGTH, m_bHasFct );
   DDX_UnitValueAndTag(pDX, IDC_AGG_STRENGTH, IDC_AGG_STRENGTH_T, m_Fct, pDisplayUnits->Stress );
   if ( m_bHasFct || !pDX->m_bSaveAndValidate )
   {
      if ( !pDX->m_bSaveAndValidate )
      {
         CWnd* pWnd = GetDlgItem(IDC_AGG_STRENGTH);
         pWnd->GetWindowText(m_strFct);
      }
   }

   DDX_Text(pDX, IDC_EC_K1, m_EccK1 );
   DDV_NonNegativeDouble(pDX,IDC_EC_K1,m_EccK1);

   DDX_Text(pDX, IDC_EC_K2, m_EccK2 );
   DDV_NonNegativeDouble(pDX,IDC_EC_K2,m_EccK2);

   DDX_Text(pDX, IDC_CREEP_K1, m_CreepK1 );
   DDV_NonNegativeDouble(pDX,IDC_CREEP_K1,m_CreepK1);

   DDX_Text(pDX, IDC_CREEP_K2, m_CreepK2 );
   DDV_NonNegativeDouble(pDX,IDC_CREEP_K2,m_CreepK2);

   DDX_Text(pDX, IDC_SHRINKAGE_K1, m_ShrinkageK1 );
   DDV_NonNegativeDouble(pDX,IDC_SHRINKAGE_K1,m_ShrinkageK1);

   DDX_Text(pDX, IDC_SHRINKAGE_K2, m_ShrinkageK2 );
   DDV_NonNegativeDouble(pDX,IDC_SHRINKAGE_K2,m_ShrinkageK2);
}


BEGIN_MESSAGE_MAP(CAASHTOConcretePage, CPropertyPage)
	//{{AFX_MSG_MAP(CAASHTOConcretePage)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_BN_CLICKED(IDC_HAS_AGG_STRENGTH,OnAggSplittingStrengthClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAASHTOConcretePage message handlers
LRESULT CAASHTOConcretePage::OnCommandHelp(WPARAM, LPARAM lParam)
{
#pragma Reminder("UPDATE: Update help file reference for this topic")
   //::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_CONCRETE_ENTRY_DIALOG );
   return TRUE;
}


BOOL CAASHTOConcretePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAASHTOConcretePage::OnSetActive()
{
   CPropertyPage::OnSetActive();

   CConcreteDetailsDlg* pParent = (CConcreteDetailsDlg*)GetParent();
   BOOL bEnable = (pParent->m_General.m_Type == pgsTypes::Normal ? FALSE : TRUE);
   GetDlgItem(IDC_HAS_AGG_STRENGTH)->EnableWindow(bEnable);
   GetDlgItem(IDC_AGG_STRENGTH)->EnableWindow(bEnable);
   GetDlgItem(IDC_AGG_STRENGTH_T)->EnableWindow(bEnable);

   if ( bEnable )
   {
      OnAggSplittingStrengthClicked();
   }

   return TRUE;
}

void CAASHTOConcretePage::OnAggSplittingStrengthClicked()
{
   CButton* pButton = (CButton*)GetDlgItem(IDC_HAS_AGG_STRENGTH);
   ASSERT(pButton);
   BOOL bIsChecked = pButton->GetCheck();
   GetDlgItem(IDC_AGG_STRENGTH)->EnableWindow(bIsChecked);
   GetDlgItem(IDC_AGG_STRENGTH_T)->EnableWindow(bIsChecked);
}
