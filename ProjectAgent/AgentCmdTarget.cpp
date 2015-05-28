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

   txnEditPierData oldPierData;
   oldPierData.m_PierData = pProject->GetPierData();

   for ( IndexType brgLineIdx = 0; brgLineIdx < oldPierData.m_PierData.GetBearingLineCount(); brgLineIdx++ )
   {
      IndexType nBearings = oldPierData.m_PierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         txnDeadLoadReaction reaction;
         pProject->GetBearingReactions(brgLineIdx,brgIdx,&reaction.m_DC,&reaction.m_DW);
         oldPierData.m_DeadLoadReactions[brgLineIdx].push_back(reaction);
      }
   }

   oldPierData.m_gDC = pProject->GetDCLoadFactor();
   oldPierData.m_gDW = pProject->GetDWLoadFactor();
   for ( int i = 0; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      oldPierData.m_gLL[ratingType] = pProject->GetLiveLoadFactor(ratingType);
   }

   oldPierData.m_DesignLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pgsTypes::lrDesign_Inventory);
   oldPierData.m_LegalRoutineLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pgsTypes::lrLegal_Routine);
   oldPierData.m_LegalSpecialLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pgsTypes::lrLegal_Special);
   oldPierData.m_PermitRoutineLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pgsTypes::lrPermit_Routine);
   oldPierData.m_PermitSpecialLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pgsTypes::lrPermit_Special);

   dlg.SetPierData(oldPierData);
   if ( dlg.DoModal() == IDOK )
   {
      txnEditPierData newPierData = dlg.GetPierData();
      txnEditPier txn(oldPierData,newPierData);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}
