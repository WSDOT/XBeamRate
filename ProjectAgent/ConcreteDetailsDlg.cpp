
// ConcreteDetailsDlg.cpp : implementation file
//

///////////////////////////////////////////////////////////////////////////
// NOTE: Duplicate code warning
//
// This dialog along with all its property pages are basically repeated in
// the PGSuperLibrary project. I could not get a single implementation to
// work because of issues with the module resources.
//
// If changes are made here, the same changes are likely needed in
// the other location.

#include "stdafx.h"
#include "resource.h"
#include "ConcreteDetailsDlg.h"

#include <System\Tokenizer.h>
#include <Lrfd\ConcreteUtil.h>
#include <EAF\EAFDisplayUnits.h>
#include <IFace\Project.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConcreteDetailsDlg dialog


CConcreteDetailsDlg::CConcreteDetailsDlg(CWnd* pParent /*=NULL*/,UINT iSelectPage/*=0*/)
	: CPropertySheet(_T("Concrete Details"),pParent, iSelectPage)
{
	//{{AFX_DATA_INIT(CConcreteDetailsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   Init();
}

BEGIN_MESSAGE_MAP(CConcreteDetailsDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CConcreteDetailsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConcreteDetailsDlg message handlers

BOOL CConcreteDetailsDlg::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConcreteDetailsDlg::Init()
{
   m_psh.dwFlags |= PSH_HASHELP | PSH_NOAPPLYNOW;

   m_General.m_psp.dwFlags  |= PSP_HASHELP;
   m_AASHTO.m_psp.dwFlags   |= PSP_HASHELP;

   AddPage( &m_General );
   AddPage( &m_AASHTO );
}

CString CConcreteDetailsDlg::UpdateEc(const CString& strFc,const CString& strDensity,const CString& strK1,const CString& strK2)
{
   CString strEc;
   Float64 fc, density, k1,k2;
   Float64 ec = 0;
   if (sysTokenizer::ParseDouble(strFc, &fc) && 
       sysTokenizer::ParseDouble(strDensity,&density) &&
       sysTokenizer::ParseDouble(strK1,&k1) &&
       sysTokenizer::ParseDouble(strK2,&k2) &&
       0 < density && 0 < fc && 0 < k1 && 0 < k2
       )
   {
         CComPtr<IBroker> pBroker;
         EAFGetBroker(&pBroker);
         GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

         const unitPressure& stress_unit = pDisplayUnits->GetStressUnit().UnitOfMeasure;
         const unitDensity& density_unit = pDisplayUnits->GetDensityUnit().UnitOfMeasure;

         fc       = ::ConvertToSysUnits(fc,      stress_unit);
         density  = ::ConvertToSysUnits(density, density_unit);

         ec = k1*k2*lrfdConcreteUtil::ModE(fc,density,false);

         strEc.Format(_T("%s"),FormatDimension(ec,pDisplayUnits->GetModEUnit(),false));
   }

   return strEc;
}
