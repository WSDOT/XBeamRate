// PierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProjectAgent.h"
#include "PierDlg.h"


// CPierDlg

IMPLEMENT_DYNAMIC(CPierDlg, CPropertySheet)

CPierDlg::CPierDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
   m_ReinforcementPage(this)
{
   Init();
}

CPierDlg::CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage),
   m_ReinforcementPage(this)
{
   Init();
}

CPierDlg::~CPierDlg()
{
}

void CPierDlg::SetEditPierData(const txnEditPierData& pierData)
{
   m_PierData = pierData;
}

const txnEditPierData& CPierDlg::GetEditPierData() const
{
   return m_PierData;
}

//////////////////////////////////////////////////
// IReinforcementPageParent
CPierData2* CPierDlg::GetPierData()
{
   return NULL; // we are not associated with pier data for an entire bridge
}

CConcreteMaterial& CPierDlg::GetConcrete()
{
   return m_PierData.m_PierData.GetConcreteMaterial();
}

xbrTypes::SuperstructureConnectionType CPierDlg::GetSuperstructureConnectionType()
{
   return m_PierData.m_PierData.GetSuperstructureConnectionType();
}

matRebar::Type& CPierDlg::GetRebarType()
{
   return m_PierData.m_PierData.GetRebarType();
}

matRebar::Grade& CPierDlg::GetRebarGrade()
{
   return m_PierData.m_PierData.GetRebarGrade();
}

CConcreteMaterial& CPierDlg::GetConcreteMaterial()
{
   return m_PierData.m_PierData.GetConcreteMaterial();
}

xbrLongitudinalRebarData& CPierDlg::GetLongitudinalRebar()
{
   return m_PierData.m_PierData.GetLongitudinalRebar();
}

xbrStirrupData& CPierDlg::GetLowerXBeamStirrups()
{
   return m_PierData.m_PierData.GetLowerXBeamStirrups();
}

xbrStirrupData& CPierDlg::GetFullDepthStirrups()
{
   return m_PierData.m_PierData.GetFullDepthStirrups();
}

//////////////////////////////////////////////////
void CPierDlg::Init()
{
   m_psh.dwFlags |= PSH_HASHELP | PSH_NOAPPLYNOW;

   m_SuperstructurePage.m_psp.dwFlags  |= PSP_HASHELP;
   m_SubstructurePage.m_psp.dwFlags    |= PSP_HASHELP;
   m_ReinforcementPage.m_psp.dwFlags   |= PSP_HASHELP;
   m_BearingsPage.m_psp.dwFlags        |= PSP_HASHELP;
   m_LiveLoadPage.m_psp.dwFlags        |= PSP_HASHELP;

   AddPage(&m_SuperstructurePage);
   AddPage(&m_SubstructurePage);
   AddPage(&m_ReinforcementPage);
   AddPage(&m_BearingsPage);
   AddPage(&m_LiveLoadPage);
}


BEGIN_MESSAGE_MAP(CPierDlg, CPropertySheet)
END_MESSAGE_MAP()


// CPierDlg message handlers
