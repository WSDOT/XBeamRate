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

#pragma once

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>
#include <IFace\XBeamRateAgent.h>
#include <EAF\EAFDisplayUnits.h>

class CXBeamRateDoc;
struct IBroker;

// {E5744BFC-68DE-4c00-93BA-7643A1167ABB}
DEFINE_GUID(CLSID_XBeamRateDocProxyAgent, 
0xe5744bfc, 0x68de, 0x4c00, 0x93, 0xba, 0x76, 0x43, 0xa1, 0x16, 0x7a, 0xbb);

/*****************************************************************************
CLASS 
   CXBeamRateDocProxyAgent

   Proxy agent for CXBeamRate document.

   Instances of this object allow the CDocument class to be plugged into the
   Agent-Broker architecture.
*****************************************************************************/

class CXBeamRateDocProxyAgent :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<CXBeamRateDocProxyAgent,&CLSID_XBeamRateDocProxyAgent>,
	public IConnectionPointContainerImpl<CXBeamRateDocProxyAgent>,
   //public CProxyIExtendUIEventSink<CXBeamRateDocProxyAgent>,
   public IAgentEx,
   public IAgentUIIntegration,
   public IXBRProjectEventSink,
   public IXBeamRate,
   public IEAFDisplayUnitsEventSink,
   public IXBRUIEvents
   //public IVersionInfo,
{
public:
   CXBeamRateDocProxyAgent();
   ~CXBeamRateDocProxyAgent();

BEGIN_COM_MAP(CXBeamRateDocProxyAgent)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
   COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IAgentUIIntegration)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
   COM_INTERFACE_ENTRY(IXBeamRate)
   COM_INTERFACE_ENTRY(IEAFDisplayUnitsEventSink)
   COM_INTERFACE_ENTRY(IXBRUIEvents)
   //COM_INTERFACE_ENTRY(IVersionInfo)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXBeamRateDocProxyAgent)
   //CONNECTION_POINT_ENTRY( IID_IExtendUIEventSink )
END_CONNECTION_POINT_MAP()

public:
   void SetDocument(CXBeamRateDoc* pDoc);
   //void OnStatusChanged();

// IAgentEx
public:
   STDMETHOD(SetBroker)(/*[in]*/ IBroker* pBroker);
	STDMETHOD(RegInterfaces)();
	STDMETHOD(Init)();
	STDMETHOD(Init2)();
	STDMETHOD(Reset)();
	STDMETHOD(ShutDown)();
   STDMETHOD(GetClassID)(CLSID* pCLSID);

// IAgentUIIntegration
public:
   STDMETHOD(IntegrateWithUI)(BOOL bIntegrate);

// IXBeamRate
public:
   virtual void GetUnitServer(IUnitServer** ppUnitServer);

// IXBRProjectEventSink
public:
   virtual HRESULT OnProjectChanged();

// IEAFDisplayUnitsEventSink
public:
   virtual HRESULT OnUnitsChanged(eafTypes::UnitMode newUnitsMode);

// IXBRUIEvents
public:
   virtual void HoldEvents(bool bHold=true);
   virtual void FirePendingEvents();
   virtual void CancelPendingEvents();
   virtual void FireEvent(CView* pSender = NULL,LPARAM lHint = 0,boost::shared_ptr<CObject> pHint = boost::shared_ptr<CObject>());


//// IVersionInfo
//public:
//   virtual CString GetVersionString(bool bIncludeBuildNumber=false);
//   virtual CString GetVersion(bool bIncludeBuildNumber=false);

// IViews ??? 
   virtual void CreateReportView(CollectionIndexType rptIdx,bool bPromptForSpec=true);
   virtual void CreateGraphView(CollectionIndexType graphIdx);

private:
   DECLARE_EAF_AGENT_DATA;

   void AdviseEventSinks();
   void UnadviseEventSinks();
   DWORD m_dwProjectCookie;
   DWORD m_dwDisplayUnitsCookie;

   void CreateToolBars();
   void RemoveToolBars();
   UINT m_StdToolBarID;

   void RegisterViews();
   void UnregisterViews();
   long m_ReportViewKey;
   long m_GraphingViewKey;
   
   CXBeamRateDoc* m_pMyDocument;

   int m_EventHoldCount;
   bool m_bFiringEvents;
   struct UIEvent
   {
      CView* pSender;
      LPARAM lHint;
      boost::shared_ptr<CObject> pHint;
   };
   std::vector<UIEvent> m_UIEvents;
};

