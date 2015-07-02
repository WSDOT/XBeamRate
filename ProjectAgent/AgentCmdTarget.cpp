#include "stdafx.h"
#include "resource.h"
#include "AgentCmdTarget.h"

#include <MFCTools\Prompts.h>
#include <EAF\EAFTransactions.h>
#include <txnEditProject.h>
#include <txnEditPier.h>

#include "PierDlg.h"

#include <IFace\Project.h>


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

   //CString strOldProjectName, strNewProjectName;
   //GET_IFACE(IXBRProject,pProject);
   //strOldProjectName = pProject->GetProjectName();

   //if ( AfxQuestion(_T("Project Name"),_T("Enter project name"),strOldProjectName,strNewProjectName) )
   //{
   //   txnEditProject txn(strOldProjectName,strNewProjectName);
   //   GET_IFACE(IEAFTransactions,pTransactions);
   //   pTransactions->Execute(txn);
   //}
}

void CAgentCmdTarget::OnEditPier()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IXBRProject,pProject);
   CPierDlg dlg(_T("Edit Pier"));

   PierIDType pierID = INVALID_ID;

   txnEditPierData oldPierData;
   oldPierData.m_PierData = pProject->GetPierData(pierID);

   for ( IndexType brgLineIdx = 0; brgLineIdx < oldPierData.m_PierData.GetBearingLineCount(); brgLineIdx++ )
   {
      IndexType nBearings = oldPierData.m_PierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         txnDeadLoadReaction reaction;
         pProject->GetBearingReactions(pierID,brgLineIdx,brgIdx,&reaction.m_DC,&reaction.m_DW);
         oldPierData.m_DeadLoadReactions[brgLineIdx].push_back(reaction);
      }
   }

   oldPierData.m_gDC = pProject->GetDCLoadFactor();
   oldPierData.m_gDW = pProject->GetDWLoadFactor();
   for ( int i = 0; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      oldPierData.m_gLL[ratingType] = pProject->GetLiveLoadFactor(pierID,ratingType);
   }

   oldPierData.m_DesignLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrDesign_Inventory);
   oldPierData.m_LegalRoutineLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrLegal_Routine);
   oldPierData.m_LegalSpecialLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrLegal_Special);
   oldPierData.m_PermitRoutineLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrPermit_Routine);
   oldPierData.m_PermitSpecialLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrPermit_Special);

   dlg.SetEditPierData(oldPierData);
   if ( dlg.DoModal() == IDOK )
   {
      txnEditPierData newPierData = dlg.GetEditPierData();
      txnEditPier txn(oldPierData,newPierData);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}
