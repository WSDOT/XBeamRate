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

#include <EAF\EAFAutoProgress.h>
#include <XBeamRateExt\XBeamRateUtilities.h>
#include <XBeamRateExt\StatusItem.h>

#include <numeric>

#include <System\Flags.h>

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
   m_pModelData = std::auto_ptr<std::map<PierIDType,ModelData>>(new std::map<PierIDType,ModelData>());
   m_pUnitLiveLoadResults = std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>>(new std::map<PierIDType,std::set<UnitLiveLoadResult>>());
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

CAnalysisAgentImp::ModelData* CAnalysisAgentImp::GetModelData(PierIDType pierID)
{
   BuildModel(pierID); // builds or updates the model if necessary
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
   bool operator<(const XBeamNode& other)const { return X < other.X; }
   bool operator==(const XBeamNode& other)const { return IsEqual(X,other.X); }
};

void CAnalysisAgentImp::BuildModel(PierIDType pierID)
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

   GET_IFACE(IProgress,pProgress);
   CEAFAutoProgress ap(pProgress);
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

   ApplyUnitLiveLoad(pierID,pModelData);
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

void CAnalysisAgentImp::Invalidate()
{
   InvalidateModels();
   InvalidateResults();
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

IndexType CAnalysisAgentImp::GetLiveLoadConfigurationCount(PierIDType pierID)
{
   ModelData* pModelData = GetModelData(pierID);
   return (IndexType)(pModelData->m_NextLiveLoadCaseID - FIRST_LIVELOAD_ID);
}

IndexType CAnalysisAgentImp::GetLoadedLaneCount(PierIDType pierID,IndexType liveLoadConfigIdx)
{
   ModelData* pModelData = GetModelData(pierID);
   LiveLoadConfiguration key;
   key.m_LoadCaseID = FIRST_LIVELOAD_ID + liveLoadConfigIdx;
   std::set<LiveLoadConfiguration>::iterator found = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(found != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llConfig = *found;
   return llConfig.m_nLoadedLanes;
}

WheelLineConfiguration CAnalysisAgentImp::GetLiveLoadConfiguration(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,IndexType liveLoadConfigIdx)
{
   ModelData* pModelData = GetModelData(pierID);
   LiveLoadConfiguration key;
   key.m_LoadCaseID = FIRST_LIVELOAD_ID + liveLoadConfigIdx;
   std::set<LiveLoadConfiguration>::iterator found = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(found != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llConfig = *found;

   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   WheelLineConfiguration wheelConfig;
   std::vector<std::pair<Float64,Float64>>::iterator iter(llConfig.m_Loading.begin());
   std::vector<std::pair<Float64,Float64>>::iterator end(llConfig.m_Loading.end());
   for ( ; iter != end; iter++ )
   {
      std::pair<Float64,Float64> p(*iter);
      WheelLinePlacement placement;
      placement.P = R*p.first;
      placement.Xxb = llConfig.m_Xoffset + p.second;
      wheelConfig.push_back(placement);
   }
   return wheelConfig;
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

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,Float64* pMin,Float64* pMax,WheelLineConfiguration* pMinConfiguration,WheelLineConfiguration* pMaxConfiguration)
{
   UnitLiveLoadResult liveLoadResult = GetUnitLiveLoadResult(pierID,poi);

   *pMin = liveLoadResult.m_MzMin;
   *pMax = liveLoadResult.m_MzMax;


   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   ModelData* pModelData = GetModelData(pierID);

   LiveLoadConfiguration key;
   key.m_LoadCaseID = liveLoadResult.m_lcidMzMin;
   std::set<LiveLoadConfiguration>::iterator foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMinConfig = (*foundLLConfig);
   IndexType minLoadedLanes = llMinConfig.m_nLoadedLanes;

   key.m_LoadCaseID = liveLoadResult.m_lcidMzMax;
   foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMaxConfig = (*foundLLConfig);
   IndexType maxLoadedLanes = llMaxConfig.m_nLoadedLanes;

   *pMin *= R*minLoadedLanes;
   *pMax *= R*maxLoadedLanes;

   if ( pMinConfiguration )
   {
      pMinConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMinConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMinConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMinConfig.m_Xoffset + p.second;
         pMinConfiguration->push_back(placement);
      }
   }
   
   if ( pMaxConfiguration )
   {
      pMaxConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMaxConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMaxConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMaxConfig.m_Xoffset + p.second;
         pMaxConfiguration->push_back(placement);
      }
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax,WheelLineConfiguration* pMinLeftConfiguration,WheelLineConfiguration* pMinRightConfiguration,WheelLineConfiguration* pMaxLeftConfiguration,WheelLineConfiguration* pMaxRightConfiguration)
{
   UnitLiveLoadResult liveLoadResult = GetUnitLiveLoadResult(pierID,poi);

   *pMin = liveLoadResult.m_FyMin;
   *pMax = liveLoadResult.m_FyMax;


   GET_IFACE(IXBRProject,pProject);
   Float64 R = pProject->GetLiveLoadReaction(pierID,ratingType,vehicleIdx); // single lane reaction

   ModelData* pModelData = GetModelData(pierID);

   LiveLoadConfiguration key;
   key.m_LoadCaseID = liveLoadResult.m_lcidFyLeftMin;
   std::set<LiveLoadConfiguration>::iterator foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMinLeftConfig = (*foundLLConfig);
   IndexType minLoadedLanesLeft = llMinLeftConfig.m_nLoadedLanes;

   key.m_LoadCaseID = liveLoadResult.m_lcidFyRightMin;
   foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMinRightConfig = (*foundLLConfig);
   IndexType minLoadedLanesRight = llMinRightConfig.m_nLoadedLanes;

   key.m_LoadCaseID = liveLoadResult.m_lcidFyLeftMax;
   foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMaxLeftConfig = (*foundLLConfig);
   IndexType maxLoadedLanesLeft = llMaxLeftConfig.m_nLoadedLanes;

   key.m_LoadCaseID = liveLoadResult.m_lcidFyRightMax;
   foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   LiveLoadConfiguration& llMaxRightConfig = (*foundLLConfig);
   IndexType maxLoadedLanesRight = llMaxRightConfig.m_nLoadedLanes;

   pMin->Left()  *= R*minLoadedLanesLeft;
   pMin->Right() *= R*minLoadedLanesRight;
   
   pMax->Left()  *= R*maxLoadedLanesLeft;
   pMax->Right() *= R*maxLoadedLanesRight;

   if ( pMinLeftConfiguration )
   {
      pMinLeftConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMinLeftConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMinLeftConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMinLeftConfig.m_Xoffset + p.second;
         pMinLeftConfiguration->push_back(placement);
      }
   }

   if ( pMinRightConfiguration )
   {
      pMinRightConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMinRightConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMinRightConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMinRightConfig.m_Xoffset + p.second;
         pMinRightConfiguration->push_back(placement);
      }
   }
   
   if ( pMaxLeftConfiguration )
   {
      pMaxLeftConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMaxLeftConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMaxLeftConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMaxLeftConfig.m_Xoffset + p.second;
         pMaxLeftConfiguration->push_back(placement);
      }
   }
   
   if ( pMaxRightConfiguration )
   {
      pMaxRightConfiguration->clear();

      std::vector<std::pair<Float64,Float64>>::iterator iter(llMaxRightConfig.m_Loading.begin());
      std::vector<std::pair<Float64,Float64>>::iterator end(llMaxRightConfig.m_Loading.end());
      for ( ; iter != end; iter++ )
      {
         std::pair<Float64,Float64> p(*iter);
         WheelLinePlacement placement;
         placement.P = R*p.first;
         placement.Xxb = llMaxRightConfig.m_Xoffset + p.second;
         pMaxRightConfiguration->push_back(placement);
      }
   }
}

void CAnalysisAgentImp::GetMoment(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<Float64>* pvMin,std::vector<Float64>* pvMax,std::vector<WheelLineConfiguration>* pvMinConfiguration,std::vector<WheelLineConfiguration>* pvMaxConfiguration)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   if ( pvMinConfiguration )
   {
      pvMinConfiguration->clear();
      pvMinConfiguration->reserve(vPoi.size());
   }
   if ( pvMaxConfiguration )
   {
      pvMaxConfiguration->clear();
      pvMaxConfiguration->reserve(vPoi.size());
   }
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      Float64 min,max;
      WheelLineConfiguration minConfig, maxConfig;
      GetMoment(pierID,ratingType,vehicleIdx,poi,&min,&max,pvMinConfiguration ? &minConfig : NULL,pvMaxConfiguration ? &maxConfig : NULL);
      pvMin->push_back(min);
      pvMax->push_back(max);
      if ( pvMinConfiguration )
      {
         pvMinConfiguration->push_back(minConfig);
      }
      if ( pvMaxConfiguration )
      {
         pvMaxConfiguration->push_back(maxConfig);
      }
   }
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LoadRatingType ratingType,VehicleIndexType vehicleIdx,const std::vector<xbrPointOfInterest>& vPoi,std::vector<sysSectionValue>* pvMin,std::vector<sysSectionValue>* pvMax,std::vector<WheelLineConfiguration>* pvMinLeftConfiguration,std::vector<WheelLineConfiguration>* pvMinRightConfiguration,std::vector<WheelLineConfiguration>* pvMaxLeftConfiguration,std::vector<WheelLineConfiguration>* pvMaxRightConfiguration)
{
   pvMin->clear();
   pvMin->reserve(vPoi.size());
   pvMax->clear();
   pvMax->reserve(vPoi.size());
   if ( pvMinLeftConfiguration )
   {
      pvMinLeftConfiguration->clear();
      pvMinLeftConfiguration->reserve(vPoi.size());
   }
   if ( pvMinRightConfiguration )
   {
      pvMinRightConfiguration->clear();
      pvMinRightConfiguration->reserve(vPoi.size());
   }
   if ( pvMaxLeftConfiguration )
   {
      pvMaxLeftConfiguration->clear();
      pvMaxLeftConfiguration->reserve(vPoi.size());
   }
   if ( pvMaxRightConfiguration )
   {
      pvMaxRightConfiguration->clear();
      pvMaxRightConfiguration->reserve(vPoi.size());
   }
   BOOST_FOREACH(const xbrPointOfInterest& poi,vPoi)
   {
      sysSectionValue min,max;
      WheelLineConfiguration minLeftConfig, minRightConfig, maxLeftConfig, maxRightConfig;
      GetShear(pierID,ratingType,vehicleIdx,poi,&min,&max,pvMinLeftConfiguration ? &minLeftConfig : NULL,pvMinRightConfiguration ? &minRightConfig : NULL,pvMaxLeftConfiguration ? &maxLeftConfig : NULL,pvMaxRightConfiguration ? &maxRightConfig : NULL);
      pvMin->push_back(min);
      pvMax->push_back(max);
      if ( pvMinLeftConfiguration )
      {
         pvMinLeftConfiguration->push_back(minLeftConfig);
      }
      if ( pvMinRightConfiguration )
      {
         pvMinRightConfiguration->push_back(minRightConfig);
      }
      if ( pvMaxLeftConfiguration )
      {
         pvMaxLeftConfiguration->push_back(maxLeftConfig);
      }
      if ( pvMaxRightConfiguration )
      {
         pvMaxRightConfiguration->push_back(maxRightConfig);
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

   for ( VehicleIndexType vehicleIdx = 0; vehicleIdx < nLiveLoadReactions; vehicleIdx++ )
   {
      sysSectionValue min,max;
      GetShear(pierID,ratingType,vehicleIdx,poi,&min,&max,NULL,NULL,NULL,NULL);
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
   Float64 gDC = pProject->GetDCLoadFactor();
   Float64 gDW = pProject->GetDWLoadFactor();
   Float64 gCR = pProject->GetCRLoadFactor();
   Float64 gSH = pProject->GetSHLoadFactor();
   Float64 gPS = pProject->GetPSLoadFactor();
   Float64 gRE = pProject->GetRELoadFactor();
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ratingType);

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

#pragma Reminder("WORKING HERE - need to include multiple presence factor in load combination")

   *pMin = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmin;
   *pMax = gDC*DC + gDW*DW + gCR*CR + gSH*SH + gPS*PS + gRE*RE + gLL*LLIMmax;
}

void CAnalysisAgentImp::GetShear(PierIDType pierID,pgsTypes::LimitState limitState,const xbrPointOfInterest& poi,sysSectionValue* pMin,sysSectionValue* pMax)
{
   pgsTypes::LoadRatingType ratingType = RatingTypeFromLimitState(limitState);

   GET_IFACE(IXBRProject,pProject);
   Float64 gDC = pProject->GetDCLoadFactor();
   Float64 gDW = pProject->GetDWLoadFactor();
   Float64 gCR = pProject->GetCRLoadFactor();
   Float64 gSH = pProject->GetSHLoadFactor();
   Float64 gPS = pProject->GetPSLoadFactor();
   Float64 gRE = pProject->GetRELoadFactor();
   Float64 gLL = pProject->GetLiveLoadFactor(pierID,ratingType);

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

#pragma Reminder("WORKING HERE - need to include multiple presence factor in load combination")

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

void CAnalysisAgentImp::ApplyUnitLiveLoad(PierIDType pierID,ModelData* pModelData)
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

   Float64 maxStepSize = ::ConvertToSysUnits(1.0,unitMeasure::Feet); // along CL Pier (in the plane of the pier)

   GET_IFACE(IXBRProject,pProject);
   Float64 LCO, RCO;
   pProject->GetCurbLineOffset(pierID,&LCO,&RCO);
   Float64 XcurbLine = pPier->ConvertPierToCrossBeamCoordinate(pierID,LCO);

   for ( IndexType nLoadedLanes = 1; nLoadedLanes <= nLanes; nLoadedLanes++ )
   {
      IndexType nLaneGaps = nLoadedLanes-1; // number of "gaps" between loaded lanes
      Float64 Wll = nLoadedLanes*wLoadedLane; // width of live load, measured in the plane of the pier
      IndexType nTotalSteps = (IndexType)ceil((Wcc - Wll)/maxStepSize); // total number of steps to move live load from left to right curb lines
      Float64 stepSize = (0 < nTotalSteps ? (Wcc-Wll)/nTotalSteps : 0); // actual step size

      // Get the position of the loaded lanes, defined as a sequence of "gap indicies".
      // The size of the gap between loaded lanes is the gap index times the step size
      //
      //  wLoadedLane      wLoadedLane         wLoadedLane
      //  |<------>|       |<------>|          |<------>|
      //  ==========       ==========          ==========
      //            |<--->|          |<------>|
      //              gap               gap
      std::vector<std::vector<IndexType>> vGapPositions;
      GetLanePositions(nTotalSteps,nLaneGaps,vGapPositions);

      // for each lane configuration, get the wheel line reaction configuration
      // and analyze the structure for the configuration. The configuration, in general,
      // will not be as wide as the curb-to-curb width, so step the configuration until
      // it reaches the right curb line.
      BOOST_FOREACH(std::vector<IndexType>& vGapPosition,vGapPositions)
      {
         // wheel line reaction configuration is a sequence of vertical load and position from left curb line
         // pairs.
         std::vector<std::pair<Float64,Float64>> vLoading = ConfigureWheelLineLoads(skew,stepSize,wLoadedLane,vGapPosition);

         // get the number of steps used to make the wheel line reaction configuration.
         IndexType nStepsUsed = std::accumulate(vGapPosition.begin(),vGapPosition.end(),(IndexType)0);

         // the number of times we need to step the wheel line reaction configuration towards the right curb line
         // is equal to the total number of steps less the number of steps used by the configuration.
         IndexType nStepsRemaining = nTotalSteps - nStepsUsed;


#if defined _DEBUG
         // at the last step, the right wheel line, in the right-most lane, must be 2' from the right curb line
         Float64 w2 = ::ConvertToSysUnits(2.0,unitMeasure::Feet);
         ATLASSERT(IsEqual(vLoading.back().second + stepSize*nStepsRemaining + w2,Wcc));
#endif

         // Step the wheel line reaction configuration towards the right curb line, analyzing the
         // cross beam for each loading position.
         for ( IndexType stepIdx = 0; stepIdx <= nStepsRemaining; stepIdx++ )
         {
            Float64 Xoffset = stepSize*stepIdx; // amount to shift the wheel line configuration towards the right curb line
            Xoffset += XcurbLine;

            LoadCaseIDType lcid = ApplyWheelLineLoadsToFemModel(pModelData,Xoffset,vLoading);

            LiveLoadConfiguration llconfig;
            llconfig.m_LoadCaseID = lcid;
            llconfig.m_nLoadedLanes = vGapPosition.size()+1;
            llconfig.m_Xoffset = Xoffset;
            llconfig.m_Loading = vLoading;
            pModelData->m_LiveLoadConfigurations.insert(llconfig);
         } // next step
      } // next lane configuration
   } // next number of loaded lanes
}

void CAnalysisAgentImp::GetLanePositions(IndexType nTotalSteps,IndexType nLaneGaps,std::vector<std::vector<IndexType>>& vGapPositions)
{
   if ( nLaneGaps == 0 )
   {
      std::vector<IndexType> vDigits;
      vGapPositions.push_back(vDigits);
      return;
   }

   static std::vector<IndexType> vDigits;
   for ( IndexType stepIdx = 0; stepIdx < nTotalSteps; stepIdx++ )
   {
      vDigits.push_back(stepIdx);
      if ( 1 < nLaneGaps )
      {
         GetLanePositions(nTotalSteps-stepIdx,nLaneGaps-1,vGapPositions);
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

std::vector<std::pair<Float64,Float64>> CAnalysisAgentImp::ConfigureWheelLineLoads(Float64 skew,Float64 stepSize,Float64 wLoadedLane,std::vector<IndexType>& vGapPosition)
{
   Float64 w3 = ::ConvertToSysUnits(3.0,unitMeasure::Feet)/cos(skew); // 6 ft spacing between wheel lines... wheel line is +/-3' from CL lane

   Float64 P = -0.5; // using a unit load live load reaction... half for each wheel line

   std::vector<std::pair<Float64,Float64>> vLoads;
   // first pair of wheel line loads... at left curb line
   vLoads.push_back(std::make_pair(P,wLoadedLane/2-w3));
   vLoads.push_back(std::make_pair(P,wLoadedLane/2+w3));

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

      vLoads.push_back(std::make_pair(P,XleftWheelLine));
      vLoads.push_back(std::make_pair(P,XrightWheelLine));
   }

   return vLoads;
}

LoadCaseIDType CAnalysisAgentImp::ApplyWheelLineLoadsToFemModel(ModelData* pModelData,Float64 Xoffset,std::vector<std::pair<Float64,Float64> >& vLoading)
{
   CComPtr<IFem2dLoadingCollection> loadings;
   pModelData->m_Model->get_Loadings(&loadings);

   LoadCaseIDType loadCaseID = pModelData->m_NextLiveLoadCaseID++; 

   CComPtr<IFem2dLoading> loading;
   loadings->Create(loadCaseID,&loading);

   CComPtr<IFem2dPointLoadCollection> pointLoads;
   loading->get_PointLoads(&pointLoads);

   LoadIDType loadID = 0;
   std::vector<std::pair<Float64,Float64>>::iterator iter(vLoading.begin());
   std::vector<std::pair<Float64,Float64>>::iterator end(vLoading.end());
   for ( ; iter != end; iter++ )
   {
      std::pair<Float64,Float64>& loading(*iter);
      Float64 P = loading.first;
      Float64 X = loading.second;

      MemberIDType mbrID;
      Float64 mbrLocation;
      GetSuperstructureFemModelLocation(pModelData,X+Xoffset,&mbrID,&mbrLocation);

      CComPtr<IFem2dPointLoad> pointLoad;
      pointLoads->Create(loadID++,mbrID,mbrLocation,0.0,P,0.0,lotGlobal,&pointLoad);
   }

   return loadCaseID;
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

void CAnalysisAgentImp::ComputeUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi)
{
   Float64 MzMin = DBL_MAX;
   Float64 MzMax = -DBL_MAX;
   LoadCaseIDType minMzLCID = INVALID_ID;
   LoadCaseIDType maxMzLCID = INVALID_ID;

   sysSectionValue FyMin = DBL_MAX;
   sysSectionValue FyMax = -DBL_MAX;
   LoadCaseIDType minFyLeftLCID = INVALID_ID;
   LoadCaseIDType maxFyLeftLCID = INVALID_ID;
   LoadCaseIDType minFyRightLCID = INVALID_ID;
   LoadCaseIDType maxFyRightLCID = INVALID_ID;

   ModelData* pModelData = GetModelData(pierID);

   std::map<PoiIDType,PoiIDType>::iterator found = pModelData->m_PoiMap.find(poi.GetID());
   ATLASSERT(found != pModelData->m_PoiMap.end());
   PoiIDType femPoiID = found->second;

   CComQIPtr<IFem2dModelResults> results(pModelData->m_Model);
   for ( LoadCaseIDType lcid = FIRST_LIVELOAD_ID; lcid < pModelData->m_NextLiveLoadCaseID; lcid++ )
   {
      Float64 FxLeft, FyLeft, MzLeft;
      HRESULT hr = results->ComputePOIForces(lcid,femPoiID,mftLeft,lotGlobalProjected,&FxLeft,&FyLeft,&MzLeft);
      ATLASSERT(SUCCEEDED(hr));

      Float64 FxRight, FyRight, MzRight;
      hr = results->ComputePOIForces(lcid,femPoiID,mftRight,lotGlobalProjected,&FxRight,&FyRight,&MzRight);
      ATLASSERT(SUCCEEDED(hr));

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

      if ( Mz < MzMin )
      {
         MzMin = Mz;
         minMzLCID = lcid;
      }

      if ( MzMax < Mz )
      {
         MzMax = Mz;
         maxMzLCID = lcid;
      }

      FyLeft  = IsZero(FyLeft)  ? 0 : FyLeft;
      FyRight = IsZero(FyRight) ? 0 : FyRight;
      sysSectionValue Fy(-FyLeft,FyRight);

      if ( Fy.Left() < FyMin.Left() )
      {
         FyMin.Left() = Fy.Left();
         minFyLeftLCID = lcid;
      }

      if ( Fy.Right() < FyMin.Right() )
      {
         FyMin.Right() = Fy.Right();
         minFyRightLCID = lcid;
      }

      if ( FyMax.Left() < Fy.Left() )
      {
         FyMax.Left() = Fy.Left();
         maxFyLeftLCID = lcid;
      }

      if ( FyMax.Right() < Fy.Right() )
      {
         FyMax.Right() = Fy.Right();
         maxFyRightLCID = lcid;
      }
   }

   ATLASSERT(minMzLCID != INVALID_ID);
   ATLASSERT(maxMzLCID != INVALID_ID);
   ATLASSERT(minFyLeftLCID != INVALID_ID);
   ATLASSERT(maxFyLeftLCID != INVALID_ID);
   ATLASSERT(minFyRightLCID != INVALID_ID);
   ATLASSERT(maxFyRightLCID != INVALID_ID);

   // Use the following if we need the number of loaded lanes associated with the min/max value
   //LiveLoadConfiguration key;
   //key.m_LoadCaseID = minMzLCID;
   //std::set<LiveLoadConfiguration>::iterator foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   //ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   //LiveLoadConfiguration& llMinConfig = (*foundLLConfig);
   //IndexType minLoadedLanes = llMinConfig.m_nLoadedLanes;

   //key.m_LoadCaseID = maxMzLCID;
   //foundLLConfig = pModelData->m_LiveLoadConfigurations.find(key);
   //ATLASSERT(foundLLConfig != pModelData->m_LiveLoadConfigurations.end());
   //LiveLoadConfiguration& llMaxConfig = (*foundLLConfig);
   //IndexType maxLoadedLanes = llMaxConfig.m_nLoadedLanes;

   UnitLiveLoadResult liveLoadResult;
   liveLoadResult.m_idPOI = poi.GetID();
   liveLoadResult.m_MzMax = MzMax;
   liveLoadResult.m_MzMin = MzMin;
   liveLoadResult.m_lcidMzMin = minMzLCID;
   liveLoadResult.m_lcidMzMax = maxMzLCID;
   liveLoadResult.m_FyMax = FyMax;
   liveLoadResult.m_FyMin = FyMin;
   liveLoadResult.m_lcidFyLeftMin = minFyLeftLCID;
   liveLoadResult.m_lcidFyLeftMax = maxFyLeftLCID;
   liveLoadResult.m_lcidFyRightMin = minFyRightLCID;
   liveLoadResult.m_lcidFyRightMax = maxFyRightLCID;

   std::set<UnitLiveLoadResult>& liveLoadResults = GetUnitLiveLoadResults(pierID);
   liveLoadResults.insert(liveLoadResult);
}

CAnalysisAgentImp::UnitLiveLoadResult CAnalysisAgentImp::GetUnitLiveLoadResult(PierIDType pierID,const xbrPointOfInterest& poi)
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


void CAnalysisAgentImp::InvalidateModels()
{
   std::map<PierIDType,ModelData>* pOldModelData = m_pModelData.release();
   m_pModelData = std::auto_ptr<std::map<PierIDType,ModelData>>(new std::map<PierIDType,ModelData>());

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


void CAnalysisAgentImp::InvalidateResults()
{
   std::map<PierIDType,std::set<UnitLiveLoadResult>>* pOldResults = m_pUnitLiveLoadResults.release();
   m_pUnitLiveLoadResults = std::auto_ptr<std::map<PierIDType,std::set<UnitLiveLoadResult>>>(new std::map<PierIDType,std::set<UnitLiveLoadResult>>());

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
