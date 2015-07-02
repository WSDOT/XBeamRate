
// SectionCutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SectionCutDlg.h"
#include <ostream>

//#include "HtmlHelp\HelpTopics.hh"
#include <EAF\EAFDisplayUnits.h>
#include <IFace\PointOfInterest.h>
#include <IFace\Pier.h>

#include <MFCTools\Format.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSectionCutDlg dialog

CSectionCutDlg::CSectionCutDlg(PierIDType pierID,const xbrPointOfInterest& initialPoi,CWnd* pParent) 
: CDialog(CSectionCutDlg::IDD, pParent),
m_PierID(pierID),
m_SliderPos(0)
{
   m_InitialPOI = initialPoi;
}

void CSectionCutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectionCutDlgEx) 
	//}}AFX_DATA_MAP

   DDX_Control(pDX, IDC_SLIDER, m_Slider);
   DDX_Control(pDX, IDC_LOCATION, m_Label);
   DDX_Slider(pDX, IDC_SLIDER, m_SliderPos);
}

BEGIN_MESSAGE_MAP(CSectionCutDlg, CDialog)
	//{{AFX_MSG_MAP(CSectionCutDlg)
   ON_WM_HSCROLL()
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectionCutDlg message handlers

BOOL CSectionCutDlg::OnInitDialog() 
{
   UpdatePOI();

   CDialog::OnInitDialog();

   // initial the slider range
   m_Slider.SetRange(0,(int)(m_vPOI.size()-1)); // the range is number of spaces along slider... 

   // initial the slider position to the current poi location
   CollectionIndexType pos = m_vPOI.size()/2; // default is mid-span
   std::vector<xbrPointOfInterest>::iterator iter;
   for ( iter = m_vPOI.begin(); iter != m_vPOI.end(); iter++ )
   {
      xbrPointOfInterest& poi = *iter;
      if ( poi.GetID() == m_InitialPOI.GetID() )
      {
         pos = (iter - m_vPOI.begin());
      }
   }
   m_Slider.SetPos((int)pos);

   UpdateSliderLabel();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSectionCutDlg::OnHelp() 
{
#pragma Reminder("IMPLEMENT - OnHelp")
   //::HtmlHelp( *this, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_DIALOG_SECTIONCUT );
}

xbrPointOfInterest CSectionCutDlg::GetPOI()
{
   ASSERT((int)m_SliderPos < (int)m_vPOI.size());
   xbrPointOfInterest poi = m_vPOI[m_SliderPos];
   return poi;
}

void CSectionCutDlg::UpdatePOI()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRPointOfInterest,pPOI);
   m_vPOI = pPOI->GetXBeamPointsOfInterest(m_PierID);
   if (m_Slider.GetSafeHwnd() != NULL )
   {
      m_Slider.SetRange(0,(int)(m_vPOI.size()-1)); // the range is number of spaces along slider... 
                                                   // subtract one so we don't go past the end of the array
   }
}

void CSectionCutDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   UpdateSliderLabel();
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSectionCutDlg::UpdateSliderLabel()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);

   ASSERT((int)m_SliderPos < (int)m_vPOI.size());
   xbrPointOfInterest poi = m_vPOI[m_Slider.GetPos()];

   Float64 Xp = pPoi->ConvertPoiToPierCoordinate(m_PierID,poi);

   GET_IFACE2(pBroker,IXBRPier,pPier);
   Float64 Xxb = pPier->ConvertPierToCrossBeamCoordinate(m_PierID,Xp);
   CString strLabel;
   strLabel.Format(_T("Location from Left Side of Cross Beam, %s"),::FormatDimension(Xxb,pDisplayUnits->GetXSectionDimUnit()));

   m_Label.SetWindowText(strLabel);
}
