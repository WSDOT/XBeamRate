///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2016  Washington State Department of Transportation
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
#include <IFace\RatingSpecification.h>

#include <Units\SysUnitsMgr.h>

#include <EAF\EAFAutoProgress.h>
#include <XBeamRateExt\XBeamRateUtilities.h>
#include <XBeamRateExt\StatusItem.h>

#include <numeric>

#include <System\Flags.h>
#include <LRFD\Utility.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_CASES 10

/////////////////////////////////////////////////////////////////////////////
// NOTE: Any time you get live load results directly from the FEM model,
// apply the multiple presence factor.
/////////////////////////////////////////////////////////////////////////////

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
   m_pModelData = std::auto_ptr<std::map<PierIDType,ModelData>>(new std::map<PierIDType,ModelData>());
   m_pUnitLiveLoadResults = std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>>(new std::map<PierIDType,std::set<UnitLiveLoadResult>>());
   return S_OK;
}

void CAnalysisAgentImp::FinalRelease()
{
   Invalidate(false);
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
   EAF_AGENT_INIT; // this macro defines pStatusCenter
   m_StatusGroupID = pStatusCenter->CreateStatusGroupID();

   // Register status callbacks that we want to use
   m_scidBridgeError = pStatusCenter->RegisterCallback(new xbrBridgeStatusCallback(eafTypes::statusError)); 

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

   // Connection point for the bridge description
   hr = pBrokerInit->FindConnectionPoint( IID_IBridgeDescriptionEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Advise( GetUnknown(), &m_dwBridgeDescCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the IConnectionPoint smart pointer so we can use it again.
   }

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

   hr = pBrokerInit->FindConnectionPoint(IID_IBridgeDescriptionEventSink, &pCP );
   if ( SUCCEEDED(hr) )
   {
      hr = pCP->Unadvise( m_dwBridgeDescCookie );
      ATLASSERT( SUCCEEDED(hr) );
      pCP.Release(); // Recycle the connection point
   }

   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

CAnalysisAgentImp::ModelData* CAnalysisAgentImp::GetModelData(PierIDType pierID,int level)
{
   BuildModel(pierID,level); // builds or updates the model if necessary
   std::map<PierIDType,ModelData>::iterator found = m_pModelData->find(pierID);
   ATLASSERT( found != m_pModelData->end() ); // should always find it!
   ModelData* pModelData = &(*found).second;
   return pModelData;
}

#define BEARING 0x0001
#define COLUMN  0x0002
#define CURB    0x0004
struct XBeamNode
{
   Float64 X;
   Int32 Type;
   JointIDType jntID;
   bool operator<(const XBeamNode& other)const { return IsLT(X,other.X,0.005); }
   bool operator==(const XBeamNode& other)const { return IsEqual(X,other.X,0.005); }
};

void CAnalysisAgentImp::BuildModel(PierIDType pierID,int level)
{
   CanModelPier(pierID,m_StatusGroupID,m_scidBridgeError); // if this is not the kind of pier we can model, an Unwind exception will be thrown

   std::map<PierIDType,ModelData>::iterator found = m_pModelData->find(pierID);
   if ( found == m_pModelData->end() )
   {
      ModelData model_data;
      m_pModelData->insert( std::make_pair(pierID,model_data) );
      found = m_pModelData->find(pierID);
   }

   ModelData* pModelData = &(found->second);

   if ( level <= pModelData->m_InitLevel )
   {
      return;
   }

   GET_IFACE(IProgress,pProgress);
   CEAFAutoProgress ap(pProgress);

   if ( MODEL_INIT_TOPOLOGY <= level && pModelData->m_InitLevel < MODEL_INIT_TOPOLOGY )
   {
      pModelData->m_Model.CoCreateInstance(CLSID_Fem2dModel);
      pModelData->m_Model->put_Name(CComBSTR(_T("XBeamRate")));

      // Build the frame model
      GET_IFACE(IXBRProject,pProject);
      GET_IFACE(IXBRPier,pPier);
      GET_IFACE(IXBRMaterial,pMaterial);
      GET_IFACE(IXBRSectionProperties,pSectProp);

      pProgress->UpdateMessage(_T("Building pier analysis model"));

      Float64 LCO, RCO;
      pProject->GetCurbLineOffset(pierID,&LCO,&RCO);

      IndexType nColumns = pProject->GetColumnCount(pierID);

      Float64 L = pPier->GetXBeamLength(pierID);

      // Get the location of all the cross beam nodes

      // beginning/end of cross beam
      std::vector<XBeamNode> vXBeamNodes;
      XBeamNode n;
      n.X = 0;
      n.Type = 0;
      vXBeamNodes.push_back(n);

      n.X = L;
      n.Type = 0;
      vXBeamNodes.push_back(n);

      // curb line
      n.X = pPier->ConvertPierToCrossBeamCoordinate(pierID,LCO);
      n.Type = CURB;
      vXBeamNodes.push_back(n);

      n.X = pPier->ConvertPierToCrossBeamCoordinate(pierID,RCO);
      n.Type = CURB;
      vXBeamNodes.push_back(n);

      // top of columns
      for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
      {
         n.X = pPier->GetColumnLocation(pierID,colIdx);
         n.Type = COLUMN;
         vXBeamNodes.push_back(n);
      }

      // bearings
      // if reactions are applied through bearings, we need a bearing transfer model
      // the first step for that model is FEM2D nodes at the bearing locations
      if ( pProject->GetReactionLoadApplicationType(pierID) == xbrTypes::rlaBearings )
      {
         IndexType nBearingLines = pPier->GetBearingLineCount(pierID);
         for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
         {
            IndexType nBearings = pPier->GetBearingCount(pierID,brgLineIdx);
            for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
            {
               n.X = pPier->GetBearingLocation(pierID,brgLineIdx,brgIdx);
               n.Type = BEARING;
               vXBeamNodes.push_back(n);
            }
         }
      }

      // sort in left-to-right order
      std::sort(vXBeamNodes.begin(),vXBeamNodes.end());

      // eliminate duplicates... if or more nodes are at the same location, merge the Type
      // attribute and eliminate the redundant node record
      std::vector<XBeamNode>::iterator result = std::adjacent_find(vXBeamNodes.begin(),vXBeamNodes.end());
      while ( result != vXBeamNodes.end() )
      {
         XBeamNode& n1 = *result;
         XBeamNode& n2 = *(result+1);
         n1.Type |= n2.Type;
         vXBeamNodes.erase(result+1);
         result = std::adjacent_find(vXBeamNodes.begin(),vXBeamNodes.end());
      }

      // Get properties
      Float64 Exb = pMaterial->GetXBeamEc(pierID);
      Float64 Axb = pSectProp->GetArea(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,L/2));
      Float64 Ixb = pSectProp->GetIxx(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,L/2));
      Float64 EAb = Exb*Axb;
      Float64 EIb = Exb*Ixb;

      // build the model
      CComPtr<IFem2dJointCollection> joints;
      pModelData->m_Model->get_Joints(&joints);

      CComPtr<IFem2dMemberCollection> members;
      pModelData->m_Model->get_Members(&members);

      JointIDType jntID = 0;
      MemberIDType xbeamMbrID = 0;
      MemberIDType columnMbrID = -1;

      std::vector<XBeamNode>::iterator iter(vXBeamNodes.begin());
      std::vector<XBeamNode>::iterator end(vXBeamNodes.end());

      XBeamNode* pPrevNode = &(*iter);;
      JointIDType prevJointID = jntID++;
      pPrevNode->jntID = prevJointID;
      CComPtr<IFem2dJoint> joint;
      joints->Create(prevJointID,pPrevNode->X,0,&joint);
      ColumnIndexType colIdx = 0;

      iter++;
      for ( ; iter != end; iter++ )
      {
         XBeamNode* pThisNode = &(*iter);
         JointIDType thisJointID = jntID++;
         pThisNode->jntID = thisJointID;

         joint.Release();
         joints->Create(thisJointID,pThisNode->X,0,&joint);
         
         CComPtr<IFem2dMember> mbr;
         members->Create(xbeamMbrID++,prevJointID,thisJointID,EAb,EIb,&mbr);
         BeamMember capMbr;
         capMbr.Xs = pPrevNode->X;
         capMbr.Xe = pThisNode->X;
         capMbr.mbrID = xbeamMbrID-1;
         pModelData->m_XBeamMembers.push_back(capMbr);

         if ( sysFlags<Int32>::IsSet(pThisNode->Type,COLUMN) )
         {
            Float64 columnHeight = pPier->GetColumnHeight(pierID,colIdx);

            // create joint at bottom of column
            joint.Release();
            joints->Create(jntID++,pThisNode->X,-columnHeight,&joint);

            pgsTypes::ColumnFixityType columnFixity = pPier->GetColumnFixity(pierID,colIdx);
            joint->Support(); // fully fixed
            if ( columnFixity == pgsTypes::cftPinned )
            {
               joint->ReleaseDof(jrtMz);
            }

            // create column member
            Float64 Ecol = pMaterial->GetColumnEc(pierID,colIdx);
            Float64 Acol = pSectProp->GetArea(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,colIdx,0.0));
            Float64 Icol = pSectProp->GetIyy(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,colIdx,0.0));
            mbr.Release();
            members->Create(columnMbrID--,thisJointID,jntID-1,Ecol*Acol,Ecol*Icol,&mbr);

            colIdx++;
         }

         pPrevNode = pThisNode;
         prevJointID = thisJointID;
      }

      // create the "superstructure" model upon which we will run the live load
      if ( pProject->GetReactionLoadApplicationType(pierID) == xbrTypes::rlaCrossBeam )
      {
         // live load is applied directly to the cross beam... the superstruture members
         // and the XBeam members are one in the same
         pModelData->m_SuperstructureMembers = pModelData->m_XBeamMembers;
      }
      else
      {
         // live load is applied to a load transfer model.

         // dummy properties of the transfer model
         Float64 Y = ::ConvertToSysUnits(1.0,unitMeasure::Inch); // offset the transfer model a small distance above the XBeam model
         Float64 EI = EIb*10000; // use members that are considerably stiffer than the XBeam members
         Float64 EA = EAb*10000;

         std::vector<XBeamNode>::iterator iter(vXBeamNodes.begin());
         std::vector<XBeamNode>::iterator end(vXBeamNodes.end());

         XBeamNode* pPrevNode = &(*iter);;
         JointIDType prevJointID = jntID++;
         CComPtr<IFem2dJoint> joint;
         joints->Create(prevJointID,pPrevNode->X,Y,&joint);

         iter++;
         for ( ; iter != end; iter++ )
         {
            XBeamNode* pThisNode = &(*iter);
            JointIDType thisJointID = jntID++;

            joint.Release();
            joints->Create(thisJointID,pThisNode->X,Y,&joint);
            
            CComPtr<IFem2dMember> mbr;
            members->Create(xbeamMbrID++,prevJointID,thisJointID,EAb,EIb,&mbr);
            BeamMember ssMbr;
            ssMbr.Xs = pPrevNode->X;
            ssMbr.Xe = pThisNode->X;
            ssMbr.mbrID = xbeamMbrID-1;
            pModelData->m_SuperstructureMembers.push_back(ssMbr);

            if ( sysFlags<Int32>::IsSet(pThisNode->Type,BEARING) )
            {
               mbr.Release();
               members->Create(columnMbrID--,thisJointID,pThisNode->jntID,EA,EI,&mbr);

               mbr->ReleaseEnd(metEnd,mbrReleaseMz);
            }

            pPrevNode = pThisNode;
            prevJointID = thisJointID;
         }
      }

      ComputeLiveLoadLocations(pierID,pModelData);

      pModelData->m_InitLevel = MODEL_INIT_TOPOLOGY;
   }

   if ( MODEL_INIT_LOADS <= level && pModelData->m_InitLevel < MODEL_INIT_LOADS )
   {
      ApplyWheelLineLoadsToFemModel(pModelData);
      ApplyDeadLoad(pierID,pModelData);

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

      pModelData->m_InitLevel = MODEL_INIT_LOADS;

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
   BOOST_FOREACH(BeamMember& capMbr,pModelData->m_XBeamMembers)
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
   LoadCaseIDType crLoadCaseID = GetLoadCaseID(xbrTypes::pftCRReactions);
   LoadCaseIDType shLoadCaseID = GetLoadCaseID(xbrTypes::pftSHReactions);
   LoadCaseIDType psLoadCaseID = GetLoadCaseID(xbrTypes::pftPSReactions);
   LoadCaseIDType reLoadCaseID = GetLoadCaseID(xbrTypes::pftREReactions);

   LoadIDType dcLoadID = 0;
   LoadIDType dwLoadID = 0;
   LoadIDType shLoadID = 0;
   LoadIDType crLoadID = 0;
   LoadIDType psLoadID = 0;
   LoadIDType reLoadID = 0;

   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   CComPtr<IFem2dLoading> dcLoading;
   loadings->Create(dcLoadCaseID,&dcLoading);

   CComPtr<IFem2dLoading> dwLoading;
   loadings->Create(dwLoadCaseID,&dwLoading);

   CComPtr<IFem2dLoading> shLoading;
   loadings->Create(shLoadCaseID,&shLoading);

   CComPtr<IFem2dLoading> crLoading;
   loadings->Create(crLoadCaseID,&crLoading);

   CComPtr<IFem2dLoading> psLoading;
   loadings->Create(psLoadCaseID,&psLoading);

   CComPtr<IFem2dLoading> reLoading;
   loadings->Create(reLoadCaseID,&reLoading);


   CComPtr<IFem2dPointLoadCollection> dcPointLoads;
   dcLoading->get_PointLoads(&dcPointLoads);

   CComPtr<IFem2dPointLoadCollection> dwPointLoads;
   dwLoading->get_PointLoads(&dwPointLoads);

   CComPtr<IFem2dPointLoadCollection> shPointLoads;
   shLoading->get_PointLoads(&shPointLoads);

   CComPtr<IFem2dPointLoadCollection> crPointLoads;
   crLoading->get_PointLoads(&crPointLoads);

   CComPtr<IFem2dPointLoadCollection> psPointLoads;
   psLoading->get_PointLoads(&psPointLoads);

   CComPtr<IFem2dPointLoadCollection> rePointLoads;
   reLoading->get_PointLoads(&rePointLoads);


   CComPtr<IFem2dDistributedLoadCollection> dcDistLoads;
   dcLoading->get_DistributedLoads(&dcDistLoads);

   CComPtr<IFem2dDistributedLoadCollection> dwDistLoads;
   dwLoading->get_DistributedLoads(&dwDistLoads);

   CComPtr<IFem2dDistributedLoadCollection> shDistLoads;
   shLoading->get_DistributedLoads(&shDistLoads);

   CComPtr<IFem2dDistributedLoadCollection> crDistLoads;
   crLoading->get_DistributedLoads(&crDistLoads);

   CComPtr<IFem2dDistributedLoadCollection> psDistLoads;
   psLoading->get_DistributedLoads(&psDistLoads);

   CComPtr<IFem2dDistributedLoadCollection> reDistLoads;
   reLoading->get_DistributedLoads(&reDistLoads);


   IndexType nBearingLines = pProject->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      xbrTypes::ReactionLoadType reactionType = pProject->GetBearingReactionType(pierID,brgLineIdx);

      IndexType nBearings = pProject->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = pPier->GetBearingLocation(pierID,brgLineIdx,brgIdx);

         Float64 DC, DW, CR, SH, PS, RE, W;
         pProject->GetBearingReactions(pierID,brgLineIdx,brgIdx,&DC,&DW,&CR,&SH,&PS,&RE,&W);

         if ( reactionType == xbrTypes::rltConcentrated || IsZero(W) )
         {
            // Concentrated load (or uniform load, but W is zero so treat as concentrated load)
            MemberIDType mbrID;
            Float64 mbrLocation;
            GetXBeamFemModelLocation(pModelData,Xbrg,&mbrID,&mbrLocation);

            CComPtr<IFem2dPointLoad> dcLoad;
            dcPointLoads->Create(dcLoadID++,mbrID,mbrLocation,0.0,-DC,0.0,lotGlobal,&dcLoad);

            CComPtr<IFem2dPointLoad> dwLoad;
            dwPointLoads->Create(dwLoadID++,mbrID,mbrLocation,0.0,-DW,0.0,lotGlobal,&dwLoad);

            CComPtr<IFem2dPointLoad> shLoad;
            shPointLoads->Create(shLoadID++,mbrID,mbrLocation,0.0,-SH,0.0,lotGlobal,&shLoad);

            CComPtr<IFem2dPointLoad> crLoad;
            crPointLoads->Create(crLoadID++,mbrID,mbrLocation,0.0,-CR,0.0,lotGlobal,&crLoad);

            CComPtr<IFem2dPointLoad> psLoad;
            psPointLoads->Create(psLoadID++,mbrID,mbrLocation,0.0,-PS,0.0,lotGlobal,&psLoad);

            CComPtr<IFem2dPointLoad> reLoad;
            rePointLoads->Create(reLoadID++,mbrID,mbrLocation,0.0,-RE,0.0,lotGlobal,&reLoad);
         }
         else
         {
            // Distributed load
            MemberIDType startMbrID, endMbrID;
            Float64 startLocation, endLocation;
            GetXBeamFemModelLocation(pModelData,Xbrg-W/2,&startMbrID,&startLocation);
            GetXBeamFemModelLocation(pModelData,Xbrg+W/2,&endMbrID,&endLocation);

            if ( startMbrID == endMbrID )
            {
               // distributed load is contained within a single FEM member
               CComPtr<IFem2dDistributedLoad> dcLoad;
               dcDistLoads->Create(dcLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-DC,-DC,lotGlobalProjected,&dcLoad);

               CComPtr<IFem2dDistributedLoad> dwLoad;
               dwDistLoads->Create(dwLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-DW,-DW,lotGlobalProjected,&dwLoad);

               CComPtr<IFem2dDistributedLoad> shLoad;
               shDistLoads->Create(shLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-SH,-SH,lotGlobalProjected,&shLoad);

               CComPtr<IFem2dDistributedLoad> crLoad;
               crDistLoads->Create(crLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-CR,-CR,lotGlobalProjected,&crLoad);

               CComPtr<IFem2dDistributedLoad> psLoad;
               psDistLoads->Create(psLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-PS,-PS,lotGlobalProjected,&psLoad);

               CComPtr<IFem2dDistributedLoad> reLoad;
               reDistLoads->Create(reLoadID++,startMbrID,loadDirFy,startLocation,endLocation,-RE,-RE,lotGlobalProjected,&reLoad);
            }
            else
            {
               // distributed loads span over multiple FEM members

               // apply loads to first member
               CComPtr<IFem2dDistributedLoad> dcLoad;
               dcDistLoads->Create(dcLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-DC,-DC,lotGlobalProjected,&dcLoad);

               CComPtr<IFem2dDistributedLoad> dwLoad;
               dwDistLoads->Create(dwLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-DW,-DW,lotGlobalProjected,&dwLoad);

               CComPtr<IFem2dDistributedLoad> shLoad;
               shDistLoads->Create(shLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-SH,-SH,lotGlobalProjected,&shLoad);

               CComPtr<IFem2dDistributedLoad> crLoad;
               crDistLoads->Create(crLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-CR,-CR,lotGlobalProjected,&crLoad);

               CComPtr<IFem2dDistributedLoad> psLoad;
               psDistLoads->Create(psLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-PS,-PS,lotGlobalProjected,&psLoad);

               CComPtr<IFem2dDistributedLoad> reLoad;
               reDistLoads->Create(reLoadID++,startMbrID,loadDirFy,startLocation,-1.0,-RE,-RE,lotGlobalProjected,&reLoad);

               // apply loads to intermediate members
               for ( MemberIDType mbrID = startMbrID+1; mbrID < endMbrID-1; mbrID++ )
               {
                  dcLoad.Release();
                  dwLoad.Release();
                  shLoad.Release();
                  crLoad.Release();
                  psLoad.Release();
                  reLoad.Release();

                  dcDistLoads->Create(dcLoadID++,mbrID,loadDirFy,0.0,-1.0,-DC,-DC,lotGlobalProjected,&dcLoad);
                  dwDistLoads->Create(dwLoadID++,mbrID,loadDirFy,0.0,-1.0,-DW,-DW,lotGlobalProjected,&dwLoad);
                  shDistLoads->Create(shLoadID++,mbrID,loadDirFy,0.0,-1.0,-SH,-SH,lotGlobalProjected,&shLoad);
                  crDistLoads->Create(crLoadID++,mbrID,loadDirFy,0.0,-1.0,-CR,-CR,lotGlobalProjected,&crLoad);
                  psDistLoads->Create(psLoadID++,mbrID,loadDirFy,0.0,-1.0,-PS,-PS,lotGlobalProjected,&psLoad);
                  reDistLoads->Create(reLoadID++,mbrID,loadDirFy,0.0,-1.0,-RE,-RE,lotGlobalProjected,&reLoad);
               }

               // apply loads to last member
               dcLoad.Release();
               dwLoad.Release();
               shLoad.Release();
               crLoad.Release();
               psLoad.Release();
               reLoad.Release();

               dcDistLoads->Create(dcLoadID++,endMbrID,loadDirFy,0.0,endLocation,-DC,-DC,lotGlobalProjected,&dcLoad);
               dwDistLoads->Create(dwLoadID++,endMbrID,loadDirFy,0.0,endLocation,-DW,-DW,lotGlobalProjected,&dwLoad);
               shDistLoads->Create(shLoadID++,endMbrID,loadDirFy,0.0,endLocation,-SH,-SH,lotGlobalProjected,&shLoad);
               crDistLoads->Create(crLoadID++,endMbrID,loadDirFy,0.0,endLocation,-CR,-CR,lotGlobalProjected,&crLoad);
               psDistLoads->Create(psLoadID++,endMbrID,loadDirFy,0.0,endLocation,-PS,-PS,lotGlobalProjected,&psLoad);
               reDistLoads->Create(reLoadID++,endMbrID,loadDirFy,0.0,endLocation,-RE,-RE,lotGlobalProjected,&reLoad);
            }
         }
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
   GetXBeamFemModelLocation(pModelData,Xpoi,pMbrID,pMbrLocation);
}

void CAnalysisAgentImp::GetXBeamFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   std::vector<BeamMember>::iterator iter(pModelData->m_XBeamMembers.begin());
   std::vector<BeamMember>::iterator end(pModelData->m_XBeamMembers.end());
   for ( ; iter != end; iter++ )
   {
      BeamMember& capMbr(*iter);
      if ( InRange(capMbr.Xs,X,capMbr.Xe) )
      {
         *pMbrID = capMbr.mbrID;
         *pMbrLocation = X - capMbr.Xs;
         return;
      }
   }
   ATLASSERT(false); // should never get here
}

void CAnalysisAgentImp::GetSuperstructureFemModelLocation(ModelData* pModelData,Float64 X,MemberIDType* pMbrID,Float64* pMbrLocation)
{
   std::vector<BeamMember>::iterator iter(pModelData->m_SuperstructureMembers.begin());
   std::vector<BeamMember>::iterator end(pModelData->m_SuperstructureMembers.end());
   for ( ; iter != end; iter++ )
   {
      BeamMember& capMbr(*iter);
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

   case xbrTypes::pftCRReactions:
      return 4;

   case xbrTypes::pftSHReactions:
      return 5;

   case xbrTypes::pftPSReactions:
      return 6;

   case xbrTypes::pftREReactions:
      return 7;

   default:
      ATLASSERT(false);
   }

   return INVALID_ID;
}

void CAnalysisAgentImp::Invalidate(bool bCreateNewDataStructures)
{
   InvalidateModels(bCreateNewDataStructures);
   InvalidateResults(bCreateNewDataStructures);
}

//////////////////////////////////////////////////////////////////////
// IProjectEventSink
HRESULT CAnalysisAgentImp::OnProjectChanged()
{
   Invalidate();
   return S_OK;
}

//////////////////////////////////////////////////////////
// IBridgeDescriptionEventSink
HRESULT CAnalysisAgentImp::OnBridgeChanged(CBridgeChangedHint* pHint)
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

   Invalidate();
   return S_OK;
}

HRESULT CAnalysisAgentImp::OnGirderFamilyChanged()
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

   Invalidate();
   return S_OK;
}

HRESULT CAnalysisAgentImp::OnGirderChanged(const CGirderKey& girderKey,Uint32 lHint)
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

   Invalidate();
   return S_OK;
}

HRESULT CAnalysisAgentImp::OnLiveLoadChanged()
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

   Invalidate();
   return S_OK;
}

HRESULT CAnalysisAgentImp::OnLiveLoadNameChanged(LPCTSTR strOldName,LPCTSTR strNewName)
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

   Invalidate();
   return S_OK;
}

HRESULT CAnalysisAgentImp::OnConstructionLoadChanged()
{
   GET_IFACE(IEAFStatusCenter,pStatusCenter);
   pStatusCenter->RemoveByStatusGroupID(m_StatusGroupID);

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

IndexType CAnalysisAgentImp::GetLiveLoadConfigurationCount(PierIDType pierID,pgsTypes::LoadRatingType ratingType)
{
   ModelData* pModelData = GetModelData(pierID);
   GET_IFACE_NOCHECK(IXBRRatingSpecification,pRatingSpec);
   if ( ::IsPermitRatingType(ratingType) && pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmAASHTO )
   {
      return pModelData->m_LastSingleLaneLLConfigIdx + 1;
   }
   else
   {
      return pModelData->m_LiveLoadConfigurations.size();
   }
}

IndexType CAnalysisAgentImp::GetLoadedLaneCount(PierIDType pierID,IndexType liveLoadConfigIdx)
{
   ModelData* pModelData = GetModelData(pierID);
   return pModelData->m_LiveLoadConfigurations[liveLoadConfigIdx].m_LoadCases.size();
}

std::vector<Float64> CAnalysisAgentImp::GetWheelLineLocations(PierIDType pierID)
{
   ModelData* pModelData = GetModelData(pierID,MODEL_INIT_TOPOLOGY);
   return GetWheelLineLocations(pModelData);
}

WheelLineConfiguration CAnalysisAgentImp::GetLiveLoadConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType liveLoadConfigIdx,IndexType permitLaneIdx)
{
   ModelData* pModelData = GetModelData(pierID);
   LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[liveLoadConfigIdx];

#if defined _DEBUG
   if ( liveLoadConfigIdx <= pModelData->m_LastSingleLaneLLConfigIdx )
   {
      ATLASSERT(llConfig.m_LoadCases.size() == 1);
   }
#endif

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   bool bIsPermit = ::IsPermitRatingType(ratingType);

   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   bool bIsWSDOTMethod = (pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmWSDOT ? true : false);

   Float64 Rlgl = 0;
   if ( bIsPermit && bIsWSDOTMethod )
   {
      Rlgl = GetMaxLegalReaction(pierID);
   }
#if defined _DEBUG
   else
   {
      ATLASSERT(permitLaneIdx == INVALID_INDEX); // permit lane idx isn't used for this case, expecting it to be INVALID_INDEX
   }
#endif

   WheelLineConfiguration wheelConfig;
   IndexType laneIdx = 0;
   BOOST_FOREACH(LoadCaseIDType lcid,llConfig.m_LoadCases)
   {
      std::map<LoadCaseIDType,LaneConfiguration>::iterator found = pModelData->m_LaneConfigurations.find(lcid);
      ATLASSERT(found != pModelData->m_LaneConfigurations.end());

      LaneConfiguration& laneConfig = found->second;

      if ( bIsPermit && bIsWSDOTMethod )
      {
         if ( laneIdx == permitLaneIdx )
         {
            WheelLinePlacement leftWheelLinePlacement;
            leftWheelLinePlacement.P = -0.5*R;
            leftWheelLinePlacement.Xxb = laneConfig.Xleft;
            wheelConfig.push_back(leftWheelLinePlacement);

            WheelLinePlacement rightWheelLinePlacement;
            rightWheelLinePlacement.P = -0.5*R;
            rightWheelLinePlacement.Xxb = laneConfig.Xright;
            wheelConfig.push_back(rightWheelLinePlacement);
         }
         else
         {
            WheelLinePlacement leftWheelLinePlacement;
            leftWheelLinePlacement.P = -0.5*Rlgl;
            leftWheelLinePlacement.Xxb = laneConfig.Xleft;
            wheelConfig.push_back(leftWheelLinePlacement);

            WheelLinePlacement rightWheelLinePlacement;
            rightWheelLinePlacement.P = -0.5*Rlgl;
            rightWheelLinePlacement.Xxb = laneConfig.Xright;
            wheelConfig.push_back(rightWheelLinePlacement);
         }
      }
      else
      {
         WheelLinePlacement leftWheelLinePlacement;
         leftWheelLinePlacement.P = -0.5*R;
         leftWheelLinePlacement.Xxb = laneConfig.Xleft;
         wheelConfig.push_back(leftWheelLinePlacement);

         WheelLinePlacement rightWheelLinePlacement;
         rightWheelLinePlacement.P = -0.5*R;
         rightWheelLinePlacement.Xxb = laneConfig.Xright;
         wheelConfig.push_back(rightWheelLinePlacement);
      }

      laneIdx++;
   }

   return wheelConfig;
}

void CAnalysisAgentImp::GetGoverningMomentLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvMin,std::vector<IndexType>* pvMax)
{
   pvMin->clear();
   pvMax->clear();

   UnitLiveLoadResult& unitLiveLoadResult = GetUnitLiveLoadResult(pierID,poi);

   *pvMin = unitLiveLoadResult.m_MzMinLiveLoadConfigs;
   *pvMax = unitLiveLoadResult.m_MzMaxLiveLoadConfigs;
}

void CAnalysisAgentImp::GetGoverningShearLiveLoadConfigurations(PierIDType pierID,const xbrPointOfInterest& poi,std::vector<IndexType>* pvLLConfig)
{
   pvLLConfig->clear();

   UnitLiveLoadResult& unitLiveLoadResult = GetUnitLiveLoadResult(pierID,poi);
   *pvLLConfig = unitLiveLoadResult.m_FyLiveLoadConfigs;
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

   Float64 FxL, FxR, FyL, FyR, MzL, MzR;
   results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&FxL,&FyL,&MzL);
   results->ComputePOIForces(lcid,femPoiID,mftRight,lotGlobalProjected,&FxR,&FyR,&MzR);

   Float64 Mz;
   if ( IsZero(poi.GetDistFromStart()) )
   {
      Mz = -MzR;
   }
   else
   {
      Mz = MzL;
   }
   Mz = IsZero(Mz) ? 0 : Mz;
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

   Float64 FxL, FxR, FyL, FyR, MzL, MzR;
   results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&FxL,&FyL,&MzL);
   results->ComputePOIForces(lcid,femPoiID,mftRight,lotGlobalProjected,&FxR,&FyR,&MzR);

   FyL = IsZero(FyL) ? 0 : FyL;
   FyR = IsZero(FyR) ? 0 : FyR;

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

Float64 CAnalysisAgentImp::GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi)
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

sysSectionValue CAnalysisAgentImp::GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const xbrPointOfInterest& poi)
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

std::vector<Float64> CAnalysisAgentImp::GetMoment(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi)
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

std::vector<sysSectionValue> CAnalysisAgentImp::GetShear(PierIDType pierID,xbrTypes::CombinedForceType lcType,const std::vector<xbrPointOfInterest>& vPoi)
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

Float64 CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi)
{
   ModelData* pModelData = GetModelData(pierID);

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[llConfigIdx];

   Float64 FxLeft(0), FxRight(0);
   Float64 FyLeft(0), FyRight(0);
   Float64 MzLeft(0), MzRight(0);

   BOOST_FOREACH(LoadCaseIDType lcid,llConfig.m_LoadCases)
   {
      Float64 fxLeft, fyLeft, mzLeft;
      HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotMember,&fxLeft,&fyLeft,&mzLeft);
      ATLASSERT(SUCCEEDED(hr));

      Float64 fxRight, fyRight, mzRight;
      hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotMember,&fxRight,&fyRight,&mzRight);
      ATLASSERT(SUCCEEDED(hr));

      FxLeft += fxLeft;
      FyLeft += fyLeft;
      MzLeft += mzLeft;

      FxRight += fxRight;
      FyRight += fyRight;
      MzRight += mzRight;
   }

   Float64 Mz;
   if ( IsZero(poi.GetDistFromStart()) )
   {
      Mz = -MzRight;
   }
   else
   {
      Mz = MzLeft;
   }

   Mz = IsZero(Mz) ? 0 : Mz;

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction
   IndexType nLoadedLanes = GetLoadedLaneCount(pierID,llConfigIdx);
   Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(nLoadedLanes);

   if ( ::IsPermitRatingType(ratingType) && nLoadedLanes == 1 )
   {
      // MBE 6A.4.5.4.2a ... no MPF for permit cases
      mpf = 1.0;
   }

   Mz *= R*mpf;

   return Mz;
}

sysSectionValue CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const xbrPointOfInterest& poi)
{
   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);
   LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[llConfigIdx];

   Float64 FxLeft(0), FxRight(0);
   Float64 FyLeft(0), FyRight(0);
   Float64 MzLeft(0), MzRight(0);

   BOOST_FOREACH(LoadCaseIDType lcid,llConfig.m_LoadCases)
   {
      Float64 fxLeft, fyLeft, mzLeft;
      HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotMember,&fxLeft,&fyLeft,&mzLeft);
      ATLASSERT(SUCCEEDED(hr));

      Float64 fxRight, fyRight, mzRight;
      hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotMember,&fxRight,&fyRight,&mzRight);
      ATLASSERT(SUCCEEDED(hr));

      FxLeft += fxLeft;
      FyLeft += fyLeft;
      MzLeft += mzLeft;

      FxRight += fxRight;
      FyRight += fyRight;
      MzRight += mzRight;
   }

   FyLeft  = IsZero(FyLeft)  ? 0 : FyLeft;
   FyRight = IsZero(FyRight) ? 0 : FyRight;
   sysSectionValue Fy(-FyLeft,FyRight);

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   IndexType nLoadedLanes = GetLoadedLaneCount(pierID,llConfigIdx);
   Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(nLoadedLanes);

   if ( ::IsPermitRatingType(ratingType) && nLoadedLanes == 1 )
   {
      // MBE 6A.4.5.4.2a ... no MPF for permit cases
      mpf = 1.0;
   }

   Fy *= R*mpf;

   return Fy;
}

std::vector<Float64> CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<Float64> vM;
   vM.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 m = GetMoment(pierID,ratingType,vehicleIdx,llConfigIdx,poi);
      vM.push_back(m);
   }
   return vM;
}

std::vector<sysSectionValue> CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,const std::vector<xbrPointOfInterest>& vPoi)
{
   std::vector<sysSectionValue> vV;
   vV.reserve(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue v = GetShear(pierID,ratingType,vehicleIdx,llConfigIdx,poi);
      vV.push_back(v);
   }
   return vV;
}

//////////////////////////////////

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,Float64* pMpermit,Float64* pMlegal)
{
   ATLASSERT(::IsPermitRatingType(permitRatingType));
#if defined _DEBUG
   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   ATLASSERT(pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmWSDOT);
   // This method is only used for WSDOT permit ratings
#endif

   IndexType nLoadedLanes = GetLoadedLaneCount(pierID,llConfigIdx);
   if ( nLoadedLanes == 1 )
   {
      // if there is only one loaded lane, it is the permit vehicle... just use the regular implementation
      *pMpermit = GetMoment(pierID,permitRatingType,vehicleIdx,llConfigIdx,poi);
      *pMlegal = 0;
      return;
   }

   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);

   // Get the results for all loaded lanes having the same vehicle reaction
   LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[llConfigIdx];
   ATLASSERT(llConfig.m_LoadCases.size() == nLoadedLanes);

   Float64 FxLeftLegal(0), FxRightLegal(0);
   Float64 FyLeftLegal(0), FyRightLegal(0);
   Float64 MzLeftLegal(0), MzRightLegal(0);

   Float64 FxLeftPermit(0), FxRightPermit(0);
   Float64 FyLeftPermit(0), FyRightPermit(0);
   Float64 MzLeftPermit(0), MzRightPermit(0);

   HRESULT hr = S_OK;

   for ( IndexType laneIdx = 0; laneIdx < nLoadedLanes; laneIdx++ )
   {
      // get unit lane load forces
      LoadCaseIDType lcid = llConfig.m_LoadCases[laneIdx];
      Float64 fxLeft, fyLeft, mzLeft;
      hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotMember,&fxLeft,&fyLeft,&mzLeft);
      ATLASSERT(SUCCEEDED(hr));

      Float64 fxRight, fyRight, mzRight;
      hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotMember,&fxRight,&fyRight,&mzRight);
      ATLASSERT(SUCCEEDED(hr));

      if ( laneIdx == permitLaneIdx )
      {
         FxLeftPermit += fxLeft;
         FyLeftPermit += fyLeft;
         MzLeftPermit += mzLeft;

         FxRightPermit += fxRight;
         FyRightPermit += fyRight;
         MzRightPermit += mzRight;
      }
      else
      {
         FxLeftLegal += fxLeft;
         FyLeftLegal += fyLeft;
         MzLeftLegal += mzLeft;

         FxRightLegal += fxRight;
         FyRightLegal += fyRight;
         MzRightLegal += mzRight;
      }
   }

   if ( IsZero(poi.GetDistFromStart()) )
   {
      *pMpermit = -MzRightPermit;
      *pMlegal  = -MzRightLegal;
   }
   else
   {
      *pMpermit = MzLeftPermit;
      *pMlegal  = MzLeftLegal;
   }

   GET_IFACE(IXBRProject,pProject);
   Float64 Rpermit = pProject->GetLiveLoadReaction(pierID,permitRatingType,vehicleIdx); // single lane reaction
   Float64 Rlegal  = GetMaxLegalReaction(pierID);

   *pMpermit *= Rpermit;
   *pMlegal  *= Rlegal;

   *pMpermit = IsZero(*pMpermit) ? 0 : *pMpermit;
   *pMlegal  = IsZero(*pMlegal)  ? 0 : *pMlegal;

   Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(nLoadedLanes);
   if ( nLoadedLanes == 1 )
   {
      mpf = 1.0;
   }

   // NOTE: Multiple presense factor is applied to both the permit and legal loads. Consider the following
   // (ignore load factors)
   // Q = DC + DW + mpf(LL)
   // LL = LL_Permit + LL_Legal
   // therefore Q = DC + DW + mpf(LL_Permit + LL_Legal);
   // Re-arrange into the rating factor equation
   // RF = [Q - DC - DW - mpf(LL_Legal)]/[mpf(LL_Permit)]

   *pMlegal  *= mpf;
   *pMpermit *= mpf;
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMpermit,std::vector<Float64>* pvMlegal)
{
   pvMpermit->clear();
   pvMpermit->resize(vPoi.size());
   pvMlegal->clear();
   pvMlegal->resize(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 Mpermit,Mlegal;
      GetMoment(pierID,permitRatingType,vehicleIdx,llConfigIdx,permitLaneIdx,poi,&Mpermit,&Mlegal);
      pvMpermit->push_back(Mpermit);
      pvMlegal->push_back(Mlegal);
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const xbrPointOfInterest& poi,sysSectionValue* pVpermit,sysSectionValue* pVlegal)
{
   ATLASSERT(::IsPermitRatingType(permitRatingType));
#if defined _DEBUG
   GET_IFACE(IXBRRatingSpecification,pRatingSpec);
   ATLASSERT(pRatingSpec->GetPermitRatingMethod() == xbrTypes::prmWSDOT);
   // This method is only used for WSDOT permit ratings
#endif

   IndexType nLoadedLanes = GetLoadedLaneCount(pierID,llConfigIdx);
   if ( nLoadedLanes == 1 )
   {
      // if there is only one loaded lane, it is the permit vehicle... just use the regular implementation
      *pVpermit = GetShear(pierID,permitRatingType,vehicleIdx,llConfigIdx,poi);
      *pVlegal = 0;
      return;
   }

   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);

   // Get the results for all loaded lanes having the same vehicle reaction
   LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[llConfigIdx];
   ATLASSERT(llConfig.m_LoadCases.size() == nLoadedLanes);

   Float64 FxLeftLegal(0), FxRightLegal(0);
   Float64 FyLeftLegal(0), FyRightLegal(0);
   Float64 MzLeftLegal(0), MzRightLegal(0);

   Float64 FxLeftPermit(0), FxRightPermit(0);
   Float64 FyLeftPermit(0), FyRightPermit(0);
   Float64 MzLeftPermit(0), MzRightPermit(0);

   HRESULT hr = S_OK;

   for ( IndexType laneIdx = 0; laneIdx < nLoadedLanes; laneIdx++ )
   {
      // get unit lane load forces
      LoadCaseIDType lcid = llConfig.m_LoadCases[laneIdx];
      Float64 fxLeft, fyLeft, mzLeft;
      hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotMember,&fxLeft,&fyLeft,&mzLeft);
      ATLASSERT(SUCCEEDED(hr));

      Float64 fxRight, fyRight, mzRight;
      hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotMember,&fxRight,&fyRight,&mzRight);
      ATLASSERT(SUCCEEDED(hr));

      if ( laneIdx == permitLaneIdx )
      {
         FxLeftPermit += fxLeft;
         FyLeftPermit += fyLeft;
         MzLeftPermit += mzLeft;

         FxRightPermit += fxRight;
         FyRightPermit += fyRight;
         MzRightPermit += mzRight;
      }
      else
      {
         FxLeftLegal += fxLeft;
         FyLeftLegal += fyLeft;
         MzLeftLegal += mzLeft;

         FxRightLegal += fxRight;
         FyRightLegal += fyRight;
         MzRightLegal += mzRight;
      }
   }

   *pVpermit = sysSectionValue(-FyLeftPermit,FyRightPermit);
   *pVlegal  = sysSectionValue(-FyLeftLegal, FyRightLegal );

   GET_IFACE(IXBRProject,pProject);
   Float64 Rpermit = pProject->GetLiveLoadReaction(pierID,permitRatingType,vehicleIdx); // single lane reaction
   Float64 Rlegal  = GetMaxLegalReaction(pierID);

   *pVpermit *= Rpermit;
   *pVlegal  *= Rlegal;

   pVpermit->Left()  = IsZero(pVpermit->Left())  ? 0 : pVpermit->Left();
   pVpermit->Right() = IsZero(pVpermit->Right()) ? 0 : pVpermit->Right();
   pVlegal->Left()   = IsZero(pVlegal->Left())   ? 0 : pVlegal->Left();
   pVlegal->Right()  = IsZero(pVlegal->Right())  ? 0 : pVlegal->Right();

   Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(nLoadedLanes);
   if ( nLoadedLanes == 1 )
   {
      mpf = 1.0;
   }

   // NOTE: Multiple presense factor is applied to both the permit and legal loads. Consider the following
   // (ignore load factors)
   // Q = DC + DW + mpf(LL)
   // LL = LL_Permit + LL_Legal
   // therefore Q = DC + DW + mpf(LL_Permit + LL_Legal);
   // Re-arrange into the rating factor equation
   // RF = [Q - DC - DW - mpf(LL_Legal)]/[mpf(LL_Permit)]

   *pVlegal  *= mpf;
   *pVpermit *= mpf;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType permitRatingType,VehicleIndexType vehicleIdx,IndexType llConfigIdx,IndexType permitLaneIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvVpermit,std::vector<sysSectionValue>* pvVlegal)
{
   pvVpermit->clear();
   pvVpermit->resize(vPoi.size());
   pvVlegal->clear();
   pvVlegal->resize(vPoi.size());
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue Vpermit,Vlegal;
      GetShear(pierID,permitRatingType,vehicleIdx,llConfigIdx,permitLaneIdx,poi,&Vpermit,&Vlegal);
      pvVpermit->push_back(Vpermit);
      pvVlegal->push_back(Vlegal);
   }
}

//////////////////////////////////

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration*/)
{
   UnitLiveLoadResult& liveLoadResult = GetUnitLiveLoadResult(pierID,poi);

   // permit rating results is always based on single loaded lane. the load factors make adjustments and account for
   // the presence of vehicles in other lanes (See MBE 6A.4.5.4.2a)
   bool bIsPermitRating = ::IsPermitRatingType(ratingType);
   if ( bIsPermitRating )
   {
      Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(1); // mpf for one loaded lane
      *pMin = liveLoadResult.m_MzMin_SingleLane/mpf;
      *pMax = liveLoadResult.m_MzMax_SingleLane/mpf;
   }
   else
   {
      *pMin = liveLoadResult.m_MzMin;
      *pMax = liveLoadResult.m_MzMax;
   }

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   *pMin *= R;
   *pMax *= R;

   *pMin = IsZero(*pMin) ? 0 : *pMin;
   *pMax = IsZero(*pMax) ? 0 : *pMax;

   if ( pMinLLConfigIdx )
   {
      *pMinLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_MzMin_SingleLane : liveLoadResult.m_llConfigIdx_MzMin);
   }

   if ( pMaxLLConfigIdx )
   {
      *pMaxLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_MzMax_SingleLane : liveLoadResult.m_llConfigIdx_MzMax);
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,IndexType* pMinLLConfigIdx,IndexType* pMaxLLConfigIdx/*WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration*/)
{
   UnitLiveLoadResult& liveLoadResult = GetUnitLiveLoadResult(pierID,poi);

   // permit rating results is always based on single loaded lane. the load factors make adjustments and account for
   // the presence of vehicles in other lanes (See MBE 6A.4.5.4.2a)
   bool bIsPermitRating = ::IsPermitRatingType(ratingType);
   if ( bIsPermitRating )
   {
      Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(1); // mpf for one loaded lane
      *pMin = liveLoadResult.m_FyMin_SingleLane/mpf;
      *pMax = liveLoadResult.m_FyMax_SingleLane/mpf;
   }
   else
   {
      *pMin = liveLoadResult.m_FyMin;
      *pMax = liveLoadResult.m_FyMax;
   }

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   pMin->Left()  *= R;
   pMin->Right() *= R;
   
   pMax->Left()  *= R;
   pMax->Right() *= R;

   pMin->Left()  = IsZero(pMin->Left())  ? 0 : pMin->Left();
   pMin->Right() = IsZero(pMin->Right()) ? 0 : pMin->Right();
   pMax->Left()  = IsZero(pMax->Left())  ? 0 : pMax->Left();
   pMax->Right() = IsZero(pMax->Right()) ? 0 : pMax->Right();

   if ( pMinLLConfigIdx )
   {
      if ( IsEqual(MaxMagnitude(pMin->Left(),pMin->Right()),pMin->Left()) )
      {
         *pMinLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_FyLeftMin_SingleLane : liveLoadResult.m_llConfigIdx_FyLeftMin);
      }
      else
      {
         *pMinLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_FyRightMin_SingleLane : liveLoadResult.m_llConfigIdx_FyRightMin);
      }
   }

   if ( pMaxLLConfigIdx )
   {
      if ( IsEqual(MaxMagnitude(pMax->Left(),pMax->Right()),pMax->Left()) )
      {
         *pMaxLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_FyLeftMax_SingleLane : liveLoadResult.m_llConfigIdx_FyLeftMax);
      }
      else
      {
         *pMaxLLConfigIdx = (bIsPermitRating ? liveLoadResult.m_llConfigIdx_FyRightMax_SingleLane : liveLoadResult.m_llConfigIdx_FyRightMax);
      }
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration*/)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());

   if ( pvMinLLConfigIdx )
   {
      pvMinLLConfigIdx->clear();
      pvMinLLConfigIdx->reserve(vPoi.size());
   }

   if ( pvMaxLLConfigIdx )
   {
      pvMaxLLConfigIdx->clear();
      pvMaxLLConfigIdx->reserve(vPoi.size());
   }

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      //WheelLineConfiguration minConfig, maxConfig;
      IndexType minLLConfigIdx, maxLLConfigIdx;
      GetMoment(pierID,ratingType,vehicleIdx,poi,&min,&max,pvMinLLConfigIdx ? &minLLConfigIdx : NULL,pvMaxLLConfigIdx ? &maxLLConfigIdx : NULL);
      pvMin->push_back(min);
      pvMax->push_back(max);
      if ( pvMinLLConfigIdx )
      {
         pvMinLLConfigIdx->push_back(minLLConfigIdx);
      }
      if ( pvMaxLLConfigIdx )
      {
         pvMaxLLConfigIdx->push_back(maxLLConfigIdx);
      }
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<IndexType>* pvMinLLConfigIdx,std::vector<IndexType>* pvMaxLLConfigIdx/*std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration*/)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   if ( pvMinLLConfigIdx )
   {
      pvMinLLConfigIdx->clear();
      pvMinLLConfigIdx->reserve(vPoi.size());
   }
   if ( pvMaxLLConfigIdx )
   {
      pvMaxLLConfigIdx->clear();
      pvMaxLLConfigIdx->reserve(vPoi.size());
   }

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      //WheelLineConfiguration minLeftConfig, minRightConfig, maxLeftConfig, maxRightConfig;
      IndexType minLLConfigIdx, maxLLConfigIdx;
      GetShear(pierID,ratingType,vehicleIdx,poi,&min,&max,pvMinLLConfigIdx ? &minLLConfigIdx : NULL,pvMaxLLConfigIdx ? &maxLLConfigIdx : NULL);
      pvMin->push_back(min);
      pvMax->push_back(max);

      if ( pvMinLLConfigIdx )
      {
         pvMinLLConfigIdx->push_back(minLLConfigIdx);
      }

      if ( pvMaxLLConfigIdx )
      {
         pvMaxLLConfigIdx->push_back(maxLLConfigIdx);
      }
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,VehicleIndexType* pMinVehicleIdx,VehicleIndexType* pMaxVehicleIdx)
{
   GET_IFACE(IXBRProject,pProject);
   IndexType nLiveLoadReactions = pProject->GetLiveLoadReactionCount(pierID,ratingType);

   *pMin = DBL_MAX;
   *pMax = -DBL_MAX;
   if ( pMinVehicleIdx )
   {
      *pMinVehicleIdx = INVALID_INDEX;
   }
   
   if ( pMaxVehicleIdx )
   {
      *pMaxVehicleIdx = INVALID_INDEX;
   }

   if ( nLiveLoadReactions == 0 )
   {
      *pMin = 0;
      *pMax = 0;
      return;
   }

   for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nLiveLoadReactions; vehicleIdx++ )
   {
      Float64 min,max;
      GetMoment(pierID,ratingType,vehicleIdx,poi,&min,&max,NULL,NULL);

      if ( min < *pMin )
      {
         *pMin = min;
         if ( pMinVehicleIdx )
         {
            *pMinVehicleIdx = vehicleIdx;
         }
      }

      if ( *pMax < max )
      {
         *pMax = max;
         if ( pMaxVehicleIdx )
         {
            *pMaxVehicleIdx = vehicleIdx;
         }
      }
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,VehicleIndexType* pMinLeftVehicleIdx,VehicleIndexType* pMinRightVehicleIdx,VehicleIndexType* pMaxLeftVehicleIdx,VehicleIndexType* pMaxRightVehicleIdx)
{
   GET_IFACE(IXBRProject,pProject);
   IndexType nLiveLoadReactions = pProject->GetLiveLoadReactionCount(pierID,ratingType);

   *pMin = DBL_MAX;
   *pMax = -DBL_MAX;
   if ( pMinLeftVehicleIdx )
   {
      *pMinLeftVehicleIdx = INVALID_INDEX;
   }
   if ( pMinRightVehicleIdx )
   {
      *pMinRightVehicleIdx = INVALID_INDEX;
   }
   
   if ( pMaxLeftVehicleIdx )
   {
      *pMaxLeftVehicleIdx = INVALID_INDEX;
   }
   if ( pMaxRightVehicleIdx )
   {
      *pMaxRightVehicleIdx = INVALID_INDEX;
   }

   if ( nLiveLoadReactions == 0 )
   {
      *pMin = 0;
      *pMax = 0;
      return;
   }

   for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nLiveLoadReactions; vehicleIdx++ )
   {
      sysSectionValue min,max;
      GetShear(pierID,ratingType,vehicleIdx,poi,&min,&max,NULL,NULL);
      if ( min.Left() < (*pMin).Left() )
      {
         pMin->Left() = min.Left();
         if ( pMinLeftVehicleIdx )
         {
            *pMinLeftVehicleIdx = vehicleIdx;
         }
      }

      if ( min.Right() < (*pMin).Right() )
      {
         pMin->Right() = min.Right();
         if ( pMinRightVehicleIdx )
         {
            *pMinRightVehicleIdx = vehicleIdx;
         }
      }

      if ( pMax->Left() < max.Left() )
      {
         pMax->Left() = max.Left();
         if ( pMaxLeftVehicleIdx )
         {
            *pMaxLeftVehicleIdx = vehicleIdx;
         }
      }

      if ( pMax->Right() < max.Right() )
      {
         pMax->Right() = max.Right();
         if ( pMaxRightVehicleIdx )
         {
            *pMaxRightVehicleIdx = vehicleIdx;
         }
      }
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<VehicleIndexType>* pvMinVehicleIdx,std::vector<VehicleIndexType>* pvMaxVehicleIdx)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   if ( pvMinVehicleIdx )
   {
      pvMinVehicleIdx->clear();
      pvMinVehicleIdx->reserve(vPoi.size());
   }
   if ( pvMaxVehicleIdx )
   {
      pvMaxVehicleIdx->clear();
      pvMaxVehicleIdx->reserve(vPoi.size());
   }
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      VehicleIndexType minIdx, maxIdx;
      GetMoment(pierID,ratingType,poi,&min,&max,&minIdx,&maxIdx);
      pvMin->push_back(min);
      pvMax->push_back(max);

      if ( pvMinVehicleIdx )
      {
         pvMinVehicleIdx->push_back(minIdx);
      }

      if ( pvMaxVehicleIdx )
      {
         pvMaxVehicleIdx->push_back(maxIdx);
      }
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<VehicleIndexType>* pvMinLeftVehicleIdx,std::vector<VehicleIndexType>* pvMinRightVehicleIdx,std::vector<VehicleIndexType>* pvMaxLeftVehicleIdx,std::vector<VehicleIndexType>* pvMaxRightVehicleIdx)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   if ( pvMinLeftVehicleIdx )
   {
      pvMinLeftVehicleIdx->clear();
      pvMinLeftVehicleIdx->reserve(vPoi.size());
   }
   if ( pvMinRightVehicleIdx )
   {
      pvMinRightVehicleIdx->clear();
      pvMinRightVehicleIdx->reserve(vPoi.size());
   }
   if ( pvMaxLeftVehicleIdx )
   {
      pvMaxLeftVehicleIdx->clear();
      pvMaxLeftVehicleIdx->reserve(vPoi.size());
   }
   if ( pvMaxRightVehicleIdx )
   {
      pvMaxRightVehicleIdx->clear();
      pvMaxRightVehicleIdx->reserve(vPoi.size());
   }

   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      VehicleIndexType minLeftIdx, minRightIdx, maxLeftIdx, maxRightIdx;
      GetShear(pierID,ratingType,poi,&min,&max,&minLeftIdx,&minRightIdx,&maxLeftIdx,&maxRightIdx);
      pvMin->push_back(min);
      pvMax->push_back(max);

      if ( pvMinLeftVehicleIdx )
      {
         pvMinLeftVehicleIdx->push_back(minLeftIdx);
      }

      if ( pvMinRightVehicleIdx )
      {
         pvMinRightVehicleIdx->push_back(minRightIdx);
      }

      if ( pvMaxLeftVehicleIdx )
      {
         pvMaxLeftVehicleIdx->push_back(maxLeftIdx);
      }

      if ( pvMaxRightVehicleIdx )
      {
         pvMaxRightVehicleIdx->push_back(maxRightIdx);
      }
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax)
{
   pgsTypes::LoadRatingType ratingType = RatingTypeFromLimitState(limitState);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor(limitState);
   Float64 gDW = pProject->GetDWLoadFactor(limitState);
   Float64 gCR = pProject->GetCRLoadFactor(limitState);
   Float64 gSH = pProject->GetSHLoadFactor(limitState);
   Float64 gPS = pProject->GetPSLoadFactor(limitState);
   Float64 gRE = pProject->GetRELoadFactor(limitState);
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,limitState,INVALID_INDEX);

   Float64 DC = 0;
   std::vector<xbrTypes::ProductForceType> vDC = GetLoads(xbrTypes::lcDC);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDC)
   {
      Float64 dc = GetMoment(pierID,pfType,poi);
      DC += dc;
   }

   Float64 DW = 0;
   std::vector<xbrTypes::ProductForceType> vDW = GetLoads(xbrTypes::lcDW);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDW)
   {
      Float64 dw = GetMoment(pierID,pfType,poi);
      DW += dw;
   }

   Float64 CR = 0;
   std::vector<xbrTypes::ProductForceType> vCR = GetLoads(xbrTypes::lcCR);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vCR)
   {
      Float64 cr = GetMoment(pierID,pfType,poi);
      CR += cr;
   }

   Float64 SH = 0;
   std::vector<xbrTypes::ProductForceType> vSH = GetLoads(xbrTypes::lcSH);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vSH)
   {
      Float64 sh = GetMoment(pierID,pfType,poi);
      SH += sh;
   }

   Float64 PS = 0;
   std::vector<xbrTypes::ProductForceType> vPS = GetLoads(xbrTypes::lcPS);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vPS)
   {
      Float64 ps = GetMoment(pierID,pfType,poi);
      PS += ps;
   }

   Float64 RE = 0;
   std::vector<xbrTypes::ProductForceType> vRE = GetLoads(xbrTypes::lcRE);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vRE)
   {
      Float64 re = GetMoment(pierID,pfType,poi);
      RE += re;
   }

   Float64 LLIMmin, LLIMmax;
   GetMoment(pierID,ratingType,poi,&LLIMmin,&LLIMmax,NULL,NULL);

   *pMin = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmin;
   *pMax = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmax;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax)
{
   pgsTypes::LoadRatingType ratingType = RatingTypeFromLimitState(limitState);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor(limitState);
   Float64 gDW = pProject->GetDWLoadFactor(limitState);
   Float64 gCR = pProject->GetCRLoadFactor(limitState);
   Float64 gSH = pProject->GetSHLoadFactor(limitState);
   Float64 gPS = pProject->GetPSLoadFactor(limitState);
   Float64 gRE = pProject->GetRELoadFactor(limitState);
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,limitState,INVALID_INDEX);

   sysSectionValue DC = 0;
   std::vector<xbrTypes::ProductForceType> vDC = GetLoads(xbrTypes::lcDC);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDC)
   {
      sysSectionValue dc = GetShear(pierID,pfType,poi);
      DC += dc;
   }

   sysSectionValue DW = 0;
   std::vector<xbrTypes::ProductForceType> vDW = GetLoads(xbrTypes::lcDW);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vDW)
   {
      sysSectionValue dw = GetShear(pierID,pfType,poi);
      DW += dw;
   }

   sysSectionValue CR = 0;
   std::vector<xbrTypes::ProductForceType> vCR = GetLoads(xbrTypes::lcCR);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vCR)
   {
      sysSectionValue cr = GetShear(pierID,pfType,poi);
      CR += cr;
   }

   sysSectionValue SH = 0;
   std::vector<xbrTypes::ProductForceType> vSH = GetLoads(xbrTypes::lcSH);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vSH)
   {
      sysSectionValue sh = GetShear(pierID,pfType,poi);
      SH += sh;
   }

   sysSectionValue PS = 0;
   std::vector<xbrTypes::ProductForceType> vPS = GetLoads(xbrTypes::lcPS);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vPS)
   {
      sysSectionValue ps = GetShear(pierID,pfType,poi);
      PS += ps;
   }

   sysSectionValue RE = 0;
   std::vector<xbrTypes::ProductForceType> vRE = GetLoads(xbrTypes::lcRE);
   BOOST_FOREACH(xbrTypes::ProductForceType pfType,vRE)
   {
      sysSectionValue re = GetShear(pierID,pfType,poi);
      RE += re;
   }

   sysSectionValue LLIMmin, LLIMmax;
   GetShear(pierID,ratingType,poi,&LLIMmin,&LLIMmax,NULL,NULL,NULL,NULL);

   *pMin = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmin;
   *pMax = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmax;
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

std::vector<xbrTypes::ProductForceType> CAnalysisAgentImp::GetLoads(xbrTypes::CombinedForceType lcType)
{
   std::vector<xbrTypes::ProductForceType> vPFTypes;
   switch(lcType)
   {
   case xbrTypes::lcDC:
      vPFTypes.push_back(xbrTypes::pftLowerXBeam);
      vPFTypes.push_back(xbrTypes::pftUpperXBeam);
      vPFTypes.push_back(xbrTypes::pftDCReactions);
      break;

   case xbrTypes::lcDW:
      vPFTypes.push_back(xbrTypes::pftDWReactions);
      break;

   case xbrTypes::lcSH:
      vPFTypes.push_back(xbrTypes::pftSHReactions);
      break;

   case xbrTypes::lcCR:
      vPFTypes.push_back(xbrTypes::pftCRReactions);
      break;

   case xbrTypes::lcPS:
      vPFTypes.push_back(xbrTypes::pftPSReactions);
      break;

   case xbrTypes::lcRE:
      vPFTypes.push_back(xbrTypes::pftREReactions);
      break;

   default:
      ATLASSERT(false);
   }

   return vPFTypes;
}

void CAnalysisAgentImp::ComputeLiveLoadLocations(PierIDType pierID,ModelData* pModelData)
{
   GET_IFACE(IXBRPier,pPier);
   Float64 Wcc = pPier->GetCurbToCurbWidth(pierID); // normal to alignment

   // Get the lane configuration for the curb-to-curb width (measured normal to the alignment)
   Float64 wLane,wLoadedLane;
   IndexType nLanes;
   GetLaneInfo(Wcc,&wLane,&nLanes,&wLoadedLane);


   // Adjust Wcc and wLoadedLane so that they are measured in the plane of the pier
   Float64 skew = pPier->GetSkewAngle(pierID);
   Wcc /= cos(skew);
   wLoadedLane /= cos(skew);

   GET_IFACE(IXBRProject,pProject);
   Float64 maxStepSize = pProject->GetMaxLiveLoadStepSize();

   nLanes = Min(nLanes,pProject->GetMaxLoadedLanes());

   Float64 LCO, RCO;
   pProject->GetCurbLineOffset(pierID,&LCO,&RCO);
   Float64 XcurbLine = pPier->ConvertPierToCrossBeamCoordinate(pierID,LCO);

   for ( IndexType nLoadedLanes = 1; nLoadedLanes <= nLanes; nLoadedLanes++ )
   {
      IndexType nLaneGaps = nLoadedLanes-1; // number of "gaps" between loaded lanes
      Float64 Wll = nLoadedLanes*wLoadedLane; // width of live load, measured in the plane of the pier
      IndexType nTotalSteps = (IndexType)ceil((Wcc - Wll)/maxStepSize); // total number of steps to move live load from left to right curb lines
      Float64 stepSize = (0 < nTotalSteps ? (Wcc-Wll)/nTotalSteps : 0); // actual step size

      // The lane configuration (position of the loaded lanes) is defined as a sequence of "gap indicies".
      // The size of the gap between lanes is the gap position index times the step size.
      // The step size of the gaps is equal to the step size for movig the lane configuration 
      // along the cross beam
      //
      //  wLoadedLane      wLoadedLane         wLoadedLane
      //  |<------>|       |<------>|          |<------>|
      //  ==========       ==========          ==========
      //            |<--->|          |<------>|
      //              gap               gap
      std::vector<std::vector<IndexType>> vGapPositions;
      GetLaneGapConfiguration(nTotalSteps,nLaneGaps,vGapPositions);

      // for each lane configuration, get the wheel line reaction configuration
      // and analyze the structure for the configuration. The configuration, in general,
      // will not be as wide as the curb-to-curb width, so step the configuration until
      // it reaches the right curb line.

      //
      //  wLoadedLanewLoadedLane         wLoadedLane
      //  |<------>|<------>|           |<------>|
      //  ===================           ==========   ---->>> move configuration by (stepIdx)*(step size) until the right lane gets to the right edge of the XBeam
      //           |         |<------->|
      //           gap=0         gap = (gap position idx)*(step size)
      //
      // Increment the gap position and repeat...

      BOOST_FOREACH(std::vector<IndexType>& vGapPosition,vGapPositions)
      {
         // location of wheel lines measured from the left curb line... with the first wheel line being 
         // at its left-most position. The position of the second wheel line is governed by
         // the gage spacing of the live load (6', adjusted for skew). The position of the remaining
         // wheel lines are governed by the position of the lane (gap position).
         std::vector<Float64> vWheelLinePositions = GetWheelLinePositions(skew,stepSize,wLoadedLane,vGapPosition);
         ATLASSERT(vWheelLinePositions.size() % 2 == 0); // must be even number... two wheel lines per lane
         ATLASSERT(vWheelLinePositions.size()/2 == nLoadedLanes); // must be 2 times as many wheel lines as loaded lanes

         // get the number of steps used to make the wheel line reaction configuration.
         IndexType nStepsUsed = std::accumulate(vGapPosition.begin(),vGapPosition.end(),(IndexType)0);

         // the number of times we need to step the wheel line reaction configuration towards the right curb line
         // is equal to the total number of steps less the number of steps used by the configuration.
         IndexType nStepsRemaining = nTotalSteps - nStepsUsed;

#if defined _DEBUG
         // at the last step, the right wheel line, in the right-most lane, must be 2' from the right curb line
         Float64 w2 = ::ConvertToSysUnits(2.0,unitMeasure::Feet); // 2' shy distance from curb-line
         w2 /= cos(skew);
         ATLASSERT(IsEqual(vWheelLinePositions.back() + stepSize*nStepsRemaining + w2,Wcc));
#endif

         // Step the wheel line reaction configuration towards the right curb line, analyzing the
         // cross beam for each loading position.
         for ( IndexType stepIdx = 0; stepIdx <= nStepsRemaining; stepIdx++ )
         {
            // Xoffset is the amount to shift the wheel line configuration towards the right curb line.
            // Remember the vWheelLinePositions is a configuration with the lanes all shifted to the
            // left curb line. Xoffset moves the entire configuration towards the right curb line
            Float64 Xoffset = stepSize*stepIdx; 
            Xoffset += XcurbLine;

            // Create the information for the wheel line loads that are to be applied to the FEM model.
            // Wheel line position is adjusted from default position by Xoffset.
            LiveLoadConfiguration llConfig;
            llConfig.m_LoadCases = InitializeWheelLineLoads(pModelData,Xoffset,vWheelLinePositions);
            ATLASSERT(llConfig.m_LoadCases.size() == nLoadedLanes);

            pModelData->m_LiveLoadConfigurations.push_back(llConfig);
         } // next step
      } // next gap position
   } // next number of loaded lanes
}

void CAnalysisAgentImp::GetLaneGapConfiguration(IndexType nTotalSteps,IndexType nLaneGaps,std::vector<std::vector<IndexType>>& vGapPositions)
{
   if ( nLaneGaps == 0 )
   {
      std::vector<IndexType> vDigits;
      vGapPositions.push_back(vDigits);
      return;
   }

   static std::vector<IndexType> vDigits;
   for ( IndexType stepIdx = 0; stepIdx <= nTotalSteps; stepIdx++ )
   {
      vDigits.push_back(stepIdx);
      if ( 1 < nLaneGaps )
      {
         GetLaneGapConfiguration(nTotalSteps-stepIdx,nLaneGaps-1,vGapPositions);
      }
      else
      {
         vGapPositions.push_back(vDigits);
         vDigits.pop_back();
      }
   }

   if ( 0 < vDigits.size() )
   {
      vDigits.pop_back();
   }
}

std::vector<Float64> CAnalysisAgentImp::GetWheelLinePositions(Float64 skew,Float64 stepSize,Float64 wLoadedLane,std::vector<IndexType>& vGapPosition)
{
   Float64 w3 = ::ConvertToSysUnits(3.0,unitMeasure::Feet); // 6 ft spacing between wheel lines... wheel line is +/-3' from CL lane
   w3 /= cos(skew); // we are working in the plane of the pier, so make skew adjustment

   std::vector<Float64> vLoadPositions;
   // first pair of wheel line loads... at left curb line
   vLoadPositions.push_back(wLoadedLane/2-w3);
   vLoadPositions.push_back(wLoadedLane/2+w3);

   std::vector<IndexType>::iterator gapPositionIter(vGapPosition.begin());
   std::vector<IndexType>::iterator gapPositionIterEnd(vGapPosition.end());
   IndexType laneIdx = 1;
   Float64 totalGapWidth = 0;
   for ( ; gapPositionIter != gapPositionIterEnd; gapPositionIter++, laneIdx++ )
   {
      IndexType gapPosition(*gapPositionIter);
      Float64 gapWidth = stepSize*gapPosition;
      totalGapWidth += gapWidth;
      
      Float64 XclLane = totalGapWidth + laneIdx*wLoadedLane + wLoadedLane/2;

      Float64 XleftWheelLine  = XclLane - w3;
      Float64 XrightWheelLine = XclLane + w3;

      vLoadPositions.push_back(XleftWheelLine);
      vLoadPositions.push_back(XrightWheelLine);
   }

   return vLoadPositions;
}

std::vector<LoadCaseIDType> CAnalysisAgentImp::InitializeWheelLineLoads(ModelData* pModelData,Float64 Xoffset,std::vector<Float64>& vWheelLinePositions)
{
   // keep track of the last load case ID used for one loaded lane
   // vLoadPositions has two point loads per lane, so the number of loaded lanes
   // is half the size of the container
   IndexType nLoadedLanes = vWheelLinePositions.size()/2;

   if ( nLoadedLanes == 1 )
   {
      pModelData->m_LastSingleLaneLLConfigIdx = pModelData->m_LiveLoadConfigurations.size();
   }

   // for each loaded lane, create a FEM2D load case that has the two
   // wheel line loads
   std::vector<LoadCaseIDType> vLoadCases;
   std::vector<Float64>::iterator iter(vWheelLinePositions.begin());
   std::vector<Float64>::iterator end(vWheelLinePositions.end());
   for ( ; iter != end; iter += 2 )
   {
      // these are the left curb line justified position of the wheel lines
      Float64 Xleft  = *iter;
      Float64 Xright = *(iter+1);

      // adjust the wheel line positions for the current step
      Xleft  += Xoffset;
      Xright += Xoffset;

      // Check to see if we've previously had a vehicle in this position (stored based on Xcenter)
      Float64 Xcenter = (Xleft + Xright)/2;
      std::map<Float64,LoadCaseIDType,Float64_less>::iterator found = pModelData->m_LiveLoadCases.find(Xcenter);
      if ( found != pModelData->m_LiveLoadCases.end() )
      {
         // there is already a load case for this lane position... simply reference it
         LoadCaseIDType lcid = found->second;
         vLoadCases.push_back(lcid);
         continue;
      }

      // This is a new position... create a FEM2d load case

      LoadCaseIDType lcid = pModelData->m_NextLiveLoadCaseID++;
      vLoadCases.push_back(lcid);

      // save the lane position and load case ID for future reference
      pModelData->m_LiveLoadCases.insert(std::make_pair(Xcenter,lcid));

      // save the lane configuration for future reference
      LaneConfiguration laneConfig;
      laneConfig.Xleft  = Xleft;
      laneConfig.Xright = Xright;
      pModelData->m_LaneConfigurations.insert(std::make_pair(lcid,laneConfig));
   }

   return vLoadCases;
}

void CAnalysisAgentImp::ApplyWheelLineLoadsToFemModel(ModelData* pModelData)
{
   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   std::map<LoadCaseIDType,LaneConfiguration>::iterator iter(pModelData->m_LaneConfigurations.begin());
   std::map<LoadCaseIDType,LaneConfiguration>::iterator end(pModelData->m_LaneConfigurations.end());
   for ( ; iter != end; iter++ )
   {
      LoadCaseIDType lcid = iter->first;
      LaneConfiguration& laneConfig = iter->second;

      // create the load case
      CComPtr<IFem2dLoading> loading;
      loadings->Create(lcid,&loading);

      CComPtr<IFem2dPointLoadCollection> pointLoads;
      loading->get_PointLoads(&pointLoads);

      // create the loading for each wheel line
      LoadIDType loadID = 0;
      Float64 P = -0.5; // unit lane load = 0.5 per wheel line

      MemberIDType mbrID;
      Float64 mbrLocation;

      // left wheel line load
      GetSuperstructureFemModelLocation(pModelData,laneConfig.Xleft,&mbrID,&mbrLocation);

      CComPtr<IFem2dPointLoad> pointLoad;
      pointLoads->Create(loadID++,mbrID,mbrLocation,0.0,P,0.0,lotGlobal,&pointLoad);

      // right wheel line load
      GetSuperstructureFemModelLocation(pModelData,laneConfig.Xright,&mbrID,&mbrLocation);

      pointLoad.Release();
      pointLoads->Create(loadID++,mbrID,mbrLocation,0.0,P,0.0,lotGlobal,&pointLoad);
   }
}

std::set<CAnalysisAgentImp::UnitLiveLoadResult>& CAnalysisAgentImp::GetUnitLiveLoadResults(PierIDType pierID)
{
   std::map<PierIDType,std::set<UnitLiveLoadResult>>::iterator found = m_pUnitLiveLoadResults->find(pierID);
   if ( found == m_pUnitLiveLoadResults->end() )
   {
      std::set<UnitLiveLoadResult> results;
      std::pair<std::map<PierIDType,std::set<UnitLiveLoadResult>>::iterator,bool> result = m_pUnitLiveLoadResults->insert(std::make_pair(pierID,results));
      ATLASSERT(result.second == true);
      found = result.first;
   }

   std::set<UnitLiveLoadResult>& results = (*found).second;
   return results;
}

struct Result
{
   Float64 Value;
   IndexType llConfigIdx;
   Result(Float64 v,IndexType l) : Value(v), llConfigIdx(l) {}
   bool operator<(const Result& result)const { return Value < result.Value; }
};

void CAnalysisAgentImp::ComputeUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi)
{
   Float64 MzMin = DBL_MAX;
   Float64 MzMax = -DBL_MAX;
   IndexType minMz_llConfigIdx = INVALID_INDEX;
   IndexType maxMz_llConfigIdx = INVALID_INDEX;

   Float64 MzMin_SingleLane = DBL_MAX;
   Float64 MzMax_SingleLane = -DBL_MAX;
   IndexType minMz_llConfigIdx_SingleLane = INVALID_INDEX;
   IndexType maxMz_llConfigIdx_SingleLane = INVALID_INDEX;

   sysSectionValue FyMin = DBL_MAX;
   sysSectionValue FyMax = -DBL_MAX;
   IndexType minFyLeft_llConfigIdx = INVALID_INDEX;
   IndexType maxFyLeft_llConfigIdx = INVALID_INDEX;
   IndexType minFyRight_llConfigIdx = INVALID_INDEX;
   IndexType maxFyRight_llConfigIdx = INVALID_INDEX;

   sysSectionValue FyMin_SingleLane = DBL_MAX;
   sysSectionValue FyMax_SingleLane = -DBL_MAX;
   IndexType minFyLeft_llConfigIdx_SingleLane = INVALID_INDEX;
   IndexType maxFyLeft_llConfigIdx_SingleLane = INVALID_INDEX;
   IndexType minFyRight_llConfigIdx_SingleLane = INVALID_INDEX;
   IndexType maxFyRight_llConfigIdx_SingleLane = INVALID_INDEX;

   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   std::set<Result> moments;
   std::set<Result> shears;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);
   IndexType nLiveLoadConfigs = pModelData->m_LiveLoadConfigurations.size();
   for ( IndexType llConfigIdx = 0; llConfigIdx < nLiveLoadConfigs; llConfigIdx++ )
   {
      LiveLoadConfiguration& llConfig = pModelData->m_LiveLoadConfigurations[llConfigIdx];

      IndexType nLoadedLanes = llConfig.m_LoadCases.size();

      Float64 mpf = lrfdUtility::GetMultiplePresenceFactor(nLoadedLanes);

      Float64 FxLeft(0), FyLeft(0), MzLeft(0);
      Float64 FxRight(0), FyRight(0), MzRight(0);

      for ( IndexType laneIdx = 0; laneIdx < nLoadedLanes; laneIdx++ )
      {
         LoadCaseIDType lcid = llConfig.m_LoadCases[laneIdx];

         Float64 fxLeft, fyLeft, mzLeft;
         HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotMember,&fxLeft,&fyLeft,&mzLeft);
         ATLASSERT(SUCCEEDED(hr));

         Float64 fxRight, fyRight, mzRight;
         hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotMember,&fxRight,&fyRight,&mzRight);
         ATLASSERT(SUCCEEDED(hr));

         FxLeft += fxLeft;
         FyLeft += fyLeft;
         MzLeft += mzLeft;

         FxRight += fxRight;
         FyRight += fyRight;
         MzRight += mzRight;
      }

      Float64 Mz;
      if ( IsZero(poi.GetDistFromStart()) )
      {
         Mz = -MzRight;
      }
      else
      {
         Mz = MzLeft;
      }

      Mz = IsZero(Mz) ? 0 : Mz;
      Mz *= mpf;

      moments.insert(Result(Mz,llConfigIdx));

      if ( Mz < MzMin )
      {
         MzMin = Mz;
         minMz_llConfigIdx = llConfigIdx;
      }

      if ( MzMax < Mz )
      {
         MzMax = Mz;
         maxMz_llConfigIdx = llConfigIdx;
      }


      if ( llConfigIdx <= pModelData->m_LastSingleLaneLLConfigIdx )
      {
         if ( Mz < MzMin_SingleLane )
         {
            MzMin_SingleLane = Mz;
            minMz_llConfigIdx_SingleLane = llConfigIdx;
         }

         if ( MzMax_SingleLane < Mz )
         {
            MzMax_SingleLane = Mz;
            maxMz_llConfigIdx_SingleLane = llConfigIdx;
         }
      }

      FyLeft  = IsZero(FyLeft)  ? 0 : FyLeft;
      FyRight = IsZero(FyRight) ? 0 : FyRight;
      sysSectionValue Fy(-FyLeft,FyRight);

      Fy *= mpf;

      shears.insert(Result(Max(fabs(Fy.Left()),fabs(Fy.Right())),llConfigIdx)); // want shear with maximum magnitude

      if ( Fy.Left() < FyMin.Left() )
      {
         FyMin.Left() = Fy.Left();
         minFyLeft_llConfigIdx = llConfigIdx;
      }

      if ( Fy.Right() < FyMin.Right() )
      {
         FyMin.Right() = Fy.Right();
         minFyRight_llConfigIdx = llConfigIdx;
      }

      if ( FyMax.Left() < Fy.Left() )
      {
         FyMax.Left() = Fy.Left();
         maxFyLeft_llConfigIdx = llConfigIdx;
      }

      if ( FyMax.Right() < Fy.Right() )
      {
         FyMax.Right() = Fy.Right();
         maxFyRight_llConfigIdx = llConfigIdx;
      }


      if ( llConfigIdx <= pModelData->m_LastSingleLaneLLConfigIdx )
      {
         if ( Fy.Left() < FyMin_SingleLane.Left() )
         {
            FyMin_SingleLane.Left() = Fy.Left();
            minFyLeft_llConfigIdx_SingleLane = llConfigIdx;
         }

         if ( Fy.Right() < FyMin_SingleLane.Right() )
         {
            FyMin_SingleLane.Right() = Fy.Right();
            minFyRight_llConfigIdx_SingleLane = llConfigIdx;
         }

         if ( FyMax_SingleLane.Left() < Fy.Left() )
         {
            FyMax_SingleLane.Left() = Fy.Left();
            maxFyLeft_llConfigIdx_SingleLane = llConfigIdx;
         }

         if ( FyMax_SingleLane.Right() < Fy.Right() )
         {
            FyMax_SingleLane.Right() = Fy.Right();
            maxFyRight_llConfigIdx_SingleLane = llConfigIdx;
         }
      }
   }

   ATLASSERT(minMz_llConfigIdx != INVALID_INDEX);
   ATLASSERT(maxMz_llConfigIdx != INVALID_INDEX);
   ATLASSERT(minFyLeft_llConfigIdx != INVALID_INDEX);
   ATLASSERT(maxFyLeft_llConfigIdx != INVALID_INDEX);
   ATLASSERT(minFyRight_llConfigIdx != INVALID_INDEX);
   ATLASSERT(maxFyRight_llConfigIdx != INVALID_INDEX);

   ATLASSERT(minMz_llConfigIdx_SingleLane != INVALID_INDEX);
   ATLASSERT(maxMz_llConfigIdx_SingleLane != INVALID_INDEX);
   ATLASSERT(minFyLeft_llConfigIdx_SingleLane != INVALID_INDEX);
   ATLASSERT(maxFyLeft_llConfigIdx_SingleLane != INVALID_INDEX);
   ATLASSERT(minFyRight_llConfigIdx_SingleLane != INVALID_INDEX);
   ATLASSERT(maxFyRight_llConfigIdx_SingleLane != INVALID_INDEX);

   UnitLiveLoadResult liveLoadResult;
   liveLoadResult.m_idPOI = poi.GetID();
   liveLoadResult.m_MzMax = MzMax;
   liveLoadResult.m_MzMin = MzMin;
   liveLoadResult.m_llConfigIdx_MzMin = minMz_llConfigIdx;
   liveLoadResult.m_llConfigIdx_MzMax = maxMz_llConfigIdx;
   liveLoadResult.m_FyMax = FyMax;
   liveLoadResult.m_FyMin = FyMin;
   liveLoadResult.m_llConfigIdx_FyLeftMin = minFyLeft_llConfigIdx;
   liveLoadResult.m_llConfigIdx_FyLeftMax = maxFyLeft_llConfigIdx;
   liveLoadResult.m_llConfigIdx_FyRightMin = minFyRight_llConfigIdx;
   liveLoadResult.m_llConfigIdx_FyRightMax = maxFyRight_llConfigIdx;

   liveLoadResult.m_MzMax_SingleLane = MzMax_SingleLane;
   liveLoadResult.m_MzMin_SingleLane = MzMin_SingleLane;
   liveLoadResult.m_llConfigIdx_MzMin_SingleLane = minMz_llConfigIdx_SingleLane;
   liveLoadResult.m_llConfigIdx_MzMax_SingleLane = maxMz_llConfigIdx_SingleLane;
   liveLoadResult.m_FyMax_SingleLane = FyMax_SingleLane;
   liveLoadResult.m_FyMin_SingleLane = FyMin_SingleLane;
   liveLoadResult.m_llConfigIdx_FyLeftMin_SingleLane = minFyLeft_llConfigIdx_SingleLane;
   liveLoadResult.m_llConfigIdx_FyLeftMax_SingleLane = maxFyLeft_llConfigIdx_SingleLane;
   liveLoadResult.m_llConfigIdx_FyRightMin_SingleLane = minFyRight_llConfigIdx_SingleLane;
   liveLoadResult.m_llConfigIdx_FyRightMax_SingleLane = maxFyRight_llConfigIdx_SingleLane;

   // moments is sorted least to greatest... N minimum moments are
   // at the begining of the sequence... the N maximum moments are at
   // the end of the sequence.
   // Use forward iterator at start of sequence
   int N = MAX_CASES; // using 50 min/max moments
   std::set<Result>::iterator fmIter(moments.begin());
   std::set<Result>::iterator fmEnd(moments.end());
   for ( int i = 0; i < N && fmIter != fmEnd; i++, fmIter++)
   {
      Result& mr(*fmIter);
      liveLoadResult.m_MzMinLiveLoadConfigs.push_back(mr.llConfigIdx);
   }

   // Use reverse iterator at end of sequence
   std::set<Result>::reverse_iterator rmIter(moments.rbegin());
   std::set<Result>::reverse_iterator rmEnd(moments.rend());
   std::set<Result>::reverse_iterator rvIter(shears.rbegin());
   std::set<Result>::reverse_iterator rvEnd(shears.rend());
   for ( int i = 0; i < N && rmIter != rmEnd && rvIter != rvEnd; i++, rmIter++, rvIter++ )
   {
      Result& mr(*rmIter);
      liveLoadResult.m_MzMaxLiveLoadConfigs.push_back(mr.llConfigIdx);

      Result& vr(*rvIter);
      liveLoadResult.m_FyLiveLoadConfigs.push_back(vr.llConfigIdx);
   }

   std::set<UnitLiveLoadResult>& liveLoadResults = GetUnitLiveLoadResults(pierID);
   liveLoadResults.insert(liveLoadResult);
}

CAnalysisAgentImp::UnitLiveLoadResult& CAnalysisAgentImp::GetUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi)
{
   std::set<UnitLiveLoadResult>& liveLoadResults = GetUnitLiveLoadResults(pierID);
   UnitLiveLoadResult key;
   key.m_idPOI = poi.GetID();
   std::set<UnitLiveLoadResult>::iterator found = liveLoadResults.find(key);
   if ( found == liveLoadResults.end() )
   {
      ComputeUnitLiveLoadResult(pierID,poi);
      found = liveLoadResults.find(key);
      ATLASSERT(found != liveLoadResults.end());
   }

   return *found;
}

Float64 CAnalysisAgentImp::GetMaxLegalReaction(PierIDType pierID)
{
   GET_IFACE(IXBRProject,pProject);

   Float64 Rlgl = -DBL_MAX;
   for ( int i = 0; i < 2; i++ )
   {
      pgsTypes::LoadRatingType legalRatingType = (i == 0 ? pgsTypes::lrLegal_Routine : pgsTypes::lrLegal_Special);
      VehicleIndexType nVehicles = pProject->GetLiveLoadReactionCount(pierID,legalRatingType);
      for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nVehicles; vehicleIdx++ )
      {
         Float64 r_legal = pProject->GetLiveLoadReaction(pierID,legalRatingType,vehicleIdx);
         Rlgl = Max(Rlgl,r_legal);
      }
   }
   return Rlgl;
}

std::vector<Float64> CAnalysisAgentImp::GetWheelLineLocations(ModelData* pModelData)
{
   std::vector<Float64> vWheelLineLocations;

   std::map<LoadCaseIDType,LaneConfiguration>::iterator iter(pModelData->m_LaneConfigurations.begin());
   std::map<LoadCaseIDType,LaneConfiguration>::iterator end(pModelData->m_LaneConfigurations.end());
   for ( ; iter != end; iter++ )
   {
      LaneConfiguration& laneConfig(iter->second);
      vWheelLineLocations.push_back(laneConfig.Xleft);
      vWheelLineLocations.push_back(laneConfig.Xright);
   }
   return vWheelLineLocations;
}

void CAnalysisAgentImp::InvalidateModels(bool bCreateNewDataStructures)
{
   std::map<PierIDType,ModelData>* pOldModelData = m_pModelData.release();
   if ( bCreateNewDataStructures )
   {
      m_pModelData = std::auto_ptr<std::map<PierIDType,ModelData>>(new std::map<PierIDType,ModelData>());
   }

#if defined _USE_MULTITHREADING
   m_ThreadManager.CreateThread(CAnalysisAgentImp::DeleteModels,(LPVOID)(pOldModelData));
#else
   CAnalysisAgentImp::DeleteModels((LPVOID)(pOldModelData));
#endif
}

UINT CAnalysisAgentImp::DeleteModels(LPVOID pParam)
{
   WATCH(_T("Begin: DeleteModels"));
   
   std::map<PierIDType,ModelData>* pModelData = (std::map<PierIDType,ModelData>*)pParam;
   pModelData->clear();
   delete pModelData;

   WATCH(_T("End: DeleteModels"));

   return 0;
}

void CAnalysisAgentImp::InvalidateResults(bool bCreateNewDataStructures)
{
   std::map<PierIDType,std::set<UnitLiveLoadResult>>* pOldResults = m_pUnitLiveLoadResults.release();
   if ( bCreateNewDataStructures )
   {
      m_pUnitLiveLoadResults = std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>>(new std::map<PierIDType,std::set<UnitLiveLoadResult>>());
   }

#if defined _USE_MULTITHREADING
   m_ThreadManager.CreateThread(CAnalysisAgentImp::DeleteResults,(LPVOID)(pOldResults));
#else
   CAnalysisAgentImp::DeleteResults((LPVOID)(pOldResults));
#endif
}

UINT CAnalysisAgentImp::DeleteResults(LPVOID pParam)
{
   WATCH(_T("Begin: DeleteResults"));
   
   std::map<PierIDType,std::set<UnitLiveLoadResult>>* pResults = (std::map<PierIDType,std::set<UnitLiveLoadResult>>*)pParam;
   pResults->clear();
   delete pResults;

   WATCH(_T("End: DeleteResults"));

   return 0;
}
