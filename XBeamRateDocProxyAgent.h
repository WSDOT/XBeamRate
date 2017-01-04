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

#pragma once

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>
#include <IFace\XBeamRateAgent.h>
#include <IFace\RatingSpecification.h>
#include <IFace\VersionInfo.h>
#include <EAF\EAFDisplayUnits.h>

class CXBeamRateDoc;
struct IBroker;

// {0AA45B23-96C9-431c-BBC9-1D8CCC2BE5FC}
DEFINE_GUID(CLSID_XBeamRateDocProxyAgent, 
0xaa45b23, 0x96c9, 0x431c, 0xbb, 0xc9, 0x1d, 0x8c, 0xcc, 0x2b, 0xe5, 0xfc);

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
   public IXBeamRate,
   public IXBRUIEvents,
   public IXBRProjectEventSink,
   public IXBRRatingSpecificationEventSink,
   public IEAFDisplayUnitsEventSink,
   public IXBRVersionInfo,
   public IXBRViews,
   public IXBREditByUI
{
public:
   CXBeamRateDocProxyAgent();
   ~CXBeamRateDocProxyAgent();

BEGIN_COM_MAP(CXBeamRateDocProxyAgent)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
   COM_INTERFACE_ENTRY(IAgent)
   COM_INTERFACE_ENTRY(IAgentEx)
   COM_INTERFACE_ENTRY(IAgentUIIntegration)
   COM_INTERFACE_ENTRY(IXBeamRate)
   COM_INTERFACE_ENTRY(IXBRUIEvents)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
   COM_INTERFACE_ENTRY(IXBRRatingSpecificationEventSink)
   COM_INTERFACE_ENTRY(IEAFDisplayUnitsEventSink)
   COM_INTERFACE_ENTRY(IXBRVersionInfo)
   COM_INTERFACE_ENTRY(IXBRViews)
   COM_INTERFACE_ENTRY(IXBREditByUI)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXBeamRateDocProxyAgent)
   //CONNECTION_POINT_ENTRY( IID_IExtendUIEventSink )
END_CONNECTION_POINT_MAP()

public:
   void SetDocument(CXBeamRateDoc* pDoc);
   //void OnStatusChanged();
   void OnResetHints();

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

// IXBRRatingSpecificationEventSink
public:
   virtual HRESULT OnRatingSpecificationChanged();

// IEAFDisplayUnitsEventSink
public:
   virtual HRESULT OnUnitsChanging();
   virtual HRESULT OnUnitsChanged(eafTypes::UnitMode newUnitsMode);

// IXBRUIEvents
public:
   virtual void HoldEvents(bool bHold=true);
   virtual void FirePendingEvents();
   virtual void CancelPendingEvents();
   virtual void FireEvent(CView* pSender = NULL,LPARAM lHint = 0,boost::shared_ptr<CObject> pHint = boost::shared_ptr<CObject>());

// IXBRVersionInfo
public:
   virtual CString GetVersionString(bool bIncludeBuildNumber=false);
   virtual CString GetVersion(bool bIncludeBuildNumber=false);

// IXBRViews
public:
   virtual void CreateReportView(CollectionIndexType rptIdx,BOOL bPromptForSpec=TRUE);
   virtual void CreateGraphView(CollectionIndexType graphIdx);
   virtual void CreatePierView();

// IXBREditByUI
public:
   virtual UINT GetStdToolBarID();

private:
   DECLARE_EAF_AGENT_DATA;

   void AdviseEventSinks();
   void UnadviseEventSinks();
   DWORD m_dwProjectCookie;
   DWORD m_dwRatingSpecCookie;
   DWORD m_dwDisplayUnitsCookie;

   void CreateToolBars();
   void RemoveToolBars();
   UINT m_StdToolBarID;

   void CreateAcceleratorKeys();
   void RemoveAcceleratorKeys();

   void CreateStatusBar();
   void ResetStatusBar();

   void RegisterViews();
   void UnregisterViews();
   long m_ReportViewKey;
   long m_GraphingViewKey;
   long m_PierViewKey;
   
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

