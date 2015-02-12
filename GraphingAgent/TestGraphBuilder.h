///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
// Copyright © 1999-2015  Washington State Department of Transportation
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

#include <EAF\EAFGraphBuilderBase.h>
#include "TestGraphController.h"

class CEAFGraphChildFrame;

class CTestGraphBuilder : public CEAFGraphBuilderBase
{
public:
   CTestGraphBuilder();
   CTestGraphBuilder(const CTestGraphBuilder& other);
   virtual BOOL CreateGraphController(CWnd* pParent,UINT nID);
   virtual void DrawGraphNow(CWnd* pGraphWnd,CDC* pDC);
   virtual CGraphBuilder* Clone();

protected:
   virtual CEAFGraphControlWindow* GetGraphControlWindow();

   afx_msg void OnGraphTypeChanged();

   virtual bool UpdateNow() { return true; }

   DECLARE_MESSAGE_MAP()

private:
   CTestGraphController m_GraphControls;
};
