
#include "StdAfx.h"
#include <XBeamRateExt\StirrupData.h>
#include <Lrfd\RebarPool.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

xbrStirrupData::xbrStirrupData()
{
   Symmetric = true;
   Zones.push_back(StirrupZone());
}

xbrStirrupData::xbrStirrupData(const xbrStirrupData& rOther)
{
   MakeCopy(rOther);
}

xbrStirrupData::~xbrStirrupData()
{
}

xbrStirrupData& xbrStirrupData::operator= (const xbrStirrupData& rOther)
{
   if( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

bool xbrStirrupData::operator == (const xbrStirrupData& rOther) const
{
   if ( Symmetric != rOther.Symmetric )
      return false;

   if ( Zones != rOther.Zones)
      return false;

   return true;
}

bool xbrStirrupData::operator != (const xbrStirrupData& rOther) const
{
   return !operator==( rOther );
}

HRESULT xbrStirrupData::Save(IStructuredSave* pStrSave)
{
   HRESULT hr = S_OK;

   pStrSave->BeginUnit(_T("StirrupData"),1.0);
   pStrSave->put_Property(_T("Symmetric"),CComVariant(Symmetric));
   BOOST_FOREACH(StirrupZone& zone,Zones)
   {
      pStrSave->BeginUnit(_T("Zone"),1.0);
      pStrSave->put_Property(_T("Length"), CComVariant(zone.Length) );
      pStrSave->put_Property(_T("BarSpacing"), CComVariant(zone.BarSpacing) );
      pStrSave->put_Property(_T("BarSize"),CComVariant((long)zone.BarSize) );
      pStrSave->put_Property(_T("nBars"),   CComVariant(zone.nBars) );
      pStrSave->EndUnit(); // Zone
   }
   pStrSave->EndUnit();

   return hr;
}

HRESULT xbrStirrupData::Load(IStructuredLoad* pStrLoad)
{
   CComVariant var;
   CHRException hr;

   try
   {
      hr = pStrLoad->BeginUnit(_T("StirrupData"));

      var.vt = VT_BOOL;
      hr = pStrLoad->get_Property(_T("Symmetric"),&var);
      Symmetric = (var.boolVal == VARIANT_TRUE ? true : false);

      while ( SUCCEEDED(pStrLoad->BeginUnit(_T("Zone"))) )
      {
         StirrupZone zone;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("Length"),&var);
         zone.Length = var.dblVal;

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("BarSpacing"),&var);
         zone.BarSpacing = var.dblVal;

         var.vt = VT_I4;
         hr = pStrLoad->get_Property(_T("BarSize"),&var);
         BarSizeType key = var.lVal;
         zone.BarSize = matRebar::Size(key);

         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("nBars"),&var);
         zone.nBars = var.dblVal;

         hr = pStrLoad->EndUnit(); // Zone
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

void xbrStirrupData::MakeCopy(const xbrStirrupData& rOther)
{
   Symmetric = rOther.Symmetric;
   Zones = rOther.Zones;
}

void xbrStirrupData::MakeAssignment(const xbrStirrupData& rOther)
{
   MakeCopy( rOther );
}
