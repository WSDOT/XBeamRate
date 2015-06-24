
#pragma once
#include "PGSComponentInfo.h"

class ATL_NO_VTABLE CPGSuperComponentInfo : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<CPGSuperComponentInfo, &CLSID_PGSuperComponentInfo>,
   public IPGSuperComponentInfo
{
public:
   CPGSuperComponentInfo()
   {
   }

DECLARE_REGISTRY_RESOURCEID(IDR_PGSUPERCOMPONENTINFO)
DECLARE_CLASSFACTORY_SINGLETON(CPGSuperComponentInfo)

BEGIN_COM_MAP(CPGSuperComponentInfo)
   COM_INTERFACE_ENTRY(IPGSuperComponentInfo)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CPGSuperComponentInfo)
END_CONNECTION_POINT_MAP()

   HRESULT FinalConstruct();
   void FinalRelease();

// IPGSuperComponentInfo
public:
   virtual BOOL Init(CPGSuperDoc* pDoc);
   virtual void Terminate();
   virtual CString GetName();
   virtual CString GetDescription();
   virtual HICON GetIcon();
   virtual bool HasMoreInfo();
   virtual void OnMoreInfo();
};

OBJECT_ENTRY_AUTO(__uuidof(PGSuperComponentInfo), CPGSuperComponentInfo)
