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

#include "stdafx.h"
#include "txnEditPier.h"

#include <IFace\Project.h>

txnEditPier::txnEditPier(Float64 oldLeftOverhang,Float64 oldRightOverhang,IndexType oldColumnCount,Float64 oldColumnHeight,Float64 oldColumnSpacing,
                         Float64 newLeftOverhang,Float64 newRightOverhang,IndexType newColumnCount,Float64 newColumnHeight,Float64 newColumnSpacing)
{
   m_LeftOverhang[0] = oldLeftOverhang;
   m_LeftOverhang[1] = newLeftOverhang;
   m_RightOverhang[0] = oldRightOverhang;
   m_RightOverhang[1] = newRightOverhang;
   m_nColumns[0] = oldColumnCount;
   m_nColumns[1] = newColumnCount;
   m_ColumnHeight[0] = oldColumnHeight;
   m_ColumnHeight[1] = newColumnHeight;
   m_ColumnSpacing[0] = oldColumnSpacing;
   m_ColumnSpacing[1] = newColumnSpacing;
}

txnEditPier::~txnEditPier(void)
{
}

bool txnEditPier::Execute()
{
   Execute(1);
   return true;
}

void txnEditPier::Undo()
{
   Execute(0);
}

void txnEditPier::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   //GET_IFACE2(pBroker,IEvents, pEvents);
   //pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRProject,pProject);
   pProject->SetOverhangs(m_LeftOverhang[i],m_RightOverhang[i]);
   pProject->SetColumns(m_nColumns[i],m_ColumnHeight[i],m_ColumnSpacing[i]);

   //pEvents->FirePendingEvents();
}

txnTransaction* txnEditPier::CreateClone() const
{
   return new txnEditPier(m_LeftOverhang[0],m_RightOverhang[0],m_nColumns[0],m_ColumnHeight[0],m_ColumnSpacing[0],
                          m_LeftOverhang[1],m_RightOverhang[1],m_nColumns[1],m_ColumnHeight[1],m_ColumnSpacing[1]);
}

std::_tstring txnEditPier::Name() const
{
   return _T("Edit Pier");
}

bool txnEditPier::IsUndoable()
{
   return true;
}

bool txnEditPier::IsRepeatable()
{
   return false;
}
