
#include "stdafx.h"
#include "resource.h"
#include "XBeamRateDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXBeamRateDocTemplate,CEAFDocTemplate)

CXBeamRateDocTemplate::CXBeamRateDocTemplate(UINT nIDResource,
                                       IEAFCommandCallback* pCallback,
                                       CRuntimeClass* pDocClass,
                                       CRuntimeClass* pFrameClass,
                                       CRuntimeClass* pViewClass,
                                       HMENU hSharedMenu,
                                       int maxViewCount)
: CEAFDocTemplate(nIDResource,pCallback,pDocClass,pFrameClass,pViewClass,hSharedMenu,maxViewCount)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CString strDocName;
   GetDocString(strDocName,CDocTemplate::docName);

   HICON hIcon = AfxGetApp()->LoadIcon(IDR_XBEAMRATE);
   m_TemplateGroup.AddItem( new CEAFTemplateItem(this,strDocName,NULL,hIcon) );
   m_TemplateGroup.SetIcon(hIcon);
}

CString CXBeamRateDocTemplate::GetTemplateGroupItemDescription(const CEAFTemplateItem* pItem) const
{
   CString strDescription(_T("Create a new XBeam Rate project (Cross Beam Load Rating). "));
   return strDescription;
}
