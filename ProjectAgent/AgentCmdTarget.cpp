#include "stdafx.h"
#include "resource.h"
#include "AgentCmdTarget.h"

#include <MFCTools\Prompts.h>
#include <IFace\Project.h>
#include <EAF\EAFTransactions.h>
#include <txnEditProject.h>
#include <txnEditPier.h>

#include "PierDlg.h"

BEGIN_MESSAGE_MAP(CAgentCmdTarget,CCmdTarget)
   ON_COMMAND(ID_EDIT_PROJECT_NAME,OnEditProjectName)
   ON_COMMAND(ID_EDIT_PIER, &CAgentCmdTarget::OnEditPier)
END_MESSAGE_MAP()

CAgentCmdTarget::CAgentCmdTarget()
{
}

void CAgentCmdTarget::Init(IBroker* pBroker)
{
   m_pBroker = pBroker;
}

void CAgentCmdTarget::OnEditProjectName()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CString strOldProjectName, strNewProjectName;
   GET_IFACE(IXBRProject,pProject);
   strOldProjectName = pProject->GetProjectName();

   if ( AfxQuestion(_T("Project Name"),_T("Enter project name"),strOldProjectName,strNewProjectName) )
   {
      txnEditProject txn(strOldProjectName,strNewProjectName);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}

void CAgentCmdTarget::OnEditPier()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IXBRProject,pProject);
   CPierDlg dlg(_T("Edit Pier"));
   dlg.m_LayoutPage.m_LeftOverhang  = pProject->GetLeftOverhang();
   dlg.m_LayoutPage.m_RightOverhang = pProject->GetRightOverhang();
   dlg.m_LayoutPage.m_nColumns = pProject->GetColumnCount();
   dlg.m_LayoutPage.m_ColumnHeight = pProject->GetColumnHeight(0);
   dlg.m_LayoutPage.m_ColumnSpacing = pProject->GetSpacing(0);
   if ( dlg.DoModal() == IDOK )
   {
      txnEditPier txn(pProject->GetLeftOverhang(),pProject->GetRightOverhang(),pProject->GetColumnCount(),pProject->GetColumnHeight(0),pProject->GetSpacing(0),
                      dlg.m_LayoutPage.m_LeftOverhang,dlg.m_LayoutPage.m_RightOverhang,dlg.m_LayoutPage.m_nColumns,dlg.m_LayoutPage.m_ColumnHeight,dlg.m_LayoutPage.m_ColumnSpacing);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}
