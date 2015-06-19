
#pragma once

#include <EAF\EAFDocTemplate.h>

class CXBeamRateDocTemplate : public CEAFDocTemplate
{
public:
   CXBeamRateDocTemplate(UINT nIDResource,
                      IEAFCommandCallback* pCallback,
                      CRuntimeClass* pDocClass,
                      CRuntimeClass* pFrameClass,
                      CRuntimeClass* pViewClass,
                      HMENU hSharedMenu = NULL,
                      int maxViewCount = -1);

   virtual CString GetTemplateGroupItemDescription(const CEAFTemplateItem* pItem) const;

   DECLARE_DYNAMIC(CXBeamRateDocTemplate)
};
