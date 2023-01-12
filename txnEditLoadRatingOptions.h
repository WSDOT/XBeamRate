///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright � 1999-2023  Washington State Department of Transportation
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

struct txnLoadRatingOptions
{
   pgsTypes::AnalysisType m_AnalysisType;
   xbrTypes::PermitRatingMethod m_PermitRatingMethod;
   xbrTypes::EmergencyRatingMethod m_EmergencyRatingMethod;
   Float64 m_MaxLLStepSize;
   IndexType m_MaxLoadedLanes;
   Float64 m_SystemFactorFlexure;
   Float64 m_SystemFactorShear;
   bool m_bDoAnalyzeNegativeMomentBetweenFOC;
   Float64 m_MinColumnWidthForNegMoment;
};

class txnEditLoadRatingOptions :
   public txnTransaction
{
public:
   txnEditLoadRatingOptions(const txnLoadRatingOptions& oldOptions,const txnLoadRatingOptions& newOptions);
   ~txnEditLoadRatingOptions(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

   txnLoadRatingOptions m_Options[2];
};
