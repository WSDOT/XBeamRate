// PierLayoutPat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProjectAgent.h"
#include "PierLayoutPage.h"


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
}


BEGIN_MESSAGE_MAP(CPierLayoutPage, CPropertyPage)
END_MESSAGE_MAP()


// CPierLayoutPage message handlers
