#pragma once

#include "PierLayoutPage.h"

// CPierDlg

class CPierDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CPierDlg)

public:
	CPierDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPierDlg();

protected:
	DECLARE_MESSAGE_MAP()

   void Init();

   CPierLayoutPage m_LayoutPage;
};


