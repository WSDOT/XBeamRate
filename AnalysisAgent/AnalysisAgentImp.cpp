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

   pBrokerInit->RegInterface( IID_IAnalysisResults,    this );

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
   hr = pBrokerInit->FindConnectionPoint( IID_IProjectEventSink, &pCP );
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

   hr = pBrokerInit->FindConnectionPoint(IID_IProjectEventSink, &pCP );
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
   if ( m_Model == NULL )
   {
      m_Model.CoCreateInstance(CLSID_Fem2dModel);

      // Build the frame model
      GET_IFACE(IProject,pProject);

      // some dummy dimensions
      Float64 leftOverhang = pProject->GetLeftOverhang();
      Float64 rightOverhang = pProject->GetRightOverhang();
      IndexType nColumns = pProject->GetColumnCount();

      Float64 EAb = 1;
      Float64 EIb = 1;
      Float64 EAc = 1;
      Float64 EIc = 1;

      CComPtr<IFem2dJointCollection> joints;
      m_Model->get_Joints(&joints);

      CComPtr<IFem2dMemberCollection> members;
      m_Model->get_Members(&members);

      JointIDType jntID = 0;

      // Start joint at left end of cross beam
      CComPtr<IFem2dJoint> joint;
      joints->Create(jntID++,0,0,&joint);

      // create joint at top of first column
      joint.Release();
      joints->Create(jntID++,leftOverhang,0,&joint);

      // create left overhang member
      MemberIDType mbrID = 0;
      CComPtr<IFem2dMember> mbr;
      members->Create(mbrID++,jntID-2,jntID-1,EAb,EIb,&mbr);

      Float64 x = leftOverhang;
      for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
      {
         Float64 space = (colIdx < nColumns-1 ? pProject->GetSpacing(colIdx) : rightOverhang);
         Float64 columnHeight = pProject->GetColumnHeight(colIdx);

         // create joint at bottom of column
         joint.Release();
         joints->Create(jntID++,x,-columnHeight,&joint);

         joint->Support(); // fully fixed
         if ( pProject->GetColumnBaseType(colIdx) == xbrTypes::cbtPinned )
         {
            joint->ReleaseDof(jrtMz); // pinned
         }

         // create column member
         mbr.Release();
         members->Create(mbrID++,jntID-2,jntID-1,EAc,EIc,&mbr);

         x += space;

         // create next top of column joint
         joint.Release();
         joints->Create(jntID++,x,0,&joint);

         // create cross beam member
         mbr.Release();
         members->Create(mbrID++,jntID-3,jntID-1,EAb,EIb,&mbr);
      }

      // create some dummy loads
      CComPtr<IFem2dLoadingCollection> loadings;
      m_Model->get_Loadings(&loadings);

      LoadCaseIDType loadCaseID = 0;
      CComPtr<IFem2dLoading> loading;
      loadings->Create(loadCaseID,&loading);

      CComPtr<IFem2dDistributedLoadCollection> distLoads;
      loading->get_DistributedLoads(&distLoads);

      LoadIDType loadID = 0;
      for ( MemberIDType id = 0; id < mbrID; id++ )
      {
         CComPtr<IFem2dDistributedLoad> distLoad;
         distLoads->Create(loadID++,id,loadDirFy,0.0,-1.0,-10,-10,lotMember,&distLoad);
      }
   }
}

void CAnalysisAgentImp::Invalidate()
{
   m_Model.Release();
}

//////////////////////////////////////////////////////////////////////
// IProjectEventSink
HRESULT CAnalysisAgentImp::OnProjectChanged()
{
   Invalidate();
   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IAnalysisResults
Float64 CAnalysisAgentImp::GetResult()
{
   Validate();

   CComQIPtr<IFem2dModelResults> results(m_Model);
   Float64 startFx, startFy, startMz;
   Float64 endFx,   endFy,   endMz;
   results->ComputeMemberForces(0,0,&startFx,&startFy,&startMz,&endFx,&endFy,&endMz);
   return endMz;
}
