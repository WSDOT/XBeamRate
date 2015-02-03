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

void CPierDlg::Init()
{
   AddPage(&m_LayoutPage);
}


BEGIN_MESSAGE_MAP(CPierDlg, CPropertySheet)
END_MESSAGE_MAP()


// CPierDlg message handlers
