
#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePlugin_i.h"
#include "PGSuperComponentInfo.h"
#include <MFCTools\VersionInfo.h>

HRESULT CPGSuperComponentInfo::FinalConstruct()
{
   return S_OK;
}

void CPGSuperComponentInfo::FinalRelease()
{
}

BOOL CPGSuperComponentInfo::Init(CPGSuperDoc* pDoc)
{
   return TRUE;
}

void CPGSuperComponentInfo::Terminate()
{
}

CString CPGSuperComponentInfo::GetName()
{
   return _T("XBeam Rate Extensions");
}

CString CPGSuperComponentInfo::GetDescription()
{
   CString strDesc;
   strDesc.Format(_T("Adds cross beam load rating capabilties to PGSuper"));
   return strDesc;
}

HICON CPGSuperComponentInfo::GetIcon()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return AfxGetApp()->LoadIcon(IDR_XBEAMRATE);
}

bool CPGSuperComponentInfo::HasMoreInfo()
{
   return false;
}

void CPGSuperComponentInfo::OnMoreInfo()
{
}
