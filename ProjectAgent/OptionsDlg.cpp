// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

   DDX_CBEnum(pDX,IDC_PERMIT_FACTORS,m_PermitRatingMethod);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
END_MESSAGE_MAP()


// COptionsDlg message handlers


BOOL COptionsDlg::OnInitDialog()
{
   FillPermitFactorList();

   CDialog::OnInitDialog();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::FillPermitFactorList()
{
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_PERMIT_FACTORS);
   pCB->ResetContent();

   int idx = pCB->AddString(_T("AASHTO MBE Equation 6A.4.2.1-1"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmAASHTO);

   idx = pCB->AddString(_T("WSDOT BDM Equation 13.1.1A-2"));
   pCB->SetItemData(idx,(DWORD_PTR)xbrTypes::prmWSDOT);
}
