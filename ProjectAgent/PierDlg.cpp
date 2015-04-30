// PierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProjectAgent.h"
#include "PierDlg.h"


// CPierDlg

IMPLEMENT_DYNAMIC(CPierDlg, CPropertySheet)

CPierDlg::CPierDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init();
}

CPierDlg::CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init();
}

CPierDlg::~CPierDlg()
{
}

void CPierDlg::SetPierData(const txnEditPierData& pierData)
{
   m_PierData = pierData;
}

const txnEditPierData& CPierDlg::GetPierData() const
{
   return m_PierData;
}

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
