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
HRESULT CTestAgentImp::RunTest(LPCTSTR lpszResultsFile)
{
   std::_tofstream ofResults;
   ofResults.open(lpszResultsFile);
   if ( !ofResults )
   {
      return E_FAIL;
   }

   ofResults << _T("Test");

   return S_OK;
}