
#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePlugin_i.h"
#include "PGSpliceComponentInfo.h"
#include <MFCTools\VersionInfo.h>

HRESULT CPGSpliceComponentInfo::FinalConstruct()
{
   return S_OK;
}

void CPGSpliceComponentInfo::FinalRelease()
{
}

BOOL CPGSpliceComponentInfo::Init(CPGSpliceDoc* pDoc)
{
   return TRUE;
}

void CPGSpliceComponentInfo::Terminate()
{
}

CString CPGSpliceComponentInfo::GetName()
{
   return _T("XBeam Rate Extensions");
}

CString CPGSpliceComponentInfo::GetDescription()
{
   CString strDesc;
   strDesc.Format(_T("Adds cross beam load rating capabilties to PGSplice"));
   return strDesc;
}

HICON CPGSpliceComponentInfo::GetIcon()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return AfxGetApp()->LoadIcon(IDR_XBEAMRATE);
}

bool CPGSpliceComponentInfo::HasMoreInfo()
{
   return false;
}

void CPGSpliceComponentInfo::OnMoreInfo()
{
}
