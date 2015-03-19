// SuperstructureLayoutPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SuperstructureLayoutPage.h"
#include "PierDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>

void DDX_BearingGrid(CDataExchange* pDX,CBearingLayoutGrid& grid,std::vector<txnBearingLineData>& brgData)
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


// CSuperstructureLayoutPage dialog

IMPLEMENT_DYNAMIC(CSuperstructureLayoutPage, CPropertyPage)

CSuperstructureLayoutPage::CSuperstructureLayoutPage()
	: CPropertyPage(CSuperstructureLayoutPage::IDD)
{

}

CSuperstructureLayoutPage::~CSuperstructureLayoutPage()
{
}

void CSuperstructureLayoutPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_CBEnum(pDX,IDC_TRANSVERSE_MEASUREMENT_TYPE,pParent->m_PierData.m_TransverseMeasurementType);
   DDX_UnitValueAndTag(pDX,IDC_DECK_ELEVATION,IDC_DECK_ELEVATION_UNIT,pParent->m_PierData.m_DeckElevation,pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_CPO,IDC_CPO_UNIT,pParent->m_PierData.m_CrownPointOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_BLO,IDC_BLO_UNIT,pParent->m_PierData.m_BridgeLineOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_Text(pDX,IDC_SKEW,pParent->m_PierData.m_strOrientation);
   DDX_CBIndex(pDX,IDC_BEARING_LINE_COUNT,pParent->m_PierData.m_nBearingLines);

   for ( IndexType brgLineIdx = 0; brgLineIdx < pParent->m_PierData.m_nBearingLines; brgLineIdx++ )
   {
      // need to use two different grids
      DDX_BearingGrid(pDX,m_Grid,pParent->m_PierData.m_BearingLines[brgLineIdx]);
   }
}


BEGIN_MESSAGE_MAP(CSuperstructureLayoutPage, CPropertyPage)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


// CSuperstructureLayoutPage message handlers
BOOL CSuperstructureLayoutPage::OnInitDialog()
{
   m_Grid.SubclassDlgItem(IDC_BEARING_GRID, this);
   m_Grid.CustomInit();

   FillTransverseMeasureComboBox();

   CPropertyPage::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSuperstructureLayoutPage::OnHelp() 
{
#pragma Reminder("UPDATE: Update the help context id")
   //::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_PIERDETAILS_CONNECTIONS );
}

void CSuperstructureLayoutPage::FillTransverseMeasureComboBox()
{
   CComboBox* pcbTransverseMeasure = (CComboBox*)GetDlgItem(IDC_TRANSVERSE_MEASUREMENT_TYPE);
   pcbTransverseMeasure->ResetContent();
   int idx = pcbTransverseMeasure->AddString(_T("normal to the alignment"));
   pcbTransverseMeasure->SetItemData(idx,(DWORD_PTR)xbrTypes::tdmNormalToAlignment);
   idx = pcbTransverseMeasure->AddString(_T("in the plane of the pier"));
   pcbTransverseMeasure->SetItemData(idx,(DWORD_PTR)xbrTypes::tdmPlaneOfPier);
}
