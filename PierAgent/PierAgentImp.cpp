///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2017  Washington State Department of Transportation
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

// PierAgentImp.cpp : Implementation of CPierAgentImp
#include "stdafx.h"
#include "PierAgent.h"
#include "PierAgentImp.h"

#include <IFace\Project.h>
#include <IFace\AnalysisResults.h>
#include <algorithm>
#include <Math\Math.h>

#include <PsgLib\UnitServer.h>
#include <Lrfd\Lrfd.h>
#include <EAF\EAFAutoProgress.h>

#include <XBeamRateExt\XBeamRateUtilities.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class FindByID
{
public:
   FindByID(PoiIDType id) : m_ID(id) {}
   bool operator()(const xbrPointOfInterest& other) const
   {
      if ( m_ID == other.GetID() )
      {
         return true;
      }

      return false;
   }

private:
   PoiIDType m_ID;
};

bool ComparePoiLocation(const xbrPointOfInterest& poi1,const xbrPointOfInterest& poi2)
{
   if ( poi1.IsColumnPOI() != poi2.IsColumnPOI() )
   {
      return false;
   }

   if ( !IsEqual(poi1.GetDistFromStart(),poi2.GetDistFromStart()) )
   {
      return false;
   }

   return true;
}

StageIndexType GetStageIndex(xbrTypes::Stage stage)
{
   return (stage == xbrTypes::Stage1 ? 0 : 1);
}


BarSize GetBarSize(matRebar::Size size)
{
   switch(size)
   {
   case matRebar::bs3:  return bs3;
   case matRebar::bs4:  return bs4;
   case matRebar::bs5:  return bs5;
   case matRebar::bs6:  return bs6;
   case matRebar::bs7:  return bs7;
   case matRebar::bs8:  return bs8;
   case matRebar::bs9:  return bs9;
   case matRebar::bs10: return bs10;
   case matRebar::bs11: return bs11;
   case matRebar::bs14: return bs14;
   case matRebar::bs18: return bs18;
   default:
      ATLASSERT(false); // should not get here
   }
   
   ATLASSERT(false); // should not get here
   return bs3;
}

RebarGrade GetRebarGrade(matRebar::Grade grade)
{
   RebarGrade matGrade;
   switch(grade)
   {
   case matRebar::Grade40: matGrade = Grade40; break;
   case matRebar::Grade60: matGrade = Grade60; break;
   case matRebar::Grade75: matGrade = Grade75; break;
   case matRebar::Grade80: matGrade = Grade80; break;
   case matRebar::Grade100: matGrade = Grade100; break;
   default:
      ATLASSERT(false);
   }

#if defined _DEBUG
   if ( matGrade == Grade100 )
   {
      // grade 100 wasn't introduced until 6th Edition, 2013 interims
      ATLASSERT(lrfdVersionMgr::SixthEditionWith2013Interims <= lrfdVersionMgr::GetVersion());
   }
#endif

   return matGrade;
}

MaterialSpec GetRebarSpecification(matRebar::Type type)
{
   return (type == matRebar::A615 ? msA615 : (type == matRebar::A706 ? msA706 : msA1035));
}


/////////////////////////////////////////////////////////////////////////////
// CPierAgentImp
CPierAgentImp::CPierAgentImp()
{
   m_pBroker = 0;
   m_NextPoiID = 0;
}

CPierAgentImp::~CPierAgentImp()
{
}

HRESULT CPierAgentImp::FinalConstruct()
{
   return S_OK;
}

void CPierAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CPierAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CPierAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CPierAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface(IID_IXBRPier,              this);
   pBrokerInit->RegInterface(IID_IXBRSectionProperties, this);
   pBrokerInit->RegInterface(IID_IXBRMaterial,          this);
   pBrokerInit->RegInterface(IID_IXBRRebar,             this);
   pBrokerInit->RegInterface(IID_IXBRStirrups,          this);
   pBrokerInit->RegInterface(IID_IXBRPointOfInterest,   this);

   return S_OK;
};

STDMETHODIMP CPierAgentImp::Init()
{
   //EAF_AGENT_INIT;

   //
   // Attach to connection points for interfaces this agent depends on
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CPierAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CPierAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CPierAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_PierAgent;
   return S_OK;
}

STDMETHODIMP CPierAgentImp::ShutDown()
{
   //
   // Detach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   hr = pBrokerInit->FindConnectionPoint(IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Unadvise( m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the connection point

   EAF_AGENT_CLEAR_INTERFACE_CACHE;

   return S_OK;
}

//////////////////////////////////////////
// IXBRPier
Float64 CPierAgentImp::GetSkewAngle(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IAngle> objAngle;
   pier->get_SkewAngle(&objAngle);

   Float64 skew;
   objAngle->get_Value(&skew);

   return skew;
}

IndexType CPierAgentImp::GetBearingLineCount(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IBearingLayout> bearingLayout;
   pier->get_BearingLayout(&bearingLayout);

   IndexType nBearingLines;
   bearingLayout->get_BearingLineCount(&nBearingLines);
   return nBearingLines;
}

IndexType CPierAgentImp::GetBearingCount(PierIDType pierID,IndexType brgLineIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IBearingLayout> bearingLayout;
   pier->get_BearingLayout(&bearingLayout);

   IndexType nBearings;
   bearingLayout->get_BearingCount(brgLineIdx,&nBearings);
   return nBearings;
}

Float64 CPierAgentImp::GetBearingLocation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IBearingLayout> bearingLayout;
   pier->get_BearingLayout(&bearingLayout);

   Float64 Xxb;
   bearingLayout->get_BearingLocation(brgLineIdx,brgIdx,&Xxb);
   return Xxb;
}

Float64 CPierAgentImp::GetBearingElevation(PierIDType pierID,IndexType brgLineIdx,IndexType brgIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IBearingLayout> bearingLayout;
   pier->get_BearingLayout(&bearingLayout);

   Float64 Y;
   bearingLayout->get_BearingElevation(brgLineIdx,brgIdx,&Y);
   return Y;
}

IndexType CPierAgentImp::GetColumnCount(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IColumnLayout> columnLayout;
   pier->get_ColumnLayout(&columnLayout);

   IndexType nColumns;
   columnLayout->get_ColumnCount(&nColumns);

   return nColumns;
}

Float64 CPierAgentImp::GetColumnLocation(PierIDType pierID,IndexType colIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IColumnLayout> columnLayout;
   pier->get_ColumnLayout(&columnLayout);

   Float64 Xxb;
   columnLayout->get_ColumnLocation(colIdx,&Xxb);
   return Xxb;
}

Float64 CPierAgentImp::GetColumnHeight(PierIDType pierID,IndexType colIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IColumnLayout> columnLayout;
   pier->get_ColumnLayout(&columnLayout);

   CComPtr<IColumn> column;
   columnLayout->get_Column(colIdx,&column);

   Float64 h;
   column->get_Height(&h);
   return h;
}

Float64 CPierAgentImp::GetTopColumnElevation(PierIDType pierID,IndexType colIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IColumnLayout> columnLayout;
   pier->get_ColumnLayout(&columnLayout);

   Float64 topElev;
   columnLayout->get_TopColumnElevation(colIdx,&topElev);
   return topElev;
}

Float64 CPierAgentImp::GetBottomColumnElevation(PierIDType pierID,IndexType colIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IColumnLayout> columnLayout;
   pier->get_ColumnLayout(&columnLayout);

   Float64 botElev;
   columnLayout->get_BottomColumnElevation(colIdx,&botElev);
   return botElev;
}

pgsTypes::ColumnFixityType CPierAgentImp::GetColumnFixity(PierIDType pierID,IndexType colIdx)
{
   GET_IFACE(IXBRProject,pProject);
   return pProject->GetColumnFixity(pierID,colIdx);
}

Float64 CPierAgentImp::GetMaxColumnHeight(PierIDType pierID)
{
   IndexType nColumns = GetColumnCount(pierID);
   Float64 Hmax = 0;
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 h = GetColumnHeight(pierID,colIdx);
      Hmax = Max(Hmax,h);
   }

   return Hmax;
}

Float64 CPierAgentImp::GetXBeamLength(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   Float64 length;
   xbeam->get_Length(&length);

   return length;
}

void CPierAgentImp::GetUpperXBeamProfile(PierIDType pierID,IShape** ppShape)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   xbeam->get_Profile(1,ppShape); // stage 1 is upper x-beam
}

void CPierAgentImp::GetLowerXBeamProfile(PierIDType pierID,IShape** ppShape)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   xbeam->get_Profile(0,ppShape); // stage 0 is lower x-beam
}

void CPierAgentImp::GetTopSurface(PierIDType pierID,xbrTypes::Stage stage,IPoint2dCollection** ppPoints)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   StageIndexType stageIdx = GetStageIndex(stage);
   xbeam->get_TopSurface(stageIdx,ppPoints);
}

void CPierAgentImp::GetBottomSurface(PierIDType pierID,xbrTypes::Stage stage,IPoint2dCollection** ppPoints)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   StageIndexType stageIdx = GetStageIndex(stage);
   xbeam->get_BottomSurface(stageIdx,ppPoints);
}

Float64 CPierAgentImp::GetCrownPointOffset(PierIDType pierID)
{
   CComPtr<IPoint2d> pnt;
   GetCrownPoint(pierID,&pnt);
   Float64 cpo;
   pnt->get_X(&cpo);
   return cpo;
}

Float64 CPierAgentImp::GetCrownPointLocation(PierIDType pierID)
{
   Float64 cpo = GetCrownPointOffset(pierID);
   Float64 Xcrown = ConvertPierToCurbLineCoordinate(pierID,cpo);
   return Xcrown;
}

Float64 CPierAgentImp::GetElevation(PierIDType pierID,Float64 Xcl)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 elev;
   pier->get_Elevation(Xcl,&elev);
   return elev;
}

Float64 CPierAgentImp::GetCurbToCurbWidth(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);
   Float64 Wcc;
   pier->get_CurbToCurbWidth(clmNormalToAlignment,&Wcc);
   return Wcc;
}

Float64 CPierAgentImp::ConvertCrossBeamToCurbLineCoordinate(PierIDType pierID,Float64 Xxb)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xcl;
   pier->ConvertCrossBeamToCurbLineCoordinate(Xxb,&Xcl);
   return Xcl;
}

Float64 CPierAgentImp::ConvertCurbLineToCrossBeamCoordinate(PierIDType pierID,Float64 Xcl)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xxb;
   pier->ConvertCurbLineToCrossBeamCoordinate(Xcl,&Xxb);
   return Xxb;
}

Float64 CPierAgentImp::ConvertPierToCrossBeamCoordinate(PierIDType pierID,Float64 Xpier)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xxb;
   pier->ConvertPierToCrossBeamCoordinate(Xpier,&Xxb);
   return Xxb;
}

Float64 CPierAgentImp::ConvertCrossBeamToPierCoordinate(PierIDType pierID,Float64 Xxb)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xpier;
   pier->ConvertCrossBeamToPierCoordinate(Xxb,&Xpier);
   return Xpier;
}

Float64 CPierAgentImp::ConvertPierToCurbLineCoordinate(PierIDType pierID,Float64 Xpier)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xcl;
   pier->ConvertPierToCurbLineCoordinate(Xpier,&Xcl);
   return Xcl;
}

Float64 CPierAgentImp::ConvertCurbLineToPierCoordinate(PierIDType pierID,Float64 Xcl)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xpier;
   pier->ConvertCurbLineToPierCoordinate(Xcl,&Xpier);
   return Xpier;
}

//////////////////////////////////////////
// IXBRSectionProperties
Float64 CPierAgentImp::GetDepth(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;
      Float64 H;

      GET_IFACE(IXBRProject,pProject);
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      return D1;
   }
   else
   {
      CComPtr<IPier> pier;
      GetPierModel(pierID,&pier);

      CComPtr<ICrossBeam> xbeam;
      pier->get_CrossBeam(&xbeam);

      StageIndexType stageIdx = GetStageIndex(stage);

      Float64 Xxb = poi.GetDistFromStart();

      Float64 H;
      xbeam->get_Depth(stageIdx,Xxb,&H);
      return H;
   }
}

Float64 CPierAgentImp::GetArea(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      GET_IFACE(IXBRProject,pProject);

      Float64 H;
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1/4;
      }
      else
      {
         return D1*D2;
      }
   }
   else
   {
      CComPtr<IShape> shape;
      GetXBeamShape(pierID,stage,poi,&shape);

      CComPtr<IShapeProperties> shapeProps;
      shape->get_ShapeProperties(&shapeProps);

      Float64 area;
      shapeProps->get_Area(&area);
      return area;
   }
}

Float64 CPierAgentImp::GetIxx(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      GET_IFACE(IXBRProject,pProject);

      Float64 H;
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1*D1*D1/64;
      }
      else
      {
         return D1*D2*D2*D2/12;
      }
   }
   else
   {
      CComPtr<IShape> shape;
      GetXBeamShape(pierID,stage,poi,&shape);

      CComPtr<IShapeProperties> shapeProps;
      shape->get_ShapeProperties(&shapeProps);

      Float64 ixx;
      shapeProps->get_Ixx(&ixx);
      return ixx;
   }
}

Float64 CPierAgentImp::GetIyy(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      GET_IFACE(IXBRProject,pProject);

      Float64 H;
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return M_PI*D1*D1*D1*D1/64;
      }
      else
      {
         return D1*D1*D1*D2/12;
      }
   }
   else
   {
      CComPtr<IShape> shape;
      GetXBeamShape(pierID,stage,poi,&shape);

      CComPtr<IShapeProperties> shapeProps;
      shape->get_ShapeProperties(&shapeProps);

      Float64 iyy;
      shapeProps->get_Iyy(&iyy);
      return iyy;
   }
}

Float64 CPierAgentImp::GetYtop(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      GET_IFACE(IXBRProject,pProject);

      Float64 H;
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return D1/2;
      }
      else
      {
         return D2/2;
      }
   }
   else
   {
      CComPtr<IShape> shape;
      GetXBeamShape(pierID,stage,poi,&shape);

      CComPtr<IShapeProperties> shapeProps;
      shape->get_ShapeProperties(&shapeProps);

      Float64 Ytop;
      shapeProps->get_Ytop(&Ytop);
      return Ytop;
   }
}

Float64 CPierAgentImp::GetYbot(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   if ( poi.IsColumnPOI() )
   {
      CColumnData::ColumnShapeType shapeType;
      Float64 D1, D2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;

      GET_IFACE(IXBRProject,pProject);

      Float64 H;
      pProject->GetColumnProperties(pierID,poi.GetColumnIndex(),&shapeType,&D1,&D2,&columnHeightType,&H);

      if (shapeType == CColumnData::cstCircle)
      {
         return D1/2;
      }
      else
      {
         return D2/2;
      }
   }
   else
   {
      CComPtr<IShape> shape;
      GetXBeamShape(pierID,stage,poi,&shape);

      CComPtr<IShapeProperties> shapeProps;
      shape->get_ShapeProperties(&shapeProps);

      Float64 Ybot;
      shapeProps->get_Ybottom(&Ybot);
      return Ybot;
   }
}

Float64 CPierAgentImp::GetStop(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   Float64 Ixx = GetIxx(pierID,stage,poi);
   Float64 Yt  = GetYtop(pierID,stage,poi);
   return Ixx/Yt;
}

Float64 CPierAgentImp::GetSbot(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   Float64 Ixx = GetIxx(pierID,stage,poi);
   Float64 Yb  = GetYbot(pierID,stage,poi);
   return Ixx/Yb;
}

void CPierAgentImp::GetXBeamShape(PierIDType pierID,const xbrPointOfInterest& poi,IShape** ppShape)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   Float64 Xxb = poi.GetDistFromStart();
   xbeam->get_BasicShape(Xxb,ppShape);
}

void CPierAgentImp::GetXBeamShape(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,IShape** ppShape)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   StageIndexType stageIdx = GetStageIndex(stage);

   Float64 Xxb = poi.GetDistFromStart();
   xbeam->get_Shape(stageIdx,Xxb,ppShape);
}

//////////////////////////////////////////
// IXBRMaterial
Float64 CPierAgentImp::GetXBeamDensity(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   return concrete.WeightDensity;
}

Float64 CPierAgentImp::GetXBeamFc(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   return concrete.Fc;
}

Float64 CPierAgentImp::GetXBeamEc(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   Float64 Ec;
   if ( concrete.bUserEc )
   {
      Ec = concrete.Ec;
   }
   else
   {
      Ec = lrfdConcreteUtil::ModE(concrete.Fc,concrete.StrengthDensity,false);
      if ( lrfdVersionMgr::ThirdEditionWith2005Interims <= lrfdVersionMgr::GetVersion() )
      {
         Ec *= (concrete.EcK1*concrete.EcK2);
      }
   }
   return Ec;
}

Float64 CPierAgentImp::GetXBeamModulusOfRupture(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   Float64 fr = lrfdConcreteUtil::ModRupture(concrete.Fc,(lrfdConcreteUtil::DensityType)concrete.Type);
   Float64 lambda = GetXBeamLambda(pierID);
   return lambda*fr;
}

Float64 CPierAgentImp::GetXBeamLambda(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);

   Float64 lambda = lrfdConcreteUtil::ComputeConcreteDensityModificationFactor((matConcrete::Type)concrete.Type,concrete.StrengthDensity,concrete.bHasFct,concrete.Fct,concrete.Fc);
   return lambda;
}

Float64 CPierAgentImp::GetColumnEc(PierIDType pierID,IndexType colIdx)
{
   // right now, everything uses the same material
   return GetXBeamEc(pierID);
}

void CPierAgentImp::GetRebarProperties(PierIDType pierID,Float64* pE,Float64* pFy,Float64* pFu)
{
   matRebar::Type rebarType;
   matRebar::Grade rebarGrade;
   GET_IFACE(IXBRProject,pProject);
   pProject->GetRebarMaterial(pierID,&rebarType,&rebarGrade);

   *pE  = matRebar::GetE(rebarType,rebarGrade);
   *pFy = matRebar::GetYieldStrength(rebarType,rebarGrade);
   *pFu = matRebar::GetUltimateStrength(rebarType,rebarGrade);
}


//////////////////////////////////////////
// IXBRRebar
void CPierAgentImp::GetRebarSection(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi,IRebarSection** ppRebarSection)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   Float64 Xxb = poi.GetDistFromStart();

   StageIndexType stageIdx = GetStageIndex(stage);

   rebarLayout->CreateRebarSection(Xxb,stageIdx,ppRebarSection);
}

IndexType CPierAgentImp::GetRebarRowCount(PierIDType pierID)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   IndexType nItems;
   rebarLayout->get_Count(&nItems);

#if defined _DEBUG
   // double check to make sure all input is accounted for
   // we used one rebar layout item per input row
   GET_IFACE(IXBRProject,pProject);
   const xbrLongitudinalRebarData& rebarData = pProject->GetLongitudinalRebar(pierID);
   ATLASSERT(nItems == rebarData.RebarRows.size());
#endif

   return nItems;
}

IndexType CPierAgentImp::GetRebarCount(PierIDType pierID,IndexType rowIdx)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   CComPtr<IRebarLayoutItem> rebarLayoutItem;
   rebarLayout->get_Item(rowIdx,&rebarLayoutItem);

   IndexType nBars;
   rebarLayoutItem->get_Count(&nBars);

#if defined _DEBUG
   GET_IFACE(IXBRProject,pProject);
   const xbrLongitudinalRebarData& rebarData = pProject->GetLongitudinalRebar(pierID);
   ATLASSERT( nBars == rebarData.RebarRows[rowIdx].NumberOfBars);
#endif

   return nBars;
}

void CPierAgentImp::GetRebarProfile(PierIDType pierID,IndexType rowIdx,IPoint2dCollection** ppPoints)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   CComPtr<IRebarLayoutItem> rebarLayoutItem;
   rebarLayout->get_Item(rowIdx,&rebarLayoutItem);

#if defined _DEBUG
   IndexType nPatterns;
   rebarLayoutItem->get_Count(&nPatterns);
   ATLASSERT(nPatterns == 1); // we are using one pattern per layout item in the pier model
#endif // _DEBUG

   CComPtr<IRebarPattern> rebarPattern;
   rebarLayoutItem->get_Item(0,&rebarPattern);

   CComQIPtr<ICrossBeamRebarPattern> xbRebarPattern(rebarPattern);

   // all of our patterns are horizontal so it doesn't matter
   // which bar we get the profile for... use bar 0
   xbRebarPattern->get_DisplayProfile(0,ppPoints);

   // The rebar profile is in rebar layout coordinates
   // Convert it to Pier Coordinates

   Float64 XxbStart;
   rebarLayoutItem->get_Start(&XxbStart);
   Float64 XpStart;
   pier->ConvertCrossBeamToPierCoordinate(XxbStart,&XpStart);

   (*ppPoints)->Offset(XxbStart,0);
}

Float64 CPierAgentImp::GetDevLengthFactor(PierIDType pierID,const xbrPointOfInterest& poi,IRebarSectionItem* pRebarSectionItem)
{
   CComPtr<IRebar> rebar;
   pRebarSectionItem->get_Rebar(&rebar);
   USES_CONVERSION;
   CComBSTR name;
   rebar->get_Name(&name);

   matRebar::Size size = lrfdRebarPool::GetBarSize(OLE2CT(name));

   Float64 Ab, db, fy;
   rebar->get_NominalArea(&Ab);
   rebar->get_NominalDiameter(&db);
   rebar->get_YieldStrength(&fy);

   GET_IFACE(IXBRProject,pProject);
   const CConcreteMaterial& concrete = pProject->GetConcrete(pierID);
   Float64 fc = concrete.Fc;

   matConcrete::Type type = matConcrete::Normal;
   bool hasFct = false;
   Float64 Fct = 0.0;

   REBARDEVLENGTHDETAILS details = lrfdRebar::GetRebarDevelopmentLengthDetails(size, Ab, db, fy, (matConcrete::Type)type, fc, hasFct, Fct, concrete.StrengthDensity );


   // Get distances from section cut to ends of bar
   Float64 fra = 1.0;

   Float64 start,end;
   pRebarSectionItem->get_LeftExtension(&start);
   pRebarSectionItem->get_RightExtension(&end);

   if ( start < end )
   {
      // closer to the left end... check for hook... if hooked, consider fully developed
      HookType ht;
      pRebarSectionItem->get_LeftHook(&ht);
      if ( ht != htNone )
      {
         return 1.0;
      }
   }
   else if ( end < start )
   {
      // closer to the right end... check for hook... if hooked, consider fully developed
      HookType ht;
      pRebarSectionItem->get_RightHook(&ht);
      if ( ht != htNone )
      {
         return 1.0;
      }
   }

   // Not a hooked bar... 
   Float64 cut_length = Min(start, end);
   if (0.0 < cut_length)
   {
      Float64 fra = cut_length/details.ldb;
      fra = Min(fra, 1.0);

      return fra;
   }
   else
   {
      ATLASSERT(cut_length == 0.0); // sections shouldn't be cutting bars that don't exist
      return 0.0;
   }
}

Float64 CPierAgentImp::GetRebarRowLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx)
{
   // rebar are modeled in horizontal rows so it doesn't matter which bar we get 
   // the location for. all bars in a horizontal row have the same elevation
   CComPtr<IPoint2d> point;
   GetRebarLocation(pierID,poi,rowIdx,0,&point);

   Float64 y;
   point->get_Y(&y);

   return y;
}

void CPierAgentImp::GetRebarLocation(PierIDType pierID,const xbrPointOfInterest& poi,IndexType rowIdx,IndexType barIdx,IPoint2d** ppPoint)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<ICrossBeam> xbeam;
   pier->get_CrossBeam(&xbeam);

   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   Float64 Xxb = poi.GetDistFromStart();

   CComPtr<IRebarSection> rebarSection;
   rebarLayout->CreateRebarSection(Xxb,1,&rebarSection);

   IndexType nItems;
   rebarSection->get_Count(&nItems);

   CComPtr<IRebarSectionItem> rebarSectionItem;
   rebarSection->get_Item(barIdx,&rebarSectionItem);

   rebarSectionItem->get_Location(ppPoint);
}

Float64 CPierAgentImp::GetRebarDepth(PierIDType pierID,const xbrPointOfInterest& poi,xbrTypes::Stage stage,IPoint2d* pRebarLocation)
{
   Float64 Xcl = ConvertCrossBeamToCurbLineCoordinate(pierID,poi.GetDistFromStart());
   Float64 Ydeck = GetElevation(pierID,Xcl);

   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);
   Float64 tDeck;
   pier->get_DeckThickness(&tDeck);

   Float64 Y;
   pRebarLocation->get_Y(&Y);

   Float64 Yb = Ydeck - Y - tDeck;

   if ( stage == xbrTypes::Stage1 )
   {
      // for Stage 1, measure from the top of the lower cross beam
      CComPtr<ICrossBeam> xbeam;
      pier->get_CrossBeam(&xbeam);
      
      Float64 lower_depth;
      xbeam->get_Depth(0,poi.GetDistFromStart(),&lower_depth);
      
      Float64 full_depth;
      xbeam->get_FullDepth(poi.GetDistFromStart(),&full_depth);

      Float64 upper_depth = full_depth - lower_depth;

      Yb -= upper_depth;
   }

   return Yb;
}

//////////////////////////////////////////
// IXBRStirrups
ZoneIndexType CPierAgentImp::FindStirrupZone(PierIDType pierID,xbrTypes::Stage stage,const xbrPointOfInterest& poi)
{
   ATLASSERT(!poi.IsColumnPOI());

   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   ZoneIndexType zoneIdx = INVALID_INDEX;
   ZoneIndexType zIdx = 0;
   for( const auto& zone : vStirrupZones)
   {
      if (::InRange(zone.Xstart,poi.GetDistFromStart(),zone.Xend) )
      {
         zoneIdx = zIdx;
         break;
      }
      zIdx++;
   }
   ATLASSERT(zoneIdx != INVALID_INDEX); // zone wasn't found???
   return zoneIdx;
}

ZoneIndexType CPierAgentImp::GetStirrupZoneCount(PierIDType pierID,xbrTypes::Stage stage)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   return vStirrupZones.size();
}

void CPierAgentImp::GetStirrupZoneBoundary(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx,Float64* pXstart,Float64* pXend)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   *pXstart = zone.Xstart;
   *pXend   = zone.Xend;
}

Float64 CPierAgentImp::GetStirrupZoneSpacing(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.S;
}

Float64 CPierAgentImp::GetStirrupZoneReinforcement(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.Av_over_S;
}

Float64 CPierAgentImp::GetStirrupLegCount(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.nLegs;
}

IndexType CPierAgentImp::GetStirrupCount(PierIDType pierID,xbrTypes::Stage stage,ZoneIndexType zoneIdx)
{
   std::vector<StirrupZone>& vStirrupZones = GetStirrupZones(pierID,stage);
   const StirrupZone& zone = vStirrupZones[zoneIdx];
   return zone.nStirrups;
}

//////////////////////////////////////////
// IXBRointOfInterest
std::vector<xbrPointOfInterest> CPierAgentImp::GetXBeamPointsOfInterest(PierIDType pierID,PoiAttributeType attrib)
{
   std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   if ( attrib == 0 )
   {
      return vPoi;
   }

   std::vector<xbrPointOfInterest> vFilteredPoi;
   for (const auto& poi : vPoi)
   {
      if ( poi.HasAttribute(attrib) )
      {
         vFilteredPoi.push_back(poi);
      }
   }

   return vFilteredPoi;
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetColumnPointsOfInterest(PierIDType pierID,ColumnIndexType colIdx)
{
   ATLASSERT(false); // not really using column POI just yet
   return std::vector<xbrPointOfInterest>();
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetMomentRatingPointsOfInterest(PierIDType pierID)
{
   return GetRatingPointsOfInterest(pierID,false);
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetShearRatingPointsOfInterest(PierIDType pierID)
{
   return GetRatingPointsOfInterest(pierID,true);
}

Float64 CPierAgentImp::ConvertPoiToPierCoordinate(PierIDType pierID,const xbrPointOfInterest& poi)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xxb = poi.GetDistFromStart();
   Float64 Xp;
   pier->ConvertCrossBeamToPierCoordinate(Xxb,&Xp);
   return Xp;
}

xbrPointOfInterest CPierAgentImp::ConvertPierCoordinateToPoi(PierIDType pierID,Float64 Xp)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   Float64 Xxb;
   pier->ConvertPierToCrossBeamCoordinate(Xp,&Xxb);

   xbrPointOfInterest poi;
   if ( FindXBeamPoi(pierID,Xxb,&poi) )
   {
      return poi;
   }
   poi.SetDistFromStart(Xxb);
   return poi;
}

xbrPointOfInterest CPierAgentImp::GetNearestPointOfInterest(PierIDType pierID,const xbrPointOfInterest& poi)
{
   ATLASSERT(!poi.IsColumnPOI());
   Float64 Xxb = poi.GetDistFromStart();

   const std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   std::vector<xbrPointOfInterest>::const_iterator iter1(vPoi.begin());
   std::vector<xbrPointOfInterest>::const_iterator iter2(iter1+1);
   std::vector<xbrPointOfInterest>::const_iterator end2(vPoi.end());
   for ( ; iter2 != end2; iter1++, iter2++ )
   {
      const xbrPointOfInterest& prevPOI = *iter1;
      const xbrPointOfInterest& nextPOI = *iter2;

      ATLASSERT( !prevPOI.IsColumnPOI() );
      ATLASSERT( !nextPOI.IsColumnPOI() );

      if ( InRange(prevPOI.GetDistFromStart(),Xxb,nextPOI.GetDistFromStart()) )
      {
         Float64 d1 = Xxb - prevPOI.GetDistFromStart();
         Float64 d2 = nextPOI.GetDistFromStart() - Xxb;

         if ( d1 < d2 )
         {
            return prevPOI;
         }
         else
         {
            return nextPOI;
         }
      }
   }

   ATLASSERT(false); // nothing is nearest??? should never happen
   return xbrPointOfInterest();
}

xbrPointOfInterest CPierAgentImp::GetNextPointOfInterest(PierIDType pierID,PoiIDType poiID)
{
   const std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   std::vector<xbrPointOfInterest>::const_iterator begin(vPoi.begin());
   std::vector<xbrPointOfInterest>::const_iterator end(vPoi.end());
   std::vector<xbrPointOfInterest>::const_iterator found( std::find_if(begin, end, FindByID(poiID) ) );
   if ( found == end )
   {
      ATLASSERT(false); // poi not found
      return xbrPointOfInterest();
   }

   xbrPointOfInterest foundPoi(*found);
   if ( found == end-1 )
   {
      // at the last poi so we can't advance to the next
      return foundPoi;
   }

   xbrPointOfInterest poi(*(found+1));
   return poi;
}

xbrPointOfInterest CPierAgentImp::GetPrevPointOfInterest(PierIDType pierID,PoiIDType poiID)
{
   const std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   std::vector<xbrPointOfInterest>::const_iterator begin(vPoi.begin());
   std::vector<xbrPointOfInterest>::const_iterator end(vPoi.end());
   std::vector<xbrPointOfInterest>::const_iterator found( std::find_if(begin, end, FindByID(poiID) ) );
   if ( found == end )
   {
      ATLASSERT(false); // poi not found
      return xbrPointOfInterest();
   }

   xbrPointOfInterest foundPoi(*found);
   if ( found == begin )
   {
      // at the beginning so we can't back up one poi
      return foundPoi;
   }

   xbrPointOfInterest poi(*(found-1));
   return poi;
}

//////////////////////////////////////////
// IXBRProjectEventSink
HRESULT CPierAgentImp::OnProjectChanged()
{
   Invalidate();
   return S_OK;
}

//////////////////////////////////////////
void CPierAgentImp::ValidatePierModel(PierIDType pierID)
{
   std::map<PierIDType,CComPtr<IPier>>::iterator found(m_PierModels.find(pierID));
   if ( found != m_PierModels.end() )
   {
      return;
   }

   CComPtr<IPierEx> pierModel;
   pierModel.CoCreateInstance(CLSID_Pier);
   m_PierModels.insert(std::make_pair(pierID,pierModel));

   GET_IFACE(IXBRProject,pProject);
   const xbrPierData& pierData = pProject->GetPierData(pierID);

   xbrTypes::PierType pierType = pProject->GetPierType(pierID);
   pierModel->put_Type((PierType)pierType);

   // Superstructure Information
   pgsTypes::OffsetMeasurementType curbLineDatum = pierData.GetCurbLineDatum();

   Float64 leftCLO, rightCLO;
   pierData.GetCurbLineOffset(&leftCLO,&rightCLO);
   if ( curbLineDatum == pgsTypes::omtBridge )
   {
      leftCLO  += pierData.GetBridgeLineOffset();
      rightCLO += pierData.GetBridgeLineOffset();
   }
   pierModel->put_CurbLineOffset(qcbLeft, leftCLO);
   pierModel->put_CurbLineOffset(qcbRight,rightCLO);

   if ( pierData.GetDeckSurfaceType() == xbrPierData::Simplified )
   {
      CComPtr<IPoint2dCollection> deckProfile;
      deckProfile.CoCreateInstance(CLSID_Point2dCollection);

      Float64 elevDeck = pierData.GetDeckElevation();

      Float64 sl,sr;
      pierData.GetCrownSlope(&sl,&sr);

      Float64 cpo = pierData.GetCrownPointOffset();

      Float64 elevCP;
      if ( IsZero(cpo) )
      {
         elevCP = elevDeck;
      }
      else
      {
         if ( cpo < 0 )
         {
            elevCP = -cpo*sr + elevDeck;
         }
         else
         {
            ATLASSERT(0 < cpo);
            elevCP = elevDeck + sl*cpo;
         }
      }

      // left curbline point
      Float64 elevLCL = elevCP - sl*(leftCLO - cpo);
      CComPtr<IPoint2d> pntLCL;
      pntLCL.CoCreateInstance(CLSID_Point2d);
      pntLCL->Move(leftCLO,elevLCL);
      deckProfile->Add(pntLCL);

      // Crown Point
      CComPtr<IPoint2d> pntCP;
      pntCP.CoCreateInstance(CLSID_Point2d);
      pntCP->Move(cpo,elevCP);
      deckProfile->Add(pntCP);

      // right curb line point
      Float64 elevRCL = elevCP + sr*(rightCLO - cpo);
      CComPtr<IPoint2d> pntRCL;
      pntRCL.CoCreateInstance(CLSID_Point2d);
      pntRCL->Move(rightCLO,elevRCL);
      deckProfile->Add(pntRCL);

      pierModel->putref_DeckProfile(deckProfile);
   }
   else
   {
      CComPtr<IPoint2dCollection> deckProfile;
      pierData.GetDeckProfile(&deckProfile);
      pierModel->putref_DeckProfile(deckProfile);
   }

   pierModel->put_DeckThickness(pierData.GetDeckThickness());

   CComPtr<IAngle> skew;
   skew.CoCreateInstance(CLSID_Angle);
   skew->FromString(CComBSTR(pierData.GetSkew()));
   pierModel->putref_SkewAngle(skew);


   // Create Cross Beam
   CComPtr<ILinearCrossBeam> xbeam;
   xbeam.CoCreateInstance(CLSID_LinearCrossBeam);

   Float64 H1, H2, H3, H4, X1, X2, X3, X4, W1;
   pierData.GetLowerXBeamDimensions(&H1,&H2,&H3,&H4,&X1,&X2,&X3,&X4,&W1);
   xbeam->put_H1(H1);
   xbeam->put_H2(H2);
   xbeam->put_H3(H3);
   xbeam->put_H4(H4);
   xbeam->put_X1(X1);
   xbeam->put_X2(X2);
   xbeam->put_X3(X3);
   xbeam->put_X4(X4);
   xbeam->put_W1(W1);

   Float64 H5, W2;
   pierData.GetDiaphragmDimensions(&H5,&W2);
   xbeam->put_H5(H5);
   xbeam->put_W2(W2);

   // Create Column Layout
   CComPtr<IColumnLayout> columnLayout;
   columnLayout.CoCreateInstance(CLSID_ColumnLayout);

   ColumnIndexType nColumns = pProject->GetColumnCount(pierID);
   SpacingIndexType nSpaces = nColumns-1;
   columnLayout->put_Uniform(VARIANT_FALSE);
   columnLayout->put_ColumnCount(nColumns);
   for ( SpacingIndexType spaceIdx = 0; spaceIdx < nSpaces; spaceIdx++ )
   {
      Float64 space = pProject->GetColumnSpacing(pierID,spaceIdx);
      columnLayout->put_Spacing(spaceIdx,space);
   }

   Float64 X5 = pProject->GetXBeamLeftOverhang(pierID);
   Float64 X6 = pProject->GetXBeamRightOverhang(pierID);
   columnLayout->put_Overhang(qcbLeft, X5);
   columnLayout->put_Overhang(qcbRight,X6);


   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      CColumnData::ColumnHeightMeasurementType colMeasurementType = pProject->GetColumnHeightMeasurementType(pierID,colIdx);
      Float64 h = pProject->GetColumnHeight(pierID,colIdx);

      CComPtr<IColumn> column;
      columnLayout->get_Column(colIdx,&column);

      if ( colMeasurementType == CColumnData::chtHeight )
      {
         column->put_Height(h);
      }
      else
      {
         column->put_BaseElevation(h);
      }
   }

   // Set the reference column.
   pgsTypes::OffsetMeasurementType refColMeasure;
   ColumnIndexType refColIdx;
   Float64 refColOffset;
   pierData.GetRefColumnLocation(&refColMeasure,&refColIdx,&refColOffset);
   if ( refColMeasure == pgsTypes::omtBridge )
   {
      // the reference column needs to be measured from the alignment
      // for the product model
      Float64 blo = pierData.GetBridgeLineOffset();
      refColOffset += blo;
   }
   columnLayout->SetReferenceColumn(refColIdx,refColOffset);

   //
   // Create the bearing layout
   //
   CComPtr<IBearingLayout> bearingLayout;
   bearingLayout.CoCreateInstance(CLSID_BearingLayout);
   IndexType nBearingLines = pierData.GetBearingLineCount();
   bearingLayout->put_BearingLineCount(nBearingLines);
   for (IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      const xbrBearingLineData& brgLineData = pierData.GetBearingLineData(brgLineIdx);

      bearingLayout->put_BearingLineOffset(brgLineIdx,brgLineData.GetBearingLineOffset());

      pgsTypes::OffsetMeasurementType datum;
      IndexType refBrgIdx;
      Float64 refBrgOffset;
      brgLineData.GetReferenceBearing(&datum,&refBrgIdx,&refBrgOffset);

      if ( datum == pgsTypes::omtBridge )
      {
         // the reference bearing needs to be measured from the alignment
         // for the product model
         refBrgOffset += pierData.GetBridgeLineOffset();
      }
      bearingLayout->SetReferenceBearing(brgLineIdx,refBrgIdx,refBrgOffset);

      IndexType nBearings = brgLineData.GetBearingCount();
      bearingLayout->put_BearingCount(brgLineIdx,nBearings);
      for ( IndexType spaceIdx = 0; spaceIdx < nBearings-1; spaceIdx++ )
      {
         Float64 s = brgLineData.GetSpacing(spaceIdx);
         bearingLayout->put_Spacing(brgLineIdx,spaceIdx,s);
      }
   }

   // finish the pier model (need pier to be complete because we need its geometry to layout the rebar)
   pierModel->putref_CrossBeam(xbeam);
   pierModel->putref_BearingLayout(bearingLayout);
   pierModel->putref_ColumnLayout(columnLayout);

   // XBeam Rebar Layout
   CComPtr<IRebarLayout> rebarLayout;
   xbeam->get_RebarLayout(&rebarLayout);

   Float64 Lxb;
   xbeam->get_Length(&Lxb);

   CComPtr<IRebarFactory> rebar_factory;
   rebar_factory.CoCreateInstance(CLSID_RebarFactory);

   // Rebar factory needs a unit server object for units conversion
   CComPtr<IUnitServer> unitServer;
   unitServer.CoCreateInstance(CLSID_UnitServer);
   HRESULT hr = ConfigureUnitServer(unitServer);
   ATLASSERT(SUCCEEDED(hr));

   CComPtr<IUnitConvert> unit_convert;
   unitServer->get_UnitConvert(&unit_convert);

   matRebar::Type rebarType;
   matRebar::Grade rebarGrade;
   pProject->GetRebarMaterial(pierID,&rebarType,&rebarGrade);

   RebarGrade matGrade = GetRebarGrade(rebarGrade);
   MaterialSpec matSpec = GetRebarSpecification(rebarType);

   const xbrLongitudinalRebarData& rebarData = pProject->GetLongitudinalRebar(pierID);
   for (const auto& row : rebarData.RebarRows)
   {
      CComPtr<IFixedLengthRebarLayoutItem> rebarLayoutItem;
      rebarLayoutItem.CoCreateInstance(CLSID_FixedLengthRebarLayoutItem);

      Float64 Xstart, Xend;
      if ( row.LayoutType == xbrTypes::blLeftEnd )
      {
         Xstart = row.Start;
         Xend   = Xstart + row.Length;
      }
      else if ( row.LayoutType == xbrTypes::blRightEnd )
      {
         Xstart = Lxb - row.Start - row.Length;
         Xend   = Xstart + row.Length;
      }
      else if ( row.LayoutType == xbrTypes::blFullLength )
      {
         Xstart = 0;
         Xend = Lxb;
      }
      else
      {
         ATLASSERT(false);
      }

      rebarLayoutItem->put_Start(Xstart);
      rebarLayoutItem->put_End(Xend);

      BarSize matSize = GetBarSize(row.BarSize);

      CComPtr<IRebar> rebar;
      rebar_factory->CreateRebar(matSpec,matGrade,matSize,unit_convert,0,&rebar);

      Float64 db;
      rebar->get_NominalDiameter(&db);


      CComPtr<ICrossBeamRebarPattern> rebarPattern;
      rebarPattern.CoCreateInstance(CLSID_CrossBeamRebarPattern);
      rebarPattern->putref_CrossBeam(xbeam);
      rebarPattern->putref_Rebar(rebar);
      rebarPattern->put_Datum((CrossBeamRebarDatum)row.Datum);
      rebarPattern->put_Cover(row.Cover);
      rebarPattern->put_Count(row.NumberOfBars);
      rebarPattern->put_Spacing(row.BarSpacing);

      // Assuming 90 degree hooks
      if ( row.bHookStart )
      {
         rebarPattern->put_Hook(qcbLeft,ht90);
      }

      if ( row.bHookEnd )
      {
         rebarPattern->put_Hook(qcbRight,ht90);
      }

      rebarLayoutItem->AddRebarPattern(rebarPattern);

      rebarLayout->Add(rebarLayoutItem);
   }
}

void CPierAgentImp::GetPierModel(PierIDType pierID,IPier** ppPierModel)
{
   ValidatePierModel(pierID);

   std::map<PierIDType,CComPtr<IPier>>::iterator found(m_PierModels.find(pierID));
   ATLASSERT( found != m_PierModels.end() );
   found->second.CopyTo(ppPierModel);
}

void CPierAgentImp::Invalidate()
{
   m_NextPoiID = 0;
   m_PierModels.clear();
   m_XBeamPoi.clear();
   m_StirrupZones[xbrTypes::Stage1].clear();
   m_StirrupZones[xbrTypes::Stage2].clear();
}

std::vector<CPierAgentImp::StirrupZone>& CPierAgentImp::GetStirrupZones(PierIDType pierID,xbrTypes::Stage stage)
{
   std::map<PierIDType,std::vector<StirrupZone>>::iterator found(m_StirrupZones[stage].find(pierID));
   if ( found == m_StirrupZones[stage].end() )
   {
      ValidateStirrupZones(pierID,stage);
      found = m_StirrupZones[stage].find(pierID);
   }

   return found->second;
}

void CPierAgentImp::ValidateStirrupZones(PierIDType pierID,xbrTypes::Stage stage)
{
   GET_IFACE(IXBRProject,pProject);
   const xbrPierData& pierData = pProject->GetPierData(pierID);
   std::vector<StirrupZone> vStirrupZones;
   if ( stage == xbrTypes::Stage1 )
   {
      ValidateStirrupZones(pierID,pierData.GetLowerXBeamStirrups(),&vStirrupZones);
   }
   else
   {
      if ( pProject->GetPierType(pierID) == xbrTypes::pctIntegral )
      {
         // there are only full depth stirrups for integral piers
         ValidateStirrupZones(pierID,pierData.GetFullDepthStirrups(), &vStirrupZones);
      }
   }

   m_StirrupZones[stage].insert(std::make_pair(pierID,vStirrupZones));
}

void CPierAgentImp::ValidateStirrupZones(PierIDType pierID,const xbrStirrupData& stirrupData,std::vector<StirrupZone>* pvStirrupZones)
{
   // Map the input stirrup zones into actual stirrup zones
   pvStirrupZones->clear();

   if ( stirrupData.Zones.size() == 0 )
   {
      return;
   }

   Float64 L = GetXBeamLength(pierID);

   GET_IFACE(IXBRProject,pProject);
   matRebar::Type type;
   matRebar::Grade grade;
   pProject->GetRebarMaterial(pierID,&type,&grade);
   lrfdRebarPool* pRebarPool = lrfdRebarPool::GetInstance();

   if ( stirrupData.Symmetric )
   {
      Float64 Xstart = 0;
      std::vector<xbrStirrupData::StirrupZone>::const_iterator iter(stirrupData.Zones.begin());
      std::vector<xbrStirrupData::StirrupZone>::const_iterator end(stirrupData.Zones.end());
      for ( ; iter != end; iter++ )
      {
         const xbrStirrupData::StirrupZone& zone(*iter);
         const matRebar* pRebar = pRebarPool->GetRebar(type,grade,zone.BarSize);
         Float64 av = pRebar->GetNominalArea();
         Float64 Av = av*zone.nBars;
         
         ATLASSERT(!IsZero(zone.BarSpacing));// UI should have prevented this
         Float64 Av_over_S = Av/zone.BarSpacing;

         StirrupZone myZone;
         myZone.Av_over_S = Av_over_S;
         myZone.BarSize = zone.BarSize;
         myZone.S = zone.BarSpacing;
         myZone.nLegs = zone.nBars;

         Float64 Xend = (iter+1 == end ? Xstart + L/2 : Xstart + zone.Length);
         bool bDone = false;
         if ( L/2 < Xend )
         {
            Xend = L/2;
            bDone = true;
         }
         myZone.Xstart = Xstart;
         myZone.Xend = Xend;
         ATLASSERT(myZone.Xstart < myZone.Xend);
         myZone.Length = Xend - Xstart;

         pvStirrupZones->push_back(myZone);
         Xstart = Xend;
         if ( bDone )
         {
            break;
         }
      }

      // make sure last zone goes to centerline of xbeam
      if ( pvStirrupZones->back().Xend < L/2 )
      {
         pvStirrupZones->back().Xend = L/2;
         pvStirrupZones->back().Length = pvStirrupZones->back().Xend - pvStirrupZones->back().Xstart;
      }

      // we've gone half-way... double the size of the last zone saved
      // This is the zone that is symmetric about the centerline of the XBeam
      pvStirrupZones->back().Length *= 2;
      pvStirrupZones->back().Xend = pvStirrupZones->back().Xstart + pvStirrupZones->back().Length;

      // fill this vector with all the previously define stirrup zones. fill in reverse
      // order and skip the center zone
      std::vector<StirrupZone> myZones;
      myZones.insert(myZones.begin(),pvStirrupZones->rbegin()+1,pvStirrupZones->rend());
      // Adjust the Start/End of the zone
      for (auto& myZone : myZones)
      {
         Float64 Xstart = myZone.Xstart;
         Float64 Xend   = myZone.Xend;

         myZone.Xstart = L - Xend;
         myZone.Xend   = L - Xstart;
         ATLASSERT(myZone.Xstart < myZone.Xend);
      }

      // put the mirror zones into the target vector
      pvStirrupZones->insert(pvStirrupZones->end(),myZones.begin(),myZones.end());
   }
   else
   {
      Float64 Xstart = 0;
      std::vector<xbrStirrupData::StirrupZone>::const_iterator iter(stirrupData.Zones.begin());
      std::vector<xbrStirrupData::StirrupZone>::const_iterator end(stirrupData.Zones.end());
      for ( ; iter != end; iter++ )
      {
         const xbrStirrupData::StirrupZone& zone(*iter);
         const matRebar* pRebar = pRebarPool->GetRebar(type,grade,zone.BarSize);
         Float64 av = pRebar->GetNominalArea();
         Float64 Av = av*zone.nBars;
         
         Float64 Av_over_S = IsZero(zone.BarSpacing) ? 0 : Av/zone.BarSpacing;

         StirrupZone myZone;
         myZone.Av_over_S = Av_over_S;
         myZone.BarSize = zone.BarSize;
         myZone.S = zone.BarSpacing;
         myZone.nLegs = zone.nBars;

         Float64 Xend = (iter+1 == end ? Xstart + L : Xstart + zone.Length);
         bool bDone = false;
         if ( L < Xend )
         {
            Xend = L;
            bDone = true;
         }
         myZone.Xstart = Xstart;
         myZone.Xend = Xend;
         ATLASSERT(myZone.Xstart < myZone.Xend);
         myZone.Length = Xend - Xstart;

         pvStirrupZones->push_back(myZone);
         Xstart = Xend;
         if ( bDone )
         {
            break;
         }
      }

      // make sure the last zone ends at L
      pvStirrupZones->back().Xend = L;
      pvStirrupZones->back().Length = pvStirrupZones->back().Xstart - pvStirrupZones->back().Xend;
   }

   // Compute the number of stirrups in each zone
   // This is a little inefficient because we are looping through all the zones again, but it is easier and more clear
   for (auto& zone : *pvStirrupZones)
   {
      ATLASSERT(0 <= zone.Xstart && zone.Xstart <= L);
      ATLASSERT(0 <= zone.Xend   && zone.Xend   <= L);
      ATLASSERT(zone.Xstart < zone.Xend);

      zone.nStirrups = IndexType(zone.Length/zone.S) + 1;
   }
}

std::vector<xbrPointOfInterest>& CPierAgentImp::GetPointsOfInterest(PierIDType pierID)
{
   std::map<PierIDType,std::vector<xbrPointOfInterest>>::iterator found(m_XBeamPoi.find(pierID));
   if ( found == m_XBeamPoi.end() )
   {
      ValidatePointsOfInterest(pierID);
      found = m_XBeamPoi.find(pierID);
      ATLASSERT(found != m_XBeamPoi.end());
   }

   return found->second;
}

void CPierAgentImp::ValidatePointsOfInterest(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);

   Float64 delta = ::ConvertToSysUnits(0.001,unitMeasure::Feet);

   Float64 H1, H2, X1, X2;
   Float64 H3, H4, X3, X4;
   Float64 W;
   pProject->GetLowerXBeamDimensions(pierID,&H1,&H2,&H3,&H4,&X1,&X2,&X3,&X4,&W);

   Float64 L = GetXBeamLength(pierID);

   std::vector<xbrPointOfInterest> vPoi;

   // Put a POI at every location the section changes depth
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,0.0,POI_SECTIONCHANGE));

   if ( !IsZero(X1) )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X1,POI_SECTIONCHANGE));
   }

   Float64 crownPoint = GetCrownPointLocation(pierID);
   if ( 0 < crownPoint && crownPoint < L )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,crownPoint,POI_SECTIONCHANGE));
   }

   if ( !IsZero(X3) )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L-X3,POI_SECTIONCHANGE));
   }

   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L,POI_SECTIONCHANGE));

   // Put POI at each side of a column so we pick up jumps in the moment and shear diagrams
   Float64 LeftOH = pProject->GetXBeamLeftOverhang(pierID); 

   // left column
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH-delta,POI_COLUMNDELTA));
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH,POI_COLUMN));
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH+delta,POI_COLUMNDELTA));

   // put POI at faces of left column
   CColumnData::ColumnShapeType shapeType;
   Float64 D1, D2;
   CColumnData::ColumnHeightMeasurementType measureType;
   Float64 H;
   pProject->GetColumnProperties(pierID,0,&shapeType,&D1,&D2,&measureType,&H);
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH-D1/2,POI_FACEOFCOLUMN));
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,LeftOH+D1/2,POI_FACEOFCOLUMN));

   ColumnIndexType nColumns = pProject->GetColumnCount(pierID);
   if ( 1 < nColumns )
   {
      SpacingIndexType nSpaces = nColumns - 1;
      Float64 X = LeftOH;
      for ( SpacingIndexType spaceIdx = 0; spaceIdx < nSpaces; spaceIdx++ )
      {
         Float64 space = pProject->GetColumnSpacing(pierID,spaceIdx);
  
         // put POI at mid-point between columns
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X+space/2,POI_MIDPOINT));

         X += space;

         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X-delta,POI_COLUMNDELTA));
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X,POI_COLUMN));
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X+delta,POI_COLUMNDELTA));

         // put POI at faces of column
         pProject->GetColumnProperties(pierID,spaceIdx,&shapeType,&D1,&D2,&measureType,&H);
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X-D1/2,POI_FACEOFCOLUMN));
         vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X+D1/2,POI_FACEOFCOLUMN));
      }
   }

   // need to pick up shear jumps at points of concentrated load
   // put POI at at bearing locations
   IndexType nBrgLines = pProject->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBrgLines; brgLineIdx++ )
   {
      xbrTypes::ReactionLoadType reactionType = pProject->GetBearingReactionType(pierID,brgLineIdx);

      IndexType nBearings = pProject->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = GetBearingLocation(pierID,brgLineIdx,brgIdx);

         if ( reactionType == xbrTypes::rltConcentrated )
         {
            // POI at CL Bearing
            //vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg-delta));
            vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg,POI_BRG));
            //vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg+delta));
         }
         else
         {
            // POI at start/end of distributed load
            Float64 W = pProject->GetBearingWidth(pierID,brgLineIdx,brgIdx);
            vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg-W/2));
            vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xbrg+W/2));
         }
      }
   }

   // Need POI on a one-foot grid
   Float64 step = ::ConvertToSysUnits(1.0,unitMeasure::Feet);
   Float64 Xpoi = 0;
   while ( ::IsLE(Xpoi,(L/2 - step)) )
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,Xpoi,POI_GRID));
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L-Xpoi,POI_GRID));

      Xpoi += step;
   }
   vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,L/2));

   GET_IFACE(IXBRProductForces,pProductForces);
   std::vector<Float64> vWheelLineLocations = pProductForces->GetWheelLineLocations(pierID);

   // Put POI at every place a wheel line load is applied
   for (const auto& X : vWheelLineLocations)
   {
      vPoi.push_back(xbrPointOfInterest(m_NextPoiID++,X,POI_WHEELLINE));
   }


   SimplifyPOIList(vPoi); // sorts, merges, and removes duplicates

   m_XBeamPoi.insert(std::make_pair(pierID,vPoi));
}

void CPierAgentImp::SimplifyPOIList(std::vector<xbrPointOfInterest>& vPoi)
{
   // put POI in left-to-right sorted order
   std::sort(vPoi.begin(),vPoi.end());

   // Merge the attributes of POI thare are at the same location so when we remove
   // duplicates the attributes don't get lost
   std::vector<xbrPointOfInterest>::iterator result = std::adjacent_find(vPoi.begin(),vPoi.end(),ComparePoiLocation);
   while ( result != vPoi.end() )
   {
      xbrPointOfInterest& poi1 = *result;
      xbrPointOfInterest& poi2 = *(result+1);
      poi1.SetAttributes(poi1.GetAttributes() | poi2.GetAttributes());
      vPoi.erase(result+1);
      result = std::adjacent_find(vPoi.begin(),vPoi.end(),ComparePoiLocation);
   }
}

bool CPierAgentImp::FindXBeamPoi(PierIDType pierID,Float64 Xxb,xbrPointOfInterest* pPoi)
{
   std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);
   for (const auto& poi : vPoi)
   {
      if ( IsEqual(poi.GetDistFromStart(),Xxb) )
      {
         *pPoi = poi;
         return true;
      }
   }

   return false;
}

void CPierAgentImp::GetCrownPoint(PierIDType pierID,IPoint2d** ppPoint)
{
   CComPtr<IPier> pier;
   GetPierModel(pierID,&pier);

   CComPtr<IPoint2dCollection> deckProfile;
   pier->get_DeckProfile(&deckProfile);

   IndexType idx = 0;
   IndexType maxIdx = INVALID_INDEX; // index of point where Ymax occurs
   Float64 Ymax = -DBL_MAX;
   Float64 lastY;
   bool bFlat = true;

   CComPtr<IEnumPoint2d> enumPoints;
   deckProfile->get__Enum(&enumPoints);
   CComPtr<IPoint2d> pnt;
   while ( enumPoints->Next(1,&pnt,nullptr) != S_FALSE )
   {
      Float64 y;
      pnt->get_Y(&y);

      // if this is the first point, capture y as the last y evaluated
      if ( idx == 0 )
      {
         lastY = y;
      }

      // if we have not already determined that the profile is not flat
      // and if the current y is not equal to the last y, then the profile
      // is not flat
      if ( bFlat && !IsEqual(y,lastY) )
      {
         ATLASSERT(bFlat == true); // once we determine the profile is not flat, we should never get here
         bFlat = false;
      }

      if ( Ymax < y )
      {
         Ymax = y;
         maxIdx = idx;
      }

      idx++;
      pnt.Release();
   }

   if ( bFlat )
   {
      // the profile is flat... use the alignment point as the crown point
      enumPoints->Reset();
      while ( enumPoints->Next(1,&pnt,nullptr) != S_FALSE )
      {
         Float64 x;
         pnt->get_X(&x);
         if ( IsZero(x) || 0 < x )
         {
            // this is the first point at or after zero
            // (the alignment is at zero)
            if ( IsZero(x) )
            {
               pnt.CopyTo(ppPoint);
               return;
            }
            else
            {
               CComPtr<IPoint2d> pntAlignment;
               pntAlignment.CoCreateInstance(CLSID_Point2d);
               Float64 y;
               pnt->get_Y(&y);
               pntAlignment->Move(0,y);
               pntAlignment.CopyTo(ppPoint);
               return;
            }
         }
         pnt.Release();
      }
   }

   ATLASSERT( !bFlat ); // should never get here if the profile is not flat

   deckProfile->get_Item(maxIdx,ppPoint);
}

std::vector<xbrPointOfInterest> CPierAgentImp::GetRatingPointsOfInterest(PierIDType pierID,bool bShear)
{
   // load rate at grid points, bearings, mid-point between columns, section change poi, face of columns, and centerline columns
   // for shear, don't include any poi that are between faces of column
   std::vector<xbrPointOfInterest>& vPoi = GetPointsOfInterest(pierID);

   bool bOverColumn = false;
   std::vector<xbrPointOfInterest> vFilteredPoi;
   for (const auto& poi : vPoi)
   {
      if ( poi.HasAttribute(POI_FACEOFCOLUMN) )
      {
         if ( bShear )
         {
            // only keep track if we are between faces of column
            // if we are getting shear poi
            bOverColumn = !bOverColumn;
         }
         vFilteredPoi.push_back(poi);
      }

      if ( !bOverColumn && 
           (poi.HasAttribute(POI_GRID) || 
            poi.HasAttribute(POI_BRG)  || 
            poi.HasAttribute(POI_MIDPOINT) || 
            poi.HasAttribute(POI_COLUMN) || 
            poi.HasAttribute(POI_SECTIONCHANGE)) 
         )
      {
         vFilteredPoi.push_back(poi);
      }
   }

   ATLASSERT(bOverColumn == false);

   return vFilteredPoi;
}
