// GeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SuperstructureLayoutPage.h"
#include "PierDlg.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>


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

 //  CString strImageName = GetImageName();
	//DDX_MetaFileStatic(pDX, IDC_DIMENSIONS, m_Dimensions,strImageName, _T("Metafile") );

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   CPierDlg* pParent = (CPierDlg*)GetParent();

   DDX_CBEnum(pDX,IDC_PIER_TYPE,pParent->m_PierData.m_PierType);
   DDX_UnitValueAndTag(pDX,IDC_DECK_ELEVATION,IDC_DECK_ELEVATION_UNIT,pParent->m_PierData.m_DeckElevation,pDisplayUnits->GetSpanLengthUnit());
   DDX_OffsetAndTag(pDX,IDC_CPO,IDC_CPO_UNIT,pParent->m_PierData.m_CrownPointOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_OffsetAndTag(pDX,IDC_BLO,IDC_BLO_UNIT,pParent->m_PierData.m_BridgeLineOffset,pDisplayUnits->GetSpanLengthUnit());
   DDX_Text(pDX,IDC_SKEW,pParent->m_PierData.m_strOrientation);

   DDX_CBEnum(pDX,IDC_CURB_LINE_MEASUREMENT,pParent->m_PierData.m_CurbLineDatum);
   DDX_UnitValueAndTag(pDX,IDC_LCO,IDC_LCO_UNIT,pParent->m_PierData.m_LeftCLO,pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_RCO,IDC_RCO_UNIT,pParent->m_PierData.m_RightCLO,pDisplayUnits->GetSpanLengthUnit());
   DDX_Text(pDX,IDC_SL,pParent->m_PierData.m_SL);
   DDX_Text(pDX,IDC_SR,pParent->m_PierData.m_SR);

   if ( !pDX->m_bSaveAndValidate )
   {
      std::_tstring strUnitTag = pDisplayUnits->GetSpanLengthUnit().UnitOfMeasure.UnitTag();
      CString strSlopeUnit;
      strSlopeUnit.Format(_T("%s/%s"),strUnitTag.c_str(),strUnitTag.c_str());
      GetDlgItem(IDC_SL_UNIT)->SetWindowText(strSlopeUnit);
      GetDlgItem(IDC_SR_UNIT)->SetWindowText(strSlopeUnit);
   }

   DDX_UnitValueAndTag(pDX,IDC_H,IDC_H_UNIT,pParent->m_PierData.m_DiaphragmHeight,pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_W,IDC_W_UNIT,pParent->m_PierData.m_DiaphragmWidth,pDisplayUnits->GetSpanLengthUnit());
}

BEGIN_MESSAGE_MAP(CSuperstructureLayoutPage, CPropertyPage)
   ON_CBN_SELCHANGE(IDC_PIER_TYPE, OnPierTypeChanged)
   ON_CBN_SELCHANGE(IDC_CURB_LINE_MEASUREMENT, OnCurbLineDatumChanged)
END_MESSAGE_MAP()


// CSuperstructureLayoutPage message handlers

BOOL CSuperstructureLayoutPage::OnInitDialog()
{
   FillCurbLineMeasureComboBox();
   FillPierTypeComboBox();

   CPropertyPage::OnInitDialog();

   OnCurbLineDatumChanged();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

CString CSuperstructureLayoutPage::GetImageName()
{
   CComboBox* pcbCurbLineMeasure = (CComboBox*)GetDlgItem(IDC_CURB_LINE_MEASUREMENT);
   int idx = pcbCurbLineMeasure->GetCurSel();
   pgsTypes::OffsetMeasurementType datum = (pgsTypes::OffsetMeasurementType)pcbCurbLineMeasure->GetItemData(idx);

   CComboBox* pcbPierType = (CComboBox*)GetDlgItem(IDC_PIER_TYPE);
   idx = pcbPierType->GetCurSel();
   xbrTypes::PierConnectionType pierType = (xbrTypes::PierConnectionType)pcbPierType->GetItemData(idx);

   if ( datum == pgsTypes::omtAlignment )
   {
      switch(pierType)
      {
      case xbrTypes::pctIntegral:
         return _T("DIMENSIONS_ALIGNMENT_INTEGRAL");
      case xbrTypes::pctContinuous:
         return _T("DIMENSIONS_ALIGNMENT_CONTINUOUS");
      case xbrTypes::pctExpansion:
         return _T("DIMENSIONS_ALIGNMENT_EXPANSION");
      }
   }
   else
   {
      switch(pierType)
      {
      case xbrTypes::pctIntegral:
         return _T("DIMENSIONS_BRIDGELINE_INTEGRAL");
      case xbrTypes::pctContinuous:
         return _T("DIMENSIONS_BRIDGELINE_CONTINUOUS");
      case xbrTypes::pctExpansion:
         return _T("DIMENSIONS_BRIDGELINE_EXPANSION");
      }
   }

   ATLASSERT(false); // should not get here
   return _T("");
}

void CSuperstructureLayoutPage::FillCurbLineMeasureComboBox()
{
   CComboBox* pcbCurbLineMeasure = (CComboBox*)GetDlgItem(IDC_CURB_LINE_MEASUREMENT);
   pcbCurbLineMeasure->ResetContent();

   int idx = pcbCurbLineMeasure->AddString(_T("Alignment"));
   pcbCurbLineMeasure->SetItemData(idx,(DWORD_PTR)pgsTypes::omtAlignment);

   idx = pcbCurbLineMeasure->AddString(_T("Bridge Line"));
   pcbCurbLineMeasure->SetItemData(idx,(DWORD_PTR)pgsTypes::omtBridge);
}

void CSuperstructureLayoutPage::FillPierTypeComboBox()
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

void CSuperstructureLayoutPage::UpdateImage()
{
   CDataExchange dx(this,FALSE);
   CString strImageName = GetImageName();
	DDX_MetaFileStatic(&dx, IDC_DIMENSIONS, m_Dimensions,strImageName, _T("Metafile") );
}

void CSuperstructureLayoutPage::OnCurbLineDatumChanged()
{
   UpdateImage();
}

void CSuperstructureLayoutPage::OnPierTypeChanged()
{
   UpdateImage();
}
