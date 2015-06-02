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

void DDX_StirrupGrid(CDataExchange* pDX,CStirrupGrid& grid,xbrStirrupData& stirrups)
{
   if ( pDX->m_bSaveAndValidate )
   {
      grid.GetStirrupData(stirrups);
   }
   else
   {
      grid.SetStirrupData(stirrups);
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

void CReinforcementPage::GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade)
{
   // The grids use this method to get the rebar material
   CPierDlg* pParent = (CPierDlg*)GetParent();
   pParent->m_PierData.m_PierData.GetRebarMaterial(pType,pGrade);
}

void CReinforcementPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   DDX_Control(pDX,IDC_REBAR_MATERIAL,m_cbRebar);
	DDX_Control(pDX, IDC_EC,           m_ctrlEc);
	DDX_Control(pDX, IDC_EC_LABEL,     m_ctrlEcCheck);
	DDX_Control(pDX, IDC_FC,           m_ctrlFc);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_UnitValueAndTag(pDX,IDC_FC,IDC_FC_UNIT,pParent->m_PierData.m_PierData.GetConcreteMaterial().Fc,pDisplayUnits->GetStressUnit());
   DDX_UnitValueAndTag(pDX,IDC_EC,IDC_EC_UNIT,pParent->m_PierData.m_PierData.GetConcreteMaterial().Ec,pDisplayUnits->GetModEUnit());
   DDX_Check_Bool(pDX,IDC_EC_LABEL,pParent->m_PierData.m_PierData.GetConcreteMaterial().bUserEc);
   
   DDX_RebarMaterial(pDX,IDC_REBAR_MATERIAL,pParent->m_PierData.m_PierData.GetRebarType(),pParent->m_PierData.m_PierData.GetRebarGrade());
   DDX_RebarGrid(pDX,m_RebarGrid,pParent->m_PierData.m_PierData.GetLongitudinalRebar());
   DDX_StirrupGrid(pDX,m_LowerXBeamGrid,pParent->m_PierData.m_PierData.GetLowerXBeamStirrups());
   DDX_StirrupGrid(pDX,m_FullDepthGrid,pParent->m_PierData.m_PierData.GetFullDepthStirrups());

   DDX_Check_Bool(pDX,IDC_LOWER_XBEAM_SYMMETRY,pParent->m_PierData.m_PierData.GetLowerXBeamStirrups().Symmetric);
   DDX_Check_Bool(pDX,IDC_FULL_DEPTH_SYMMETRY, pParent->m_PierData.m_PierData.GetFullDepthStirrups().Symmetric);
}


BEGIN_MESSAGE_MAP(CReinforcementPage, CPropertyPage)
   ON_COMMAND(IDC_ADD_REBAR, OnAddRebar)
   ON_COMMAND(IDC_REMOVE_REBAR, OnRemoveRebar)
   ON_COMMAND(IDC_ADD_LOWER_XBEAM, OnAddLowerXBeam)
   ON_COMMAND(IDC_REMOVE_LOWER_XBEAM, OnRemoveLowerXBeam)
   ON_COMMAND(IDC_ADD_FULL_DEPTH, OnAddFullDepth)
   ON_COMMAND(IDC_REMOVE_FULL_DEPTH, OnRemoveFullDepth)
   ON_BN_CLICKED(IDC_MORE_PROPERTIES, &CReinforcementPage::OnMoreProperties)
   ON_BN_CLICKED(IDC_EC_LABEL, &CReinforcementPage::OnBnClickedEc)
	ON_EN_CHANGE(IDC_FC, OnChangeFc)
   ON_BN_CLICKED(IDC_LOWER_XBEAM_SYMMETRY,OnLowerXBeamSymmetry)
   ON_BN_CLICKED(IDC_FULL_DEPTH_SYMMETRY,OnFullDepthSymmetry)
END_MESSAGE_MAP()


// CReinforcementPage message handlers

BOOL CReinforcementPage::OnInitDialog()
{
   m_RebarGrid.SubclassDlgItem(IDC_REBAR_GRID, this);
   m_RebarGrid.CustomInit();

   m_LowerXBeamGrid.SubclassDlgItem(IDC_LOWER_XBEAM_STIRRUP_GRID,this);
   m_LowerXBeamGrid.CustomInit();

   m_FullDepthGrid.SubclassDlgItem(IDC_FULL_DEPTH_STIRRUP_GRID,this);
   m_FullDepthGrid.CustomInit();

   CPropertyPage::OnInitDialog();

   if ( m_strUserEc == _T("") )
   {
      m_ctrlEc.GetWindowText(m_strUserEc);
   }

   UpdateConcreteTypeLabel();
   OnBnClickedEc();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CReinforcementPage::UpdateConcreteTypeLabel()
{
   CPierDlg* pParent = (CPierDlg*)GetParent();
   CString strLabel;
   switch( pParent->m_PierData.m_PierData.GetConcreteMaterial().Type )
   {
   case pgsTypes::Normal:
      strLabel = _T("Normal Weight Concrete");
      break;

   case pgsTypes::AllLightweight:
      strLabel = _T("All Lightweight Concrete");
      break;

   case pgsTypes::SandLightweight:
      strLabel = _T("Sand Lightweight Concrete");
      break;

   default:
      ATLASSERT(false); // should never get here
      strLabel = _T("Concrete Type Label Error");
   }

   GetDlgItem(IDC_CONCRETE_TYPE_LABEL)->SetWindowText(strLabel);
}

void CReinforcementPage::OnAddRebar()
{
   m_RebarGrid.AddBar();
}

void CReinforcementPage::OnRemoveRebar()
{
   m_RebarGrid.RemoveSelectedBars();
}

void CReinforcementPage::OnAddLowerXBeam()
{
   m_LowerXBeamGrid.AddZone();
}

void CReinforcementPage::OnRemoveLowerXBeam()
{
   m_LowerXBeamGrid.RemoveSelectedZones();
}

void CReinforcementPage::OnAddFullDepth()
{
   m_FullDepthGrid.AddZone();
}

void CReinforcementPage::OnRemoveFullDepth()
{
   m_FullDepthGrid.RemoveSelectedZones();
}

void CReinforcementPage::OnBnClickedEc()
{
   BOOL bEnable = m_ctrlEcCheck.GetCheck();

   GetDlgItem(IDC_EC_LABEL)->EnableWindow(TRUE);

   if (bEnable==FALSE)
   {
      m_ctrlEc.GetWindowText(m_strUserEc);
      UpdateEc();
   }
   else
   {
      m_ctrlEc.SetWindowText(m_strUserEc);
   }

   GetDlgItem(IDC_EC)->EnableWindow(bEnable);
   GetDlgItem(IDC_EC_UNIT)->EnableWindow(bEnable);
}

void CReinforcementPage::OnMoreProperties()
{
   UpdateData(TRUE);
   CPierDlg* pParent = (CPierDlg*)GetParent();
   CConcreteDetailsDlg concreteDlg;
   concreteDlg.Concrete = pParent->m_PierData.m_PierData.GetConcreteMaterial();
   concreteDlg.m_General.m_strUserEc = m_strUserEc;
   if ( concreteDlg.DoModal() == IDOK )
   {
      pParent->m_PierData.m_PierData.SetConcreteMaterial(concreteDlg.Concrete);
      UpdateData(FALSE);
      OnBnClickedEc();
      UpdateConcreteTypeLabel();
   }
}

void CReinforcementPage::OnChangeFc() 
{
   UpdateEc();
}

void CReinforcementPage::OnLowerXBeamSymmetry()
{
   bool bSymmetric = (IsDlgButtonChecked(IDC_LOWER_XBEAM_SYMMETRY) == BST_CHECKED ? true : false);
   m_LowerXBeamGrid.SetSymmetry(bSymmetric);
}

void CReinforcementPage::OnFullDepthSymmetry()
{
   bool bSymmetric = (IsDlgButtonChecked(IDC_FULL_DEPTH_SYMMETRY) == BST_CHECKED ? true : false);
   m_FullDepthGrid.SetSymmetry(bSymmetric);
}

void CReinforcementPage::UpdateEc()
{
   CPierDlg* pParent = (CPierDlg*)GetParent();

   // update modulus
   if (m_ctrlEcCheck.GetCheck() == 0)
   {
      // blank out ec
      CString strEc;
      m_ctrlEc.SetWindowText(strEc);

      // need to manually parse strength and density values
      CString strFc, strDensity, strK1, strK2;
      m_ctrlFc.GetWindowText(strFc);

      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);
      GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

      strDensity.Format(_T("%s"),FormatDimension(pParent->m_PierData.m_PierData.GetConcreteMaterial().StrengthDensity,pDisplayUnits->GetDensityUnit(),false));
      strK1.Format(_T("%f"),pParent->m_PierData.m_PierData.GetConcreteMaterial().EcK1);
      strK2.Format(_T("%f"),pParent->m_PierData.m_PierData.GetConcreteMaterial().EcK2);

      strEc = CConcreteDetailsDlg::UpdateEc(strFc,strDensity,strK1,strK2);
      m_ctrlEc.SetWindowText(strEc);
   }
}

void CReinforcementPage::UpdateStirrupGrids()
{
   CPierDlg* pParent = (CPierDlg*)GetParent();
   int showWindow = (pParent->m_PierData.m_PierData.GetSuperstructureConnectionType() == xbrTypes::pctIntegral ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_FULL_DEPTH_LABEL)->ShowWindow(showWindow);
   GetDlgItem(IDC_FULL_DEPTH_STIRRUP_GRID)->ShowWindow(showWindow);
   GetDlgItem(IDC_ADD_FULL_DEPTH)->ShowWindow(showWindow);
   GetDlgItem(IDC_REMOVE_FULL_DEPTH)->ShowWindow(showWindow);

   if ( showWindow == SW_SHOW )
   {
      GetDlgItem(IDC_LOWER_XBEAM_LABEL)->SetWindowText(_T("Lower Cross Beam Stirrups"));
   }
   else
   {
      GetDlgItem(IDC_LOWER_XBEAM_LABEL)->SetWindowText(_T("Stirrups"));
   }
}

BOOL CReinforcementPage::OnSetActive()
{
   UpdateStirrupGrids();
   return CPropertyPage::OnSetActive();
}
