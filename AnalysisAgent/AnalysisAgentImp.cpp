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

//////////////////////////////////////////////////////////////////////
void CAnalysisAgentImp::Validate()
{
   if ( m_Model != NULL )
   {
      return;
   }

   m_Model.CoCreateInstance(CLSID_Fem2dModel);
   m_Model->put_Name(CComBSTR(_T("XBeamRate")));

   // Build the frame model
   GET_IFACE(IXBRProject,pProject);

   // some dummy dimensions
   Float64 leftOverhang = pProject->GetXBeamLeftOverhang();
   Float64 rightOverhang = pProject->GetXBeamRightOverhang();
   IndexType nColumns = pProject->GetColumnCount();

#pragma Reminder("UPDATE: get real material and section properties")
   Float64 EAb = 1;
   Float64 EIb = 1;
   Float64 EAc = 1;
   Float64 EIc = 1;

   CComPtr<IFem2dJointCollection> joints;
   m_Model->get_Joints(&joints);

   CComPtr<IFem2dMemberCollection> members;
   m_Model->get_Members(&members);

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
   m_CapBeamMembers.push_back(capMbr);

#pragma Reminder("UPDATE: BUG: This is not the correct column height")
   // this is the raw input data... it could be the bottom elevation...
   // use the IXBRPier interface to get the actual height
   Float64 columnHeight = pProject->GetColumnHeight();
   Float64 columnSpacing = pProject->GetColumnSpacing();
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 space = (colIdx < nColumns-1 ? columnSpacing : rightOverhang);

      // create joint at bottom of column
      joint.Release();
      joints->Create(jntID++,Xe,-columnHeight,&joint);

      joint->Support(); // fully fixed
      //if ( pProject->GetColumnBaseType(colIdx) == xbrTypes::cbtPinned )
      //{
      //   joint->ReleaseDof(jrtMz); // pinned
      //}

      // create column member
      mbr.Release();
      members->Create(columnMbrID--,jntID-2,jntID-1,EAc,EIc,&mbr);

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
      m_CapBeamMembers.push_back(capMbr);
   }

   // Assign POIs
   CComPtr<IFem2dPOICollection> femPois;
   m_Model->get_POIs(&femPois);
   PoiIDType femPoiID = 0;
   GET_IFACE(IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest();
   BOOST_FOREACH(xbrPointOfInterest& poi,vPoi)
   {
      MemberIDType mbrID;
      Float64 mbrLocation;
      GetFemModelLocation(poi,&mbrID,&mbrLocation);

      CComPtr<IFem2dPOI> femPoi;
      femPois->Create(femPoiID,mbrID,mbrLocation,&femPoi);
      m_PoiMap.insert(std::make_pair(poi.GetID(),femPoiID));
      femPoiID++;
   }

   ApplyDeadLoad();

   CComQIPtr<IStructuredStorage2> ss(m_Model);
   CComPtr<IStructuredSave2> save;
   save.CoCreateInstance(CLSID_StructuredSave2);
   save->Open(CComBSTR(_T("XBeamRate_Fem2d.xml")));
   ss->Save(save);
   save->Close();
   save.Release();
   ss.Release();
}

void CAnalysisAgentImp::ApplyDeadLoad()
{
   ApplyLowerXBeamDeadLoad();
   ApplyUpperXBeamDeadLoad();
   ApplySuperstructureDeadLoadReactions();
}

void CAnalysisAgentImp::ApplyLowerXBeamDeadLoad()
{
   ValidateLowerXBeamDeadLoad();

   CComPtr<IFem2dLoadingCollection> loadings;
   m_Model->get_Loadings(&loadings);

   LoadCaseIDType loadCaseID = GetLoadCaseID(pftLowerXBeam);
   CComPtr<IFem2dLoading> loading;
   loadings->Create(loadCaseID,&loading);

   CComPtr<IFem2dDistributedLoadCollection> distLoads;
   loading->get_DistributedLoads(&distLoads);

   LoadIDType loadID = 0;
   std::vector<LowerXBeamLoad>::iterator iter(m_LowerXBeamLoads.begin());
   std::vector<LowerXBeamLoad>::iterator end(m_LowerXBeamLoads.end());
   for ( ; iter != end; iter++ )
   {
      LowerXBeamLoad& load(*iter);

      MemberIDType startMbrID, endMbrID;
      Float64 startMbrLocation , endMbrLocation;
      GetFemModelLocation(xbrPointOfInterest(INVALID_ID,load.Xs),&startMbrID,&startMbrLocation);
      GetFemModelLocation(xbrPointOfInterest(INVALID_ID,load.Xe),&endMbrID,  &endMbrLocation);

      if ( startMbrID == endMbrID )
      {
         CComPtr<IFem2dDistributedLoad> distLoad;
         distLoads->Create(loadID++,startMbrID,loadDirFy,startMbrLocation,endMbrLocation,-load.Ws,-load.We,lotMember,&distLoad);
      }
      else
      {
         CComPtr<IFem2dMemberCollection> members;
         m_Model->get_Members(&members);

         CComPtr<IFem2dJointCollection> joints;
         m_Model->get_Joints(&joints);

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

void CAnalysisAgentImp::ApplyUpperXBeamDeadLoad()
{
   CComPtr<IFem2dLoadingCollection> loadings;
   m_Model->get_Loadings(&loadings);

   LoadCaseIDType loadCaseID = GetLoadCaseID(pftUpperXBeam);
   CComPtr<IFem2dLoading> loading;
   loadings->Create(loadCaseID,&loading);

   CComPtr<IFem2dDistributedLoadCollection> distLoads;
   loading->get_DistributedLoads(&distLoads);

   LoadIDType loadID = 0;

   Float64 w = GetUpperCrossBeamLoading();
   BOOST_FOREACH(CapBeamMember& capMbr,m_CapBeamMembers)
   {
      CComPtr<IFem2dDistributedLoad> distLoad;
      distLoads->Create(loadID++,capMbr.mbrID,loadDirFy,0,-1,-w,-w,lotMember,&distLoad);
   }
}

void CAnalysisAgentImp::ApplySuperstructureDeadLoadReactions()
{
   GET_IFACE(IXBRPier,pPier);
   GET_IFACE(IXBRProject,pProject);

   LoadCaseIDType dcLoadCaseID = GetLoadCaseID(pftDCReactions);
   LoadCaseIDType dwLoadCaseID = GetLoadCaseID(pftDWReactions);

   LoadIDType dcLoadID = 0;
   LoadIDType dwLoadID = 0;

   CComPtr<IFem2dLoadingCollection> loadings;
   m_Model->get_Loadings(&loadings);

   CComPtr<IFem2dLoading> dcLoading;
   loadings->Create(dcLoadCaseID,&dcLoading);

   CComPtr<IFem2dLoading> dwLoading;
   loadings->Create(dwLoadCaseID,&dwLoading);

   CComPtr<IFem2dPointLoadCollection> dcPointLoads;
   dcLoading->get_PointLoads(&dcPointLoads);

   CComPtr<IFem2dPointLoadCollection> dwPointLoads;
   dwLoading->get_PointLoads(&dwPointLoads);

   IndexType nBearingLines = pProject->GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pProject->GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = pPier->GetBearingLocation(brgLineIdx,brgIdx);

         MemberIDType mbrID;
         Float64 mbrLocation;
         GetCapBeamFemModelLocation(Xbrg,&mbrID,&mbrLocation);

         Float64 DC, DW;
         pProject->GetBearingReactions(brgLineIdx,brgIdx,&DC,&DW);

         CComPtr<IFem2dPointLoad> dcLoad;
         dcPointLoads->Create(dcLoadID++,mbrID,mbrLocation,0.0,-DC,0.0,lotGlobal,&dcLoad);

         CComPtr<IFem2dPointLoad> dwLoad;
         dwPointLoads->Create(dwLoadID++,mbrID,mbrLocation,0.0,-DW,0.0,lotGlobal,&dwLoad);
      }
   }
}

void CAnalysisAgentImp::ValidateLowerXBeamDeadLoad()
{
   if ( 0 < m_LowerXBeamLoads.size() )
   {
      return;
   }

   GET_IFACE(IXBRPointOfInterest,pPoi);
   std::vector<xbrPointOfInterest> vPoi = pPoi->GetXBeamPointsOfInterest(POI_SECTCHANGE);

   GET_IFACE(IXBRSectionProperties,pSectProp);
   GET_IFACE(IXBRMaterial,pMaterial);

   Float64 density = pMaterial->GetXBeamDensity();
   Float64 unitWeight = density*unitSysUnitsMgr::GetGravitationalAcceleration();

   std::vector<xbrPointOfInterest>::iterator iter(vPoi.begin());
   std::vector<xbrPointOfInterest>::iterator end(vPoi.end());
   Float64 Astart = pSectProp->GetArea(xbrTypes::Stage1,*iter);
   Float64 Wstart = unitWeight*Astart;
   Float64 Xstart = (*iter).GetDistFromStart();

   iter++;
   for ( ; iter != end; iter++ )
   {
      Float64 Aend = pSectProp->GetArea(xbrTypes::Stage1,*iter);
      Float64 Wend = unitWeight*Aend;
      Float64 Xend = (*iter).GetDistFromStart();

      LowerXBeamLoad load;
      load.Xs = Xstart;
      load.Xe = Xend;
      load.Ws = Wstart;
      load.We = Wend;
      m_LowerXBeamLoads.push_back(load);

      Xstart = Xend;
      Astart = Aend;
      Wstart = Wend;
   }
}

void CAnalysisAgentImp::GetFemModelLocation(const xbrPointOfInterest& poi,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   ATLASSERT(!poi.IsColumnPOI()); // not supporting POIs on the columns yet

   Float64 Xpoi = poi.GetDistFromStart();
   GetCapBeamFemModelLocation(Xpoi,pMbrID,pMbrLocation);
}

void CAnalysisAgentImp::GetCapBeamFemModelLocation(Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   std::vector<CapBeamMember>::iterator iter(m_CapBeamMembers.begin());
   std::vector<CapBeamMember>::iterator end(m_CapBeamMembers.end());
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

LoadCaseIDType CAnalysisAgentImp::GetLoadCaseID(XBRProductForceType pfType)
{
   switch (pfType)
   {
   case pftLowerXBeam:
      return 0;

   case pftUpperXBeam:
      return 1;

   case pftDCReactions:
      return 2;

   case pftDWReactions:
      return 3;

   default:
      ATLASSERT(false);
   }

   return INVALID_ID;
}

void CAnalysisAgentImp::Invalidate()
{
   m_Model.Release();
   m_CapBeamMembers.clear();
   m_PoiMap.clear();
   m_LowerXBeamLoads.clear();
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
const std::vector<LowerXBeamLoad>& CAnalysisAgentImp::GetLowerCrossBeamLoading()
{
   ValidateLowerXBeamDeadLoad();
   return m_LowerXBeamLoads;
}

Float64 CAnalysisAgentImp::GetUpperCrossBeamLoading()
{
   GET_IFACE(IXBRProject,pProject);
   Float64 H, W;
   pProject->GetDiaphragmDimensions(&H,&W);

   if ( pProject->GetPierType() == xbrTypes::pctExpansion )
   {
      W *= 2;
   }

   GET_IFACE(IXBRMaterial,pMaterial);
   Float64 density = pMaterial->GetXBeamDensity();
   Float64 unitWeight = density*unitSysUnitsMgr::GetGravitationalAcceleration();

   Float64 w = H*W*unitWeight;
   return w;
}

//////////////////////////////////////////////////////////////////////
// IAnalysisResults
Float64 CAnalysisAgentImp::GetMoment(XBRProductForceType pfType,const xbrPointOfInterest& poi)
{
   Validate();

   std::map<PoiIDType,PoiIDType>::iterator found = m_PoiMap.find(poi.GetID());
   ATLASSERT(found != m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(m_Model);

   LoadCaseIDType lcid = GetLoadCaseID(pfType);

   Float64 Fx, Fy, Mz;
   HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&Fx,&Fy,&Mz);
   ATLASSERT(SUCCEEDED(hr));
   return Mz;
}

sysSectionValue CAnalysisAgentImp::GetShear(XBRProductForceType pfType,const xbrPointOfInterest& poi)
{
   Validate();

   std::map<PoiIDType,PoiIDType>::iterator found = m_PoiMap.find(poi.GetID());
   ATLASSERT(found != m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(m_Model);

   LoadCaseIDType lcid = GetLoadCaseID(pfType);

   Float64 Fx, FyL, FyR, Mz;
   results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&Fx,&FyL,&Mz);
   results->ComputePOIForces(lcid,femPoiID,mftRight,lotGlobalProjected,&Fx,&FyR,&Mz);

   sysSectionValue V(-FyL,FyR);
   return V;
}

Float64 CAnalysisAgentImp::GetMoment(XBRCombinedForceType lcType,const xbrPointOfInterest& poi)
{
   std::vector<XBRProductForceType> vPFTypes = GetLoads(lcType);
   Float64 M = 0;
   BOOST_FOREACH(XBRProductForceType pfType,vPFTypes)
   {
      Float64 m = GetMoment(pfType,poi);
      M += m;
   }

   return M;
}

sysSectionValue CAnalysisAgentImp::GetShear(XBRCombinedForceType lcType,const xbrPointOfInterest& poi)
{
   std::vector<XBRProductForceType> vPFTypes = GetLoads(lcType);
   sysSectionValue V(0,0);
   BOOST_FOREACH(XBRProductForceType pfType,vPFTypes)
   {
      sysSectionValue v = GetShear(pfType,poi);
      V += v;
   }

   return V;
}

void CAnalysisAgentImp::GetMoment(const xbrPointOfInterest& poi,pgsTypes::LiveLoadType liveLoadType,VehicleIndexType vehIdx,Float64* pMin,Float64* pMax)
{
   ATLASSERT(liveLoadType == pgsTypes::lltDesign ||
             liveLoadType == pgsTypes::lltLegalRating_Routine ||
             liveLoadType == pgsTypes::lltLegalRating_Special ||
             liveLoadType == pgsTypes::lltPermitRating_Routine ||
             liveLoadType == pgsTypes::lltPermitRating_Special);

   *pMin = 0;
   *pMax = 0;
}

void CAnalysisAgentImp::GetMoment(const xbrPointOfInterest& poi,pgsTypes::LiveLoadType liveLoadType,Float64* pMin,Float64* pMax)
{
   ATLASSERT(liveLoadType == pgsTypes::lltDesign ||
             liveLoadType == pgsTypes::lltLegalRating_Routine ||
             liveLoadType == pgsTypes::lltLegalRating_Special ||
             liveLoadType == pgsTypes::lltPermitRating_Routine ||
             liveLoadType == pgsTypes::lltPermitRating_Special);

   *pMin = 0;
   *pMax = 0;
}

std::vector<XBRProductForceType> CAnalysisAgentImp::GetLoads(XBRCombinedForceType lcType)
{
   std::vector<XBRProductForceType> vPFTypes;
   if ( lcType == lcDC )
   {
      vPFTypes.push_back(pftLowerXBeam);
      vPFTypes.push_back(pftUpperXBeam);
      vPFTypes.push_back(pftDCReactions);
   }
   else
   {
      vPFTypes.push_back(pftDWReactions);
   }

   return vPFTypes;
}
