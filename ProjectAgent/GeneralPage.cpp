// GeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GeneralPage.h"
#include "PierDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>


// CGeneralPage dialog

IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPage)

CGeneralPage::CGeneralPage()
	: CPropertyPage(CGeneralPage::IDD)
{

}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_CBEnum(pDX,IDC_PIER_TYPE,pParent->m_PierData.m_PierType);
   DDX_CBEnum(pDX,IDC_TRANSVERSE_MEASUREMENT_TYPE,pParent->m_PierData.m_TransverseMeasurementType);
   DDX_UnitValueAndTag(pDX,IDC_DECK_ELEVATION,IDC_DECK_ELEVATION_UNIT,pParent->m_PierData.m_DeckElevation,pDisplayUnits->GetSpanLengthUnit());
   DDX_OffsetAndTag(pDX,IDC_CPO,IDC_CPO_UNIT,pParent->m_PierData.m_CrownPointOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_OffsetAndTag(pDX,IDC_BLO,IDC_BLO_UNIT,pParent->m_PierData.m_BridgeLineOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_Text(pDX,IDC_SKEW,pParent->m_PierData.m_strOrientation);

}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPage)
   ON_CBN_SELCHANGE(IDC_PIER_TYPE, OnPierTypeChanged)
END_MESSAGE_MAP()


// CGeneralPage message handlers

BOOL CGeneralPage::OnInitDialog()
{
   FillPierTypeComboBox();
   FillTransverseMeasureComboBox();

   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralPage::FillTransverseMeasureComboBox()
{
   CComboBox* pcbTransverseMeasure = (CComboBox*)GetDlgItem(IDC_TRANSVERSE_MEASUREMENT_TYPE);
   pcbTransverseMeasure->ResetContent();
   int idx = pcbTransverseMeasure->AddString(_T("normal to the alignment"));
   pcbTransverseMeasure->SetItemData(idx,(DWORD_PTR)xbrTypes::tdmNormalToAlignment);
   idx = pcbTransverseMeasure->AddString(_T("in the plane of the pier"));
   pcbTransverseMeasure->SetItemData(idx,(DWORD_PTR)xbrTypes::tdmPlaneOfPier);
}

void CGeneralPage::FillPierTypeComboBox()
{
   CComboBox* pcbPierType = (CComboBox*)GetDlgItem(IDC_PIER_TYPE);
   pcbPierType->ResetContent();
   int idx = pcbPierType->AddString(_T("Continuous"));
   pcbPierType->SetItemData(idx,(DWORD_PTR)xbrTypes::pctContinuous);

   idx = pcbPierType->AddString(_T("Integral"));
   pcbPierType->SetItemData(idx,(DWORD_PTR)xbrTypes::pctIntegral);

   idx = pcbPierType->AddString(_T("Expansion"));
   pcbPierType->SetItemData(idx,(DWORD_PTR)xbrTypes::pctExpansion);
}

void CGeneralPage::OnPierTypeChanged()
{
}
