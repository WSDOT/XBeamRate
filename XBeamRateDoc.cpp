// XBeamRateDoc.cpp : implementation of the CXBeamRateDoc class
//

#include "stdafx.h"
#include "resource.h"
#include "XBeamRatePluginApp.h"
#include "XBeamRateDoc.h"

#include <EAF\EAFMainFrame.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc

IMPLEMENT_DYNCREATE(CXBeamRateDoc, CEAFDocument)

BEGIN_MESSAGE_MAP(CXBeamRateDoc, CEAFDocument)
	//{{AFX_MSG_MAP(CXBeamRateDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc construction/destruction

CXBeamRateDoc::CXBeamRateDoc()
{
	// TODO: add one-time construction code here

}

CXBeamRateDoc::~CXBeamRateDoc()
{
}

BOOL CXBeamRateDoc::OnNewDocument()
{
	if (!CEAFDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

HRESULT CXBeamRateDoc::WriteTheDocument(IStructuredSave* pStrSave)
{
   return S_OK;
}

HRESULT CXBeamRateDoc::LoadTheDocument(IStructuredLoad* pStrLoad)
{
   return S_OK;
}

CString CXBeamRateDoc::GetToolbarSectionName()
{
   return "XBeamRate";
}

BOOL CXBeamRateDoc::GetStatusBarMessageString(UINT nID,CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return __super::GetStatusBarMessageString(nID,rMessage);
}

BOOL CXBeamRateDoc::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return __super::GetToolTipMessageString(nID,rMessage);
}

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc diagnostics

#ifdef _DEBUG
void CXBeamRateDoc::AssertValid() const
{
	CEAFDocument::AssertValid();
}

void CXBeamRateDoc::Dump(CDumpContext& dc) const
{
	CEAFDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateDoc commands
