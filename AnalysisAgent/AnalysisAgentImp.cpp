///////////////////////////////////////////////////////////////////////
// PGSuper - Prestressed Girder SUPERstructure Design and Analysis
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

// AnalysisAgentImp.cpp : Implementation of CAnalysisAgentImp
#include "stdafx.h"
#include "AnalysisAgent.h"
#include "AnalysisAgentImp.h"

#include <IFace\Pier.h>
#include <IFace\PointOfInterest.h>

#include <Units\SysUnitsMgr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalysisAgentImp
CAnalysisAgentImp::CAnalysisAgentImp()
{
   m_pBroker = 0;
}

CAnalysisAgentImp::~CAnalysisAgentImp()
{
}

HRESULT CAnalysisAgentImp::FinalConstruct()
{
   return S_OK;
}

void CAnalysisAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CAnalysisAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CAnalysisAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   return S_OK;
}

STDMETHODIMP CAnalysisAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface( IID_IXBRProductForces, this);
   pBrokerInit->RegInterface( IID_IXBRAnalysisResults,    this );

   return S_OK;
};

STDMETHODIMP CAnalysisAgentImp::Init()
{
   //EAF_AGENT_INIT;
   return AGENT_S_SECONDPASSINIT;
}

STDMETHODIMP CAnalysisAgentImp::Init2()
{
   //
   // Attach to connection points
   //
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   CComPtr<IConnectionPoint> pCP;
   HRESULT hr = S_OK;

   // Connection point for the bridge description
   hr = pBrokerInit->FindConnectionPoint( IID_IXBRProjectEventSink, &pCP );
   ATLASSERT( SUCCEEDED(hr) );
   hr = pCP->Advise( GetUnknown(), &m_dwProjectCookie );
   ATLASSERT( SUCCEEDED(hr) );
   pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.

   return S_OK;
}

STDMETHODIMP CAnalysisAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CAnalysisAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_AnalysisAgent;
   return S_OK;
}

STDMETHODIMP CAnalysisAgentImp::ShutDown()
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

CAnalysisAgentImp::ModelData* CAnalysisAgentImp::GetModelData(PierIDType pierID)
{
   BuildModel(pierID); // builds or updates the model if necessary
   std::map<PierIDType,ModelData>::iterator found = m_ModelData.find(pierID);
   ATLASSERT( found != m_ModelData.end() ); // should always find it!
   ModelData* pModelData = &(*found).second;
   return pModelData;
}

void CAnalysisAgentImp::BuildModel(PierIDType pierID)
{
   std::map<PierIDType,ModelData>::iterator found = m_ModelData.find(pierID);
   if ( found == m_ModelData.end() )
   {
      ModelData model_data;
      m_ModelData.insert( std::make_pair(pierID,model_data) );
      found = m_ModelData.find(pierID);
   }

   ModelData* pModelData = &(found->second);

   if ( pModelData->m_Model != NULL )
   {
      return;
   }

   pModelData->m_Model.CoCreateInstance(CLSID_Fem2dModel);
   pModelData->m_Model->put_Name(CComBSTR(_T("XBeamRate")));

   // Build the frame model
   GET_IFACE(IXBRProject,pProject);
   GET_IFACE(IXBRPier,pPier);
   GET_IFACE(IXBRMaterial,pMaterial);
   GET_IFACE(IXBRSectionProperties,pSectProp);

   // some dummy dimensions
   Float64 leftOverhang = pProject->GetXBeamLeftOverhang(pierID);
   Float64 rightOverhang = pProject->GetXBeamRightOverhang(pierID);
   IndexType nColumns = pProject->GetColumnCount(pierID);

   Float64 L = pProject->GetXBeamLength(pierID);

   Float64 Exb = pMaterial->GetXBeamEc(pierID);
   Float64 Axb = pSectProp->GetArea(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,L/2));
   Float64 Ixb = pSectProp->GetIxx(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,L/2));
   Float64 EAb = Exb*Axb;
   Float64 EIb = Exb*Ixb;

   CComPtr<IFem2dJointCollection> joints;
   pModelData->m_Model->get_Joints(&joints);

   CComPtr<IFem2dMemberCollection> members;
   pModelData->m_Model->get_Members(&members);

   JointIDType jntID = 0;

   Float64 Xs = 0.0;
   Float64 Xe = leftOverhang;

   // Start joint at left end of cross beam
   CComPtr<IFem2dJoint> joint;
   joints->Create(jntID++,Xs,0,&joint);

   // create joint at top of first column
   joint.Release();
   joints->Create(jntID++,Xe,0,&joint);

   MemberIDType xbeamMbrID = 0;
   MemberIDType columnMbrID = -1;

   // create left overhang member
   CComPtr<IFem2dMember> mbr;
   members->Create(xbeamMbrID++,jntID-2,jntID-1,EAb,EIb,&mbr);
   CapBeamMember capMbr;
   capMbr.Xs = Xs;
   capMbr.Xe = Xe;
   capMbr.mbrID = xbeamMbrID-1;
   pModelData->m_CapBeamMembers.push_back(capMbr);

   Float64 columnSpacing = pProject->GetColumnSpacing(pierID);
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 columnHeight = pPier->GetColumnHeight(pierID,colIdx);
      Float64 space = (colIdx < nColumns-1 ? columnSpacing : rightOverhang);

      // create joint at bottom of column
      joint.Release();
      joints->Create(jntID++,Xe,-columnHeight,&joint);

      joint->Support(); // fully fixed

      // create column member
      Float64 Ecol = pMaterial->GetColumnEc(pierID,colIdx);
      Float64 Acol = pSectProp->GetArea(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,colIdx,0.0));
      Float64 Icol = pSectProp->GetIyy(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,colIdx,0.0));
      mbr.Release();
      members->Create(columnMbrID--,jntID-2,jntID-1,Ecol*Acol,Ecol*Icol,&mbr);

      Xs = Xe;
      Xe += space;

      // create next top of column joint
      joint.Release();
      joints->Create(jntID++,Xe,0,&joint);

      // create cross beam member
      mbr.Release();
      members->Create(xbeamMbrID++,jntID-3,jntID-1,EAb,EIb,&mbr);
      CapBeamMember capMbr;
      capMbr.Xs = Xs;
      capMbr.Xe = Xe;
      capMbr.mbrID = xbeamMbrID-1;
      pModelData->m_CapBeamMembers.push_back(capMbr);
   }

   // Assign POIs
   CComPtr<IFem2dPOICollection> femPois;
   pModelData->m_Model->get_POIs(&femPois);
   PoiIDType femPoiID = 0;
   GET_IFACE(IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID);
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      MemberIDType mbrID;
      Float64 mbrLocation;
      GetFemModelLocation(pModelData,poi,&mbrID,&mbrLocation);

      CComPtr<IFem2dPOI> femPoi;
      femPois->Create(femPoiID,mbrID,mbrLocation,&femPoi);
      pModelData->m_PoiMap.insert(std::make_pair(poi.GetID(),femPoiID));
      femPoiID++;
   }

   ApplyDeadLoad(pierID,pModelData);

#if defined _DEBUG
   CComQIPtr<IStructuredStorage2> ss(pModelData->m_Model);
   CComPtr<IStructuredSave2> save;
   save.CoCreateInstance(CLSID_StructuredSave2);
   save->Open(CComBSTR(_T("XBeamRate_Fem2d.xml")));
   ss->Save(save);
   save->Close();
   save.Release();
   ss.Release();
#endif // _DEBUG
}

void CAnalysisAgentImp::ApplyDeadLoad(PierIDType pierID,ModelData* pModelData)
{
   ApplyLowerXBeamDeadLoad(pierID,pModelData);
   ApplyUpperXBeamDeadLoad(pierID,pModelData);
   ApplySuperstructureDeadLoadReactions(pierID,pModelData);
}

void CAnalysisAgentImp::ApplyLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData)
{
   ValidateLowerXBeamDeadLoad(pierID,pModelData);

   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   LoadCaseIDType loadCaseID = GetLoadCaseID(xbrTypes::pftLowerXBeam);
   CComPtr<IFem2dLoading> loading;
   loadings->Create(loadCaseID,&loading);

   CComPtr<IFem2dDistributedLoadCollection> distLoads;
   loading->get_DistributedLoads(&distLoads);

   LoadIDType loadID = 0;
   std::vector<LowerXBeamLoad>::iterator iter(pModelData->m_LowerXBeamLoads.begin());
   std::vector<LowerXBeamLoad>::iterator end(pModelData->m_LowerXBeamLoads.end());
   for ( ; iter != end; iter++ )
   {
      LowerXBeamLoad& load(*iter);

      MemberIDType startMbrID, endMbrID;
      Float64 startMbrLocation , endMbrLocation;
      GetFemModelLocation(pModelData,xbrPointOfInterest(INVALID_ID,load.Xs),&startMbrID,&startMbrLocation);
      GetFemModelLocation(pModelData,xbrPointOfInterest(INVALID_ID,load.Xe),&endMbrID,  &endMbrLocation);

      if ( startMbrID == endMbrID )
      {
         CComPtr<IFem2dDistributedLoad> distLoad;
         distLoads->Create(loadID++,startMbrID,loadDirFy,startMbrLocation,endMbrLocation,-load.Ws,-load.We,lotMember,&distLoad);
      }
      else
      {
         CComPtr<IFem2dMemberCollection> members;
         pModelData->m_Model->get_Members(&members);

         CComPtr<IFem2dJointCollection> joints;
         pModelData->m_Model->get_Joints(&joints);

         // load from start to end of first member
         CComPtr<IFem2dMember> mbr;
         members->Find(startMbrID,&mbr);

         Float64 L;
         mbr->get_Length(&L);

         JointIDType jntID;
         mbr->get_EndJoint(&jntID);
         CComPtr<IFem2dJoint> joint;
         joints->Find(jntID,&joint);
         Float64 X;
         joint->get_X(&X);

         Float64 Ws = load.Ws;
         Float64 We = ::LinInterp(X - load.Xs,load.Ws,load.We,load.Xe - load.Xs);

         CComPtr<IFem2dDistributedLoad> distLoad;

         if ( !IsEqual(startMbrLocation,L) )
         {
            distLoads->Create(loadID++,startMbrID,loadDirFy,startMbrLocation,L,-Ws,-We,lotMember,&distLoad);
         }

         Ws = We;

         // load all intermediate members
         for ( MemberIDType mbrID = startMbrID+1; mbrID < endMbrID; mbrID++ )
         {
            mbr.Release();
            members->Find(mbrID,&mbr);
            mbr->get_EndJoint(&jntID);
            joint.Release();
            joints->Find(jntID,&joint);
            joint->get_X(&X);

            mbr->get_Length(&L);

            We = ::LinInterp(X - load.Xs,load.Ws,load.We,load.Xe - load.Xs);

            distLoad.Release();
            distLoads->Create(loadID++,mbrID,loadDirFy,0,L,-Ws,-We,lotMember,&distLoad);

            Ws = We;
         }

         // load start of last member to the end of the loading
         distLoad.Release();
         if ( !IsZero(endMbrLocation) )
         {
            distLoads->Create(loadID++,endMbrID,loadDirFy,0.0,endMbrLocation,-Ws,-load.We,lotMember,&distLoad);
         }
      }
   }
}

void CAnalysisAgentImp::ApplyUpperXBeamDeadLoad(PierIDType pierID,ModelData* pModelData)
{
   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   LoadCaseIDType loadCaseID = GetLoadCaseID(xbrTypes::pftUpperXBeam);
   CComPtr<IFem2dLoading> loading;
   loadings->Create(loadCaseID,&loading);

   CComPtr<IFem2dDistributedLoadCollection> distLoads;
   loading->get_DistributedLoads(&distLoads);

   LoadIDType loadID = 0;

   Float64 w = GetUpperCrossBeamLoading(pierID);
   BOOST_FOREACH(CapBeamMember& capMbr,pModelData->m_CapBeamMembers)
   {
      CComPtr<IFem2dDistributedLoad> distLoad;
      distLoads->Create(loadID++,capMbr.mbrID,loadDirFy,0,-1,-w,-w,lotMember,&distLoad);
   }
}

void CAnalysisAgentImp::ApplySuperstructureDeadLoadReactions(PierIDType pierID,ModelData* pModelData)
{
   GET_IFACE(IXBRPier,pPier);
   GET_IFACE(IXBRProject,pProject);

   LoadCaseIDType dcLoadCaseID = GetLoadCaseID(xbrTypes::pftDCReactions);
   LoadCaseIDType dwLoadCaseID = GetLoadCaseID(xbrTypes::pftDWReactions);

   LoadIDType dcLoadID = 0;
   LoadIDType dwLoadID = 0;

   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   CComPtr<IFem2dLoading> dcLoading;
   loadings->Create(dcLoadCaseID,&dcLoading);

   CComPtr<IFem2dLoading> dwLoading;
   loadings->Create(dwLoadCaseID,&dwLoading);

   CComPtr<IFem2dPointLoadCollection> dcPointLoads;
   dcLoading->get_PointLoads(&dcPointLoads);

   CComPtr<IFem2dPointLoadCollection> dwPointLoads;
   dwLoading->get_PointLoads(&dwPointLoads);

   IndexType nBearingLines = pProject->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pProject->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = pPier->GetBearingLocation(pierID,brgLineIdx,brgIdx);

         MemberIDType mbrID;
         Float64 mbrLocation;
         GetCapBeamFemModelLocation(pModelData,Xbrg,&mbrID,&mbrLocation);

         Float64 DC, DW;
         pProject->GetBearingReactions(pierID,brgLineIdx,brgIdx,&DC,&DW);

         CComPtr<IFem2dPointLoad> dcLoad;
         dcPointLoads->Create(dcLoadID++,mbrID,mbrLocation,0.0,-DC,0.0,lotGlobal,&dcLoad);

         CComPtr<IFem2dPointLoad> dwLoad;
         dwPointLoads->Create(dwLoadID++,mbrID,mbrLocation,0.0,-DW,0.0,lotGlobal,&dwLoad);
      }
   }
}

void CAnalysisAgentImp::ValidateLowerXBeamDeadLoad(PierIDType pierID,ModelData* pModelData)
{
   if ( 0 < pModelData->m_LowerXBeamLoads.size() )
   {
      return;
   }

   GET_IFACE(IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(pierID,POI_SECTIONCHANGE);

   GET_IFACE(IXBRSectionProperties,pSectProp);
   GET_IFACE(IXBRMaterial,pMaterial);

   Float64 density = pMaterial->GetXBeamDensity(pierID);
   Float64 unitWeight = density*unitSysUnitsMgr::GetGravitationalAcceleration();

   std::vector<xbrPointOfInterest>::iterator iter(vPoi.begin());
   std::vector<xbrPointOfInterest>::iterator end(vPoi.end());
   Float64 Astart = pSectProp->GetArea(pierID,xbrTypes::Stage1,*iter);
   Float64 Wstart = unitWeight*Astart;
   Float64 Xstart = (*iter).GetDistFromStart();

   iter++;
   for ( ; iter != end; iter++ )
   {
      Float64 Aend = pSectProp->GetArea(pierID,xbrTypes::Stage1,*iter);
      Float64 Wend = unitWeight*Aend;
      Float64 Xend = (*iter).GetDistFromStart();

      LowerXBeamLoad load;
      load.Xs = Xstart;
      load.Xe = Xend;
      load.Ws = Wstart;
      load.We = Wend;
      pModelData->m_LowerXBeamLoads.push_back(load);

      Xstart = Xend;
      Astart = Aend;
      Wstart = Wend;
   }
}

void CAnalysisAgentImp::GetFemModelLocation(ModelData* pModelData,const xbrPointOfInterest& poi,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   ATLASSERT(!poi.IsColumnPOI()); // not supporting POIs on the columns yet

   Float64 Xpoi = poi.GetDistFromStart();
   GetCapBeamFemModelLocation(pModelData,Xpoi,pMbrID,pMbrLocation);
}

void CAnalysisAgentImp::GetCapBeamFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   std::vector<CapBeamMember>::iterator iter(pModelData->m_CapBeamMembers.begin());
   std::vector<CapBeamMember>::iterator end(pModelData->m_CapBeamMembers.end());
   for ( ; iter != end; iter++ )
   {
      CapBeamMember& capMbr(*iter);
      if ( InRange(capMbr.Xs,X,capMbr.Xe) )
      {
         *pMbrID = capMbr.mbrID;
         *pMbrLocation = X - capMbr.Xs;
         return;
      }
   }
   ATLASSERT(false); // should never get here
}

LoadCaseIDType CAnalysisAgentImp::GetLoadCaseID(xbrTypes::ProductForceType pfType)
{
   switch (pfType)
   {
   case xbrTypes::pftLowerXBeam:
      return 0;

   case xbrTypes::pftUpperXBeam:
      return 1;

   case xbrTypes::pftDCReactions:
      return 2;

   case xbrTypes::pftDWReactions:
      return 3;

   default:
      ATLASSERT(false);
   }

   return INVALID_ID;
}

void CAnalysisAgentImp::Invalidate()
{
   m_ModelData.clear();
}

//////////////////////////////////////////////////////////////////////
// IProjectEventSink
HRESULT CAnalysisAgentImp::OnProjectChanged()
{
   Invalidate();
   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IXBRProductForces
const std::vector<LowerXBeamLoad>& CAnalysisAgentImp::GetLowerCrossBeamLoading(PierIDType pierID)
{
   ModelData* pModelData = GetModelData(pierID);
   return pModelData->m_LowerXBeamLoads;
}

Float64 CAnalysisAgentImp::GetUpperCrossBeamLoading(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);
   Float64 H, W;
   pProject->GetDiaphragmDimensions(pierID,&H,&W);

   if ( pProject->GetPierType(pierID) == xbrTypes::pctExpansion )
   {
      W *= 2;
   }

   GET_IFACE(IXBRMaterial,pMaterial);
   Float64 density = pMaterial->GetXBeamDensity(pierID);
   Float64 unitWeight = density*unitSysUnitsMgr::GetGravitationalAcceleration();

   Float64 w = H*W*unitWeight;
   return w;
}

//////////////////////////////////////////////////////////////////////
// IAnalysisResults
Float64 CAnalysisAgentImp::GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi)
{
   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);

   LoadCaseIDType lcid = GetLoadCaseID(pfType);

   Float64 Fx, Fy, Mz;
   HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&Fx,&Fy,&Mz);
   ATLASSERT(SUCCEEDED(hr));
   return Mz;
}

sysSectionValue CAnalysisAgentImp::GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const xbrPointOfInterest& poi)
{
   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);

   LoadCaseIDType lcid = GetLoadCaseID(pfType);

   Float64 Fx, FyL, FyR, Mz;
   results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&Fx,&FyL,&Mz);
   results->ComputePOIForces(lcid,femPoiID,mftRight,lotGlobalProjected,&Fx,&FyR,&Mz);

   sysSectionValue V(-FyL,FyR);
   return V;
}

std::vector<Float64> CAnalysisAgentImp::GetMoment(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<Float64> vM;
   vM.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 m = GetMoment(pierID,pfType,poi);
      vM.push_back(m);
   }

   return vM;
}

std::vector<sysSectionValue> CAnalysisAgentImp::GetShear(PierIDType pierID,xbrTypes::ProductForceType pfType,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<sysSectionValue> vV;
   vV.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue v = GetShear(pierID,pfType,poi);
      vV.push_back(v);
   }

   return vV;
}

Float64 CAnalysisAgentImp::GetMoment(PierIDType pierID,XBRCombinedForceType lcType,const xbrPointOfInterest& poi)
{
   std::vector<xbrTypes::ProductForceType> vPFTypes = GetLoads(lcType);
   Float64 M = 0;
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vPFTypes)
   {
      Float64 m = GetMoment(pierID,pfType,poi);
      M += m;
   }

   return M;
}

sysSectionValue CAnalysisAgentImp::GetShear(PierIDType pierID,XBRCombinedForceType lcType,const xbrPointOfInterest& poi)
{
   std::vector<xbrTypes::ProductForceType> vPFTypes = GetLoads(lcType);
   sysSectionValue V(0,0);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vPFTypes)
   {
      sysSectionValue v = GetShear(pierID,pfType,poi);
      V += v;
   }

   return V;
}

std::vector<Float64> CAnalysisAgentImp::GetMoment(PierIDType pierID,XBRCombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<Float64> vM;
   vM.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 m = GetMoment(pierID,lcType,poi);
      vM.push_back(m);
   }
   return vM;
}

std::vector<sysSectionValue> CAnalysisAgentImp::GetShear(PierIDType pierID,XBRCombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<sysSectionValue> vV;
   vV.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue v = GetShear(pierID,lcType,poi);
      vV.push_back(v);
   }
   return vV;
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax)
{
   *pMin = 0;
   *pMax = 0;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax)
{
   *pMin = 0;
   *pMax = 0;
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      GetMoment(pierID,ratingType,vehIdx,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      GetShear(pierID,ratingType,vehIdx,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax)
{
   *pMin = 0;
   *pMax = 0;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax)
{
   *pMin = 0;
   *pMax = 0;
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      GetMoment(pierID,ratingType,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      GetShear(pierID,ratingType,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax)
{
   pgsTypes::LoadRatingType ratingType = RatingTypeFromLimitState(limitState);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor();
   Float64 gDW = pProject->GetDWLoadFactor();
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ratingType);

   Float64 DC = 0;
   std::vector<xbrTypes::ProductForceType> vDC = GetLoads(lcDC);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDC)
   {
      Float64 dc = GetMoment(pierID,pfType,poi);
      DC += dc;
   }

   Float64 DW = 0;
   std::vector<xbrTypes::ProductForceType> vDW = GetLoads(lcDW);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDW)
   {
      Float64 dw = GetMoment(pierID,pfType,poi);
      DW += dw;
   }

   Float64 LLIMmin, LLIMmax;
   GetMoment(pierID,ratingType,poi,&LLIMmin,&LLIMmax);

   *pMin = gDC*DC + gDW*DW + gLL*LLIMmin;
   *pMax = gDC*DC + gDW*DW + gLL*LLIMmax;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax)
{
   pgsTypes::LoadRatingType ratingType = RatingTypeFromLimitState(limitState);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor();
   Float64 gDW = pProject->GetDWLoadFactor();
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ratingType);

   sysSectionValue DC = 0;
   std::vector<xbrTypes::ProductForceType> vDC = GetLoads(lcDC);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDC)
   {
      sysSectionValue dc = GetShear(pierID,pfType,poi);
      DC += dc;
   }

   sysSectionValue DW = 0;
   std::vector<xbrTypes::ProductForceType> vDW = GetLoads(lcDW);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDW)
   {
      sysSectionValue dw = GetShear(pierID,pfType,poi);
      DW += dw;
   }

   sysSectionValue LLIMmin, LLIMmax;
   GetShear(pierID,ratingType,poi,&LLIMmin,&LLIMmax);

   *pMin = gDC*DC + gDW*DW + gLL*LLIMmin;
   *pMax = gDC*DC + gDW*DW + gLL*LLIMmax;
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      GetMoment(pierID,limitState,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      GetShear(pierID,limitState,poi,&min,&max);
      pvMin->push_back(min);
      pvMax->push_back(max);
   }
}

std::vector<xbrTypes::ProductForceType> CAnalysisAgentImp::GetLoads(XBRCombinedForceType lcType)
{
   std::vector<xbrTypes::ProductForceType> vPFTypes;
   if ( lcType == lcDC )
   {
      vPFTypes.push_back(xbrTypes::pftLowerXBeam);
      vPFTypes.push_back(xbrTypes::pftUpperXBeam);
      vPFTypes.push_back(xbrTypes::pftDCReactions);
   }
   else
   {
      vPFTypes.push_back(xbrTypes::pftDWReactions);
   }

   return vPFTypes;
}
