#include "StdAfx.h"
#include "XBeamRateReportSpecification.h"

CXBeamRateReportSpecification::CXBeamRateReportSpecification(LPCTSTR strReportName,IBroker* pBroker,PierIDType pierID) :
CEAFBrokerReportSpecification(strReportName,pBroker),
m_PierID(pierID)
{
}

CXBeamRateReportSpecification::~CXBeamRateReportSpecification(void)
{
}

PierIDType CXBeamRateReportSpecification::GetPierID()
{
   return m_PierID;
}
