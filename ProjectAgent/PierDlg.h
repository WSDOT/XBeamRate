#pragma once

#include "SuperstructureLayoutPage.h"
#include "PierLayoutPage.h"
#include "ReinforcementPage.h"
#include "BearingsPage.h"
#include "LiveLoadReactionsPage.h"
#include "txnEditPier.h"

// CPierDlg

class CPierDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CPierDlg)

public:
	CPierDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPierDlg();

   void SetPierData(const txnEditPierData& pierData);
   const txnEditPierData& GetPierData() const;

protected:
	DECLARE_MESSAGE_MAP()

   void Init();

   txnEditPierData m_PierData;

public:
   friend CSuperstructureLayoutPage;
   friend CPierLayoutPage;
   friend CReinforcementPage;
   friend CBearingsPage;
   friend CLiveLoadReactionsPage;

   CSuperstructureLayoutPage m_SuperstructurePage;
   CPierLayoutPage m_SubstructurePage;
   CReinforcementPage m_ReinforcementPage;
   CBearingsPage m_BearingsPage;
   CLiveLoadReactionsPage m_LiveLoadPage;
};


