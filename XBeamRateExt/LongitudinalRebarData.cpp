
#include "stdafx.h"
#include <XBeamRateExt\LongitudinalRebarData.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrLongitudinalRebarData
****************************************************************************/
xbrLongitudinalRebarData::xbrLongitudinalRebarData()
{
}

xbrLongitudinalRebarData::xbrLongitudinalRebarData(const xbrLongitudinalRebarData& rOther)
{
   MakeCopy(rOther);
}

xbrLongitudinalRebarData::~xbrLongitudinalRebarData()
{
}

//======================== OPERATORS  =======================================
xbrLongitudinalRebarData& xbrLongitudinalRebarData::operator= (const xbrLongitudinalRebarData& rOther)
{
   if( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

bool xbrLongitudinalRebarData::operator == (const xbrLongitudinalRebarData& rOther) const
{
   return (RebarRows == rOther.RebarRows);
}

bool xbrLongitudinalRebarData::operator != (const xbrLongitudinalRebarData& rOther) const
{
   return !operator==( rOther );
}


HRESULT xbrLongitudinalRebarData::Save(IStructuredSave* pStrSave,IProgress* pProgress)
{
   HRESULT hr = S_OK;

   pStrSave->BeginUnit(_T("LongitudinalRebar"),1.0);

   BOOST_FOREACH(RebarRow& rebar_row,RebarRows)
   {
      pStrSave->BeginUnit(_T("RebarRow"),1.0);
      pStrSave->put_Property(_T("Datum"),        CComVariant(rebar_row.Datum));
      pStrSave->put_Property(_T("Cover"),        CComVariant(rebar_row.Cover));
      pStrSave->put_Property(_T("NumberOfBars"), CComVariant(rebar_row.NumberOfBars));
      pStrSave->put_Property(_T("BarSize"),      CComVariant(rebar_row.BarSize));
      pStrSave->put_Property(_T("BarSpacing"),   CComVariant(rebar_row.BarSpacing));
      pStrSave->EndUnit();
   }

   pStrSave->EndUnit();

   return hr;
}

HRESULT xbrLongitudinalRebarData::Load(IStructuredLoad* pStrLoad,IProgress* pProgress)
{
   USES_CONVERSION;
   CComVariant var;
   CHRException hr;

   try
   {
      hr = pStrLoad->BeginUnit(_T("LongitudinalRebar")); 

      RebarRows.clear();
      while ( SUCCEEDED(pStrLoad->BeginUnit(_T("RebarRow"))) )
      {
         RebarRow rebar_row;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("Datum"),         &var);
         rebar_row.Datum = (xbrTypes::LongitudinalRebarDatumType)(var.lVal);

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("Cover"), &var);
         rebar_row.Cover = var.dblVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("NumberOfBars"), &var);
         rebar_row.NumberOfBars = var.lVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("BarSize"), &var );
         rebar_row.BarSize = matRebar::Size(var.lVal);

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("BarSpacing"), &var);
         rebar_row.BarSpacing = var.dblVal;

         RebarRows.push_back(rebar_row);

         hr = pStrLoad->EndUnit();
      }

      hr = pStrLoad->EndUnit();
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }

   return hr;
}

void xbrLongitudinalRebarData::MakeCopy(const xbrLongitudinalRebarData& rOther)
{
   RebarRows = rOther.RebarRows;
}

void xbrLongitudinalRebarData::MakeAssignment(const xbrLongitudinalRebarData& rOther)
{
   MakeCopy( rOther );
}
