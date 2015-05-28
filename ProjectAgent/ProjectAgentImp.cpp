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
#include <IFace\XBeamRateAgent.h>

#include <WBFLUnitServer\OpenBridgeML.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


OpenBridgeML::StandardReinforcement::USBarEnum::value ConvertBarSize(matRebar::Size barSize)
{
   switch( barSize )
   {
   case matRebar::bs3:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US3;

   case matRebar::bs4:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US4;

   case matRebar::bs5:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US5;

   case matRebar::bs6:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US6;

   case matRebar::bs7:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US7;

   case matRebar::bs8:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US8;

   case matRebar::bs9:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US9;

   case matRebar::bs10:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US10;

   case matRebar::bs11:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US11;

   case matRebar::bs14:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US14;

   case matRebar::bs18:
      return OpenBridgeML::StandardReinforcement::USBarEnum::US18;
   }

   return OpenBridgeML::StandardReinforcement::USBarEnum::US3;
}

matRebar::Size ConvertBarSize(OpenBridgeML::StandardReinforcement::USBarEnum::value barSize)
{
   switch( barSize )
   {
   case OpenBridgeML::StandardReinforcement::USBarEnum::US3:
      return matRebar::bs3;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US4:
      return matRebar::bs4;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US5:
      return matRebar::bs5;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US6:
      return matRebar::bs6;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US7:
      return matRebar::bs7;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US8:
      return matRebar::bs8;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US9:
      return matRebar::bs9;
   
   case OpenBridgeML::StandardReinforcement::USBarEnum::US10:
      return matRebar::bs10;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US11:
      return matRebar::bs11;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US14:
      return matRebar::bs14;

   case OpenBridgeML::StandardReinforcement::USBarEnum::US18:
      return matRebar::bs18;
   }

   return matRebar::bs3;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectAgentImp
CProjectAgentImp::CProjectAgentImp()
{
   m_pBroker = 0;
   m_EventHoldCount = 0;
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

   pBrokerInit->RegInterface( IID_IXBRProject,             this );
   pBrokerInit->RegInterface( IID_IXBRRatingSpecification, this );
   pBrokerInit->RegInterface( IID_IXBRProjectEdit,         this );
   pBrokerInit->RegInterface( IID_IXBREvents,              this );

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

#pragma Reminder("WORKING HERE - Need better default values")

   // Create default data model
   XBeamRate::ApplicationSettings settings(XBeamRate::UnitModeEnum::US,_T("XBeam Rating Project"));

   XBeamRate::PierTypeEnum pierType = XBeamRate::PierTypeEnum::Integral;

   OpenBridgeML::Pier::CapBeamType capBeam(::ConvertToSysUnits(5.0,unitMeasure::Feet),
      ::ConvertToSysUnits(5.0,unitMeasure::Feet),
      ::ConvertToSysUnits(5.0,unitMeasure::Feet),
      ::ConvertToSysUnits(5.0,unitMeasure::Feet),::ConvertToSysUnits(1.0,unitMeasure::Feet),::ConvertToSysUnits(3.0,unitMeasure::Feet),
      ::ConvertToSysUnits(5.0,unitMeasure::Feet),::ConvertToSysUnits(1.0,unitMeasure::Feet),::ConvertToSysUnits(3.0,unitMeasure::Feet));

   OpenBridgeML::Pier::FoundationType foundation(OpenBridgeML::Pier::IdealizedFoundationEnum::Fixed);
   OpenBridgeML::Pier::PrismaticColumnType column(foundation);
   OpenBridgeML::Pier::CicularColumnSectionType circularSection(::ConvertToSysUnits(3.0,unitMeasure::Feet));
   column.CircularSection().set(circularSection);
   column.Height() = ::ConvertToSysUnits(30.0,unitMeasure::Feet);

   ColumnIndexType refColIdx = 0;
   OpenBridgeML::Pier::PierLocationType pierLocation(::ConvertToSysUnits(-5.0,unitMeasure::Feet),OpenBridgeML::Types::OffsetMeasurementEnum::Alignment,refColIdx);

   OpenBridgeML::Pier::ColumnsType columns;
   columns.PrismaticColumn().push_back( column );
   columns.Spacing().push_back(::ConvertToSysUnits(10.0,unitMeasure::Feet));
   columns.PrismaticColumn().push_back(column);

   OpenBridgeML::Pier::PierType pier(pierLocation,capBeam,columns);


   Float64 modE = ::ConvertToSysUnits(5000,unitMeasure::PSI);
   Float64 fc = ::ConvertToSysUnits(4000,unitMeasure::PSI);


   Float64 deckElevation = 0;
   Float64 bridgeLineOffset = 0;
   Float64 crownPointOffset = 0;
   OpenBridgeML::Types::OffsetMeasurementEnum curbLineDatum = OpenBridgeML::Types::OffsetMeasurementEnum::Alignment;
   Float64 LCO = ::ConvertToSysUnits(10,unitMeasure::Feet);
   Float64 RCO = ::ConvertToSysUnits(10,unitMeasure::Feet);
   Float64 SL = -0.02;
   Float64 SR = -0.02;

   Float64 diaphragmWidth = capBeam.Width();
   Float64 diaphragmHeight = ::ConvertToSysUnits(8,unitMeasure::Feet);

   LPCTSTR strOrientation = _T("00 00 0.0 L");

   XBeamRate::LiveLoadReactionsType designLiveLoad;
   designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Design Truck + Lane"),0));
   designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Design Tandem + Lane"),0));
   designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Truck Train [90%(Truck + Lane)]"),0));
   designLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("LRFD Low Boy (Dual Tandem + Lane)"),0));

   XBeamRate::LiveLoadReactionsType legalRoutineLiveLoad;
   legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3"),0));
   legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3S2"),0));
   legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Type 3-3"),0));
   //legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("0.75(Type 3-3) + Lane Load"),0)); // spans must be greater than 200 feet for this loading
   legalRoutineLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("0.75(Two Type 3-3 separated by 30ft) + Lane Load"),0));

   XBeamRate::LiveLoadReactionsType legalSpecialLiveLoad;
   legalSpecialLiveLoad.Reactions().push_back(XBeamRate::LiveLoadReactionType(_T("Notional Rating Load (NRL)"),0));

   XBeamRate::LiveLoadReactionsType permitRoutineLiveLoad;
   XBeamRate::LiveLoadReactionsType permitSpecialLiveLoad;

   XBeamRate::SystemFactor systemFactor(1.0,1.0);
   XBeamRate::ConditionFactorType conditionFactor(1.0,XBeamRate::ConditionFactorEnum::Good);
   XBeamRate::DeadLoadFactors deadLoadFactors(1.25,1.5);
   XBeamRate::LiveLoadFactors liveLoadFactors(1.75,1.35,1.80,1.60,1.30,1.15);
   m_XBeamRateXML = std::auto_ptr<XBeamRate::XBeamRate>(new XBeamRate::XBeamRate(settings,systemFactor,conditionFactor,deadLoadFactors,liveLoadFactors,pierType,deckElevation,bridgeLineOffset,crownPointOffset,strOrientation,curbLineDatum,LCO,RCO,SL,SR,diaphragmHeight,diaphragmWidth,designLiveLoad,legalRoutineLiveLoad,legalSpecialLiveLoad,permitRoutineLiveLoad,permitSpecialLiveLoad,modE,fc,pier));

   // Start off with one bearing line that has one bearing
   XBeamRate::BearingLocatorType bearingLocator(0,OpenBridgeML::Types::OffsetMeasurementEnum::Alignment,::ConvertToSysUnits(-6.0,unitMeasure::Feet));
   XBeamRate::BearingLineType backBearingLine(bearingLocator);
   XBeamRate::BearingType bearing1(0,0,0);
   XBeamRate::BearingType bearing2(0,0,0);
   XBeamRate::BearingType bearing3(0,0,0);
   Float64 S = ::ConvertToSysUnits(6.0,unitMeasure::Feet);

   backBearingLine.Bearing().push_back(bearing1);
   backBearingLine.Spacing().push_back(S);
   backBearingLine.Bearing().push_back(bearing2);
   backBearingLine.Spacing().push_back(S);
   backBearingLine.Bearing().push_back(bearing3);

   m_XBeamRateXML->BearingLine().push_back(backBearingLine);

   // create a dummy rebar row
   XBeamRate::LongitudinalRebarRowType rebarRow(XBeamRate::LongitudinalRebarDatumEnum::Bottom,::ConvertToSysUnits(2.0,unitMeasure::Inch),OpenBridgeML::StandardReinforcement::USBarEnum::US11,5,::ConvertToSysUnits(6.0,unitMeasure::Inch));
   m_XBeamRateXML->LongitudinalRebar().push_back(rebarRow);

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
   CComPtr<IXBeamRateAgent> pXBR;
   HRESULT hr = m_pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pXBR);
   if ( SUCCEEDED(hr) )
   {
      // XBeam Rate is acting as an extension to PGSuper/PGSplice
      return S_OK;
   }

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
STDMETHODIMP CProjectAgentImp::Save(IStructuredSave* pStrSave)
{
   HRESULT hr = S_OK;

   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   XBeamRate::UnitModeEnum units = (pDisplayUnits->GetUnitMode() == eafTypes::umSI ? XBeamRate::UnitModeEnum::SI : XBeamRate::UnitModeEnum::US);
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
   std::stringstream ss; // NOTE: XML Stream uses ANSI character set
   ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << T2A(strUnit);

#pragma Reminder("UDPATE: need to bind to schema for validation")
   // create the XML\C++ binding reading from the string stream
   // for instance document validation, we need to tell the XML parser where the schema files are located
   //xml_schema::properties props;
   //props.no_namespace_schema_location(_T("F:\\ARP\\XBeamRate\\Schema\\XBeamRate.xsd"));
   //props.schema_location(_T("http://www.wsdot.wa.gov/OpenBridgeML/Units"),_T("F:\\ARP\\OpenBridgeML\\Schema\\OpenBridgeML_Units.xsd"));
   //std::auto_ptr<XBeamRate::XBeamRate> xbrXML = XBeamRate::XBeamRate_(ss,0,props);
   try
   {
      m_XBeamRateXML = XBeamRate::XBeamRate_(ss,xml_schema::flags::dont_validate); // can't figure out validation right now
   }
   catch(...)
   {
      return E_FAIL;
   }

   // extract our data from the binding object
   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   XBeamRate::ApplicationSettings& settings( m_XBeamRateXML->Settings() );
   if ( settings.Units() == XBeamRate::UnitModeEnum::SI )
   {
      pDisplayUnits->SetUnitMode(eafTypes::umSI);
   }
   else
   {
      pDisplayUnits->SetUnitMode(eafTypes::umUS);
   }

   // convert the units of measure into our base units
   hr = ConvertToBaseUnits();

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
      {
         rMessage = string.Mid(pos+1);
      }
	}
	else
	{
		// not found
      TRACE1("Warning (XBeamRate::ProjectAgent): no tool tip for ID 0x%04X.\n", nID);
	}

   return TRUE;
}

//////////////////////////////////////////////////////////////////////
// IXBRProject
void CProjectAgentImp::SetProjectName(LPCTSTR strName)
{
   m_XBeamRateXML->Settings().ProjectName(strName);
   Fire_OnProjectChanged();
}

LPCTSTR CProjectAgentImp::GetProjectName()
{
   return m_XBeamRateXML->Settings().ProjectName().c_str();
}

xbrTypes::PierConnectionType CProjectAgentImp::GetPierType()
{
   xbrTypes::PierConnectionType pierType =
      (xbrTypes::PierConnectionType)(XBeamRate::PierTypeEnum::value)(m_XBeamRateXML->PierType());

   return pierType;
}

void CProjectAgentImp::SetPierType(xbrTypes::PierConnectionType pierType)
{
   m_XBeamRateXML->PierType((XBeamRate::PierTypeEnum::value)pierType);
}

void CProjectAgentImp::SetDeckElevation(Float64 deckElevation)
{
   m_XBeamRateXML->DeckElevation(deckElevation);
}

Float64 CProjectAgentImp::GetDeckElevation()
{
   return m_XBeamRateXML->DeckElevation();
}

void CProjectAgentImp::SetCrownPointOffset(Float64 cpo)
{
   m_XBeamRateXML->CrownPointOffset(cpo);
}

Float64 CProjectAgentImp::GetCrownPointOffset()
{
   return m_XBeamRateXML->CrownPointOffset();
}

void CProjectAgentImp::SetBridgeLineOffset(Float64 blo)
{
   m_XBeamRateXML->BridgeLineOffset(blo);
}

Float64 CProjectAgentImp::GetBridgeLineOffset()
{
   return m_XBeamRateXML->BridgeLineOffset();
}

void CProjectAgentImp::SetOrientation(LPCTSTR strOrientation)
{
   m_XBeamRateXML->Orientation(strOrientation);
}

LPCTSTR CProjectAgentImp::GetOrientation()
{
   return m_XBeamRateXML->Orientation().c_str();
}

pgsTypes::OffsetMeasurementType CProjectAgentImp::GetCurbLineDatum()
{
   return (pgsTypes::OffsetMeasurementType)(OpenBridgeML::Types::OffsetMeasurementEnum::value)m_XBeamRateXML->CurbLineOffsetDatum();
}

void CProjectAgentImp::SetCurbLineDatum(pgsTypes::OffsetMeasurementType datumType)
{
   m_XBeamRateXML->CurbLineOffsetDatum((OpenBridgeML::Types::OffsetMeasurementEnum::value)datumType);
}

void CProjectAgentImp::SetCurbLineOffset(Float64 leftCLO,Float64 rightCLO)
{
   m_XBeamRateXML->LeftCurbOffset(leftCLO);
   m_XBeamRateXML->RightCurbOffset(rightCLO);
}

void CProjectAgentImp::GetCurbLineOffset(Float64* pLeftCLO,Float64* pRightCLO)
{
   *pLeftCLO = m_XBeamRateXML->LeftCurbOffset();
   *pRightCLO = m_XBeamRateXML->RightCurbOffset();
}

void CProjectAgentImp::SetCrownSlopes(Float64 sl,Float64 sr)
{
   m_XBeamRateXML->LeftCrownSlope(sl);
   m_XBeamRateXML->RightCrownSlope(sr);
}

void CProjectAgentImp::GetCrownSlopes(Float64* psl,Float64* psr)
{
   *psl = m_XBeamRateXML->LeftCrownSlope();
   *psr = m_XBeamRateXML->RightCrownSlope();
}

void CProjectAgentImp::GetDiaphragmDimensions(Float64* pH,Float64* pW)
{
   *pH = m_XBeamRateXML->DiaphragmHeight();
   *pW = m_XBeamRateXML->DiaphragmWidth();
}

void CProjectAgentImp::SetDiaphragmDimensions(Float64 H,Float64 W)
{
   m_XBeamRateXML->DiaphragmHeight(H);
   m_XBeamRateXML->DiaphragmWidth(W);
}

IndexType CProjectAgentImp::GetBearingLineCount()
{
   return (IndexType)m_XBeamRateXML->BearingLine().size();
}

void CProjectAgentImp::SetBearingLineCount(IndexType nBearingLines)
{
   ATLASSERT(1 <= nBearingLines && nBearingLines <= 2);
   if ( nBearingLines != GetBearingLineCount() )
   {
      if ( nBearingLines == 1 )
      {
         // removing one bearing line
         m_XBeamRateXML->BearingLine().pop_back();
      }
      else
      {
         ATLASSERT(nBearingLines == 2);
         // add a bearing line... using a copy of the existing bearin gline
         m_XBeamRateXML->BearingLine().push_back(m_XBeamRateXML->BearingLine().back());
      }
   }

   ATLASSERT(nBearingLines == GetBearingLineCount());
}

IndexType CProjectAgentImp::GetBearingCount(IndexType brgLineIdx)
{
   ATLASSERT(brgLineIdx < GetBearingLineCount());
   return m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().size();
}

void CProjectAgentImp::SetBearingCount(IndexType brgLineIdx,IndexType nBearings)
{
   ATLASSERT(1 <= nBearings); // must always be at least one bearing
   IndexType nCurrentBearings = GetBearingCount(brgLineIdx);
   if ( nBearings != nCurrentBearings )
   {
      if ( nBearings < nCurrentBearings )
      {
         // removing bearings
         XBeamRate::BearingLineType::Bearing_iterator brgBegin = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().begin();
         XBeamRate::BearingLineType::Bearing_iterator brgEnd   = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().end();
         brgBegin += nBearings;
         m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().erase(brgBegin,brgEnd);

         // remove spacing also
         XBeamRate::BearingLineType::Spacing_iterator spaBegin = m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().begin();
         XBeamRate::BearingLineType::Spacing_iterator spaEnd   = m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().end();
         spaBegin += nBearings;
         m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().erase(spaBegin,spaEnd);
      }
      else
      {
         // adding bearings
         IndexType nToAdd = nBearings - nCurrentBearings;
         m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().insert(m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().end(),
            nToAdd,
            m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().back());

         m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().insert(m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().end(),
            nToAdd,
            m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().back());
      }
   }

   ATLASSERT(nBearings == GetBearingCount(brgLineIdx));
}

Float64 CProjectAgentImp::GetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx)
{
   ATLASSERT(brgIdx < GetBearingCount(brgLineIdx)-1);
   return m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing()[brgIdx];
}

void CProjectAgentImp::SetBearingSpacing(IndexType brgLineIdx,IndexType brgIdx,Float64 spacing)
{
   ATLASSERT(brgIdx < GetBearingCount(brgLineIdx)-1);
   m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing()[brgIdx] = spacing;
}

void CProjectAgentImp::SetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64 DC,Float64 DW)
{
   m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing()[brgIdx].DC() = DC;
   m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing()[brgIdx].DW() = DW;

   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetBearingReactions(IndexType brgLineIdx,IndexType brgIdx,Float64* pDC,Float64* pDW)
{
   *pDC = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing()[brgIdx].DC();
   *pDW = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing()[brgIdx].DW();
}

void CProjectAgentImp::GetReferenceBearing(IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum)
{
   *pRefIdx = (IndexType)m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().ReferenceBearingID();
   *pRefBearingOffset = m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Location();
   *pRefBearingDatum = (pgsTypes::OffsetMeasurementType)(OpenBridgeML::Types::OffsetMeasurementEnum::value)m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Measure();
}

void CProjectAgentImp::SetReferenceBearing(IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum)
{
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().ReferenceBearingID() = (XBeamRate::BearingLocatorType::ReferenceBearingID_type)refIdx;
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Location() = refBearingOffset;
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Measure((OpenBridgeML::Types::OffsetMeasurementEnum::value)refBearingDatum);

   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetLiveLoadReactionCount(pgsTypes::LiveLoadType liveLoadType)
{
   IndexType nReactions = 0;
   switch ( liveLoadType )
   {
   case pgsTypes::lltDesign:
      nReactions = m_XBeamRateXML->Design().Reactions().size();
      break;

   //case pgsTypes::lltPermit:
   //case pgsTypes::lltFatigue:
   //case pgsTypes::lltPedestrian:

   case pgsTypes::lltLegalRating_Routine:
      nReactions = m_XBeamRateXML->Legal_Routine().Reactions().size();
      break;

   case pgsTypes::lltLegalRating_Special:
      nReactions = m_XBeamRateXML->Legal_Special().Reactions().size();
      break;

   case pgsTypes::lltPermitRating_Routine:
      nReactions = m_XBeamRateXML->Permit_Routine().Reactions().size();
      break;

   case pgsTypes::lltPermitRating_Special:
      nReactions = m_XBeamRateXML->Permit_Special().Reactions().size();
      break;

   }

   return nReactions;
}

void CProjectAgentImp::SetLiveLoadReactions(pgsTypes::LiveLoadType liveLoadType,const std::vector<std::pair<std::_tstring,Float64>>& vLLIM)
{
   XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions = NULL;

   switch ( liveLoadType )
   {
   case pgsTypes::lltDesign:
      pReactions = &m_XBeamRateXML->Design().Reactions();
      break;

   //case pgsTypes::lltPermit:
   //case pgsTypes::lltFatigue:
   //case pgsTypes::lltPedestrian:

   case pgsTypes::lltLegalRating_Routine:
      pReactions = &m_XBeamRateXML->Legal_Routine().Reactions();
      break;

   case pgsTypes::lltLegalRating_Special:
      pReactions = &m_XBeamRateXML->Legal_Special().Reactions();
      break;

   case pgsTypes::lltPermitRating_Routine:
      pReactions = &m_XBeamRateXML->Permit_Routine().Reactions();
      break;

   case pgsTypes::lltPermitRating_Special:
      pReactions = &m_XBeamRateXML->Permit_Special().Reactions();
      break;

   }

   SetLiveLoadReactions( vLLIM, pReactions );
}

std::vector<std::pair<std::_tstring,Float64>> CProjectAgentImp::GetLiveLoadReactions(pgsTypes::LiveLoadType liveLoadType)
{
   XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions = NULL;

   switch ( liveLoadType )
   {
   case pgsTypes::lltDesign:
      pReactions = &m_XBeamRateXML->Design().Reactions();
      break;

   //case pgsTypes::lltPermit:
   //case pgsTypes::lltFatigue:
   //case pgsTypes::lltPedestrian:

   case pgsTypes::lltLegalRating_Routine:
      pReactions = &m_XBeamRateXML->Legal_Routine().Reactions();
      break;

   case pgsTypes::lltLegalRating_Special:
      pReactions = &m_XBeamRateXML->Legal_Special().Reactions();
      break;

   case pgsTypes::lltPermitRating_Routine:
      pReactions = &m_XBeamRateXML->Permit_Routine().Reactions();
      break;

   case pgsTypes::lltPermitRating_Special:
      pReactions = &m_XBeamRateXML->Permit_Special().Reactions();
      break;

   }

   return GetLiveLoadReactions( pReactions );
}

void CProjectAgentImp::SetModE(Float64 Ec)
{
   m_XBeamRateXML->ModE() = Ec;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetModE()
{
   return m_XBeamRateXML->ModE();
}

void CProjectAgentImp::SetFc(Float64 fc)
{
   m_XBeamRateXML->Fc() = fc;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetFc()
{
   return m_XBeamRateXML->Fc();
}

void CProjectAgentImp::SetXBeamDimensions(pgsTypes::PierSideType side,Float64 height,Float64 taperHeight,Float64 taperLength)
{
   if ( side == pgsTypes::pstLeft )
   {
      m_XBeamRateXML->Pier().CapBeam().LeftHeight()      = height;
      m_XBeamRateXML->Pier().CapBeam().LeftTaperHeight() = taperHeight;
      m_XBeamRateXML->Pier().CapBeam().LeftTaperLength() = taperLength;
   }
   else
   {
      m_XBeamRateXML->Pier().CapBeam().RightHeight()      = height;
      m_XBeamRateXML->Pier().CapBeam().RightTaperHeight() = taperHeight;
      m_XBeamRateXML->Pier().CapBeam().RightTaperLength() = taperLength;
   }
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetXBeamDimensions(pgsTypes::PierSideType side,Float64* pHeight,Float64* pTaperHeight,Float64* pTaperLength)
{
   if ( side == pgsTypes::pstLeft )
   {
      *pHeight      = m_XBeamRateXML->Pier().CapBeam().LeftHeight();
      *pTaperHeight = m_XBeamRateXML->Pier().CapBeam().LeftTaperHeight();
      *pTaperLength = m_XBeamRateXML->Pier().CapBeam().LeftTaperLength();
   }
   else
   {
      *pHeight      = m_XBeamRateXML->Pier().CapBeam().RightHeight();
      *pTaperHeight = m_XBeamRateXML->Pier().CapBeam().RightTaperHeight();
      *pTaperLength = m_XBeamRateXML->Pier().CapBeam().RightTaperLength();
   }
}

void CProjectAgentImp::SetXBeamWidth(Float64 width)
{
   m_XBeamRateXML->Pier().CapBeam().Width() = width;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetXBeamWidth()
{
   return m_XBeamRateXML->Pier().CapBeam().Width();
}

void CProjectAgentImp::SetXBeamOverhang(pgsTypes::PierSideType side,Float64 overhang)
{
   if ( side == pgsTypes::pstLeft )
   {
      m_XBeamRateXML->Pier().CapBeam().LeftOverhang(overhang);
   }
   else
   {
      m_XBeamRateXML->Pier().CapBeam().RightOverhang(overhang);
   }
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetXBeamOverhangs(Float64 leftOverhang,Float64 rightOverhang)
{
   m_XBeamRateXML->Pier().CapBeam().LeftOverhang(leftOverhang);
   m_XBeamRateXML->Pier().CapBeam().RightOverhang(rightOverhang);
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetXBeamOverhang(pgsTypes::PierSideType side)
{
   if ( side == pgsTypes::pstLeft )
   {
      return m_XBeamRateXML->Pier().CapBeam().LeftOverhang();
   }
   else
   {
      return m_XBeamRateXML->Pier().CapBeam().RightOverhang();
   }
}

void CProjectAgentImp::GetXBeamOverhangs(Float64* pLeftOverhang,Float64* pRightOverhang)
{
   *pLeftOverhang  = m_XBeamRateXML->Pier().CapBeam().LeftOverhang();
   *pRightOverhang = m_XBeamRateXML->Pier().CapBeam().RightOverhang();
}

void CProjectAgentImp::SetColumns(IndexType nColumns,Float64 height,CColumnData::ColumnHeightMeasurementType heightMeasure,Float64 spacing)
{
   OpenBridgeML::Pier::FoundationType foundation(OpenBridgeML::Pier::IdealizedFoundationEnum::Fixed);
   OpenBridgeML::Pier::PrismaticColumnType newColumn(foundation);

   m_XBeamRateXML->Pier().Columns().PrismaticColumn().resize(nColumns,newColumn);
   m_XBeamRateXML->Pier().Columns().Spacing().resize(nColumns-1,spacing);

   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      if ( heightMeasure == CColumnData::chtHeight )
      {
         m_XBeamRateXML->Pier().Columns().PrismaticColumn()[colIdx].BottomElevation().reset();
         m_XBeamRateXML->Pier().Columns().PrismaticColumn()[colIdx].Height() = height;
      }
      else
      {
         m_XBeamRateXML->Pier().Columns().PrismaticColumn()[colIdx].Height().reset();
         m_XBeamRateXML->Pier().Columns().PrismaticColumn()[colIdx].BottomElevation() = height;
      }
      if ( colIdx < nColumns-1 )
      {
         m_XBeamRateXML->Pier().Columns().Spacing()[colIdx] = spacing;
      }
   }
   Fire_OnProjectChanged();
}

IndexType CProjectAgentImp::GetColumnCount()
{
   return m_XBeamRateXML->Pier().Columns().PrismaticColumn().size();
}

Float64 CProjectAgentImp::GetColumnHeight(IndexType colIdx)
{
   if ( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].Height().present() )
   {
      return m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].Height().get();
   }
   else
   {
      ATLASSERT( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].BottomElevation().present() );
      return m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].BottomElevation().get();
   }
}

CColumnData::ColumnHeightMeasurementType CProjectAgentImp::GetColumnHeightMeasurementType()
{
   if ( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].Height().present() )
   {
      ATLASSERT( !m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].BottomElevation().present() );
      return CColumnData::chtHeight;
   }
   else
   {
      ATLASSERT( !m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].Height().present() );
      ATLASSERT( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].BottomElevation().present() );
      return CColumnData::chtBottomElevation;
   }
}

xbrTypes::ColumnBaseType CProjectAgentImp::GetColumnBaseType(IndexType colIdx)
{
   xbrTypes::ColumnBaseType baseType;
   if ( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[colIdx].Foundation().IdealizedFoundation() == OpenBridgeML::Pier::IdealizedFoundationEnum::Fixed) 
   {
      baseType = xbrTypes::cbtFixed;
   }
   else
   {
      baseType = xbrTypes::cbtPinned;
   }

   return baseType;
}

Float64 CProjectAgentImp::GetSpacing(IndexType spaceIdx)
{
   return m_XBeamRateXML->Pier().Columns().Spacing()[spaceIdx];
}

void CProjectAgentImp::SetColumnShape(CColumnData::ColumnShapeType shapeType,Float64 D1,Float64 D2)
{
   if ( shapeType == CColumnData::cstCircle )
   {
      OpenBridgeML::Pier::CicularColumnSectionType section(D1);
      BOOST_FOREACH(OpenBridgeML::Pier::ColumnsType::PrismaticColumn_type& column,m_XBeamRateXML->Pier().Columns().PrismaticColumn())
      {
         column.RectangularSection().reset();
         column.CircularSection().set(section);
      }
   }
   else if ( shapeType == CColumnData::cstRectangle )
   {
      OpenBridgeML::Pier::RectangularColumnSectionType section(D1,D2);
      BOOST_FOREACH(OpenBridgeML::Pier::ColumnsType::PrismaticColumn_type& column,m_XBeamRateXML->Pier().Columns().PrismaticColumn())
      {
         column.CircularSection().reset();
         column.RectangularSection().set(section);
      }
   }
   else
   {
      ATLASSERT(false); // is there a new shape type?
   }
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetColumnShape(CColumnData::ColumnShapeType* pShapeType,Float64* pD1,Float64* pD2)
{
   if ( m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].CircularSection().present() )
   {
      ATLASSERT( !m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].RectangularSection().present() );
      *pShapeType = CColumnData::cstCircle;
      *pD1 = m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].CircularSection().get().Diameter();
      *pD2 = *pD1;
   }
   else
   {
      ATLASSERT( !m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].CircularSection().present() );
      *pShapeType = CColumnData::cstRectangle;
      *pD1 = m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].RectangularSection().get().B();
      *pD2 = m_XBeamRateXML->Pier().Columns().PrismaticColumn()[0].RectangularSection().get().D();
   }
}

void CProjectAgentImp::SetTransverseLocation(ColumnIndexType colIdx,Float64 offset,pgsTypes::OffsetMeasurementType measure)
{
   m_XBeamRateXML->Pier().Location().ColumnIndex(colIdx);
   m_XBeamRateXML->Pier().Location().TransverseOffset(offset);
   m_XBeamRateXML->Pier().Location().Measure((OpenBridgeML::Types::OffsetMeasurementEnum::value)measure);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetTransverseLocation(ColumnIndexType* pColIdx,Float64* pOffset,pgsTypes::OffsetMeasurementType* pMeasure)
{
   *pColIdx = (ColumnIndexType)m_XBeamRateXML->Pier().Location().ColumnIndex();
   *pOffset = m_XBeamRateXML->Pier().Location().TransverseOffset();
   *pMeasure = (pgsTypes::OffsetMeasurementType)(OpenBridgeML::Types::OffsetMeasurementEnum::value)(m_XBeamRateXML->Pier().Location().Measure());
}

Float64 CProjectAgentImp::GetXBeamLength()
{
   Float64 X3 = m_XBeamRateXML->Pier().CapBeam().LeftOverhang();
   Float64 X4 = m_XBeamRateXML->Pier().CapBeam().RightOverhang();

   Float64 s = 0;
   BOOST_FOREACH(OpenBridgeML::Pier::ColumnsType::Spacing_type& spacing,m_XBeamRateXML->Pier().Columns().Spacing())
   {
      s += spacing;
   }

   return X3 + s + X4;
}

IndexType CProjectAgentImp::GetRebarRowCount()
{
   return m_XBeamRateXML->LongitudinalRebar().size();
}

void CProjectAgentImp::AddRebarRow(xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,Int16 nBars,Float64 spacing)
{
   XBeamRate::LongitudinalRebarRowType rebarRow(
      (XBeamRate::LongitudinalRebarDatumEnum::value)datum,
      cover,
      ConvertBarSize(barSize),
      nBars,
      spacing);

   m_XBeamRateXML->LongitudinalRebar().push_back(rebarRow);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType datum,Float64 cover,matRebar::Size barSize,Int16 nBars,Float64 spacing)
{
   XBeamRate::LongitudinalRebarRowType rebarRow(
      (XBeamRate::LongitudinalRebarDatumEnum::value)datum,
      cover,
      ConvertBarSize(barSize),
      nBars,
      spacing);

   m_XBeamRateXML->LongitudinalRebar()[rowIdx] = rebarRow;
}

void CProjectAgentImp::GetRebarRow(IndexType rowIdx,xbrTypes::LongitudinalRebarDatumType* pDatum,Float64* pCover,matRebar::Size* pBarSize,Int16* pnBars,Float64* pSpacing)
{
   XBeamRate::LongitudinalRebarRowType& rebarData = m_XBeamRateXML->LongitudinalRebar()[rowIdx];
   *pDatum = (xbrTypes::LongitudinalRebarDatumType)(XBeamRate::LongitudinalRebarDatumEnum::value)rebarData.Datum();
   *pCover = rebarData.Cover();
   *pBarSize = ConvertBarSize(rebarData.Bar());
   *pnBars = rebarData.BarCount();
   *pSpacing = rebarData.Spacing();
}

void CProjectAgentImp::RemoveRebarRow(IndexType rowIdx)
{
   m_XBeamRateXML->LongitudinalRebar().erase(m_XBeamRateXML->LongitudinalRebar().begin()+rowIdx);
   Fire_OnProjectChanged();
}

void CProjectAgentImp::RemoveRebarRows()
{
   m_XBeamRateXML->LongitudinalRebar().clear();
   Fire_OnProjectChanged();
}

void CProjectAgentImp::SetConditionFactor(pgsTypes::ConditionFactorType conditionFactorType,Float64 conditionFactor)
{
   m_XBeamRateXML->ConditionFactor().Value() = conditionFactor;
   m_XBeamRateXML->ConditionFactor().ConditionFactor() = (XBeamRate::ConditionFactorEnum::value)conditionFactorType;
   Fire_OnProjectChanged();
}

void CProjectAgentImp::GetConditionFactor(pgsTypes::ConditionFactorType* pConditionFactorType,Float64 *pConditionFactor)
{
   *pConditionFactorType = (pgsTypes::ConditionFactorType)(XBeamRate::ConditionFactorEnum::value)m_XBeamRateXML->ConditionFactor().ConditionFactor();
   *pConditionFactor = m_XBeamRateXML->ConditionFactor().Value();
}

Float64 CProjectAgentImp::GetConditionFactor()
{
   pgsTypes::ConditionFactorType cfType;
   Float64 CF;
   GetConditionFactor(&cfType,&CF);

   // MBE 6A.4.2.3
   switch(cfType)
   {
   case pgsTypes::cfGood:
      return 1.0;

   case pgsTypes::cfFair:
      return 0.95;

   case pgsTypes::cfPoor:
      return 0.85;

   case pgsTypes::cfOther:
      return CF;
   }
   ATLASSERT(false);
   return 1.0;
}

void CProjectAgentImp::SetDCLoadFactor(Float64 dc)
{
   m_XBeamRateXML->DeadLoadFactors().DC() = dc;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDCLoadFactor()
{
   return m_XBeamRateXML->DeadLoadFactors().DC();
}

void CProjectAgentImp::SetDWLoadFactor(Float64 dw)
{
   m_XBeamRateXML->DeadLoadFactors().DW() = dw;
   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetDWLoadFactor()
{
   return m_XBeamRateXML->DeadLoadFactors().DW();
}

void CProjectAgentImp::SetLiveLoadFactor(pgsTypes::LoadRatingType ratingType,Float64 ll)
{
   switch(ratingType)
   {
      case pgsTypes::lrDesign_Inventory:
         m_XBeamRateXML->LiveLoadFactors().Inventory() = ll;
         break;

      case pgsTypes::lrDesign_Operating:
         m_XBeamRateXML->LiveLoadFactors().Operating() = ll;
         break;

      case pgsTypes::lrLegal_Routine:
         m_XBeamRateXML->LiveLoadFactors().Legal_Routine() = ll;
         break;

      case pgsTypes::lrLegal_Special:
         m_XBeamRateXML->LiveLoadFactors().Legal_Special() = ll;
         break;

      case pgsTypes::lrPermit_Routine:
         m_XBeamRateXML->LiveLoadFactors().Permit_Routine() = ll;
         break;

      case pgsTypes::lrPermit_Special:
         m_XBeamRateXML->LiveLoadFactors().Permit_Special() = ll;
         break;

      default:
         ATLASSERT(false); // should never get here
   }

   Fire_OnProjectChanged();
}

Float64 CProjectAgentImp::GetLiveLoadFactor(pgsTypes::LoadRatingType ratingType)
{
   switch(ratingType)
   {
      case pgsTypes::lrDesign_Inventory:
         return m_XBeamRateXML->LiveLoadFactors().Inventory();

      case pgsTypes::lrDesign_Operating:
         return m_XBeamRateXML->LiveLoadFactors().Operating();

      case pgsTypes::lrLegal_Routine:
         return m_XBeamRateXML->LiveLoadFactors().Legal_Routine();

      case pgsTypes::lrLegal_Special:
         return m_XBeamRateXML->LiveLoadFactors().Legal_Special();

      case pgsTypes::lrPermit_Routine:
         return m_XBeamRateXML->LiveLoadFactors().Permit_Routine();

      case pgsTypes::lrPermit_Special:
         return m_XBeamRateXML->LiveLoadFactors().Permit_Special();
  
      default:
         ATLASSERT(false); // should never get here
   }

   return 1.0;
}

//////////////////////////////////////////////////////////
// IXBRRatingSpecification
void CProjectAgentImp::SetSystemFactorFlexure(Float64 sysFactor)
{
   m_XBeamRateXML->SystemFactor().Flexure() = sysFactor;
}

Float64 CProjectAgentImp::GetSystemFactorFlexure()
{
   return m_XBeamRateXML->SystemFactor().Flexure();
}

void CProjectAgentImp::SetSystemFactorShear(Float64 sysFactor)
{
   m_XBeamRateXML->SystemFactor().Shear() = sysFactor;
}

Float64 CProjectAgentImp::GetSystemFactorShear()
{
   return m_XBeamRateXML->SystemFactor().Shear();
}

//////////////////////////////////////////////////////////
// IXBRProjectEdit
void CProjectAgentImp::EditPier(int nPage)
{
   m_CommandTarget.OnEditPier();
}

////////////////////////////////////////////////////////////////////////
// IXBREvents
void CProjectAgentImp::HoldEvents()
{
   ATLASSERT(0 <= m_EventHoldCount);
   m_EventHoldCount++;

   Fire_OnHoldEvents();
}

void CProjectAgentImp::FirePendingEvents()
{
   if ( m_EventHoldCount == 0 )
   {
      return;
   }

   if ( m_EventHoldCount == 1 )
   {
      m_EventHoldCount--;
      Fire_OnProjectChanged(); // fire our event
	   Fire_OnFirePendingEvents(); // tell event listeners that it is time to fire their events
   }
   else
   {
      m_EventHoldCount--;
   }
}

void CProjectAgentImp::CancelPendingEvents()
{
   m_EventHoldCount--;
   if ( m_EventHoldCount <= 0 )
   {
      m_EventHoldCount = 0;
      Fire_OnCancelPendingEvents();
   }
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
   pEditMenu->LoadMenu(IDR_EDIT_MENU,this);
}

void CProjectAgentImp::RemoveMenus()
{
   GET_IFACE(IEAFMainMenu,pMainMenu);
   CEAFMenu* pMenu = pMainMenu->GetMainMenu();

   // remove the Edit menu
   UINT editPos = pMenu->FindMenuItem(_T("&Edit"));
   VERIFY(pMenu->RemoveMenu(editPos,MF_BYPOSITION,this));
}

HRESULT CProjectAgentImp::ConvertToBaseUnits()
{
   // Called just after the document is loaded.... at this point the data in the the XBeamRateXML
   // object is in the units defined in the input document. This could be any combination
   // of consistent units and specified units. We need to convert everything into the
   // internal consistent unit system of this application. That is what this method does.

   // Create a generic unit server. This object defaults to a KMS system of units
   CComPtr<IUnitServer> xmlDocumentUnitServer;
   xmlDocumentUnitServer.CoCreateInstance(CLSID_UnitServer);

   // See if the XML instance document has a units declaration
   XBeamRate::XBeamRate::UnitsDeclaration_optional& unitsDeclaration(m_XBeamRateXML->UnitsDeclaration());
   if ( unitsDeclaration.present() )
   {
      // there was a units declaration in the XML instance document
      // initialize the generic unit server into the units specified in the 
      // XML instance document.
      if ( !InitializeWBFLUnitServer(&unitsDeclaration.get(),xmlDocumentUnitServer) )
      {
         return E_FAIL;
      }
   }

   // The the unit server for this application
   GET_IFACE(IXBeamRate,pXBeamRate);
   CComPtr<IUnitServer> pOurUnitServer;
   pXBeamRate->GetUnitServer(&pOurUnitServer);

   // Convert the XML instance data into the internal units for this application

   // General data
   ConvertBetweenBaseUnits(m_XBeamRateXML->DeckElevation(),    xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->CrownPointOffset(), xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->BridgeLineOffset(), xmlDocumentUnitServer, pOurUnitServer);

   BOOST_FOREACH(XBeamRate::BearingLineType& brgLine,m_XBeamRateXML->BearingLine())
   {
      ConvertBetweenBaseUnits(brgLine.BearingLocator().Location(), xmlDocumentUnitServer, pOurUnitServer);
      
      BOOST_FOREACH(XBeamRate::BearingType& bearing,brgLine.Bearing())
      {
         ConvertBetweenBaseUnits(bearing.DC(), xmlDocumentUnitServer, pOurUnitServer);
         ConvertBetweenBaseUnits(bearing.DW(), xmlDocumentUnitServer, pOurUnitServer);
      }

      BOOST_FOREACH(XBeamRate::BearingLineType::Spacing_type& spacing,brgLine.Spacing())
      {
         ConvertBetweenBaseUnits(spacing, xmlDocumentUnitServer, pOurUnitServer);
      }
   }

   ConvertBetweenBaseUnits(m_XBeamRateXML->ModE(),             xmlDocumentUnitServer, pOurUnitServer);
   
   // Cap Beam
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().LeftOverhang(),     xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().LeftHeight(),       xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().LeftTaperHeight(),  xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().LeftTaperLength(),  xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().RightHeight(),      xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().RightTaperHeight(), xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().RightTaperLength(), xmlDocumentUnitServer, pOurUnitServer);
   ConvertBetweenBaseUnits(m_XBeamRateXML->Pier().CapBeam().Width(),            xmlDocumentUnitServer, pOurUnitServer);
   
   // Column
   BOOST_FOREACH(OpenBridgeML::Pier::ColumnsType::PrismaticColumn_type& column,m_XBeamRateXML->Pier().Columns().PrismaticColumn())
   {
      if ( column.Height().present() )
      {
         ConvertBetweenBaseUnits(column.Height().get(), xmlDocumentUnitServer, pOurUnitServer);
      }

      if ( column.BottomElevation().present() )
      {
         ConvertBetweenBaseUnits(column.BottomElevation().get(), xmlDocumentUnitServer, pOurUnitServer);
      }

      if ( column.CircularSection().present() )
      {
         ConvertBetweenBaseUnits(column.CircularSection().get().Diameter(), xmlDocumentUnitServer, pOurUnitServer);
      }
      else if ( column.RectangularSection().present() )
      {
         ConvertBetweenBaseUnits(column.RectangularSection().get().B(), xmlDocumentUnitServer, pOurUnitServer);
         ConvertBetweenBaseUnits(column.RectangularSection().get().D(), xmlDocumentUnitServer, pOurUnitServer);
      }
   }

   BOOST_FOREACH(OpenBridgeML::Pier::ColumnsType::Spacing_type& spacing,m_XBeamRateXML->Pier().Columns().Spacing())
   {
      ConvertBetweenBaseUnits(spacing, xmlDocumentUnitServer, pOurUnitServer);
   }

   BOOST_FOREACH(XBeamRate::LongitudinalRebarRowType& rowType,m_XBeamRateXML->LongitudinalRebar())
   {
      ConvertBetweenBaseUnits(rowType.Cover(),xmlDocumentUnitServer,pOurUnitServer);
      ConvertBetweenBaseUnits(rowType.Spacing(),xmlDocumentUnitServer,pOurUnitServer);
   }

   return S_OK;
}

void CProjectAgentImp::SetLiveLoadReactions(const std::vector<std::pair<std::_tstring,Float64>>& vLLIM,XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions)
{
   pReactions->clear();
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator iter(vLLIM.begin());
   std::vector<std::pair<std::_tstring,Float64>>::const_iterator end(vLLIM.end());
   for ( ; iter != end; iter++ )
   {
      const std::pair<std::_tstring,Float64>& item(*iter);
      XBeamRate::LiveLoadReactionType reaction(item.first.c_str(),item.second);
      pReactions->push_back(reaction);
   }
}

std::vector<std::pair<std::_tstring,Float64>> CProjectAgentImp::GetLiveLoadReactions(const XBeamRate::LiveLoadReactionsType::Reactions_sequence* pReactions)
{
   std::vector<std::pair<std::_tstring,Float64>> vLLIM;
   if ( pReactions == NULL )
   {
      return vLLIM;
   }

   XBeamRate::LiveLoadReactionsType::Reactions_const_iterator iter(pReactions->begin());
   XBeamRate::LiveLoadReactionsType::Reactions_const_iterator end(pReactions->end());
   for ( ; iter != end; iter++ )
   {
      const XBeamRate::LiveLoadReactionType& reaction(*iter);
      std::_tstring strName(reaction.Name());
      Float64 r = reaction.LLIM();

      vLLIM.push_back(std::make_pair(strName,r));
   }

   return vLLIM;
}
