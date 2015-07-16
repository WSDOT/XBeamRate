
#pragma once

#include <EAF\EAFBrokerReportSpecification.h>

class CXBeamRateReportSpecification :
   public CEAFBrokerReportSpecification
{
public:
   CXBeamRateReportSpecification(LPCTSTR strReportName,IBroker* pBroker,PierIDType pierID);
   ~CXBeamRateReportSpecification(void);

   PierIDType GetPierID();

protected:
   PierIDType m_PierID;
};

