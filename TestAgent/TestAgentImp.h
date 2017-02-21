///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2017  Washington State Department of Transportation
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

// TestAgentImp.h : Declaration of the CTestAgentImp

// This agent is responsible for creating structural analysis models
// and providing analysis results

#pragma once

#include "resource.h"       // main symbols
#include <TestAgent.h>
#include "TestAgentCLSID.h"

#include <EAF\EAFInterfaceCache.h>

class xbrPointOfInterest;

/////////////////////////////////////////////////////////////////////////////
// CTestAgentImp
class ATL_NO_VTABLE CTestAgentImp : 
	public CComObjectRootEx<CComSingleThreadModel>,
   //public CComRefCountTracer<CTestAgentImp,CComObjectRootEx<CComSingleThreadModel> >,
	public CComCoClass<CTestAgentImp, &CLSID_TestAgent>,
	//public IConnectionPointContainerImpl<CTestAgentImp>, // needed if we implement a connection point
   //public CProxyIProjectEventSink<CTestAgentImp>,// needed if we implement a connection point
   public IAgentEx,
   public IXBRTest
{  
public:
	CTestAgentImp(); 
   virtual ~CTestAgentImp();

   DECLARE_PROTECT_FINAL_CONSTRUCT();

   HRESULT FinalConstruct();
   void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_TESTAGENT)

BEGIN_COM_MAP(CTestAgentImp)
	COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IXBRTest)
END_COM_MAP()

// IAgentEx
public:
	STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker) override;
   STDMETHOD(RegInterfaces)() override;
	STDMETHOD(Init)() override;
	STDMETHOD(Reset)() override;
	STDMETHOD(ShutDown)() override;
   STDMETHOD(Init2)() override;
   STDMETHOD(GetClassID)(CLSID* pCLSID) override;

// IXBRTest
public:
   virtual HRESULT RunTest(PierIDType pierID,LPCTSTR lpszResultsFile) override;

private:
   DECLARE_EAF_AGENT_DATA;

   CString GetProcessID();

   void RunProductLoadActionsTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunCombinedLoadActionsTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunLiveLoadActionsTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunLimitStateLoadActionsTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunMomentCapacityTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunShearCapacityTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunCrackedSectionTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi);
   void RunLoadRatingTest(std::_tostream& os,LPCTSTR lpszProcessID,PierIDType pierID);
};

OBJECT_ENTRY_AUTO(CLSID_TestAgent, CTestAgentImp)

