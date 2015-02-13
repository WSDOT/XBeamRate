#pragma once

#include <EAF\EAFInterfaceCache.h>
#include <IFace\Project.h>

using namespace XBR;

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
   public IProjectEventSink
   //public IEAFDisplayUnitsEventSink,
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
   COM_INTERFACE_ENTRY(IProjectEventSink)
   //COM_INTERFACE_ENTRY(IEAFDisplayUnitsEventSink)
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


// IProjectEventSink
public:
   virtual HRESULT OnProjectChanged();

//// IEAFDisplayUnitsEventSink
//public:
//   virtual HRESULT OnUnitsChanged(eafTypes::UnitMode newUnitsMode);


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
   //DWORD m_dwDisplayUnitsCookie;

   void RegisterViews();
   void UnregisterViews();
   long m_ReportViewKey;
   long m_GraphingViewKey;
   
   CXBeamRateDoc* m_pMyDocument;
};

