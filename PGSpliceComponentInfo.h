
#pragma once
#include "PGSComponentInfo.h"

class ATL_NO_VTABLE CPGSpliceComponentInfo : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<CPGSpliceComponentInfo, &CLSID_PGSpliceComponentInfo>,
   public IPGSpliceComponentInfo
{
public:
   CPGSpliceComponentInfo()
   {
   }

DECLARE_REGISTRY_RESOURCEID(IDR_PGSPLICECOMPONENTINFO)
DECLARE_CLASSFACTORY_SINGLETON(CPGSpliceComponentInfo)

BEGIN_COM_MAP(CPGSpliceComponentInfo)
   COM_INTERFACE_ENTRY(IPGSpliceComponentInfo)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CPGSpliceComponentInfo)
END_CONNECTION_POINT_MAP()

   HRESULT FinalConstruct();
   void FinalRelease();

// IPGSpliceComponentInfo
public:
   virtual BOOL Init(CPGSpliceDoc* pDoc);
   virtual void Terminate();
   virtual CString GetName();
   virtual CString GetDescription();
   virtual HICON GetIcon();
   virtual bool HasMoreInfo();
   virtual void OnMoreInfo();
};

OBJECT_ENTRY_AUTO(__uuidof(PGSpliceComponentInfo), CPGSpliceComponentInfo)
