// Plugin.h : Declaration of the CXBeamRateAppPlugin

#ifndef __PLUGIN_H_
#define __PLUGIN_H_

#include "resource.h"       // main symbols
#include "XBeamRatePlugin_i.h"
#include <EAF\EAFAppPlugIn.h>
#include <WBFLUnitServer.h>

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateAppPlugin
class ATL_NO_VTABLE CXBeamRateAppPlugin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXBeamRateAppPlugin, &CLSID_XBeamRateAppPlugin>,
	public IEAFAppPlugin
{
public:
	CXBeamRateAppPlugin()
	{
	}

   HRESULT FinalConstruct();
   void FinalRelease();

   LPCTSTR GetAppName() { return _T("XBeamRate"); }

   void GetAppUnitSystem(IAppUnitSystem** ppAppUnitSystem);

DECLARE_REGISTRY_RESOURCEID(IDR_XBEAMRATEAPP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CXBeamRateAppPlugin)
	COM_INTERFACE_ENTRY(IEAFAppPlugin)
END_COM_MAP()

// IEAFAppPlugin
public:
   virtual BOOL Init(CEAFApp* pParent);
   virtual void Terminate();
   virtual void IntegrateWithUI(BOOL bIntegrate);
   virtual std::vector<CEAFDocTemplate*> CreateDocTemplates();
   virtual HMENU GetSharedMenuHandle();
   virtual UINT GetDocumentResourceID();
   virtual CString GetName();

private:
   CComPtr<IAppUnitSystem> m_AppUnitSystem;
};

#endif //__PLUGIN_H_
