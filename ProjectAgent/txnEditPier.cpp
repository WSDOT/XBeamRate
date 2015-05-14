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

txnBearingData::txnBearingData()
{
   m_DC = 0;
   m_DW = 0;
   m_S = 0;
}

txnLiveLoadData::txnLiveLoadData()
{
}

txnLongutindalRebarData::txnLongutindalRebarData()
{
   datum = xbrTypes::Bottom;
   cover = 0;
   barType = matRebar::bs3;
   nBars = 1;
   spacing = 0;
}

/////////////////////////////////////////////////////////////////
txnEditPierData::txnEditPierData()
{
   m_DeckElevation = 0;
   m_CrownPointOffset = 0;
   m_BridgeLineOffset = 0;

   m_strOrientation = _T("00 00 0.00");
   
   m_CurbLineDatum = pgsTypes::omtAlignment;
   m_LeftCLO = 0;
   m_RightCLO = 0;
   m_SL = 0;
   m_SR = 0;

   m_DiaphragmHeight = 0;
   m_DiaphragmWidth = 0;

   m_nBearingLines = 1;
   for ( int i = 0; i < 2; i++ )
   {
      m_BearingLines[i].push_back(txnBearingData());
      m_RefBearingIdx[i] = 0;
      m_RefBearingLocation[i] = 0;
      m_RefBearingDatum[i] = pgsTypes::omtAlignment;
   }

   m_Ec = 0;

   m_nColumns = 0;
   m_RefColumnIdx = 0;
   m_TransverseOffset = 0;
   m_TransverseOffsetMeasurement = pgsTypes::omtAlignment;
   m_XBeamWidth = 0;
   for ( int i = 0; i < 2; i++ )
   {
      m_XBeamHeight[i] = 0;
      m_XBeamTaperHeight[i] = 0;
      m_XBeamTaperLength[i] = 0;
      m_XBeamOverhang[i] = 0;
   }

   m_ColumnHeightMeasurementType = CColumnData::chtHeight;
   m_ColumnHeight = 0;
   m_ColumnSpacing = 0;

   m_ColumnShape = CColumnData::cstCircle;
   m_B = 0;
   m_D = 0;
}

/////////////////////////////////////////////////////////////////
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

#pragma Reminder("UPDATE: implement event holding")
   //GET_IFACE2(pBroker,IEvents, pEvents);
   //pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRProject,pProject);

   pProject->SetPierType(m_PierData[i].m_PierType);
   pProject->SetDeckElevation(m_PierData[i].m_DeckElevation);
   pProject->SetCrownPointOffset(m_PierData[i].m_CrownPointOffset);
   pProject->SetBridgeLineOffset(m_PierData[i].m_BridgeLineOffset);
   pProject->SetOrientation(m_PierData[i].m_strOrientation);

   pProject->SetCurbLineDatum(m_PierData[i].m_CurbLineDatum);
   pProject->SetCurbLineOffset(m_PierData[i].m_LeftCLO,m_PierData[i].m_RightCLO);
   pProject->SetCrownSlopes(m_PierData[i].m_SL,m_PierData[i].m_SR);

   pProject->SetDiaphragmDimensions(m_PierData[i].m_DiaphragmHeight,m_PierData[i].m_DiaphragmWidth);

   pProject->SetBearingLineCount(m_PierData[i].m_nBearingLines);

   for ( IndexType brgLineIdx = 0; brgLineIdx < m_PierData[i].m_nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = m_PierData[i].m_BearingLines[brgLineIdx].size();
      pProject->SetBearingCount(brgLineIdx,nBearings);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         if ( brgIdx < nBearings-1 )
         {
            pProject->SetBearingSpacing(brgLineIdx,brgIdx,m_PierData[i].m_BearingLines[brgLineIdx].at(brgIdx).m_S);
         }

         pProject->SetBearingReactions(brgLineIdx,brgIdx,
                                       m_PierData[i].m_BearingLines[brgLineIdx].at(brgIdx).m_DC,
                                       m_PierData[i].m_BearingLines[brgLineIdx].at(brgIdx).m_DW);
      }

      pProject->SetReferenceBearing(brgLineIdx,m_PierData[i].m_RefBearingIdx[brgLineIdx],m_PierData[i].m_RefBearingLocation[brgLineIdx],m_PierData[i].m_RefBearingDatum[brgLineIdx]);
   }

   pProject->SetModE(m_PierData[i].m_Ec);

   pProject->RemoveRebarRows();
   BOOST_FOREACH(txnLongutindalRebarData& rebarData,m_PierData[i].m_Rebar)
   {
      pProject->AddRebarRow(rebarData.datum,rebarData.cover,rebarData.barType,rebarData.nBars,rebarData.spacing);
   }


   for ( int j = 0; j < 2; j++ )
   {
      pgsTypes::PierSideType side = (pgsTypes::PierSideType)j;
      pProject->SetXBeamDimensions(side,m_PierData[i].m_XBeamHeight[side],m_PierData[i].m_XBeamTaperHeight[side],m_PierData[i].m_XBeamTaperLength[side]);
      pProject->SetXBeamOverhang(side,m_PierData[i].m_XBeamOverhang[side]);
   }
   
   pProject->SetXBeamWidth(m_PierData[i].m_XBeamWidth);
   pProject->SetColumns(m_PierData[i].m_nColumns,m_PierData[i].m_ColumnHeight,m_PierData[i].m_ColumnHeightMeasurementType,m_PierData[i].m_ColumnSpacing);
   pProject->SetColumnShape(m_PierData[i].m_ColumnShape,m_PierData[i].m_B,m_PierData[i].m_D);
   pProject->SetTransverseLocation(m_PierData[i].m_RefColumnIdx,m_PierData[i].m_TransverseOffset,m_PierData[i].m_TransverseOffsetMeasurement);

   pProject->SetLiveLoadReactions(pgsTypes::lltDesign,m_PierData[i].m_DesignLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lltLegalRating_Routine,m_PierData[i].m_LegalRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lltLegalRating_Special,m_PierData[i].m_LegalSpecialLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lltPermitRating_Routine,m_PierData[i].m_PermitRoutineLiveLoad.m_LLIM);
   pProject->SetLiveLoadReactions(pgsTypes::lltPermitRating_Special,m_PierData[i].m_PermitSpecialLiveLoad.m_LLIM);

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
