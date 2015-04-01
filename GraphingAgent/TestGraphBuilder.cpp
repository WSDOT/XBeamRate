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
#include "TestGraphBuilder.h"

#include <EAF\EAFGraphChildFrame.h>
#include <EAF\EAFGraphView.h>

#include <EAF\EAFUtilities.h>
#include <EAF\EAFDisplayUnits.h>
#include <MathEx.h>
#include <GraphicsLib\GraphicsLib.h>
#include <UnitMgt\UnitValueNumericalFormatTools.h>

#include <IFace\AnalysisResults.h>
#include <IFace\PointOfInterest.h>

#include <Colors.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTestGraphBuilder, CEAFGraphBuilderBase)
   ON_BN_CLICKED(IDC_MOMENT, &CTestGraphBuilder::OnGraphTypeChanged)
   ON_BN_CLICKED(IDC_SHEAR, &CTestGraphBuilder::OnGraphTypeChanged)
END_MESSAGE_MAP()


CTestGraphBuilder::CTestGraphBuilder()
{
   SetName(_T("XBeam Rate Test Graph Builder"));
}

CTestGraphBuilder::CTestGraphBuilder(const CTestGraphBuilder& other) :
CEAFGraphBuilderBase(other)
{
}

CEAFGraphControlWindow* CTestGraphBuilder::GetGraphControlWindow()
{
   return &m_GraphControls;
}

CGraphBuilder* CTestGraphBuilder::Clone()
{
   // set the module state or the commands wont route to the
   // the graph control window
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return new CTestGraphBuilder(*this);
}

BOOL CTestGraphBuilder::CreateGraphController(CWnd* pParent,UINT nID)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !m_GraphControls.Create(pParent,IDD_TEST_GRAPH_CONTROLS, CBRS_LEFT, nID) )
   {
      TRACE0("Failed to create control bar\n");
      return FALSE; // failed to create
   }

   return TRUE;
}

void CTestGraphBuilder::OnGraphTypeChanged()
{
   CEAFGraphView* pGraphView = m_pFrame->GetGraphView();
   pGraphView->Invalidate();
   pGraphView->UpdateWindow();
}

void CTestGraphBuilder::DrawGraphNow(CWnd* pGraphWnd,CDC* pDC)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   int graphType = m_GraphControls.GetGraphType();
   arvPhysicalConverter* pVerticalAxisFormat;
   if ( graphType == MOMENT_GRAPH )
      pVerticalAxisFormat = new MomentTool(pDisplayUnits->GetMomentUnit());
   else
      pVerticalAxisFormat = new ShearTool(pDisplayUnits->GetShearUnit());

   arvPhysicalConverter* pHorizontalAxisFormat = new LengthTool(pDisplayUnits->GetSpanLengthUnit());
   grGraphXY graph(*pHorizontalAxisFormat,*pVerticalAxisFormat);

   std::_tstring strYAxisTitle;
   if ( graphType == MOMENT_GRAPH )
      strYAxisTitle = _T("Moment (") + ((MomentTool*)pVerticalAxisFormat)->UnitTag() + _T(")");
   else
      strYAxisTitle = _T("Shear (") + ((ShearTool*)pVerticalAxisFormat)->UnitTag() + _T(")");

   graph.SetYAxisTitle(strYAxisTitle);

   graph.SetXAxisTitle(_T("Location (") + ((LengthTool*)pHorizontalAxisFormat)->UnitTag() + _T(")"));

   IndexType graphIdx = graph.CreateDataSeries();

   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);
   GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( graphType == MOMENT_GRAPH )
      {
         Float64 Mz = pResults->GetMoment(pftLowerXBeam,poi);
         Mz = pVerticalAxisFormat->Convert(Mz);
         gpPoint2d point(X,Mz);
         graph.AddPoint(graphIdx,point);
      }
      else
      {
         sysSectionValue V = pResults->GetShear(pftLowerXBeam,poi);
         Float64 Vl = pVerticalAxisFormat->Convert(V.Left());
         Float64 Vr = pVerticalAxisFormat->Convert(V.Right());
         graph.AddPoint(graphIdx,gpPoint2d(X,Vl));
         graph.AddPoint(graphIdx,gpPoint2d(X,Vr));
      }
   }


   //int graphType = m_GraphControls.GetGraphType();

   //// first x axis
   //const unitmgtScalar& scalar = pDisplayUnits->GetScalarFormat();
   //arvPhysicalConverter* pFormat = new ScalarTool(scalar);
   //grGraphXY graph(*pFormat,*pFormat);

   //IndexType idx = graph.CreateDataSeries();
   //for ( int i = 0; i <= 360; i++ )
   //{
   //   Float64 angle = ::ToRadians((Float64)(i));
   //   Float64 y;
   //   if ( graphType == SINE_GRAPH )
   //      y = sin(angle);
   //   else
   //      y = cos(angle);

   //   gpPoint2d point(angle,y);
   //   graph.AddPoint(idx,point);
   //}


   CRect wndRect;
   pGraphWnd->GetClientRect(&wndRect);
   graph.SetOutputRect(wndRect);
   graph.Draw(pDC->GetSafeHdc());

   delete pVerticalAxisFormat;
   delete pHorizontalAxisFormat;
}
