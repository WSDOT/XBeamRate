///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
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

// XBeamRateAgent.h : Declaration of the CXBeamRateAgent

// This agent is a PGSuper/PGSplice extension agent.

#pragma once
#include "resource.h"       // main symbols
#include "XBeamRatePlugin_i.h"
#include <EAF\EAFInterfaceCache.h>
#include <IFace\ExtendUI.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CXBeamRateAgent

class ATL_NO_VTABLE CXBeamRateAgent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXBeamRateAgent, &CLSID_XBeamRateAgent>,
	public IAgentEx,
   public IAgentPersist,
   public IAgentUIIntegration,
   public IAgentReportingIntegration,
   public IAgentGraphingIntegration,
   //public IEditBridgeCallback,
   public IEditPierCallback//, // not a COM interface
   //public IEditTemporarySupportCallback,
   //public IEditSpanCallback,  // not a COM interface
   //public IEditSegmentCallback,
   //public IEditClosureJointCallback,
   //public IEditSplicedGirderCallback,
   //public IEditGirderCallback,
   //public IExtendUIEventSink
{
public:
   CXBeamRateAgent()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_XBEAMRATEAGENT)

DECLARE_NOT_AGGREGATABLE(CXBeamRateAgent)

BEGIN_COM_MAP(CXBeamRateAgent)
	COM_INTERFACE_ENTRY(IAgent)
	COM_INTERFACE_ENTRY(IAgentEx)
	COM_INTERFACE_ENTRY(IAgentPersist)
   COM_INTERFACE_ENTRY(IAgentUIIntegration)
   COM_INTERFACE_ENTRY(IAgentReportingIntegration)
   COM_INTERFACE_ENTRY(IAgentGraphingIntegration)
   //COM_INTERFACE_ENTRY(IExtendUIEventSink)
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
   STDMETHOD(SetBroker)(IBroker* pBroker);
   STDMETHOD(RegInterfaces)();
   STDMETHOD(Init)();
   STDMETHOD(Init2)();
   STDMETHOD(Reset)();
   STDMETHOD(ShutDown)();
   STDMETHOD(GetClassID)(CLSID* pCLSID);

// IAgentPersist
public:
   STDMETHOD(Load)(/*[in]*/ IStructuredLoad* pStrLoad);
   STDMETHOD(Save)(/*[in]*/ IStructuredSave* pStrSave);

// IAgentUIIntegration
public:
   STDMETHOD(IntegrateWithUI)(BOOL bIntegrate);

// IAgentReportingIntegration
public:
   STDMETHOD(IntegrateWithReporting)(BOOL bIntegrate);

// IAgentGraphingIntegration
public:
   STDMETHOD(IntegrateWithGraphing)(BOOL bIntegrate);

//
//// IEAFCommandCallback
//public:
//   virtual BOOL OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
//   virtual BOOL GetStatusBarMessageString(UINT nID, CString& rMessage) const;
//   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const;
//
//
//// IEditBridgeCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditBridgeData* pBridgeData);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditBridgeData* pBridgeData);
//   virtual void EditPier_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pPierPropertyPage);
//   virtual void EditTemporarySupport_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pTempSupportPropertyPage);
//   virtual void EditSpan_OnOK(CPropertyPage* pBridgePropertyPage,CPropertyPage* pSpanPropertyPage);

// IEditPierCallback
public:
   virtual CPropertyPage* CreatePropertyPage(IEditPierData* pEditPierData);
   virtual CPropertyPage* CreatePropertyPage(IEditPierData* pEditPierData,CPropertyPage* pBridgePropertyPage);
   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditPierData* pEditPierData);
   virtual IDType GetEditBridgeCallbackID();

//// IEditTemporarySupportCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditTemporarySupportData* pEditTemporarySupportData);
//   virtual CPropertyPage* CreatePropertyPage(IEditTemporarySupportData* pEditTemporarySupportData,CPropertyPage* pBridgePropertyPage);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditTemporarySupportData* pEditTemporarySupportData);
//   //virtual IDType GetEditBridgeCallbackID();
//
//// IEditSpanCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditSpanData* pEditSpanData);
//   virtual CPropertyPage* CreatePropertyPage(IEditSpanData* pEditSpanData,CPropertyPage* pBridgePropertyPage);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditSpanData* pEditSpanData);
//   //virtual IDType GetEditBridgeCallbackID();
//
//// IEditSegmentCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditSegmentData* pSegmentData);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditSegmentData* pSegmentData);
//   virtual IDType GetEditSplicedGirderCallbackID();
//   virtual CPropertyPage* CreatePropertyPage(IEditSegmentData* pEditSegmentData,CPropertyPage* pSplicedGirderPropertyPage);
//
//// IEditClosureJointCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditClosureJointData* pClosureJointData);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditClosureJointData* pClosureJointData);
//   //virtual IDType GetEditSplicedGirderCallbackID();
//   virtual CPropertyPage* CreatePropertyPage(IEditClosureJointData* pEditClosureJointData,CPropertyPage* pSplicedGirderPropertyPage);
//
//// IEditSplicedGirderCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditSplicedGirderData* pGirderData);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditSplicedGirderData* pGirderData);
//   virtual void EditSegment_OnOK(CPropertyPage* pSplicedGirderPropertyPage,CPropertyPage* pSegmentPropertyPage);
//   virtual void EditClosureJoint_OnOK(CPropertyPage* pSplicedGirderPropertyPage,CPropertyPage* pClosureJointPropertyPage);
//
//// IEditGirderCallback
//public:
//   virtual CPropertyPage* CreatePropertyPage(IEditGirderData* pGirderData);
//   virtual txnTransaction* OnOK(CPropertyPage* pPage,IEditGirderData* pGirderData);
//
//
//// IExtendUIEventSink
//public:
//   virtual HRESULT OnHintsReset();

private:
   void RegisterUIExtensions();
   void UnregisterUIExtensions();
   //IDType m_EditBridgeCallbackID;
   IDType m_EditPierCallbackID;
   //IDType m_EditTemporarySupportCallbackID;
   //IDType m_EditSpanCallbackID;
   //IDType m_EditSegmentCallbackID;
   //IDType m_EditClosureJointCallbackID;
   //IDType m_EditSplicedGirderCallbackID;
   //IDType m_EditGirderCallbackID;
};

OBJECT_ENTRY_AUTO(__uuidof(XBeamRateAgent), CXBeamRateAgent)
