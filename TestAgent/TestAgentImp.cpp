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

// TestAgentImp.cpp : Implementation of CTestAgentImp
#include "stdafx.h"
#include "TestAgent.h"
#include "TestAgentImp.h"

#include <EAF\EAFAutoProgress.h>
#include <IFace\VersionInfo.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestAgentImp
CTestAgentImp::CTestAgentImp()
{
   m_pBroker = 0;
}

CTestAgentImp::~CTestAgentImp()
{
}

HRESULT CTestAgentImp::FinalConstruct()
{
   return S_OK;
}

void CTestAgentImp::FinalRelease()
{
}

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CTestAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CTestAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface( IID_IXBRTest, this);

   return S_OK;
};

STDMETHODIMP CTestAgentImp::Init()
{
   EAF_AGENT_INIT; // this macro defines pStatusCenter
   m_StatusGroupID = pStatusCenter->CreateStatusGroupID();

   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CTestAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CTestAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CTestAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_TestAgent;
   return S_OK;
}

STDMETHODIMP CTestAgentImp::ShutDown()
{
   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

//////////////////////////////////////////
// IXBRTest
HRESULT CTestAgentImp::RunTest(PierIDType pierID,LPCTSTR lpszResultsFile)
{
   std::_tofstream ofResults;
   ofResults.open(lpszResultsFile);
   if ( !ofResults )
   {
      return E_FAIL;
   }

   // create progress window
   GET_IFACE(IProgress,pProgress);
   CEAFAutoProgress ap(pProgress);

   CString strProcessID = GetProcessID();
   RunProductLoadActionsTest(ofResults,strProcessID,pierID);

   return S_OK;
}

CString CTestAgentImp::GetProcessID()
{
   // the process ID is going to be the version number
   GET_IFACE(IXBRVersionInfo,pVI);
   return pVI->GetVersion(true);
}

void CTestAgentImp::RunProductLoadActionsTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID)
{
   GET_IFACE(IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi( pPoi->GetXBeamPointsOfInterest(pierID,POI_GRID) );

   GET_IFACE(IXBRAnalysisResults,pResults);

   for ( int i = 0; i < 8; i++ )
   {
      xbrTypes::ProductForceType pfType = (xbrTypes::ProductForceType)i;

      CString strMomentTestID;
      strMomentTestID.Format(_T("7770%d01"),i);

      CString strShearTestID;
      strShearTestID.Format(_T("7770%d02"),i);

      BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
      {
         Float64 M = pResults->GetMoment(pierID,pfType,poi);
         os << lpszProcessID << _T(", ") << (LPCTSTR)strMomentTestID << _T(", ") << poi.GetDistFromStart() << _T(", ") << M << std::endl;

         sysSectionValue V = pResults->GetShear(pierID,pfType,poi);
         os << lpszProcessID << _T(", ") << (LPCTSTR)strShearTestID << _T(", ") << poi.GetDistFromStart() << _T(", ") << V.Right() << std::endl;
      }
   }
}
