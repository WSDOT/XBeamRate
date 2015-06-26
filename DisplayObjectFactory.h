
#pragma once

// DisplayObjectFactory.h : header file
//

#include <DManip\DManip.h>

/////////////////////////////////////////////////////////////////////////////
// CDisplayObjectFactory command target

class CDisplayObjectFactory : public CCmdTarget
{
public:
	CDisplayObjectFactory();

// Attributes
public:

// Operations
public:


// Implementation
protected:
	virtual ~CDisplayObjectFactory();

   DECLARE_INTERFACE_MAP()

   // iDisplayObjectFactory Implementation
   BEGIN_INTERFACE_PART(Factory,iDisplayObjectFactory)
      STDMETHOD_(void,Create)(CLIPFORMAT cfFormat,COleDataObject* pDataObject,iDisplayObject** dispObj);
   END_INTERFACE_PART(Factory)

private:
   CComPtr<iDisplayObjectFactory> m_Factory;
};
