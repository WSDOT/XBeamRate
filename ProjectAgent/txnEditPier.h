///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
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

#include <WBFLCore.h>
#include <PgsExt\ColumnData.h>

class txnEditPierData
{
public:
   xbrTypes::TransverseDimensionMeasurementType m_TransverseMeasurementType;
   Float64 m_DeckElevation;
   Float64 m_CrownPointOffset;
   Float64 m_BridgeLineOffset;

   CString m_strOrientation;

   IndexType m_nBearingLines;

   Float64 m_Ec;

   ColumnIndexType m_nColumns;
   ColumnIndexType m_RefColumnIdx;
   Float64 m_TransverseOffset;
   pgsTypes::OffsetMeasurementType m_TransverseOffsetMeasurement;
   Float64 m_XBeamWidth;
   Float64 m_XBeamHeight[2];
   Float64 m_XBeamTaperHeight[2];
   Float64 m_XBeamTaperLength[2];
   Float64 m_XBeamOverhang[2];

   CColumnData::ColumnHeightMeasurementType m_ColumnHeightMeasurementType;
   Float64 m_ColumnHeight;
   Float64 m_ColumnSpacing;

   CColumnData::ColumnShapeType m_ColumnShape;
   Float64 m_B;
   Float64 m_D;
};

class txnEditPier :
   public txnTransaction
{
public:
   txnEditPier(const txnEditPierData& oldPierData,const txnEditPierData& newPierData);
   ~txnEditPier(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

   txnEditPierData m_PierData[2];

};
