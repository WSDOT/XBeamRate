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

// ProjectAgentImp.cpp : Implementation of CProjectAgentImp
#include "stdafx.h"
#include "ProjectAgent.h"
#include "ProjectAgentImp.h"

#include <EAF\EAFDisplayUnits.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
CProjectAgentImp::CProjectAgentImp()
{
   m_pBroker = 0;
}

CProjectAgentImp::~CProjectAgentImp()
{
}

HRESULT CProjectAgentImp::FinalConstruct()
{
   return S_OK;
}

void CProjectAgentImp::FinalRelease()
{
}

#if defined _DEBUG
bool CProjectAgentImp::AssertValid() const
{
   return true;
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////
// IAgent
STDMETHODIMP CProjectAgentImp::SetBroker(IBroker* pBroker)
{
   EAF_AGENT_SET_BROKER(pBroker);
   m_CommandTarget.Init(pBroker);
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::RegInterfaces()
{
   CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);

   pBrokerInit->RegInterface( IID_IProject,    this );

   return S_OK;
};

STDMETHODIMP CProjectAgentImp::Init()
{
   //EAF_AGENT_INIT;

   ////
   //// Attach to connection points for interfaces this agent depends on
   ////
   //CComQIPtr<IBrokerInitEx2,&IID_IBrokerInitEx2> pBrokerInit(m_pBroker);
   //CComPtr<IConnectionPoint> pCP;
   //HRESULT hr = S_OK;

   ApplicationSettings settings(UnitModeEnum::US,_T("XBeam Rating Project"));

   OpenBridgeML::Pier::CapBeamType capBeam(5.0,5.0);

   OpenBridgeML::Pier::IdealizedSupportType supportType(OpenBridgeML::Pier::IdealizedSupportEnum::Fixed,OpenBridgeML::Pier::IdealizedSupportEnum::Fixed);
   OpenBridgeML::Pier::BaseElement baseElement(supportType);
   OpenBridgeML::Pier::ColumnType column(10.0,baseElement);

   OpenBridgeML::Pier::ColumnsType columns;
   columns.Column().push_back( column );
   columns.Spacing().push_back(5.0);
   columns.Column().push_back(column);

   OpenBridgeML::Pier::Pier pier(capBeam,columns);

   m_XBeamRateXML = std::auto_ptr<XBeamRate>(new XBeamRate(settings,pier));

   return S_OK;
}

STDMETHODIMP CProjectAgentImp::Reset()
{
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::Init2()
{
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::GetClassID(CLSID* pCLSID)
{
   *pCLSID = CLSID_ProjectAgent;
   return S_OK;
}

STDMETHODIMP CProjectAgentImp::ShutDown()
{
   EAF_AGENT_CLEAR_INTERFACE_CACHE;
   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IAgentUIIntegration
STDMETHODIMP CProjectAgentImp::IntegrateWithUI(BOOL bIntegrate)
{
   if ( bIntegrate )
   {
      CreateMenus();
   }
   else
   {
      RemoveMenus();
   }

   return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IAgentPersist
STDMETHODIMP CProjectAgentImp::Load(IStructuredLoad* pStrLoad)
{
   USES_CONVERSION;
   HRESULT hr = S_OK;

   // Load the entire node for this agent
   CComBSTR bstrUnit;
   pStrLoad->LoadRawUnit(&bstrUnit);

   // remove the <Agent></Agent> tags... remove the <CLSID> element
   CString strUnit(OLE2T(bstrUnit));
   int i = strUnit.Find(_T("</CLSID>")) + CString(_T("</CLSID>")).GetLength();
   strUnit = strUnit.Right(strUnit.GetLength()-i);
   strUnit.Replace(_T("</Agent>"),_T(""));

   // add the processing instruction at the head of the XML stream
   std::stringstream ss;
   ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << T2A(strUnit);

   // create the XML\C++ binding reading from the string stream
   // for instance document validation, we need to tell the XML parser where the schema files are located
   //xml_schema::properties props;
   //props.no_namespace_schema_location(_T("F:\\ARP\\XBeamRate\\Schema\\XBeamRate.xsd"));
   //props.schema_location(_T("http://www.wsdot.wa.gov/OpenBridgeML/Units"),_T("F:\\ARP\\OpenBridgeML\\Schema\\OpenBridgeML_Units.xsd"));
   //std::auto_ptr<XBeamRate> xbrXML = XBeamRate_(ss,0,props);
   m_XBeamRateXML = XBeamRate_(ss,xml_schema::flags::dont_validate); // can't figure out validation right now

   // extract our data from the binding object
   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   ApplicationSettings& settings( m_XBeamRateXML->Settings() );
   if ( settings.Units() == UnitModeEnum::SI )
      pDisplayUnits->SetUnitMode(eafTypes::umSI);
   else
      pDisplayUnits->SetUnitMode(eafTypes::umUS);

   return hr;
}

STDMETHODIMP CProjectAgentImp::Save(IStructuredSave* pStrSave)
{
   HRESULT hr = S_OK;

   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   UnitModeEnum units = (pDisplayUnits->GetUnitMode() == eafTypes::umSI ? UnitModeEnum::SI : UnitModeEnum::US);
   m_XBeamRateXML->Settings().Units(units);

   // write the XML stream into the string-stream
   std::ostringstream ss;
   XBeamRate_(ss,*m_XBeamRateXML);

   // remove the initial <?xml> processing instruction
   USES_CONVERSION;
   std::_tstring string = A2T(ss.str().c_str());
   std::_tstring::size_type a = string.find_first_of(_T("<"));
   std::_tstring::size_type b = string.find_first_of(_T(">"));
   string.replace(a,b+1,_T(""));

   // dump the XML stream into our structure storage container
   pStrSave->SaveRawUnit(string.c_str());

   return hr;
}


//////////////////////////////////////////////////////////////////////
// IEAFCommandCallback
BOOL CProjectAgentImp::OnCommandMessage(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
   return m_CommandTarget.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}

BOOL CProjectAgentImp::GetStatusBarMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // load appropriate string
	if ( rMessage.LoadString(nID) )
	{
		// first newline terminates actual string
      rMessage.Replace('\n','\0');
	}
	else
	{
		// not found
      TRACE1("Warning (XBeamRate::ProjectAgent): no message line prompt for ID 0x%04X.\n", nID);
	}

   return TRUE;
}

BOOL CProjectAgentImp::GetToolTipMessageString(UINT nID, CString& rMessage) const
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CString string;
   // load appropriate string
	if ( string.LoadString(nID) )
	{
		// tip is after first newline 
      int pos = string.Find('\n');
      if ( 0 < pos )
         rMessage = string.Mid(pos+1);
	}
	else
	{
		// not found
      TRACE1("Warning (XBeamRate::ProjectAgent): no tool tip for ID 0x%04X.\n", nID);
	}

   return TRUE;
}

//////////////////////////////////////////////////////////////////////
// IProject
void CProjectAgentImp::SetProjectName(LPCTSTR strName)
{
   m_XBeamRateXML->Settings().ProjectName(strName);
   Fire_OnProjectChanged();
}

LPCTSTR CProjectAgentImp::GetProjectName()
{
   return m_XBeamRateXML->Settings().ProjectName().c_str();
}

Float64 CProjectAgentImp::GetLeftOverhang()
{
   return m_XBeamRateXML->Pier().CapBeam().LeftOverhang();
}

Float64 CProjectAgentImp::GetRightOverhang()
{
   return m_XBeamRateXML->Pier().CapBeam().RightOverhang();
}

IndexType CProjectAgentImp::GetColumnCount()
{
   return m_XBeamRateXML->Pier().Columns().Column().size();
}

Float64 CProjectAgentImp::GetColumnHeight(IndexType colIdx)
{
   return m_XBeamRateXML->Pier().Columns().Column()[colIdx].Height();
}

Float64 CProjectAgentImp::GetSpacing(IndexType spaceIdx)
{
   return m_XBeamRateXML->Pier().Columns().Spacing()[spaceIdx];
}

//////////////////////////////////////////////////////////
void CProjectAgentImp::CreateMenus()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   // add an "Edit" menu to the right of the file menu
   UINT filePos = pMenu->FindMenuItem(_T("&File"));

   CEAFMenu* pEditMenu = pMenu->CreatePopupMenu(filePos+1,_T("&Edit"));
   pEditMenu->LoadMenu(IDR_MENU,this);
}

void CProjectAgentImp::RemoveMenus()
{
   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   // remove the Edit menu
   UINT editPos = pMenu->FindMenuItem(_T("&Edit"));
   VERIFY(pMenu->RemoveMenu(editPos,MF_BYPOSITION,this));
}
