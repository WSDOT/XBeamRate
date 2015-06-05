#pragma once

#include "SuperstructureLayoutPage.h"
#include "PierLayoutPage.h"
#include <XBeamRateExt\ReinforcementPage.h>
#include "BearingsPage.h"
#include "LiveLoadReactionsPage.h"
#include "txnEditPier.h"

// CPierDlg

class CPierDlg : public CPropertySheet, public IReinforcementPageParent
{
	DECLARE_DYNAMIC(CPierDlg)

public:
	CPierDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPierDlg();

   void SetEditPierData(const txnEditPierData& pierData);
   const txnEditPierData& GetEditPierData() const;

   // IReinforcementPageParent
   virtual CPierData2* GetPierData();
   virtual CConcreteMaterial& GetConcrete();
   virtual xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType();
   virtual matRebar::Type& GetRebarType();
   virtual matRebar::Grade& GetRebarGrade();
   virtual CConcreteMaterial& GetConcreteMaterial();
   virtual xbrLongitudinalRebarData& GetLongitudinalRebar();
   virtual xbrStirrupData& GetLowerXBeamStirrups();
   virtual xbrStirrupData& GetFullDepthStirrups();

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


