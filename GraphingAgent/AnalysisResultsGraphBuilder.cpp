///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2015  Washington State Department of Transportation
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
#include "AnalysisResultsGraphBuilder.h"

#include <EAF\EAFGraphChildFrame.h>
#include <EAF\EAFGraphView.h>

#include <EAF\EAFUtilities.h>
#include <EAF\EAFDisplayUnits.h>
#include <EAF\EAFAutoProgress.h>
#include <MathEx.h>
#include <UnitMgt\UnitValueNumericalFormatTools.h>

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\RatingSpecification.h>

#include <XBeamRateExt\XBeamRateUtilities.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <PgsExt\PierData2.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CXBRAnalysisResultsGraphBuilder, CEAFGraphBuilderBase)
   ON_BN_CLICKED(IDC_MOMENT, &CXBRAnalysisResultsGraphBuilder::OnGraphTypeChanged)
   ON_BN_CLICKED(IDC_SHEAR, &CXBRAnalysisResultsGraphBuilder::OnGraphTypeChanged)
   ON_LBN_SELCHANGE(IDC_LOADING,&CXBRAnalysisResultsGraphBuilder::OnLbnSelChanged)
   ON_CBN_SELCHANGE(IDC_PIERS,&CXBRAnalysisResultsGraphBuilder::OnPierChanged)
END_MESSAGE_MAP()


CXBRAnalysisResultsGraphBuilder::CXBRAnalysisResultsGraphBuilder()
{
   SetName(_T("Analysis Results"));
}

CXBRAnalysisResultsGraphBuilder::CXBRAnalysisResultsGraphBuilder(const CXBRAnalysisResultsGraphBuilder& other) :
CEAFGraphBuilderBase(other)
{
}

CEAFGraphControlWindow* CXBRAnalysisResultsGraphBuilder::GetGraphControlWindow()
{
   return &m_GraphController;
}

CGraphBuilder* CXBRAnalysisResultsGraphBuilder::Clone()
{
   // set the module state or the commands wont route to the
   // the graph control window
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return new CXBRAnalysisResultsGraphBuilder(*this);
}

const CXBRAnalysisResultsGraphDefinitions& CXBRAnalysisResultsGraphBuilder::GetGraphDefinitions()
{
   return m_GraphDefinitions;
}

BOOL CXBRAnalysisResultsGraphBuilder::CreateGraphController(CWnd* pParent,UINT nID)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !m_GraphController.Create(pParent,IDD_ANALYSIS_RESULTS_GRAPH_CONTROLLER, CBRS_LEFT, nID) )
   {
      TRACE0("Failed to create control bar\n");
      return FALSE; // failed to create
   }

   UpdateGraphDefinitions();

   return TRUE;
}

void CXBRAnalysisResultsGraphBuilder::OnGraphTypeChanged()
{
   CEAFGraphView* pGraphView = m_pFrame->GetGraphView();
   pGraphView->Invalidate();
   pGraphView->UpdateWindow();
}

void CXBRAnalysisResultsGraphBuilder::OnLbnSelChanged()
{
   OnGraphTypeChanged();
}

void CXBRAnalysisResultsGraphBuilder::OnPierChanged()
{
   OnGraphTypeChanged();
}

bool CXBRAnalysisResultsGraphBuilder::UpdateNow()
{
   if ( IsStandAlone() )
   {
      return true;
   }
   else
   {
      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);

      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      PierIndexType nPiers = pIBridgeDesc->GetPierCount();
      for ( PierIndexType pierIdx = 1; pierIdx < nPiers-1; pierIdx++ )
      {
         const CPierData2* pPier = pIBridgeDesc->GetPier(pierIdx);
         if ( pPier->GetPierModelType() == pgsTypes::pmtPhysical )
         {
            m_GraphController.EnableControls(TRUE);
            return true;
         }
      }
      m_GraphController.EnableControls(FALSE);
      m_ErrorMsg = _T("Cross Beam results are not available.\nThe bridge model has only idealized piers.");
      return false;
   }
}

void CXBRAnalysisResultsGraphBuilder::UpdateGraphDefinitions()
{
   m_GraphDefinitions.Clear();

   PierIDType pierID = m_GraphController.GetPierID();
   if ( !IsStandAlone() && pierID == INVALID_ID )
   {
      // nothing to graph
      return;
   }

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   // include time dependnet load cases if
   // 1) we are running as a stand alone application
   // 2) we are running as a PGS extension and the losses are computed with the time-step method
   bool bIncludeTimeDependentLoads = true;

   if ( IsPGSExtension() )
   {
      GET_IFACE2( pBroker, ILossParameters, pLossParams);
      if ( pLossParams->GetLossMethod() != pgsTypes::TIME_STEP )
      {
         // PGS extension and not time-step analysis... don't include time dependent loads
         bIncludeTimeDependentLoads = false;
      }
   }

   IDType graphID = 0;
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Lower Cross Beam Dead Load"),        xbrTypes::pftLowerXBeam));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure Diaphragm Dead Load"),xbrTypes::pftUpperXBeam));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure DC Reactions"),       xbrTypes::pftDCReactions));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure DW Reactions"),       xbrTypes::pftDWReactions));
   if ( bIncludeTimeDependentLoads )
   {
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure CR Reactions"),xbrTypes::pftCRReactions));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure SH Reactions"),xbrTypes::pftSHReactions));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure RE Reactions"),xbrTypes::pftREReactions));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Superstructure PS Reactions"),xbrTypes::pftPSReactions));
   }

   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("DC"),xbrTypes::lcDC));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("DW"),xbrTypes::lcDW));
   if ( bIncludeTimeDependentLoads )
   {
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("CR"),xbrTypes::lcCR));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("SH"),xbrTypes::lcSH));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("RE"),xbrTypes::lcRE));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("PS"),xbrTypes::lcPS));
   }

   GET_IFACE2(pBroker,IXBRProject,pProject);
   for ( int i = 1; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      IndexType nVehicles = pProject->GetLiveLoadReactionCount(pierID,ratingType);
      for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nVehicles; vehicleIdx++ )
      {
         std::_tstring strName = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);
         if ( strName != NO_LIVE_LOAD_DEFINED )
         {
            m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,strName.c_str(),ratingType,vehicleIdx));
         }
      }
   }

   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("LLIM (Inventory/Operating)"),pgsTypes::lrDesign_Inventory));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("LLIM (Legal Routine)"),      pgsTypes::lrLegal_Routine));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("LLIM (Legal Special)"),      pgsTypes::lrLegal_Special));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("LLIM (Permit Routine)"),     pgsTypes::lrPermit_Routine));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("LLIM (Permit Special)"),     pgsTypes::lrPermit_Special));

   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength I (Inventory)"),      pgsTypes::StrengthI_Inventory));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength I (Operating)"),      pgsTypes::StrengthI_Operating));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength I (Legal Routine)"),  pgsTypes::StrengthI_LegalRoutine));
   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength I (Legal Special)"),  pgsTypes::StrengthI_LegalSpecial));

   GET_IFACE2(pBroker,IXBRRatingSpecification,pRatingSpec);
   if ( pRatingSpec->GetPermitRatingMethod() != xbrTypes::prmWSDOT )
   {
      // NOTE: Strength II limit state doesn't make sense for WSDOT method. The graphs show the limit state based on a controlling envelope. For the WSDOT method
      // we don't have a controlling envelope per se. WSDOT method minimizes RF based on a combinatin of Legal and Permit loads
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength II (Permit Routine)"),pgsTypes::StrengthII_PermitRoutine));
      m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Strength II (Permit Special)"),pgsTypes::StrengthII_PermitSpecial));
   }

   m_GraphDefinitions.AddGraphDefinition(CXBRAnalysisResultsGraphDefinition(graphID++,_T("Capacity")));

   m_GraphController.FillLoadingList();

   m_GraphColor.SetGraphCount(m_GraphDefinitions.GetGraphDefinitionCount());
}

void CXBRAnalysisResultsGraphBuilder::DrawGraphNow(CWnd* pGraphWnd,CDC* pDC)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   GET_IFACE2(pBroker,IProgress,pProgress);
   CEAFAutoProgress ap(pProgress);
   pProgress->UpdateMessage(_T("Building Graph"));
   CWaitCursor wait;

   ActionType actionType = m_GraphController.GetActionType();

   arvPhysicalConverter* pVerticalAxisFormat;
   if ( actionType == actionMoment )
   {
      pVerticalAxisFormat = new MomentTool(pDisplayUnits->GetMomentUnit());
   }
   else
   {
      pVerticalAxisFormat = new ShearTool(pDisplayUnits->GetShearUnit());
   }

   arvPhysicalConverter* pHorizontalAxisFormat = new LengthTool(pDisplayUnits->GetSpanLengthUnit());
   grGraphXY graph(*pHorizontalAxisFormat,*pVerticalAxisFormat);

   graph.SetGridPenStyle(GRAPH_GRID_PEN_STYLE, GRAPH_GRID_PEN_WEIGHT, GRAPH_GRID_COLOR);
   graph.SetClientAreaColor(GRAPH_BACKGROUND);

   graph.SetTitle(GetGraphTitle(actionType));

   std::_tstring strYAxisTitle;
   if ( actionType == actionMoment )
   {
      strYAxisTitle = _T("Moment (") + ((MomentTool*)pVerticalAxisFormat)->UnitTag() + _T(")");
   }
   else
   {
      strYAxisTitle = _T("Shear (") + ((ShearTool*)pVerticalAxisFormat)->UnitTag() + _T(")");
   }

   graph.SetYAxisTitle(strYAxisTitle.c_str());

   graph.SetXAxisTitle(std::_tstring(_T("Location (") + ((LengthTool*)pHorizontalAxisFormat)->UnitTag() + _T(")")).c_str());

   PierIDType pierID = m_GraphController.GetPierID();

   CXBRAnalysisResultsGraphDefinitions graphDefs = m_GraphController.GetSelectedGraphDefinitions();
   IndexType nGraphs = graphDefs.GetGraphDefinitionCount();
   for ( IndexType idx = 0; idx < nGraphs; idx++ )
   {
      CXBRAnalysisResultsGraphDefinition& graphDef = graphDefs.GetGraphDefinition(idx);

      IndexType selectedGraphIdx = m_GraphDefinitions.GetGraphIndex(graphDef.m_ID);
      COLORREF color = m_GraphColor.GetColor(selectedGraphIdx);

      IndexType graphIdx, maxGraphIdx, minGraphIdx;
      if ( graphDef.m_GraphType == graphCapacity ||
           graphDef.m_GraphType == graphVehicularLiveLoad ||
           graphDef.m_GraphType == graphLiveLoad ||
           graphDef.m_GraphType == graphLimitState
         )
      {
         maxGraphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,color);
         minGraphIdx = graph.CreateDataSeries(_T(""),PS_SOLID,2,color);
      }
      else
      {
         graphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,color);
      }

      GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
      std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID);

      if ( graphDef.m_GraphType == graphProduct )
      {
         BuildProductForceGraph(pierID,vPoi,graphDef,actionType,graphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphCombined )
      {
         BuildCombinedForceGraph(pierID,vPoi,graphDef,actionType,graphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphVehicularLiveLoad )
      {
         BuildVehicularLiveLoadGraph(pierID,vPoi,graphDef,actionType,minGraphIdx,maxGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphLiveLoad )
      {
         BuildLiveLoadGraph(pierID,vPoi,graphDef,actionType,minGraphIdx,maxGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphLimitState )
      {
         BuildLimitStateGraph(pierID,vPoi,graphDef,actionType,minGraphIdx,maxGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphCapacity )
      {
         BuildCapacityGraph(pierID,vPoi,actionType,minGraphIdx,maxGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else
      {
         ATLASSERT(false);
      }
   }

   CRect rect = GetView()->GetDrawingRect();
   graph.SetOutputRect(rect);
   graph.Draw(pDC->GetSafeHdc());

   delete pVerticalAxisFormat;
   delete pHorizontalAxisFormat;
}

LPCTSTR CXBRAnalysisResultsGraphBuilder::GetGraphTitle(ActionType actionType)
{
   switch(actionType)
   {
   case actionMoment:
      return _T("Moment");

   case actionShear:
      return _T("Shear");
   }
   ATLASSERT(false);
   return _T("Unknown Graph Type");
}

void CXBRAnalysisResultsGraphBuilder::BuildProductForceGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CXBRAnalysisResultsGraphDefinition& graphDef,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   xbrTypes::ProductForceType pfType = graphDef.m_LoadType.ProductLoadType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pResults->GetMoment(pierID,pfType,poi);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(graphIdx,point);
      }
      else
      {
         sysSectionValue V = pResults->GetShear(pierID,pfType,poi);
         Float64 Vl = pVerticalAxisFormat->Convert(V.Left());
         Float64 Vr = pVerticalAxisFormat->Convert(V.Right());
         graph.AddPoint(graphIdx,gpPoint2d(X,Vl));
         graph.AddPoint(graphIdx,gpPoint2d(X,Vr));
      }
   }
}

void CXBRAnalysisResultsGraphBuilder::BuildCombinedForceGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CXBRAnalysisResultsGraphDefinition& graphDef,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   xbrTypes::CombinedForceType comboType = graphDef.m_LoadType.CombinedLoadType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pResults->GetMoment(pierID,comboType,poi);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(graphIdx,point);
      }
      else
      {
         sysSectionValue V = pResults->GetShear(pierID,comboType,poi);
         Float64 Vl = pVerticalAxisFormat->Convert(V.Left());
         Float64 Vr = pVerticalAxisFormat->Convert(V.Right());
         graph.AddPoint(graphIdx,gpPoint2d(X,Vl));
         graph.AddPoint(graphIdx,gpPoint2d(X,Vr));
      }
   }
}

void CXBRAnalysisResultsGraphBuilder::BuildVehicularLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CXBRAnalysisResultsGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   pgsTypes::LoadRatingType ratingType = graphDef.m_LoadType.LiveLoadType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mmin, Mmax;
         pResults->GetMoment(pierID,ratingType,graphDef.m_VehicleIndex,poi,&Mmin,&Mmax,NULL,NULL);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
      }
      else
      {
         sysSectionValue Vmin, Vmax;
         pResults->GetShear(pierID,ratingType,graphDef.m_VehicleIndex,poi,&Vmin,&Vmax,NULL,NULL);
         Float64 Vlmax = pVerticalAxisFormat->Convert(Vmax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(Vmax.Right());
         Float64 Vlmin = pVerticalAxisFormat->Convert(Vmin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(Vmin.Right());
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRAnalysisResultsGraphBuilder::BuildLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CXBRAnalysisResultsGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   pgsTypes::LoadRatingType ratingType = graphDef.m_LoadType.LiveLoadType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mmin, Mmax;
         pResults->GetMoment(pierID,ratingType,poi,&Mmin,&Mmax,NULL,NULL);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
      }
      else
      {
         sysSectionValue Vmin, Vmax;
         pResults->GetShear(pierID,ratingType,poi,&Vmin,&Vmax,NULL,NULL,NULL,NULL);
         Float64 Vlmax = pVerticalAxisFormat->Convert(Vmax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(Vmax.Right());
         Float64 Vlmin = pVerticalAxisFormat->Convert(Vmin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(Vmin.Right());
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRAnalysisResultsGraphBuilder::BuildLimitStateGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CXBRAnalysisResultsGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   pgsTypes::LimitState limitState = graphDef.m_LoadType.LimitStateType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mmin, Mmax;
         pResults->GetMoment(pierID,limitState,poi,&Mmin,&Mmax);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
      }
      else
      {
         sysSectionValue Vmin, Vmax;
         pResults->GetShear(pierID,limitState,poi,&Vmin,&Vmax);
         Float64 Vlmax = pVerticalAxisFormat->Convert(Vmax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(Vmax.Right());
         Float64 Vlmin = pVerticalAxisFormat->Convert(Vmin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(Vmin.Right());
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRAnalysisResultsGraphBuilder::BuildCapacityGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2_NOCHECK(pBroker,IXBRMomentCapacity,pMomentCapacity);
   GET_IFACE2_NOCHECK(pBroker,IXBRShearCapacity,pShearCapacity);

   xbrTypes::Stage stage = xbrTypes::Stage2;

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pMomentCapacity->GetMomentCapacity(pierID,stage,poi,true);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(maxGraphIdx,point);

         Mz = pMomentCapacity->GetMomentCapacity(pierID,stage,poi,false);
         Mz = pVerticalAxisFormat->Convert(Mz);
         point.Y() = Mz;
         graph.AddPoint(minGraphIdx,point);
      }
      else
      {
         Float64 V = pShearCapacity->GetShearCapacity(pierID,stage,poi);
         V = pVerticalAxisFormat->Convert(V);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,V));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,-V));
      }
   }
}