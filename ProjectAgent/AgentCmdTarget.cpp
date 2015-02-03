#include "stdafx.h"
#include "resource.h"
#include "AgentCmdTarget.h"

#include <MFCTools\Prompts.h>
#include <IFace\Project.h>
#include <EAF\EAFTransactions.h>
#include <txnEditProject.h>

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
   GET_IFACE(IProject,pProject);
   strOldProjectName = pProject->GetProjectName();

   if ( AfxQuestion(_T("Project Name"),_T("Enter project name"),strOldProjectName,strNewProjectName) )
   {
      txnEditProject txn(m_pBroker,strOldProjectName,strNewProjectName);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}

void CAgentCmdTarget::OnEditPier()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // TODO: Add your command handler code here
   CPierDlg dlg(_T("Edit Pier"));
   if ( dlg.DoModal() == IDOK )
   {
   }
}
