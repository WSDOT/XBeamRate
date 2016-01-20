///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2016  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

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
   virtual xbrTypes::PierType GetPierType();
   virtual matRebar::Type& GetRebarType();
   virtual matRebar::Grade& GetRebarGrade();
   virtual CConcreteMaterial& GetConcreteMaterial();
   virtual xbrLongitudinalRebarData& GetLongitudinalRebar();
   virtual xbrStirrupData& GetLowerXBeamStirrups();
   virtual xbrStirrupData& GetFullDepthStirrups();
   virtual pgsTypes::ConditionFactorType& GetConditionFactorType();
   virtual Float64& GetConditionFactor();

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


