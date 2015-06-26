
// DisplayObjectFactory.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayObjectFactory.h"

#include "mfcdual.h"
#include "SectionCut.h"
#include <WBFLDManip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayObjectFactory

CDisplayObjectFactory::CDisplayObjectFactory()
{
   ::CoCreateInstance(CLSID_DisplayObjectFactory,NULL,CLSCTX_ALL,IID_iDisplayObjectFactory,(void**)&m_Factory);
}


CDisplayObjectFactory::~CDisplayObjectFactory()
{
}

BEGIN_INTERFACE_MAP(CDisplayObjectFactory,CCmdTarget)
   INTERFACE_PART(CDisplayObjectFactory,IID_iDisplayObjectFactory,Factory)
END_INTERFACE_MAP()

DELEGATE_CUSTOM_INTERFACE(CDisplayObjectFactory,Factory);


/////////////////////////////////////////////////////////////////////////////
// CDisplayObjectFactory message handlers
STDMETHODIMP_(void) CDisplayObjectFactory::XFactory::Create(CLIPFORMAT cfFormat,COleDataObject* pDataObject,iDisplayObject** dispObj)
{
   METHOD_PROLOGUE(CDisplayObjectFactory,Factory);

   if ( cfFormat == CSectionCutDisplayImpl::ms_Format )
   {
      CComPtr<iPointDisplayObject> doSectionCut;
      ::CoCreateInstance(CLSID_PointDisplayObject,NULL,CLSCTX_ALL,IID_iPointDisplayObject,(void**)&doSectionCut);

      doSectionCut->SetSelectionType(stAll);

      CSectionCutDisplayImpl* pDisplayImpl = new CSectionCutDisplayImpl();
      CComPtr<iDrawPointStrategy> strategy;
      strategy.Attach((iDrawPointStrategy*)pDisplayImpl->GetInterface(&IID_iDrawPointStrategy));
      doSectionCut->SetDrawingStrategy(strategy);

      CComPtr<iDisplayObjectEvents> events;
      events.Attach((iDisplayObjectEvents*)pDisplayImpl->GetInterface(&IID_iDisplayObjectEvents));
      doSectionCut->RegisterEventSink(events.Detach());
      
      CComPtr<iDragData> dragData;
      dragData.Attach((iDragData*)pDisplayImpl->GetInterface(&IID_iDragData));
      CComQIPtr<iDraggable,&IID_iDraggable> draggable(doSectionCut);
      draggable->SetDragData(dragData.Detach());

      if ( pDataObject )
      {
         // Initialize from data object
         CComPtr<iDragDataSource> source;
         ::CoCreateInstance(CLSID_DragDataSource,NULL,CLSCTX_ALL,IID_iDragDataSource,(void**)&source);
         source->SetDataObject(pDataObject);

         // rebuild the display object from the data source
         draggable->OnDrop(source);
      }

      (*dispObj) = doSectionCut;
      (*dispObj)->AddRef();
   }
   else
   {
      pThis->m_Factory->Create(cfFormat,pDataObject,dispObj);
   }
}
