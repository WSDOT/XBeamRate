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
   GET_IFACE_(XBR,IProject,pProject);
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

   GET_IFACE_(XBR,IProject,pProject);
   CPierDlg dlg(_T("Edit Pier"));

   txnEditPierData oldPierData;
   oldPierData.m_TransverseMeasurementType = pProject->GetTransverseDimensionsMeasurementType();
   oldPierData.m_DeckElevation = pProject->GetDeckElevation();
   oldPierData.m_CrownPointOffset = pProject->GetCrownPointOffset();
   oldPierData.m_BridgeLineOffset = pProject->GetBridgeLineOffset();
   oldPierData.m_strOrientation = pProject->GetOrientation();

   oldPierData.m_nBearingLines = pProject->GetBearingLineCount();

   oldPierData.m_Ec = pProject->GetModE();
   for ( int i = 0; i < 2; i++ )
   {
      pgsTypes::PierSideType side = (pgsTypes::PierSideType)i;
      pProject->GetXBeamDimensions(side,&oldPierData.m_XBeamHeight[side],&oldPierData.m_XBeamTaperHeight[side],&oldPierData.m_XBeamTaperLength[side]);
      oldPierData.m_XBeamOverhang[side] = pProject->GetXBeamOverhang(side);
   }
   oldPierData.m_XBeamWidth = pProject->GetXBeamWidth();

   oldPierData.m_nColumns = pProject->GetColumnCount();
   oldPierData.m_ColumnHeight = pProject->GetColumnHeight(0);
   oldPierData.m_ColumnHeightMeasurementType = pProject->GetColumnHeightMeasurementType();
   oldPierData.m_ColumnSpacing = pProject->GetSpacing(0);
   pProject->GetColumnShape(&oldPierData.m_ColumnShape,&oldPierData.m_B,&oldPierData.m_D);

   pProject->GetTransverseLocation(&oldPierData.m_RefColumnIdx,&oldPierData.m_TransverseOffset,&oldPierData.m_TransverseOffsetMeasurement);

   dlg.SetPierData(oldPierData);
   if ( dlg.DoModal() == IDOK )
   {
      txnEditPierData newPierData = dlg.GetPierData();
      txnEditPier txn(oldPierData,newPierData);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}
