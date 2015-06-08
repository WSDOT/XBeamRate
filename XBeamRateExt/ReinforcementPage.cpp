// ReinforcementPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <XBeamRateExt\ReinforcementPage.h>
#include "LongitudinalRebarGrid.h"
#include "StirrupGrid.h"

#include <IFace\ExtendUI.h>
#include <IFace\XBeamRateAgent.h>

#include <PgsExt\PierData2.h>
#include <PGSuperUnits.h> // for FormatDimension


#include <PgsExt\ConcreteDetailsDlg.h>

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

CReinforcementPage::CReinforcementPage(IReinforcementPageParent* pParent) :
CPropertyPage()
{
   m_pParent = pParent;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   CComPtr<IXBeamRateAgent> pXBR;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pXBR);
   UINT nIDCaption = (SUCCEEDED(hr) ? IDS_EXTENSION_TITLE : IDS_STANDALONE_TITLE);

   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   Construct(IDD_REINFORCEMENT_PAGE,nIDCaption);

   m_pRebarGrid = new CLongitudinalRebarGrid();

   m_pLowerXBeamGrid = new CStirrupGrid();
   m_pFullDepthGrid = new CStirrupGrid();

   m_psp.dwFlags |= PSP_HASHELP;
}

CReinforcementPage::~CReinforcementPage()
{
   delete m_pRebarGrid;
   delete m_pLowerXBeamGrid;
   delete m_pFullDepthGrid;
}

IReinforcementPageParent* CReinforcementPage::GetPageParent()
{
   return m_pParent;
}

void CReinforcementPage::GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade)
{
   // The grids use this method to get the rebar material
   *pType = m_pParent->GetRebarType();
   *pGrade = m_pParent->GetRebarGrade();
}

void CReinforcementPage::OnEnableDelete(UINT nIDC,bool bEnableDelete)
{
   if (nIDC == IDC_LOWER_XBEAM_STIRRUP_GRID )
   {
      GetDlgItem(IDC_REMOVE_LOWER_XBEAM)->EnableWindow(bEnableDelete ? TRUE : FALSE);
   }
   else
   {
      GetDlgItem(IDC_REMOVE_FULL_DEPTH)->EnableWindow(bEnableDelete ? TRUE : FALSE);
   }
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

   DDX_UnitValueAndTag(pDX,IDC_FC,IDC_FC_UNIT,m_pParent->GetConcrete().Fc,pDisplayUnits->GetStressUnit());
   DDX_UnitValueAndTag(pDX,IDC_EC,IDC_EC_UNIT,m_pParent->GetConcrete().Ec,pDisplayUnits->GetModEUnit());
   DDX_Check_Bool(pDX,IDC_EC_LABEL,m_pParent->GetConcrete().bUserEc);
   
   DDX_RebarMaterial(pDX,IDC_REBAR_MATERIAL,m_pParent->GetRebarType(),m_pParent->GetRebarGrade());
   DDX_RebarGrid(pDX,*m_pRebarGrid,m_pParent->GetLongitudinalRebar());
   DDX_StirrupGrid(pDX,*m_pLowerXBeamGrid,m_pParent->GetLowerXBeamStirrups());
   DDX_StirrupGrid(pDX,*m_pFullDepthGrid,m_pParent->GetFullDepthStirrups());

   DDX_Check_Bool(pDX,IDC_LOWER_XBEAM_SYMMETRY,m_pParent->GetLowerXBeamStirrups().Symmetric);
   DDX_Check_Bool(pDX,IDC_FULL_DEPTH_SYMMETRY, m_pParent->GetFullDepthStirrups().Symmetric);
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
   ON_COMMAND(ID_HELP,OnHelp)
END_MESSAGE_MAP()


// CReinforcementPage message handlers

BOOL CReinforcementPage::OnInitDialog()
{
   m_pRebarGrid->SubclassDlgItem(IDC_REBAR_GRID, this);
   m_pRebarGrid->CustomInit();

   m_pLowerXBeamGrid->SubclassDlgItem(IDC_LOWER_XBEAM_STIRRUP_GRID,this);
   m_pLowerXBeamGrid->CustomInit();

   m_pFullDepthGrid->SubclassDlgItem(IDC_FULL_DEPTH_STIRRUP_GRID,this);
   m_pFullDepthGrid->CustomInit();

   CPropertyPage::OnInitDialog();

   OnEnableDelete(IDC_LOWER_XBEAM_STIRRUP_GRID,false);
   OnEnableDelete(IDC_FULL_DEPTH_STIRRUP_GRID,false);

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

BOOL CALLBACK ShowChildWindow(HWND hwnd,LPARAM lParam)
{
   ::ShowWindow(hwnd,(int)lParam);
   return TRUE;
}

BOOL CReinforcementPage::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class
   const CPierData2* pPier = m_pParent->GetPierData();
   if ( pPier )
   {
      if ( pPier->GetPierModelType() == pgsTypes::pmtIdealized )
      {
         EnumChildWindows(GetSafeHwnd(),ShowChildWindow,SW_HIDE);
         CWnd* pWnd = GetDlgItem(IDC_MESSAGE);
         pWnd->ShowWindow(SW_SHOW);
      }
      else
      {
         EnumChildWindows(GetSafeHwnd(),ShowChildWindow,SW_SHOW);
         CWnd* pWnd = GetDlgItem(IDC_MESSAGE);
         pWnd->ShowWindow(SW_HIDE);
      }

      // Not editing concrete properties when this dialog is an extension page
      GetDlgItem(IDC_CONCRETE_TYPE_LABEL)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_FC_LABEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_FC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_FC_UNIT)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_EC_LABEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EC_UNIT)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_MORE_PROPERTIES)->ShowWindow(SW_HIDE);
   }

   UpdateStirrupGrids();
   return CPropertyPage::OnSetActive();
}

void CReinforcementPage::UpdateConcreteTypeLabel()
{
   CString strLabel;
   switch( m_pParent->GetConcrete().Type )
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
   m_pRebarGrid->AddBar();
}

void CReinforcementPage::OnRemoveRebar()
{
   m_pRebarGrid->RemoveSelectedBars();
}

void CReinforcementPage::OnAddLowerXBeam()
{
   m_pLowerXBeamGrid->AddZone();
}

void CReinforcementPage::OnRemoveLowerXBeam()
{
   m_pLowerXBeamGrid->RemoveSelectedZones();
}

void CReinforcementPage::OnAddFullDepth()
{
   m_pFullDepthGrid->AddZone();
}

void CReinforcementPage::OnRemoveFullDepth()
{
   m_pFullDepthGrid->RemoveSelectedZones();
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
   CConcreteDetailsDlg dlg(true  /*properties are based on f'c*/,
                           false /*don't enable Compute Time Parameters option*/,
                           false /*hide the CopyFromLibrary buton*/);

   CConcreteMaterial& concrete = m_pParent->GetConcrete();

   dlg.m_fc28 = concrete.Fc;
   dlg.m_Ec28 = concrete.Ec;
   dlg.m_bUserEc28 = concrete.bUserEc;

   dlg.m_General.m_Type    = concrete.Type;
   dlg.m_General.m_AggSize = concrete.MaxAggregateSize;
   dlg.m_General.m_Ds      = concrete.StrengthDensity;
   dlg.m_General.m_Dw      = concrete.WeightDensity;
   dlg.m_General.m_strUserEc  = m_strUserEc;

   dlg.m_AASHTO.m_EccK1       = concrete.EcK1;
   dlg.m_AASHTO.m_EccK2       = concrete.EcK2;
   dlg.m_AASHTO.m_CreepK1     = concrete.CreepK1;
   dlg.m_AASHTO.m_CreepK2     = concrete.CreepK2;
   dlg.m_AASHTO.m_ShrinkageK1 = concrete.ShrinkageK1;
   dlg.m_AASHTO.m_ShrinkageK2 = concrete.ShrinkageK2;
   dlg.m_AASHTO.m_bHasFct     = concrete.bHasFct;
   dlg.m_AASHTO.m_Fct         = concrete.Fct;

   if ( dlg.DoModal() == IDOK )
   {
      concrete.Fc               = dlg.m_fc28;
      concrete.Ec               = dlg.m_Ec28;
      concrete.bUserEc          = dlg.m_bUserEc28;

      concrete.Type             = dlg.m_General.m_Type;
      concrete.MaxAggregateSize = dlg.m_General.m_AggSize;
      concrete.StrengthDensity  = dlg.m_General.m_Ds;
      concrete.WeightDensity    = dlg.m_General.m_Dw;
      concrete.EcK1             = dlg.m_AASHTO.m_EccK1;
      concrete.EcK2             = dlg.m_AASHTO.m_EccK2;
      concrete.CreepK1          = dlg.m_AASHTO.m_CreepK1;
      concrete.CreepK2          = dlg.m_AASHTO.m_CreepK2;
      concrete.ShrinkageK1      = dlg.m_AASHTO.m_ShrinkageK1;
      concrete.ShrinkageK2      = dlg.m_AASHTO.m_ShrinkageK2;
      concrete.bHasFct          = dlg.m_AASHTO.m_bHasFct;
      concrete.Fct              = dlg.m_AASHTO.m_Fct;

      m_strUserEc  = dlg.m_General.m_strUserEc;
      m_ctrlEc.SetWindowText(m_strUserEc);

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
   m_pLowerXBeamGrid->SetSymmetry(bSymmetric);
}

void CReinforcementPage::OnFullDepthSymmetry()
{
   bool bSymmetric = (IsDlgButtonChecked(IDC_FULL_DEPTH_SYMMETRY) == BST_CHECKED ? true : false);
   m_pFullDepthGrid->SetSymmetry(bSymmetric);
}

void CReinforcementPage::UpdateEc()
{
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

      strDensity.Format(_T("%s"),FormatDimension(m_pParent->GetConcrete().StrengthDensity,pDisplayUnits->GetDensityUnit(),false));
      strK1.Format(_T("%f"),m_pParent->GetConcrete().EcK1);
      strK2.Format(_T("%f"),m_pParent->GetConcrete().EcK2);

      strEc = CConcreteDetailsDlg::UpdateEc(strFc,strDensity,strK1,strK2);
      m_ctrlEc.SetWindowText(strEc);
   }
}

void CReinforcementPage::UpdateStirrupGrids()
{
   int showWindow = (m_pParent->GetSuperstructureConnectionType() == xbrTypes::pctIntegral ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_FULL_DEPTH_LABEL)->ShowWindow(showWindow);
   GetDlgItem(IDC_FULL_DEPTH_STIRRUP_GRID)->ShowWindow(showWindow);
   GetDlgItem(IDC_ADD_FULL_DEPTH)->ShowWindow(showWindow);
   GetDlgItem(IDC_REMOVE_FULL_DEPTH)->ShowWindow(showWindow);
   GetDlgItem(IDC_FULL_DEPTH_SYMMETRY)->ShowWindow(showWindow);

   if ( showWindow == SW_SHOW )
   {
      GetDlgItem(IDC_LOWER_XBEAM_LABEL)->SetWindowText(_T("Lower Cross Beam Stirrups"));
   }
   else
   {
      GetDlgItem(IDC_LOWER_XBEAM_LABEL)->SetWindowText(_T("Stirrups"));
   }
}

void CReinforcementPage::OnHelp()
{
   //AFX_MANAGE_STATE(AfxGetStaticModuleState());
   //CString strHelpFile(AfxGetApp()->m_pszHelpFilePath);

   //DWORD_PTR dwTopic = ??;

   //::HtmlHelp( *this, strHelpFile, HH_HELP_CONTEXT, dwTopic);
#pragma Reminder("IMPLEMENT: Need help topic")
}