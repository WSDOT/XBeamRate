///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
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

#include "StdAfx.h"
#include "XBeamRateReportSpecificationBuilder.h"
#include "XBeamRateReportSpecification.h"
#include "PierReportDlg.h"

#include <XBeamRateExt\XBeamRateUtilities.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <IFace\Selection.h>

CXBeamRateReportSpecificationBuilder::CXBeamRateReportSpecificationBuilder(IBroker* pBroker) :
CEAFBrokerReportSpecificationBuilder(pBroker)
{
}

CXBeamRateReportSpecificationBuilder::~CXBeamRateReportSpecificationBuilder(void)
{
}

boost::shared_ptr<CReportSpecification> CXBeamRateReportSpecificationBuilder::CreateReportSpec(const CReportDescription& rptDesc,boost::shared_ptr<CReportSpecification>& pRptSpec)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if ( IsPGSExtension() )
   {
      GET_IFACE(IBridgeDescription,pIBridgeDesc);
      PierIndexType nPiers = pIBridgeDesc->GetPierCount();
      if ( nPiers < 3 )
      {
         CString strMsg(_T("The bridge must have at least two spans for cross beam analysis"));
         AfxMessageBox(strMsg,MB_OK | MB_ICONINFORMATION);
         return boost::shared_ptr<CReportSpecification>();
      }
   }

   CPierReportDlg dlg(m_pBroker,rptDesc,pRptSpec);

   if ( dlg.DoModal() == IDOK )
   {
      boost::shared_ptr<CReportSpecification> pRptSpec( new CXBeamRateReportSpecification(rptDesc.GetReportName(),m_pBroker,dlg.m_PierID) );

      std::vector<std::_tstring> chList = dlg.m_ChapterList;
      rptDesc.ConfigureReportSpecification(chList,pRptSpec);

      return pRptSpec;
   }

   return boost::shared_ptr<CReportSpecification>();
}

boost::shared_ptr<CReportSpecification> CXBeamRateReportSpecificationBuilder::CreateDefaultReportSpec(const CReportDescription& rptDesc)
{
   PierIDType pierID = INVALID_ID;

   if ( IsPGSExtension() )
   {
      GET_IFACE(ISelection,pSelection);
      PierIndexType selPierIdx = pSelection->GetSelectedPier();

      GET_IFACE(IBridgeDescription,pIBridgeDesc);
      PierIndexType nPiers = pIBridgeDesc->GetPierCount();

      if ( selPierIdx == INVALID_INDEX || selPierIdx == 0 || selPierIdx == nPiers-1 )
      {
         boost::shared_ptr<CReportSpecification> nullSpec;
         return CreateReportSpec(rptDesc,nullSpec);
      }

      if ( selPierIdx != INVALID_INDEX )
      {
         const CPierData2* pPier = pIBridgeDesc->GetPier(selPierIdx);
         pierID = pPier->GetID();
      }
   }

   // Use all chapters at the maximum level
   boost::shared_ptr<CReportSpecification> pRptSpec( new CXBeamRateReportSpecification(rptDesc.GetReportName(),m_pBroker,pierID) );
   rptDesc.ConfigureReportSpecification(pRptSpec);
   return pRptSpec;
}
