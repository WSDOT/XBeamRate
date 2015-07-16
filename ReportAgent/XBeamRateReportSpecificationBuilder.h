
#pragma once

#include <EAF\EAFBrokerReportSpecificationBuilder.h>

class CXBeamRateReportSpecificationBuilder :
   public CEAFBrokerReportSpecificationBuilder
{
public:
   CXBeamRateReportSpecificationBuilder(IBroker* pBroker);
   ~CXBeamRateReportSpecificationBuilder(void);

   virtual boost::shared_ptr<CReportSpecification> CreateReportSpec(const CReportDescription& rptDesc,boost::shared_ptr<CReportSpecification>& pRptSpec);
};
