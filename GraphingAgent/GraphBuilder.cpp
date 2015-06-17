///////////////////////////////////////////////////////////////////////
// ExtensionAgentExample - Extension Agent Example Project for PGSuper
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
#include "GraphBuilder.h"

#include <EAF\EAFGraphChildFrame.h>
#include <EAF\EAFGraphView.h>

#include <EAF\EAFUtilities.h>
#include <EAF\EAFDisplayUnits.h>
#include <MathEx.h>
#include <GraphicsLib\GraphicsLib.h>
#include <UnitMgt\UnitValueNumericalFormatTools.h>

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>
#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>

#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <PgsExt\PierData2.h>

#include <Colors.h>
#define GRAPH_BACKGROUND WHITE //RGB(220,255,220)
#define GRAPH_GRID_PEN_STYLE PS_DOT
#define GRAPH_GRID_PEN_WEIGHT 1
#define GRAPH_GRID_COLOR GREY50 //RGB(0,150,0)
#define GRAPH_PEN_WEIGHT 2

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CXBRGraphBuilder, CEAFGraphBuilderBase)
   ON_BN_CLICKED(IDC_MOMENT, &CXBRGraphBuilder::OnGraphTypeChanged)
   ON_BN_CLICKED(IDC_SHEAR, &CXBRGraphBuilder::OnGraphTypeChanged)
   ON_LBN_SELCHANGE(IDC_LOADING,&CXBRGraphBuilder::OnLbnSelChanged)
   ON_CBN_SELCHANGE(IDC_PIERS,&CXBRGraphBuilder::OnPierChanged)
END_MESSAGE_MAP()


CXBRGraphBuilder::CXBRGraphBuilder()
{
   SetName(_T("Analysis Results"));
   UpdateGraphDefinitions();
}

CXBRGraphBuilder::CXBRGraphBuilder(const CXBRGraphBuilder& other) :
CEAFGraphBuilderBase(other)
{
   UpdateGraphDefinitions();
}

CEAFGraphControlWindow* CXBRGraphBuilder::GetGraphControlWindow()
{
   return &m_GraphController;
}

CGraphBuilder* CXBRGraphBuilder::Clone()
{
   // set the module state or the commands wont route to the
   // the graph control window
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return new CXBRGraphBuilder(*this);
}

const CGraphDefinitions& CXBRGraphBuilder::GetGraphDefinitions()
{
   return m_GraphDefinitions;
}

BOOL CXBRGraphBuilder::CreateGraphController(CWnd* pParent,UINT nID)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !m_GraphController.Create(pParent,IDD_GRAPH_CONTROLLER, CBRS_LEFT, nID) )
   {
      TRACE0("Failed to create control bar\n");
      return FALSE; // failed to create
   }

   return TRUE;
}

void CXBRGraphBuilder::OnGraphTypeChanged()
{
   CEAFGraphView* pGraphView = m_pFrame->GetGraphView();
   pGraphView->Invalidate();
   pGraphView->UpdateWindow();
}

void CXBRGraphBuilder::OnLbnSelChanged()
{
   OnGraphTypeChanged();
}

void CXBRGraphBuilder::OnPierChanged()
{
   OnGraphTypeChanged();
}

bool CXBRGraphBuilder::UpdateNow()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   CComPtr<IXBeamRateAgent> pAgent;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent);
   if ( SUCCEEDED(hr) )
   {
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
   else
   {
      return true;
   }
}

void CXBRGraphBuilder::UpdateGraphDefinitions()
{
   IDType graphID = 0;
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Lower Cross Beam Dead Load"),xbrTypes::pftLowerXBeam));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Upper Cross Beam Dead Load"),xbrTypes::pftUpperXBeam));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Superstructre DC Reactions"),xbrTypes::pftDCReactions));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Superstructre DW Reactions"),xbrTypes::pftDWReactions));

   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("DC"),lcDC));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("DW"),lcDW));

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRProject,pProject);
   for ( int i = 1; i < 6; i++ )
   {
      pgsTypes::LoadRatingType ratingType = (pgsTypes::LoadRatingType)i;
      IndexType nVehicles = pProject->GetLiveLoadReactionCount(ratingType);
      for ( VehicleIndexType vehIdx = 0; vehIdx < nVehicles; vehIdx++ )
      {
         LPCTSTR strName = pProject->GetLiveLoadName(ratingType,vehIdx);
         m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,strName,ratingType,vehIdx));
      }
   }

   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("LLIM (Inventory/Operating)"),pgsTypes::lrDesign_Inventory));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("LLIM (Legal Routine)"),pgsTypes::lrLegal_Routine));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("LLIM (Legal Special)"),pgsTypes::lrLegal_Special));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("LLIM (Permit Routine)"),pgsTypes::lrPermit_Routine));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("LLIM (Permit Special)"),pgsTypes::lrPermit_Special));

   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength I (Inventory)"),pgsTypes::StrengthI_Inventory));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength I (Operating)"),pgsTypes::StrengthI_Operating));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength I (Legal Routine)"),pgsTypes::StrengthI_LegalRoutine));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength I (Legal Special)"),pgsTypes::StrengthI_LegalSpecial));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength II (Permit Routine)"),pgsTypes::StrengthII_PermitRoutine));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Strength II (Permit Special)"),pgsTypes::StrengthII_PermitSpecial));


   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Capacity")));
}

void CXBRGraphBuilder::DrawGraphNow(CWnd* pGraphWnd,CDC* pDC)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

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

   graph.SetYAxisTitle(strYAxisTitle);

   graph.SetXAxisTitle(_T("Location (") + ((LengthTool*)pHorizontalAxisFormat)->UnitTag() + _T(")"));

   PierIDType pierID = m_GraphController.GetPierID();

   CGraphDefinitions graphDefs = m_GraphController.GetSelectedGraphDefinitions();
   IndexType nGraphs = graphDefs.GetGraphDefinitionCount();
   for ( IndexType idx = 0; idx < nGraphs; idx++ )
   {
      CGraphDefinition& graphDef = graphDefs.GetGraphDefinition(idx);

      IndexType graphIdx, positiveGraphIdx, negativeGraphIdx;
      if ( graphDef.m_GraphType == graphCapacity ||
           graphDef.m_GraphType == graphVehicularLiveLoad ||
           graphDef.m_GraphType == graphLiveLoad ||
           graphDef.m_GraphType == graphLimitState
         )
      {
         positiveGraphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,RED);
         negativeGraphIdx = graph.CreateDataSeries(_T(""),PS_SOLID,2,BLUE);
      }
      else
      {
         graphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,RED);
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
         BuildVehicularLiveLoadGraph(pierID,vPoi,graphDef,actionType,positiveGraphIdx,negativeGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphLiveLoad )
      {
         BuildLiveLoadGraph(pierID,vPoi,graphDef,actionType,positiveGraphIdx,negativeGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphLimitState )
      {
         BuildLimitStateGraph(pierID,vPoi,graphDef,actionType,positiveGraphIdx,negativeGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphCapacity )
      {
         BuildCapacityGraph(pierID,vPoi,actionType,positiveGraphIdx,negativeGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else
      {
         ATLASSERT(false);
      }
   }


   CRect wndRect;
   pGraphWnd->GetClientRect(&wndRect);
   graph.SetOutputRect(wndRect);
   graph.Draw(pDC->GetSafeHdc());

   delete pVerticalAxisFormat;
   delete pHorizontalAxisFormat;
}

LPCTSTR CXBRGraphBuilder::GetGraphTitle(ActionType actionType)
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

void CXBRGraphBuilder::BuildProductForceGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
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

void CXBRGraphBuilder::BuildCombinedForceGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   XBRCombinedForceType comboType = graphDef.m_LoadType.CombinedLoadType;

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

void CXBRGraphBuilder::BuildVehicularLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
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
         pResults->GetMoment(pierID,ratingType,graphDef.m_VehicleIndex,poi,&Mmin,&Mmax);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
      }
      else
      {
         sysSectionValue Vmin, Vmax;
         pResults->GetShear(pierID,ratingType,graphDef.m_VehicleIndex,poi,&Vmin,&Vmax);
         Float64 Vlmax = pVerticalAxisFormat->Convert(Vmax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(Vmax.Right());
         Float64 Vlmin = pVerticalAxisFormat->Convert(Vmin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(Vmin.Right());
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRGraphBuilder::BuildLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
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
         pResults->GetMoment(pierID,ratingType,poi,&Mmin,&Mmax);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
      }
      else
      {
         sysSectionValue Vmin, Vmax;
         pResults->GetShear(pierID,ratingType,poi,&Vmin,&Vmax);
         Float64 Vlmax = pVerticalAxisFormat->Convert(Vmax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(Vmax.Right());
         Float64 Vlmin = pVerticalAxisFormat->Convert(Vmin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(Vmin.Right());
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRGraphBuilder::BuildLimitStateGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
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
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
      }
   }
}

void CXBRGraphBuilder::BuildCapacityGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,ActionType actionType,IndexType positiveGraphIdx,IndexType negativeGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2_NOCHECK(pBroker,IXBRMomentCapacity,pMomentCapacity);
   GET_IFACE2_NOCHECK(pBroker,IXBRShearCapacity,pShearCapacity);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pMomentCapacity->GetMomentCapacity(pierID,xbrTypes::Stage2,poi,true);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(positiveGraphIdx,point);

         Mz = pMomentCapacity->GetMomentCapacity(pierID,xbrTypes::Stage2,poi,false);
         Mz = pVerticalAxisFormat->Convert(Mz);
         point.Y() = Mz;
         graph.AddPoint(negativeGraphIdx,point);
      }
      else
      {
         Float64 V = pShearCapacity->GetShearCapacity(pierID,poi);
         V = pVerticalAxisFormat->Convert(V);
         graph.AddPoint(positiveGraphIdx,gpPoint2d(X,V));
         graph.AddPoint(negativeGraphIdx,gpPoint2d(X,-V));
      }
   }
}