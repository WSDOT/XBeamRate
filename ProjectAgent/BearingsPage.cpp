// BearingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BearingsPage.h"
#include "PierDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>

#pragma warning(disable:4244)

void DDX_BearingGrid(CDataExchange* pDX,CBearingLayoutGrid& grid,std::vector<txnBearingData>& brgData)
{
   if ( pDX->m_bSaveAndValidate )
   {
      grid.GetBearingData(brgData);
   }
   else
   {
      grid.SetBearingData(brgData);
   }
}


// CBearingsPage dialog

IMPLEMENT_DYNAMIC(CBearingsPage, CPropertyPage)

CBearingsPage::CBearingsPage()
	: CPropertyPage(CBearingsPage::IDD)
{

}

CBearingsPage::~CBearingsPage()
{
}

void CBearingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_CBItemData(pDX,IDC_BEARING_LINE_COUNT,pParent->m_PierData.m_nBearingLines);

   for ( IndexType brgLineIdx = 0; brgLineIdx < 2; brgLineIdx++ )
   {
      DDX_BearingGrid(pDX,m_Grid[brgLineIdx],pParent->m_PierData.m_BearingLines[brgLineIdx]);
      DDX_CBIndex(pDX,IDC_BACK_REF_BEARING_LIST+brgLineIdx,pParent->m_PierData.m_RefBearingIdx[brgLineIdx]);
      DDX_OffsetAndTag(pDX,IDC_BACK_REF_BEARING_LOCATION+brgLineIdx,IDC_BACK_REF_BEARING_LOCATION_UNIT+brgLineIdx,pParent->m_PierData.m_RefBearingLocation[brgLineIdx],pDisplayUnits->GetSpanLengthUnit());
      DDX_CBEnum(pDX,IDC_BACK_REF_BEARING_DATUM+brgLineIdx,pParent->m_PierData.m_RefBearingDatum[brgLineIdx]);
   }
}


BEGIN_MESSAGE_MAP(CBearingsPage, CPropertyPage)
	ON_COMMAND(ID_HELP, OnHelp)
   ON_COMMAND(IDC_ADD_BACK, OnAddBack)
   ON_COMMAND(IDC_REMOVE_BACK, OnRemoveBack)
   ON_COMMAND(IDC_ADD_AHEAD, OnAddAhead)
   ON_COMMAND(IDC_REMOVE_AHEAD, OnRemoveAhead)
   ON_COMMAND(IDC_COPY_AHEAD, OnCopyAhead)
   ON_COMMAND(IDC_COPY_BACK, OnCopyBack)
   ON_CBN_SELCHANGE(IDC_BEARING_LINE_COUNT, OnBearingLineCountChanged)
END_MESSAGE_MAP()


// CBearingsPage message handlers
BOOL CBearingsPage::OnInitDialog()
{
   m_Grid[0].SubclassDlgItem(IDC_BACK_BEARING_GRID, this);
   m_Grid[0].CustomInit();

   m_Grid[1].SubclassDlgItem(IDC_AHEAD_BEARING_GRID, this);
   m_Grid[1].CustomInit();

   CPierDlg* pParent = (CPierDlg*)GetParent();
   CComboBox* pcb = (CComboBox*)GetDlgItem(IDC_BEARING_LINE_COUNT);
   for ( IndexType brgLineIdx = 0; brgLineIdx < 2; brgLineIdx++ )
   {
      CString str;
      str.Format(_T("%d"),brgLineIdx+1);
      int idx = pcb->AddString(str);
      pcb->SetItemData(idx,(DWORD_PTR)(brgLineIdx+1));

      FillRefBearingComboBox(brgLineIdx);
      FillRefBearingDatumComboBox(brgLineIdx);
   }

   CPropertyPage::OnInitDialog();

   OnBearingLineCountChanged();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBearingsPage::OnHelp()
{
#pragma Reminder("UPDATE: Update the help context id")
   //::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_PIERDETAILS_CONNECTIONS );
}

void CBearingsPage::OnAddBack()
{
   m_Grid[0].AddBearing();
   FillRefBearingComboBox(0);
}

void CBearingsPage::OnRemoveBack()
{
   m_Grid[0].RemoveSelectedBearings();
   FillRefBearingComboBox(0);
}

void CBearingsPage::OnAddAhead()
{
   m_Grid[1].AddBearing();
   FillRefBearingComboBox(1);
}

void CBearingsPage::OnRemoveAhead()
{
   m_Grid[1].RemoveSelectedBearings();
   FillRefBearingComboBox(1);
}

void CBearingsPage::OnCopyAhead()
{
   std::vector<txnBearingData> brgData;
   m_Grid[0].GetBearingData(brgData);
   m_Grid[1].SetBearingData(brgData);

#pragma Reminder("Need to copy the transverse location also")
}

void CBearingsPage::OnCopyBack()
{
   std::vector<txnBearingData> brgData;
   m_Grid[1].GetBearingData(brgData);
   m_Grid[0].SetBearingData(brgData);

#pragma Reminder("Need to copy the transverse location also")
}

void CBearingsPage::OnBearingLineCountChanged()
{
   CComboBox* pCB = (CComboBox*)GetDlgItem(IDC_BEARING_LINE_COUNT);
   int curSel = pCB->GetCurSel();
   int show = (curSel == 0 ? SW_HIDE : SW_SHOW);

   GetDlgItem(IDC_BACK_LABEL)->SetWindowText(curSel == 0 ? _T("Along CL Pier") : _T("Along Back Bearing Line"));

   m_Grid[1].ShowWindow(show);
   GetDlgItem(IDC_COPY_AHEAD)->ShowWindow(show);
   GetDlgItem(IDC_COPY_BACK)->ShowWindow(show);
   GetDlgItem(IDC_ADD_AHEAD)->ShowWindow(show);
   GetDlgItem(IDC_REMOVE_AHEAD)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_LABEL)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_REF_BEARING_LIST)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_REF_BEARING_LOCATION)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_REF_BEARING_LOCATION_UNIT)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_REF_BEARING_DATUM)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_BEARING_LABEL)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_BEARING_LABEL1)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_BEARING_LABEL2)->ShowWindow(show);
   GetDlgItem(IDC_AHEAD_BEARING_LABEL3)->ShowWindow(show);
}

void CBearingsPage::FillRefBearingComboBox(IndexType brgLineIdx)
{
   CComboBox* pcbRefBearing = (CComboBox*)GetDlgItem(IDC_BACK_REF_BEARING_LIST+brgLineIdx);
   int curSel = pcbRefBearing->GetCurSel();
   pcbRefBearing->ResetContent();

   CString strLabel;
   CPierDlg* pParent = (CPierDlg*)GetParent();
   IndexType nBearings = pParent->m_PierData.m_BearingLines[brgLineIdx].size();
   nBearings = Max((IndexType)1,nBearings); // there is always one bearing minimum
   for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
   {
      strLabel.Format(_T("%d"),(brgIdx+1));
      pcbRefBearing->AddString(strLabel);
   }

   if ( pcbRefBearing->SetCurSel(curSel) == CB_ERR )
   {
      pcbRefBearing->SetCurSel(0);
   }
}

void CBearingsPage::FillRefBearingDatumComboBox(IndexType brgLineIdx)
{
   // do we need two datums, one for each bearing line, or is one good enough?
   CComboBox* pcbRefBearingDatum = (CComboBox*)GetDlgItem(IDC_BACK_REF_BEARING_DATUM+brgLineIdx);
   int curSel = pcbRefBearingDatum->GetCurSel();
   pcbRefBearingDatum->ResetContent();

   int idx = pcbRefBearingDatum->AddString(_T("Alignment"));
   pcbRefBearingDatum->SetItemData(idx,(DWORD_PTR)pgsTypes::omtAlignment);

   idx = pcbRefBearingDatum->AddString(_T("Bridge Line"));
   pcbRefBearingDatum->SetItemData(idx,(DWORD_PTR)pgsTypes::omtBridge);

   if ( pcbRefBearingDatum->SetCurSel(curSel) == CB_ERR )
   {
      pcbRefBearingDatum->SetCurSel(0);
   }
}
