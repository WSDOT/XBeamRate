// PierLayoutPat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProjectAgent.h"
#include "PierLayoutPage.h"

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\CustomDDX.h>

// CPierLayoutPage dialog

IMPLEMENT_DYNAMIC(CPierLayoutPage, CPropertyPage)

CPierLayoutPage::CPierLayoutPage()
	: CPropertyPage(CPierLayoutPage::IDD)
{

}

CPierLayoutPage::~CPierLayoutPage()
{
}

void CPierLayoutPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   DDX_UnitValueAndTag(pDX,IDC_LEFT_OVERHANG, IDC_LEFT_OVERHANG_UNIT, m_LeftOverhang, pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_RIGHT_OVERHANG,IDC_RIGHT_OVERHANG_UNIT,m_RightOverhang,pDisplayUnits->GetSpanLengthUnit());

   DDX_Text(pDX,IDC_COLUMN_COUNT,m_nColumns);
   DDX_UnitValueAndTag(pDX,IDC_COLUMN_LENGTH, IDC_COLUMN_LENGTH_UNIT, m_ColumnHeight, pDisplayUnits->GetSpanLengthUnit());
   DDX_UnitValueAndTag(pDX,IDC_COLUMN_SPACING,IDC_COLUMN_SPACING_UNIT,m_ColumnSpacing,pDisplayUnits->GetSpanLengthUnit());
}


BEGIN_MESSAGE_MAP(CPierLayoutPage, CPropertyPage)
END_MESSAGE_MAP()


// CPierLayoutPage message handlers
