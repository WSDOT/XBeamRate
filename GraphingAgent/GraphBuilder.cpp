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

#include <IFace\PointOfInterest.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>

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

void CXBRGraphBuilder::UpdateGraphDefinitions()
{
   IDType graphID = 0;
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Lower Cross Beam Dead Load"),pftLowerXBeam));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Upper Cross Beam Dead Load"),pftUpperXBeam));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Superstructre DC Reactions"),pftDCReactions));
   m_GraphDefinitions.AddGraphDefinition(CGraphDefinition(graphID++,_T("Superstructre DW Reactions"),pftDWReactions));

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

   CGraphDefinitions graphDefs = m_GraphController.GetSelectedGraphDefinitions();
   IndexType nGraphs = graphDefs.GetGraphDefinitionCount();
   for ( IndexType idx = 0; idx < nGraphs; idx++ )
   {
      CGraphDefinition& graphDef = graphDefs.GetGraphDefinition(idx);

      IndexType graphIdx, positiveGraphIdx, negativeGraphIdx;
      if ( graphDef.m_GraphType == graphCapacity )
      {
         positiveGraphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,RED);
         negativeGraphIdx = graph.CreateDataSeries(_T(""),PS_SOLID,2,BLUE);
      }
      else
      {
         graphIdx = graph.CreateDataSeries(graphDef.m_Name.c_str(),PS_SOLID,2,RED);
      }

      GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
      std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest();

      if ( graphDef.m_GraphType == graphProduct )
      {
         BuildProductForceGraph(vPoi,graphDef,actionType,graphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else if ( graphDef.m_GraphType == graphCapacity )
      {
         BuildCapacityGraph(vPoi,actionType,positiveGraphIdx,negativeGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
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

void CXBRGraphBuilder::BuildProductForceGraph(const std::vector<xbrPointOfInterest>& vPoi,const CGraphDefinition& graphDef,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   XBRProductForceType pfType = graphDef.m_LoadType.ProductLoadType;

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pResults->GetMoment(pfType,poi);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(graphIdx,point);
      }
      else
      {
         sysSectionValue V = pResults->GetShear(pfType,poi);
         Float64 Vl = pVerticalAxisFormat->Convert(V.Left());
         Float64 Vr = pVerticalAxisFormat->Convert(V.Right());
         graph.AddPoint(graphIdx,gpPoint2d(X,Vl));
         graph.AddPoint(graphIdx,gpPoint2d(X,Vr));
      }
   }
}

void CXBRGraphBuilder::BuildCapacityGraph(const std::vector<xbrPointOfInterest>& vPoi,ActionType actionType,IndexType positiveGraphIdx,IndexType negativeGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRLoadRating,pCapacity);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pCapacity->GetMomentCapacity(poi,true);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(positiveGraphIdx,point);

         Mz = pCapacity->GetMomentCapacity(poi,false);
         Mz = pVerticalAxisFormat->Convert(Mz);
         point.Y() = Mz;
         graph.AddPoint(negativeGraphIdx,point);
      }
      else
      {
         Float64 V = pCapacity->GetShearCapacity(poi);
         graph.AddPoint(positiveGraphIdx,gpPoint2d(X,V));
         graph.AddPoint(negativeGraphIdx,gpPoint2d(X,-V));
      }
   }
}