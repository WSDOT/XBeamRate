// ReinforcementPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ReinforcementPage.h"

#include "PierDlg.h"

#include "ConcreteDetailsDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>


void DDX_RebarGrid(CDataExchange* pDX,CLongitudinalRebarGrid& grid,xbrLongitudinalRebarData& rebars)
{
   if ( pDX->m_bSaveAndValidate )
   {
      grid.GetRebarData(rebars);
   }
   else
   {
      grid.SetRebarData(rebars);
   }
}

// CReinforcementPage dialog

IMPLEMENT_DYNAMIC(CReinforcementPage, CPropertyPage)

CReinforcementPage::CReinforcementPage()
	: CPropertyPage(CReinforcementPage::IDD)
{

}

CReinforcementPage::~CReinforcementPage()
{
}

void CReinforcementPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   DDX_Control(pDX,IDC_REBAR_MATERIAL,m_cbRebar);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_UnitValueAndTag(pDX,IDC_FC,IDC_FC_UNIT,pParent->m_PierData.m_PierData.GetFc(),pDisplayUnits->GetStressUnit());

   DDX_UnitValueAndTag(pDX,IDC_EC,IDC_EC_UNIT,pParent->m_PierData.m_PierData.GetEc(),pDisplayUnits->GetModEUnit());
   DDX_RebarMaterial(pDX,IDC_REBAR_MATERIAL,pParent->m_PierData.m_PierData.GetRebarType(),pParent->m_PierData.m_PierData.GetRebarGrade());
   DDX_RebarGrid(pDX,m_RebarGrid,pParent->m_PierData.m_PierData.GetLongitudinalRebar());
}


BEGIN_MESSAGE_MAP(CReinforcementPage, CPropertyPage)
   ON_COMMAND(IDC_ADD_REBAR, OnAddRebar)
   ON_COMMAND(IDC_REMOVE_REBAR, OnRemoveRebar)
   ON_BN_CLICKED(IDC_MORE_PROPERTIES, &CReinforcementPage::OnMoreProperties)
   ON_BN_CLICKED(IDC_EC_LABEL, &CReinforcementPage::OnBnClickedEc)
END_MESSAGE_MAP()


// CReinforcementPage message handlers

BOOL CReinforcementPage::OnInitDialog()
{
   m_RebarGrid.SubclassDlgItem(IDC_REBAR_GRID, this);
   m_RebarGrid.CustomInit();

   CPropertyPage::OnInitDialog();

   UpdateConcreteTypeLabel();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CReinforcementPage::UpdateConcreteTypeLabel()
{
#pragma Reminder("UPDATE: need to update the concrete type label")
   // m_ConcreteType is not part of CPierDlg yet

   //CPierDlg* pParent = (CPierDlg*)GetParent();
   //CString strLabel;
   //switch( pParent->m_ConcreteType )
   //{
   //case pgsTypes::Normal:
   //   strLabel = _T("Normal Weight Concrete");
   //   break;

   //case pgsTypes::AllLightweight:
   //   strLabel = _T("All Lightweight Concrete");
   //   break;

   //case pgsTypes::SandLightweight:
   //   strLabel = _T("Sand Lightweight Concrete");
   //   break;

   //default:
   //   ATLASSERT(false); // should never get here
   //   strLabel = _T("Concrete Type Label Error");
   //}

   //GetDlgItem(IDC_CONCRETE_TYPE_LABEL)->SetWindowText(strLabel);
}

void CReinforcementPage::OnAddRebar()
{
   m_RebarGrid.AddBar();
}

void CReinforcementPage::OnRemoveRebar()
{
   m_RebarGrid.RemoveSelectedBars();
}

void CReinforcementPage::OnBnClickedEc()
{
   BOOL bEnabled = IsDlgButtonChecked(IDC_EC_LABEL) ? TRUE : FALSE;
   GetDlgItem(IDC_EC)->EnableWindow(bEnabled);
   GetDlgItem(IDC_EC_UNIT)->EnableWindow(bEnabled);
}

void CReinforcementPage::OnMoreProperties()
{
   // TODO: Add your control notification handler code here
   CConcreteDetailsDlg concreteDlg;
   if ( concreteDlg.DoModal() == IDOK )
   {
   }
}
