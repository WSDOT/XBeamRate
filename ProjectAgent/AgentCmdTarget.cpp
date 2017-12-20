///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2018  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "AgentCmdTarget.h"

#include <MFCTools\Prompts.h>
#include <EAF\EAFTransactions.h>
#include <txnEditProject.h>
#include <txnEditPier.h>
#include <txnEditOptions.h>

#include "PierDlg.h"
#include "OptionsDlg.h"
#include "ProjectPropertiesDlg.h"

#include <XBeamRateExt\XBeamRateUtilities.h>

#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>


BEGIN_MESSAGE_MAP(CAgentCmdTarget,CCmdTarget)
   ON_COMMAND(ID_EDIT_PIER, &CAgentCmdTarget::OnEditPier)
   ON_COMMAND(ID_EDIT_OPTIONS,&CAgentCmdTarget::OnEditOptions)
   ON_COMMAND(ID_EDIT_PROPERTIES,&CAgentCmdTarget::OnProjectProperties)
END_MESSAGE_MAP()

CAgentCmdTarget::CAgentCmdTarget()
{
   m_bShowProjectProperties = true;
}

void CAgentCmdTarget::Init(IBroker* pBroker)
{
   m_pBroker = pBroker;
}

void CAgentCmdTarget::OnEditPier()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IXBRProject,pProject);
   CPierDlg dlg(_T("Edit Pier"));

   PierIDType pierID = INVALID_ID;

   txnEditPierData oldPierData;
   oldPierData.m_PierData = pProject->GetPierData(pierID);

   IndexType nBearingLines = oldPierData.m_PierData.GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      oldPierData.m_DeadLoadReactionType[brgLineIdx] = pProject->GetBearingReactionType(pierID,brgLineIdx);
      IndexType nBearings = oldPierData.m_PierData.GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         txnDeadLoadReaction reaction;
         pProject->GetBearingReactions(pierID,brgLineIdx,brgIdx,&reaction.m_DC,&reaction.m_DW,&reaction.m_CR,&reaction.m_SH,&reaction.m_PS,&reaction.m_RE,&reaction.m_W);
         oldPierData.m_DeadLoadReactions[brgLineIdx].push_back(reaction);
      }
   }

   ATLASSERT(IsStandAlone());
   oldPierData.m_gDC_StrengthI = pProject->GetDCLoadFactor(pgsTypes::StrengthI_Inventory);
   oldPierData.m_gDW_StrengthI = pProject->GetDWLoadFactor(pgsTypes::StrengthI_Inventory);
   oldPierData.m_gCR_StrengthI = pProject->GetCRLoadFactor(pgsTypes::StrengthI_Inventory);
   oldPierData.m_gSH_StrengthI = pProject->GetSHLoadFactor(pgsTypes::StrengthI_Inventory);
   oldPierData.m_gPS_StrengthI = pProject->GetPSLoadFactor(pgsTypes::StrengthI_Inventory);

   oldPierData.m_gDC_StrengthII = pProject->GetDCLoadFactor(pgsTypes::StrengthII_PermitRoutine);
   oldPierData.m_gDW_StrengthII = pProject->GetDWLoadFactor(pgsTypes::StrengthII_PermitRoutine);
   oldPierData.m_gCR_StrengthII = pProject->GetCRLoadFactor(pgsTypes::StrengthII_PermitRoutine);
   oldPierData.m_gSH_StrengthII = pProject->GetSHLoadFactor(pgsTypes::StrengthII_PermitRoutine);
   oldPierData.m_gPS_StrengthII = pProject->GetPSLoadFactor(pgsTypes::StrengthII_PermitRoutine);

   oldPierData.m_gDC_ServiceI = pProject->GetDCLoadFactor(pgsTypes::ServiceI_PermitRoutine);
   oldPierData.m_gDW_ServiceI = pProject->GetDWLoadFactor(pgsTypes::ServiceI_PermitRoutine);
   oldPierData.m_gCR_ServiceI = pProject->GetCRLoadFactor(pgsTypes::ServiceI_PermitRoutine);
   oldPierData.m_gSH_ServiceI = pProject->GetSHLoadFactor(pgsTypes::ServiceI_PermitRoutine);
   oldPierData.m_gPS_ServiceI = pProject->GetPSLoadFactor(pgsTypes::ServiceI_PermitRoutine);

   oldPierData.m_gLL[0] = pProject->GetLiveLoadFactor(pierID,pgsTypes::StrengthI_Inventory,INVALID_INDEX);
   oldPierData.m_gLL[1] = pProject->GetLiveLoadFactor(pierID,pgsTypes::StrengthI_Operating,INVALID_INDEX);
   oldPierData.m_gLL[2] = pProject->GetLiveLoadFactor(pierID,pgsTypes::StrengthI_LegalRoutine,INVALID_INDEX);
   oldPierData.m_gLL[3] = pProject->GetLiveLoadFactor(pierID, pgsTypes::StrengthI_LegalSpecial, INVALID_INDEX);
   oldPierData.m_gLL[4] = pProject->GetLiveLoadFactor(pierID, pgsTypes::StrengthI_LegalEmergency, INVALID_INDEX);
   oldPierData.m_gLL[5] = pProject->GetLiveLoadFactor(pierID,pgsTypes::StrengthII_PermitRoutine,INVALID_INDEX);
   oldPierData.m_gLL[6] = pProject->GetLiveLoadFactor(pierID,pgsTypes::StrengthII_PermitSpecial,INVALID_INDEX);
   oldPierData.m_gLL[7] = pProject->GetLiveLoadFactor(pierID,pgsTypes::ServiceI_PermitRoutine,INVALID_INDEX);
   oldPierData.m_gLL[8] = pProject->GetLiveLoadFactor(pierID,pgsTypes::ServiceI_PermitSpecial,INVALID_INDEX);

   oldPierData.m_DesignLiveLoad.m_LLIM        = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrDesign_Inventory);
   oldPierData.m_LegalRoutineLiveLoad.m_LLIM  = pProject->GetLiveLoadReactions(pierID,pgsTypes::lrLegal_Routine);
   oldPierData.m_LegalSpecialLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID, pgsTypes::lrLegal_Special);
   oldPierData.m_LegalEmergencyLiveLoad.m_LLIM = pProject->GetLiveLoadReactions(pierID, pgsTypes::lrLegal_Emergency);
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

void CAgentCmdTarget::OnEditOptions()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   
   txnEditOptionsData oldOptions;
   oldOptions.m_LRFDVersion = lrfdVersionMgr::GetVersion();
   oldOptions.m_LRFRVersion = lrfrVersionMgr::GetVersion();

   oldOptions.m_bLegalRating = pRatingSpec->IsRatingEnabled(pgsTypes::lrDesign_Inventory);
   oldOptions.m_bLegalRateForShear = pRatingSpec->RateForShear(pgsTypes::lrDesign_Inventory);

   oldOptions.m_bLegalRating  = pRatingSpec->IsRatingEnabled(pgsTypes::lrLegal_Routine);
   oldOptions.m_bLegalRateForShear  = pRatingSpec->RateForShear(pgsTypes::lrLegal_Routine);
   oldOptions.m_EmergencyRatingMethod = pRatingSpec->GetEmergencyRatingMethod();

   oldOptions.m_bPermitRating = pRatingSpec->IsRatingEnabled(pgsTypes::lrPermit_Routine);
   oldOptions.m_bPermitRateForShear = pRatingSpec->RateForShear(pgsTypes::lrPermit_Routine);
   oldOptions.m_bCheckYieldStress = pRatingSpec->CheckYieldStressLimit();
   oldOptions.m_YieldStressCoefficient = pRatingSpec->GetYieldStressLimitCoefficient();
   oldOptions.m_PermitRatingMethod = pRatingSpec->GetPermitRatingMethod();

   GET_IFACE(IXBRProject,pProject);
   oldOptions.m_MaxLLStepSize = pProject->GetMaxLiveLoadStepSize();
   oldOptions.m_MaxLoadedLanes = pProject->GetMaxLoadedLanes();
   oldOptions.m_LiveLoadReactionApplication = pProject->GetReactionLoadApplicationType(INVALID_ID);
   oldOptions.m_SystemFactorFlexure = pProject->GetSystemFactorFlexure();
   oldOptions.m_SystemFactorShear   = pProject->GetSystemFactorShear();

   pProject->GetFlexureResistanceFactors(&oldOptions.m_PhiC,&oldOptions.m_PhiT);
   oldOptions.m_PhiV = pProject->GetShearResistanceFactor();

   COptionsDlg dlg;
   dlg.SetOptions(oldOptions);
   if ( dlg.DoModal() == IDOK )
   {
      txnEditOptionsData newOptions = dlg.GetOptions();

      txnEditOptions txn(oldOptions,newOptions);
      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(txn);
   }
}

void CAgentCmdTarget::OnProjectProperties()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE( IXBRProjectProperties, pProjProp );

   CProjectPropertiesDlg dlg;

   dlg.m_Bridge    = pProjProp->GetBridgeName();
   dlg.m_BridgeID  = pProjProp->GetBridgeID();
   dlg.m_JobNumber = pProjProp->GetJobNumber();
   dlg.m_Engineer  = pProjProp->GetEngineer();
   dlg.m_Company   = pProjProp->GetCompany();
   dlg.m_Comments  = pProjProp->GetComments();
   dlg.m_bShowProjectProperties = ShowProjectPropertiesOnNewProject();


   if ( dlg.DoModal() == IDOK )
   {
      txnEditProjectProperties* pTxn = new txnEditProjectProperties( pProjProp->GetBridgeName(), dlg.m_Bridge,
                                                                     pProjProp->GetBridgeID(),   dlg.m_BridgeID,
                                                                     pProjProp->GetJobNumber(),  dlg.m_JobNumber,
                                                                     pProjProp->GetEngineer(),   dlg.m_Engineer,
                                                                     pProjProp->GetCompany(),    dlg.m_Company,
                                                                     pProjProp->GetComments(),   dlg.m_Comments );

         
      ShowProjectPropertiesOnNewProject(dlg.m_bShowProjectProperties);

      GET_IFACE(IEAFTransactions,pTransactions);
      pTransactions->Execute(pTxn);
   }

}

void CAgentCmdTarget::ShowProjectPropertiesOnNewProject(bool bShow)
{
   m_bShowProjectProperties = bShow;
}

bool CAgentCmdTarget::ShowProjectPropertiesOnNewProject() const
{
   return m_bShowProjectProperties;
}