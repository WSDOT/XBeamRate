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
#include "LiveLoadGraphBuilder.h"

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

#include <MFCTools\Format.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CXBRLiveLoadGraphBuilder, CEAFGraphBuilderBase)
   ON_BN_CLICKED(IDC_MOMENT, &CXBRLiveLoadGraphBuilder::OnGraphTypeChanged)
   ON_BN_CLICKED(IDC_SHEAR, &CXBRLiveLoadGraphBuilder::OnGraphTypeChanged)
   ON_BN_CLICKED(IDC_NEXT, &CXBRLiveLoadGraphBuilder::OnNext)
   ON_BN_CLICKED(IDC_PREV, &CXBRLiveLoadGraphBuilder::OnPrev)
   ON_LBN_SELCHANGE(IDC_LOADING,&CXBRLiveLoadGraphBuilder::OnLbnSelChanged)
   ON_CBN_SELCHANGE(IDC_PIERS,&CXBRLiveLoadGraphBuilder::OnPierChanged)
   ON_CBN_SELCHANGE(IDC_RATING_TYPE,&CXBRLiveLoadGraphBuilder::OnRatingTypeChanged)
   ON_CBN_SELCHANGE(IDC_VEHICLE,&CXBRLiveLoadGraphBuilder::OnVehicleTypeChanged)
END_MESSAGE_MAP()


CXBRLiveLoadGraphBuilder::CXBRLiveLoadGraphBuilder()
{
   SetName(_T("Live Load Results"));
}

CXBRLiveLoadGraphBuilder::CXBRLiveLoadGraphBuilder(const CXBRLiveLoadGraphBuilder& other) :
CEAFGraphBuilderBase(other)
{
}

CEAFGraphControlWindow* CXBRLiveLoadGraphBuilder::GetGraphControlWindow()
{
   return &m_GraphController;
}

CGraphBuilder* CXBRLiveLoadGraphBuilder::Clone()
{
   // set the module state or the commands wont route to the
   // the graph control window
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return new CXBRLiveLoadGraphBuilder(*this);
}

BOOL CXBRLiveLoadGraphBuilder::CreateGraphController(CWnd* pParent,UINT nID)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !m_GraphController.Create(pParent,IDD_LIVE_LOAD_GRAPH_CONTROLLER, CBRS_LEFT, nID) )
   {
      TRACE0("Failed to create control bar\n");
      return FALSE; // failed to create
   }

   return TRUE;
}

void CXBRLiveLoadGraphBuilder::OnGraphTypeChanged()
{
   CEAFGraphView* pGraphView = m_pFrame->GetGraphView();
   pGraphView->Invalidate();
   pGraphView->UpdateWindow();
}

void CXBRLiveLoadGraphBuilder::OnLbnSelChanged()
{
   m_GraphController.LoadingChanged();
   OnGraphTypeChanged();
}

void CXBRLiveLoadGraphBuilder::OnNext()
{
   m_GraphController.Next();
   OnGraphTypeChanged();
}

void CXBRLiveLoadGraphBuilder::OnPrev()
{
   m_GraphController.Prev();
   OnGraphTypeChanged();
}

void CXBRLiveLoadGraphBuilder::OnPierChanged()
{
   OnGraphTypeChanged();
}

void CXBRLiveLoadGraphBuilder::OnRatingTypeChanged()
{
   m_GraphController.RatingTypeChanged();
   OnGraphTypeChanged();
}

void CXBRLiveLoadGraphBuilder::OnVehicleTypeChanged()
{
   OnGraphTypeChanged();
}

bool CXBRLiveLoadGraphBuilder::UpdateNow()
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

void CXBRLiveLoadGraphBuilder::DrawGraphNow(CWnd* pGraphWnd,CDC* pDC)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   GET_IFACE2_NOCHECK(pBroker,IXBRRatingSpecification,pRatingSpec);

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

   pgsTypes::LoadRatingType ratingType = m_GraphController.GetLoadRatingType();
   VehicleIndexType vehicleIdx = m_GraphController.GetVehicleIndex();
   IndexType permitLaneIdx = m_GraphController.GetPermitLaneIndex();

   GET_IFACE2(pBroker,IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID);

   IndexType llConfigIdx = m_GraphController.GetSelectedLiveLoadConfiguration();

   if ( !::IsPermitRatingType(ratingType) || ( ::IsPermitRatingType(ratingType) && pRatingSpec->GetPermitRatingMethod() != xbrTypes::prmWSDOT ) )
   {
      // only draw envelopes if this is not a permit rating, or if it is a permit rating, we aren't using the WSDOT method
      // envelope doesn't make sense with WSDOT method.

      // Controlling envelope for all trucks for this rating type
      CString strLiveLoadName = GetLiveLoadTypeName(ratingType);
      IndexType minGraphIdx = graph.CreateDataSeries(strLiveLoadName,PS_SOLID,GRAPH_PEN_WEIGHT,GREEN);
      IndexType maxGraphIdx = graph.CreateDataSeries(NULL,PS_SOLID,GRAPH_PEN_WEIGHT,GREEN);
      BuildControllingLiveLoadGraph(pierID,vPoi,ratingType,actionType,minGraphIdx,maxGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);

      GET_IFACE2(pBroker,IXBRProject,pProject);
      std::_tstring strLiveLoad = pProject->GetLiveLoadName(pierID,ratingType,vehicleIdx);
      if ( strLiveLoad != NO_LIVE_LOAD_DEFINED )
      {
         IndexType minVehGraphIdx = graph.CreateDataSeries(strLiveLoad.c_str(),PS_SOLID,GRAPH_PEN_WEIGHT,BLUE);
         IndexType maxVehGraphIdx = graph.CreateDataSeries(NULL,PS_SOLID,GRAPH_PEN_WEIGHT,BLUE);
         BuildControllingVehicularLiveLoadGraph(pierID,vPoi,ratingType,vehicleIdx,actionType,minVehGraphIdx,maxVehGraphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
   }

   if ( llConfigIdx != INVALID_INDEX )
   {
      CString strName;
      strName.Format(_T("Live Load Configuration %d"),LABEL_INDEX(llConfigIdx));
      graph.SetSubtitle(strName);

      IndexType graphIdx = graph.CreateDataSeries(NULL,PS_SOLID,GRAPH_PEN_WEIGHT,RED);

      if ( ::IsPermitRatingType(ratingType) && pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmWSDOT )
      {
         graph.SetDataLabel(graphIdx,_T("Legal"));

         IndexType permitGraphIdx = graph.CreateDataSeries(_T("Permit"),PS_SOLID,GRAPH_PEN_WEIGHT,GREEN);
         BuildWSDOTPermitLiveLoadGraph(pierID,vPoi,ratingType,vehicleIdx,llConfigIdx,permitLaneIdx,actionType,permitGraphIdx,graphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
      else
      {
         BuildLiveLoadGraph(pierID,vPoi,ratingType,vehicleIdx,llConfigIdx,actionType,graphIdx,graph,pHorizontalAxisFormat,pVerticalAxisFormat);
      }
   }

   CRect rect = GetView()->GetDrawingRect();
   graph.SetOutputRect(rect);
   graph.Draw(pDC->GetSafeHdc());

   if ( llConfigIdx != INVALID_INDEX )
   {
      DrawLiveLoadConfig(pGraphWnd,pDC,graph,pierID,ratingType,vehicleIdx,llConfigIdx,pHorizontalAxisFormat,pVerticalAxisFormat);
   }

   delete pVerticalAxisFormat;
   delete pHorizontalAxisFormat;
}

LPCTSTR CXBRLiveLoadGraphBuilder::GetGraphTitle(ActionType actionType)
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

void CXBRLiveLoadGraphBuilder::BuildControllingLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
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
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
      }
      else
      {
         sysSectionValue FyMin, FyMax;
         pResults->GetShear(pierID,ratingType,poi,&FyMin,&FyMax,NULL,NULL,NULL,NULL);
         Float64 Vlmin = pVerticalAxisFormat->Convert(FyMin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(FyMin.Right());
         Float64 Vlmax = pVerticalAxisFormat->Convert(FyMax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(FyMax.Right());

         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vrmax));
      }
   }
}

void CXBRLiveLoadGraphBuilder::BuildControllingVehicularLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,ActionType actionType,IndexType minGraphIdx,IndexType maxGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
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
         pResults->GetMoment(pierID,ratingType,vehicleIdx,poi,&Mmin,&Mmax,NULL,NULL);
         Mmin = pVerticalAxisFormat->Convert(Mmin);
         Mmax = pVerticalAxisFormat->Convert(Mmax);
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Mmin));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Mmax));
      }
      else
      {
         sysSectionValue FyMin, FyMax;
         pResults->GetShear(pierID,ratingType,vehicleIdx,poi,&FyMin,&FyMax,NULL,NULL);
         Float64 Vlmin = pVerticalAxisFormat->Convert(FyMin.Left());
         Float64 Vrmin = pVerticalAxisFormat->Convert(FyMin.Right());
         Float64 Vlmax = pVerticalAxisFormat->Convert(FyMax.Left());
         Float64 Vrmax = pVerticalAxisFormat->Convert(FyMax.Right());
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vlmin));
         graph.AddPoint(minGraphIdx,gpPoint2d(X,Vrmin));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vlmax));
         graph.AddPoint(maxGraphIdx,gpPoint2d(X,Vrmax));
      }
   }
}

void CXBRLiveLoadGraphBuilder::BuildLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,ActionType actionType,IndexType graphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mz = pResults->GetMoment(pierID,ratingType,vehicleIdx,llConfigIdx,poi);
         Mz = pVerticalAxisFormat->Convert(Mz);
         graph.AddPoint(graphIdx,gpPoint2d(X,Mz));
      }
      else
      {
         sysSectionValue Fy = pResults->GetShear(pierID,ratingType,vehicleIdx,llConfigIdx,poi);
         Float64 Vl = pVerticalAxisFormat->Convert(Fy.Left());
         Float64 Vr = pVerticalAxisFormat->Convert(Fy.Right());
         graph.AddPoint(graphIdx,gpPoint2d(X,Vl));
         graph.AddPoint(graphIdx,gpPoint2d(X,Vr));
      }
   }
}

void CXBRLiveLoadGraphBuilder::BuildWSDOTPermitLiveLoadGraph(PierIDType pierID,const std::vector<xbrPointOfInterest>& vPoi,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,ActionType actionType,IndexType permitGraphIdx,IndexType legalGraphIdx,grGraphXY& graph,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRAnalysisResults,pResults);

   // make sure the permitLaneIdx is value... if multipe loads are selected, and the live load cases
   // have a different number of lanes, the permit lane index could be too big for some cases.
   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
   IndexType nLoadedLanes = pProductForces->GetLoadedLaneCount(pierID,llConfigIdx);
   permitLaneIdx = (nLoadedLanes <= permitLaneIdx ? nLoadedLanes-1 : permitLaneIdx);


   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 X = poi.GetDistFromStart();
      X  = pHorizontalAxisFormat->Convert(X);

      if ( actionType == actionMoment )
      {
         Float64 Mpermit, Mlegal;
         pResults->GetMoment(pierID,ratingType,vehicleIdx,llConfigIdx,permitLaneIdx,poi,&Mpermit,&Mlegal);
         Mpermit = pVerticalAxisFormat->Convert(Mpermit);
         Mlegal  = pVerticalAxisFormat->Convert(Mlegal);
         graph.AddPoint(permitGraphIdx,gpPoint2d(X,Mpermit));
         graph.AddPoint(legalGraphIdx, gpPoint2d(X,Mlegal));
      }
      else
      {
         sysSectionValue Vpermit, Vlegal;
         pResults->GetShear(pierID,ratingType,vehicleIdx,llConfigIdx,permitLaneIdx,poi,&Vpermit,&Vlegal);
         Vpermit.Left() = pVerticalAxisFormat->Convert(Vpermit.Left());
         Vpermit.Right() = pVerticalAxisFormat->Convert(Vpermit.Right());
         Vlegal.Left() = pVerticalAxisFormat->Convert(Vlegal.Left());
         Vlegal.Right() = pVerticalAxisFormat->Convert(Vlegal.Right());
         graph.AddPoint(permitGraphIdx,gpPoint2d(X,Vpermit.Left()));
         graph.AddPoint(permitGraphIdx,gpPoint2d(X,Vpermit.Right()));
         graph.AddPoint(legalGraphIdx,gpPoint2d(X,Vlegal.Left()));
         graph.AddPoint(legalGraphIdx,gpPoint2d(X,Vlegal.Right()));
      }
   }
}

void CXBRLiveLoadGraphBuilder::DrawLiveLoadConfig(CWnd* pGraphWnd,CDC* pDC,grGraphXY& graph,PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,arvPhysicalConverter* pHorizontalAxisFormat,arvPhysicalConverter* pVerticalAxisFormat)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IXBRProductForces,pProductForces);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);

   IndexType permitLaneIdx = m_GraphController.GetPermitLaneIndex();
   WheelLineConfiguration wheelLineConfig = pProductForces->GetLiveLoadConfiguration(pierID,ratingType,vehicleIdx,llConfigIdx,permitLaneIdx);

   grlibPointMapper mapper( graph.GetClientAreaPointMapper(pDC->GetSafeHdc()) );
   LONG x,y;
   mapper.WPtoDP(0,0,&x,&y);

   // Get world extents and world origin
   gpSize2d wExt  = mapper.GetWorldExt();
   gpPoint2d wOrg = mapper.GetWorldOrg();
   
   // get device extents and device origin
   LONG dx,dy;
   mapper.GetDeviceExt(&dx,&dy);

   LONG dox,doy;
   mapper.GetDeviceOrg(&dox,&doy);

   // compute a new device origin in the y direction
   doy = (LONG)Round(y - (0.0 - wOrg.Y())*((Float64)(dx)/-wExt.Dx()));

   // change the y scaling (use same scaling as X direction)
   mapper.SetWorldExt(wExt.Dx(),wExt.Dx());
   mapper.SetDeviceExt(dx,dx);

   // change the device origin
   mapper.SetDeviceOrg(dox,doy);

   CPen legal_pen(PS_SOLID,1,RED);
   CPen permit_pen(PS_SOLID,1,GREEN);
   CPen* pOldPen = pDC->SelectObject(&legal_pen);

   HFONT hFont = grGraphTool::CreateRotatedFont(pDC->GetSafeHdc(),900,8);
   HGDIOBJ hOldFont = pDC->SelectObject(hFont);

   UINT oldTextAlign = pDC->GetTextAlign();
   int nBkMode = pDC->SetBkMode(TRANSPARENT);

   LONG dx_last, dy_last;
   std::vector<WheelLinePlacement>::iterator iter(wheelLineConfig.begin());
   std::vector<WheelLinePlacement>::iterator end(wheelLineConfig.end());
   for ( ; iter != end; iter++ )
   {
      IndexType idx = std::distance(wheelLineConfig.begin(),iter);
      IndexType laneIdx = idx/2;
      if ( laneIdx == permitLaneIdx )
      {
         pDC->SelectObject(&permit_pen);
      }
      else
      {
         pDC->SelectObject(&legal_pen);
      }

      WheelLinePlacement& placement = *iter;

      Float64 X = pHorizontalAxisFormat->Convert(placement.Xxb);
      Float64 Y = pVerticalAxisFormat->Convert(0);
      LONG dx,dy;
      mapper.WPtoDP(X,Y,&dx,&dy);

      pDC->MoveTo(dx,dy);
      pDC->LineTo(dx,dy-30);

      // draw the crossbar connecting wheel lines
      if ( idx % 2 == 0 )
      {
         pDC->SetTextAlign(TA_LEFT | TA_TOP);
         dx_last = dx;
         dy_last = dy-30;
      }
      else
      {
         pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
         pDC->MoveTo(dx_last,dy_last);
         pDC->LineTo(dx,dy-30);
      }

      // arrow head
      pDC->MoveTo(dx,dy);
      pDC->LineTo(dx-10,dy-10);
      pDC->MoveTo(dx,dy);
      pDC->LineTo(dx+10,dy-10);

      CString str;
      str.Format(_T("%s"),::FormatDimension(placement.Xxb,pDisplayUnits->GetSpanLengthUnit()));
      pDC->TextOut(dx,dy-30,str);

   }

   pDC->SelectObject(pOldPen);
   pDC->SelectObject(hOldFont);
   pDC->SetTextAlign(oldTextAlign);
   pDC->SetBkMode(nBkMode);
}
