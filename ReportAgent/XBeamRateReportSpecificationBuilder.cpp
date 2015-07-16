#include "StdAfx.h"
#include "XBeamRateReportSpecificationBuilder.h"
#include "XBeamRateReportSpecification.h"

CXBeamRateReportSpecificationBuilder::CXBeamRateReportSpecificationBuilder(IBroker* pBroker) :
CEAFBrokerReportSpecificationBuilder(pBroker)
{
}

CXBeamRateReportSpecificationBuilder::~CXBeamRateReportSpecificationBuilder(void)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

}

boost::shared_ptr<CReportSpecification> CXBeamRateReportSpecificationBuilder::CreateReportSpec(const CReportDescription& rptDesc,boost::shared_ptr<CReportSpecification>& pRptSpec)
{
#pragma Reminder("WORKING HERE - need to use our own dialog box to prompt for pier id")
   // also need to list chapters....
   PierIDType pierID = 1;
   boost::shared_ptr<CReportSpecification> pTheRptSpec(new CXBeamRateReportSpecification(rptDesc.GetReportName(),m_pBroker,pierID));
   pTheRptSpec->SetChapterInfo(rptDesc.GetChapterInfo());
   return pTheRptSpec;
//   CSpanGirderReportDlg dlg(m_pBroker,rptDesc,ChaptersOnly,pRptSpec);

   //if ( dlg.DoModal() == IDOK )
   //{
   //   boost::shared_ptr<CReportSpecification> pRptSpec( new CBrokerReportSpecification(rptDesc.GetReportName(),m_pBroker) );

   //   std::vector<std::_tstring> chList = dlg.m_ChapterList;
   //   rptDesc.ConfigureReportSpecification(chList,pRptSpec);

   //   return pRptSpec;
   //}

   //return boost::shared_ptr<CReportSpecification>();
}
