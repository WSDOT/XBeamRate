///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2021  Washington State Department of Transportation
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

// XBeamRateAgent.h : Declaration of the CXBeamRateAgent

// This agent is a PGSuper/PGSplice extension agent.

#pragma once
#include "resource.h"       // main symbols
#include "CLSID.h"
#include "ReinforcementPageParent.h"

#include <EAF\EAFInterfaceCache.h>
#include <EAF\EAFUIIntegration.h>

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>

#include <IFace\ExtendUI.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>

#include <IFace\ViewEvents.h>
#include <IFace\VersionInfo.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CXBeamRateAgent;

class CMyCommandTarget : public CCmdTarget, public IBridgePlanViewEventCallback
{
public:
   CMyCommandTarget(CXBeamRateAgent* pMyAgent) {m_pMyAgent = pMyAgent;}

   afx_msg void OnViewPier();
   afx_msg void OnExportPier();
   afx_msg void OnPierCommandUpdate(CCmdUI* pCmdUI);


   // IBridgePlanViewEventCallback
   virtual void OnBackgroundContextMenu(CEAFMenu* pMenu) override;
   virtual void OnPierContextMenu(PierIndexType pierIdx,CEAFMenu* pMenu) override;
   virtual void OnSpanContextMenu(SpanIndexType spanIdx,CEAFMenu* pMenu) override;
   virtual void OnDeckContextMenu(CEAFMenu* pMenu) override;
   virtual void OnAlignmentContextMenu(CEAFMenu* pMenu) override;
   virtual void OnSectionCutContextMenu(CEAFMenu* pMenu) override;
   virtual void OnGirderContextMenu(const CGirderKey& girderKey,CEAFMenu* pMenu) override;
   virtual void OnTemporarySupportContextMenu(SupportIDType tsID,CEAFMenu* pMenu) override;
   virtual void OnGirderSegmentContextMenu(const CSegmentKey& segmentKey,CEAFMenu* pMenu) override;
   virtual void OnClosureJointContextMenu(const CSegmentKey& closureKey,CEAFMenu* pMenu) override;

   CXBeamRateAgent* m_pMyAgent;

   DECLARE_MESSAGE_MAP()
};

// CXBeamRateAgent

class ATL_NO_VTABLE CXBeamRateAgent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXBeamRateAgent, &CLSID_XBeamRateAgent>,
	public IAgentEx,
   //public IAgentPersist,
   public IXBeamRateAgent,
   public IAgentUIIntegration,
   public IAgentDocumentationIntegration,
   public IEditPierCallback, // not a COM interface
   public IEditLoadRatingOptionsCallback, // not a COM interface
   public IProjectPropertiesEventSink,
   public IXBRProjectEventSink,
   public IXBeamRate,
   public IXBRVersionInfo,
   public IEAFCommandCallback,
   public IEAFProcessCommandLine
{
public:
   CXBeamRateAgent() :
      m_CommandTarget(this)
	{
      m_PierViewKey = -1;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_XBEAMRATEAGENT)

DECLARE_NOT_AGGREGATABLE(CXBeamRateAgent)

BEGIN_COM_MAP(CXBeamRateAgent)
	COM_INTERFACE_ENTRY(IAgent)
	COM_INTERFACE_ENTRY(IAgentEx)
	//COM_INTERFACE_ENTRY(IAgentPersist)
   COM_INTERFACE_ENTRY(IXBeamRateAgent)
   COM_INTERFACE_ENTRY(IAgentUIIntegration)
   COM_INTERFACE_ENTRY(IAgentDocumentationIntegration)
   //COM_INTERFACE_ENTRY(IExtendUIEventSink)
   COM_INTERFACE_ENTRY(IProjectPropertiesEventSink)
   COM_INTERFACE_ENTRY(IXBRProjectEventSink)
   COM_INTERFACE_ENTRY(IXBeamRate)
   COM_INTERFACE_ENTRY(IXBRVersionInfo)
   COM_INTERFACE_ENTRY(IEAFCommandCallback)
   COM_INTERFACE_ENTRY(IEAFProcessCommandLine)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
   DECLARE_EAF_AGENT_DATA;

	HRESULT FinalConstruct();
	void FinalRelease()
	{
	}

   void RegisterReports();
   void RegisterGraphs();

// IAgentEx
public:
   STDMETHOD(SetBroker)(IBroker* pBroker) override;
   STDMETHOD(RegInterfaces)() override;
   STDMETHOD(Init)() override;
   STDMETHOD(Init2)() override;
   STDMETHOD(Reset)() override;
   STDMETHOD(ShutDown)() override;
   STDMETHOD(GetClassID)(CLSID* pCLSID) override;

//// IAgentPersist
//public:
//   STDMETHOD(Load)(/*[in]*/ IStructuredLoad* pStrLoad) override;
//   STDMETHOD(Save)(/*[in]*/ IStructuredSave* pStrSave) override;

// IXBeamRateAgent
public:
   virtual bool IsExtendingPGSuper() override;

// IXBeamRate
public:
   virtual void GetUnitServer(IUnitServer** ppUnitServer) override;

// IXBRVersionInfo
public:
   virtual CString GetVersionString(bool bIncludeBuildNumber=false) override;
   virtual CString GetVersion(bool bIncludeBuildNumber=false) override;

// IAgentUIIntegration
public:
   STDMETHOD(IntegrateWithUI)(BOOL bIntegrate) override;

// IAgentDocumentationIntegration
public:
   STDMETHOD(GetDocumentationSetName)(BSTR* pbstrName) override;
   STDMETHOD(LoadDocumentationMap)() override;
   STDMETHOD(GetDocumentLocation)(UINT nHID,BSTR* pbstrURL) override;

// IEditPierCallback
public:
   virtual CPropertyPage* CreatePropertyPage(IEditPierData* pEditPierData) override;
   virtual CPropertyPage* CreatePropertyPage(IEditPierData* pEditPierData,CPropertyPage* pBridgePropertyPage) override;
   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditPierData* pEditPierData) override;
   virtual IDType GetEditBridgeCallbackID() override;

// IEditLoadRatingOptionsCallback
public:
   virtual CPropertyPage* CreatePropertyPage(IEditLoadRatingOptions* pLoadRatingOptions) override;
   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditLoadRatingOptions* pLoadRatingOptions) override;

// IProjectPropertiesEventSink
public:
   virtual HRESULT OnProjectPropertiesChanged() override;

// IXBRProjectEventSink
public:
   virtual HRESULT OnProjectChanged() override;

// IEAFCommandCallback
public:
   virtual BOOL OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo) override;
   virtual BOOL GetStatusBarMessageString(UINT nID, CString& rMessage) const override;
   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const override;

// IEAFProcessCommandLine
public:
   virtual BOOL ProcessCommandLineOptions(CEAFCommandLineInfo& cmdInfo) override;

private:
   DWORD m_dwProjectPropertiesCookie;
   DWORD m_dwProjectCookie;

   CReinforcementPageParent m_ReinforcementPageParent;

   CMyCommandTarget m_CommandTarget;
   friend CMyCommandTarget;
   IDType m_BridgePlanViewCallbackID;

   CString GetDocumentationURL();
   std::map<UINT,CString> m_HelpTopics;

   void CreateMenus();
   void RemoveMenus();

   void CreateToolbar();
   void RemoveToolbar();

   void RegisterViews();
   void UnregisterViews();
   void CreatePierView();
   long m_PierViewKey;

   void RegisterUIExtensions();
   void UnregisterUIExtensions();
   IDType m_EditPierCallbackID;
   IDType m_EditLoadRatingOptionsCallbackID;
};

OBJECT_ENTRY_AUTO(__uuidof(XBeamRateAgent), CXBeamRateAgent)
