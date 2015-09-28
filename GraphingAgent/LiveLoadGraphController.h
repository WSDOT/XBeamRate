///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
// Copyright � 1999-2015  Washington State Department of Transportation
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

#include <EAF\EAFGraphControlWindow.h>
#include <IFace\AnalysisResults.h>
#include <GraphingTypes.h>

class CXBRLiveLoadGraphDefinitions;

class CXBRLiveLoadGraphController : public CEAFGraphControlWindow
{
public:
   CXBRLiveLoadGraphController();
   DECLARE_DYNCREATE(CXBRLiveLoadGraphController);

   std::vector<IndexType> GetSelectedLiveLoadConfigurations();
   ActionType GetActionType();
   PierIDType GetPierID();
   pgsTypes::LoadRatingType GetLoadRatingType();
   VehicleIndexType GetVehicleIndex();

   void EnableControls(BOOL bEnable);

   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

   void FillLoadingList();
   void SelectAll();
   void Next();
   void Prev();

   void RatingTypeChanged();

protected:

   virtual BOOL OnInitDialog();

	//{{AFX_MSG(CXBRLiveLoadGraphController)
   //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

   void FillPierList();
   void FillRatingType();
   void FillVehicleType();

#ifdef _DEBUG
public:
   void AssertValid() const;
   void Dump(CDumpContext& dc) const;
#endif //_DEBUG
};