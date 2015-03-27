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

#include <WBFLUnitServer\UnitServerUtils.h>

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

   pBrokerInit->RegInterface( XBR::IID_IProject,    this );

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
   ApplicationSettings settings(UnitModeEnum::US,_T("XBeam Rating Project"));

   PierTypeEnum pierType = PierTypeEnum::Integral;

   OpenBridgeML::Pier::CapBeamType capBeam(5.0,5.0,5.0,5.0,0,0,5.0,0,0);

   OpenBridgeML::Pier::FoundationType foundation(OpenBridgeML::Pier::IdealizedFoundationEnum::Fixed);
   OpenBridgeML::Pier::PrismaticColumnType column(foundation);
   OpenBridgeML::Pier::CicularColumnSectionType circularSection(5.0);
   column.CircularSection().set(circularSection);
   column.Height() = 10.0;

   OpenBridgeML::Pier::ColumnsType columns;
   columns.PrismaticColumn().push_back( column );
   columns.Spacing().push_back(5.0);
   columns.PrismaticColumn().push_back(column);

   OpenBridgeML::Pier::PierType pier(capBeam,columns);

   TransverseOffsetType transverseOffset(10.,OffsetMeasurementEnum::Alignment);
   ColumnIndexType refColIdx = 0;

   Float64 modE = ::ConvertToSysUnits(5000,unitMeasure::PSI);


   TransverseMeasurementEnum transverseMeasurementType = TransverseMeasurementEnum::NormalToAlignemnt;
   Float64 deckElevation = 0;
   Float64 bridgeLineOffset = 0;
   Float64 crownPointOffset = 0;

   LPCTSTR strOrientation = _T("00 00 0.0 L");

   m_XBeamRateXML = std::auto_ptr<XBeamRate>(new XBeamRate(settings,pierType,transverseMeasurementType,deckElevation,bridgeLineOffset,crownPointOffset,strOrientation,modE,refColIdx,transverseOffset,pier));

   // Start off with one bearing line that has one bearing
   BearingLocatorType bearingLocator(0,OffsetMeasurementEnum::Alignment,0.0);
   BearingLineType backBearingLine(bearingLocator);
   BearingType bearing1(0);
   BearingType bearing2(0);
   BearingType bearing3(0);
   BearingType bearing4(0);
   Float64 S = ::ConvertToSysUnits(6.0,unitMeasure::Feet);

   backBearingLine.Bearing().push_back(bearing1);
   backBearingLine.Spacing().push_back(S);
   backBearingLine.Bearing().push_back(bearing2);
   backBearingLine.Spacing().push_back(S);
   backBearingLine.Bearing().push_back(bearing3);
   backBearingLine.Spacing().push_back(S);
   backBearingLine.Bearing().push_back(bearing4);

   m_XBeamRateXML->BearingLine().push_back(backBearingLine);

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

   // create the XML\C++ binding reading from the string stream
   // for instance document validation, we need to tell the XML parser where the schema files are located
   //xml_schema::properties props;
   //props.no_namespace_schema_location(_T("F:\\ARP\\XBeamRate\\Schema\\XBeamRate.xsd"));
   //props.schema_location(_T("http://www.wsdot.wa.gov/OpenBridgeML/Units"),_T("F:\\ARP\\OpenBridgeML\\Schema\\OpenBridgeML_Units.xsd"));
   //std::auto_ptr<XBeamRate> xbrXML = XBeamRate_(ss,0,props);
   try
   {
      m_XBeamRateXML = XBeamRate_(ss,xml_schema::flags::dont_validate); // can't figure out validation right now
   }
   catch(...)
   {
      return E_FAIL;
   }

   // extract our data from the binding object
   GET_IFACE(IEAFDisplayUnits,pDisplayUnits);
   ApplicationSettings& settings( m_XBeamRateXML->Settings() );
   if ( settings.Units() == UnitModeEnum::SI )
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

xbrTypes::PierConnectionType CProjectAgentImp::GetPierType()
{
   xbrTypes::PierConnectionType pierType =
      (xbrTypes::PierConnectionType)(PierTypeEnum::value)(m_XBeamRateXML->PierType());

   return pierType;
}

void CProjectAgentImp::SetPierType(xbrTypes::PierConnectionType pierType)
{
   m_XBeamRateXML->PierType((PierTypeEnum::value)pierType);
}

xbrTypes::TransverseDimensionMeasurementType CProjectAgentImp::GetTransverseDimensionsMeasurementType()
{
   xbrTypes::TransverseDimensionMeasurementType measurementType =
      (xbrTypes::TransverseDimensionMeasurementType)(TransverseMeasurementEnum::value)(m_XBeamRateXML->TransverseDimensionMeasurementType());
   
   return measurementType;
}

void CProjectAgentImp::SetTransverseDimensionsMeasurementType(xbrTypes::TransverseDimensionMeasurementType measurementType)
{
   if ( measurementType == xbrTypes::tdmNormalToAlignment )
   {
      m_XBeamRateXML->TransverseDimensionMeasurementType(TransverseMeasurementEnum::NormalToAlignemnt);
   }
   else
   {
      m_XBeamRateXML->TransverseDimensionMeasurementType(TransverseMeasurementEnum::PlaneOfPier);
   }
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
         BearingLineType::Bearing_iterator brgBegin = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().begin();
         BearingLineType::Bearing_iterator brgEnd   = m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().end();
         brgBegin += nBearings;
         m_XBeamRateXML->BearingLine()[brgLineIdx].Bearing().erase(brgBegin,brgEnd);

         // remove spacing also
         BearingLineType::Spacing_iterator spaBegin = m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().begin();
         BearingLineType::Spacing_iterator spaEnd   = m_XBeamRateXML->BearingLine()[brgLineIdx].Spacing().end();
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

void CProjectAgentImp::GetReferenceBearing(IndexType brgLineIdx,IndexType* pRefIdx,Float64* pRefBearingOffset,pgsTypes::OffsetMeasurementType* pRefBearingDatum)
{
   *pRefIdx = (IndexType)m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().ReferenceBearingID();
   *pRefBearingOffset = m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Location();
   *pRefBearingDatum = (pgsTypes::OffsetMeasurementType)(OffsetMeasurementEnum::value)m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Measure();
}

void CProjectAgentImp::SetReferenceBearing(IndexType brgLineIdx,IndexType refIdx,Float64 refBearingOffset,pgsTypes::OffsetMeasurementType refBearingDatum)
{
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().ReferenceBearingID() = (BearingLocatorType::ReferenceBearingID_type)refIdx;
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Location() = refBearingOffset;
   m_XBeamRateXML->BearingLine()[brgLineIdx].BearingLocator().Measure((OffsetMeasurementEnum::value)refBearingDatum);

   Fire_OnProjectChanged();
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
   m_XBeamRateXML->RefColumnIdx() = colIdx;
   m_XBeamRateXML->TransverseOffset().TransverseOffset(offset);
   m_XBeamRateXML->TransverseOffset().Measure((OffsetMeasurementEnum::value)measure);
}

void CProjectAgentImp::GetTransverseLocation(ColumnIndexType* pColIdx,Float64* pOffset,pgsTypes::OffsetMeasurementType* pMeasure)
{
   *pColIdx = (ColumnIndexType)m_XBeamRateXML->RefColumnIdx();
   *pOffset = m_XBeamRateXML->TransverseOffset().TransverseOffset();
   *pMeasure = (pgsTypes::OffsetMeasurementType)(OffsetMeasurementEnum::value)(m_XBeamRateXML->TransverseOffset().Measure());
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
   XBeamRate::UnitsDeclaration_optional& unitsDeclaration(m_XBeamRateXML->UnitsDeclaration());
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

   BOOST_FOREACH(BearingLineType& brgLine,m_XBeamRateXML->BearingLine())
   {
      ConvertBetweenBaseUnits(brgLine.BearingLocator().Location(), xmlDocumentUnitServer, pOurUnitServer);
      
#pragma Reminder("Convert reactions here")
      //BOOST_FOREACH(BearingType& bearing,brgLine.Bearing())
      //{
      //   ConvertBetweenBaseUnits(bearing.?? forces/
      //}

      BOOST_FOREACH(BearingLineType::Spacing_type& spacing,brgLine.Spacing())
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

   return S_OK;
}