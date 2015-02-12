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

class txnEditPier :
   public txnTransaction
{
public:
   txnEditPier(Float64 oldLeftOverhang,Float64 oldRightOverhang,IndexType oldColumnCount,Float64 oldColumnHeight,Float64 oldColumnSpacing,
               Float64 newLeftOverhang,Float64 newRightOverhang,IndexType newColumnCount,Float64 newColumnHeight,Float64 newColumnSpacing);
   ~txnEditPier(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

	Float64 m_LeftOverhang[2];
	Float64 m_RightOverhang[2];
   IndexType m_nColumns[2];
   Float64 m_ColumnHeight[2];
   Float64 m_ColumnSpacing[2];
};
