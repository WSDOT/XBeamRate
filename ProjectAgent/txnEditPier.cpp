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

using namespace XBR;

txnEditPier::txnEditPier(const txnEditPierData& oldPierData,const txnEditPierData& newPierData)
{
   m_PierData[0] = oldPierData;
   m_PierData[1] = newPierData;
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

   GET_IFACE2_(XBR,pBroker,IProject,pProject);

   pProject->SetModE(m_PierData[i].m_Ec);

   for ( int j = 0; j < 2; j++ )
   {
      pgsTypes::PierSideType side = (pgsTypes::PierSideType)j;
      pProject->SetXBeamDimensions(side,m_PierData[i].m_XBeamHeight[side],m_PierData[i].m_XBeamTaperHeight[side],m_PierData[i].m_XBeamTaperLength[side]);
      pProject->SetXBeamOverhang(side,m_PierData[i].m_XBeamOverhang[side]);
   }
   
   pProject->SetXBeamWidth(m_PierData[i].m_XBeamWidth);
   pProject->SetColumns(m_PierData[i].m_nColumns,m_PierData[i].m_ColumnHeight,m_PierData[i].m_ColumnHeightMeasurementType,m_PierData[i].m_ColumnSpacing);

   pProject->SetTransverseLocation(m_PierData[i].m_RefColumnIdx,m_PierData[i].m_TransverseOffset,m_PierData[i].m_TransverseOffsetMeasurement);

#pragma Reminder("WORKING HERE - need to update IProject for the rest of the pier data")
   // reference column, column shape, and column height measurement type


   //pEvents->FirePendingEvents();
}

txnTransaction* txnEditPier::CreateClone() const
{
   return new txnEditPier(m_PierData[0],m_PierData[1]);
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
