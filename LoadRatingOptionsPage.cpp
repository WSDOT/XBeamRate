// LoadRatingOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LoadRatingOptionsPage.h"

#include <MFCTools\CustomDDX.h>

// CLoadRatingOptionsPage dialog

IMPLEMENT_DYNAMIC(CLoadRatingOptionsPage, CPropertyPage)

CLoadRatingOptionsPage::CLoadRatingOptionsPage()
	: CPropertyPage(CLoadRatingOptionsPage::IDD)
{

}

CLoadRatingOptionsPage::~CLoadRatingOptionsPage()
{
}

void CLoadRatingOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   DDX_CBEnum(pDX,IDC_ANALYSIS_MODE,m_AnalysisType);

   DDX_CBEnum(pDX,IDC_PERMIT_FACTORS,m_PermitRatingMethod);
}


BEGIN_MESSAGE_MAP(CLoadRatingOptionsPage, CPropertyPage)
END_MESSAGE_MAP()


// CLoadRatingOptionsPage message handlers

BOOL CLoadRatingOptionsPage::OnInitDialog()
{
   FillAnalysisModeComboBox();
   FillPermitRatingMethodComboBox();

   CPropertyPage::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadRatingOptionsPage::FillAnalysisModeComboBox()
{
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_ANALYSIS_MODE);
   pCB->ResetContent();
   int idx = pCB->AddString(_T("Simple"));
   pCB->SetItemData(idx,(DWORD_PTR)pgsTypes::Simple);

   idx = pCB->AddString(_T("Continuous"));
   pCB->SetItemData(idx,(DWORD_PTR)pgsTypes::Continuous);

   idx = pCB->AddString(_T("Envelope"));
   pCB->SetItemData(idx,(DWORD_PTR)pgsTypes::Envelope);
}

void CLoadRatingOptionsPage::FillPermitRatingMethodComboBox()
{
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_PERMIT_FACTORS);
   pCB->ResetContent();

   int idx = pCB->AddString(_T("AASHTO MBE Equation 6A.4.2.1-1"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmAASHTO);

   idx = pCB->AddString(_T("WSDOT BDM Equation 13.1.1A-2"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmWSDOT);
}
