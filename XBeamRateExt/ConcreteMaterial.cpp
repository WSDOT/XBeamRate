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

#include "stdafx.h"
#include <XBeamRateExt\ConcreteMaterial.h>

#include <Material\Concrete.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrConcreteMaterial
****************************************************************************/

// Make conversions static so they are only done once
static const Float64 gs_Fc     = ::ConvertToSysUnits(4.0,unitMeasure::KSI);
static const Float64 gs_StrengthDensity = ::ConvertToSysUnits(160.,unitMeasure::LbfPerFeet3);
static const Float64 gs_WeightDensity = gs_StrengthDensity;
static const Float64 gs_MaxAggregateSize = ::ConvertToSysUnits(0.75,unitMeasure::Inch);
static const Float64 gs_Ec = ::ConvertToSysUnits(4700., unitMeasure::KSI);


////////////////////////// PUBLIC     ///////////////////////////////////////

//======================== LIFECYCLE  =======================================
xbrConcreteMaterial::xbrConcreteMaterial()
{
   // Default material properties
   Type = pgsTypes::Normal;
   Fc  = gs_Fc;
   StrengthDensity = gs_StrengthDensity;
   WeightDensity = gs_WeightDensity;
   MaxAggregateSize = gs_MaxAggregateSize;
   EcK1 = 1.0;
   EcK2 = 1.0;
   CreepK1 = 1.0;
   CreepK2 = 1.0;
   ShrinkageK1 = 1.0;
   ShrinkageK2 = 1.0;
   bUserEc = false;
   Ec = gs_Ec;
   bHasFct = false;
   Fct = 0;
}

xbrConcreteMaterial::xbrConcreteMaterial(const xbrConcreteMaterial& rOther)
{
   MakeCopy(rOther);
}

xbrConcreteMaterial::~xbrConcreteMaterial()
{
}

//======================== OPERATORS  =======================================
xbrConcreteMaterial& xbrConcreteMaterial::operator= (const xbrConcreteMaterial& rOther)
{
   if( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

bool xbrConcreteMaterial::operator==(const xbrConcreteMaterial& rOther) const
{
   if ( Type != rOther.Type )
   {
      return false;
   }

   if ( Fc != rOther.Fc )
   {
      return false;
   }

   if ( bUserEc != rOther.bUserEc )
   {
      return false;
   }

   if ( bUserEc && !IsEqual(Ec,rOther.Ec) )
   {
      return false;
   }

   if ( !IsEqual( WeightDensity, rOther.WeightDensity ) )
   {
      return false;
   }

   if ( StrengthDensity != rOther.StrengthDensity )
   {
      return false;
   }

   if ( MaxAggregateSize != rOther.MaxAggregateSize )
   {
      return false;
   }

   if ( EcK1 != rOther.EcK1 )
   {
      return false;
   }

   if ( EcK2 != rOther.EcK2 )
   {
      return false;
   }

   if ( CreepK1 != rOther.CreepK1 )
   {
      return false;
   }

   if ( CreepK2 != rOther.CreepK2 )
   {
      return false;
   }

   if ( ShrinkageK1 != rOther.ShrinkageK1 )
   {
      return false;
   }

   if ( ShrinkageK2 != rOther.ShrinkageK2 )
   {
      return false;
   }

   if ( bHasFct != rOther.bHasFct )
   {
      return false;
   }

   if ( bHasFct && !IsEqual(Fct,rOther.Fct) )
   {
      return false;
   }

   return true;
}

bool xbrConcreteMaterial::operator!=(const xbrConcreteMaterial& rOther) const
{
   return !operator==(rOther);
}

void xbrConcreteMaterial::MakeCopy(const xbrConcreteMaterial& rOther)
{
   Type              = rOther.Type;
   Fc                = rOther.Fc;
   bUserEc           = rOther.bUserEc;
   Ec                = rOther.Ec;
   WeightDensity     = rOther.WeightDensity;
   StrengthDensity   = rOther.StrengthDensity;
   MaxAggregateSize  = rOther.MaxAggregateSize;

   // AASHTO Parameters
   EcK1              = rOther.EcK1;
   EcK2              = rOther.EcK2;
   CreepK1           = rOther.CreepK1;
   CreepK2           = rOther.CreepK2;
   ShrinkageK1       = rOther.ShrinkageK1;
   ShrinkageK2       = rOther.ShrinkageK2;
   bHasFct           = rOther.bHasFct;
   Fct               = rOther.Fct;
}


void xbrConcreteMaterial::MakeAssignment(const xbrConcreteMaterial& rOther)
{
   MakeCopy( rOther );
}

HRESULT xbrConcreteMaterial::Save(IStructuredSave* pStrSave)
{
   pStrSave->BeginUnit(_T("Concrete"),1.0);
   pStrSave->put_Property(_T("Type"),             CComVariant( matConcrete::GetTypeName((matConcrete::Type)Type,false).c_str() ));
   pStrSave->put_Property(_T("Fc"),               CComVariant(Fc));

   pStrSave->put_Property(_T("UserEc"),           CComVariant(bUserEc));

   if ( bUserEc )
   {
      pStrSave->put_Property(_T("Ec"),               CComVariant(Ec));
   }

   pStrSave->put_Property(_T("WeightDensity"),    CComVariant(WeightDensity));
   pStrSave->put_Property(_T("StrengthDensity"),  CComVariant(StrengthDensity));
   pStrSave->put_Property(_T("MaxAggregateSize"), CComVariant(MaxAggregateSize));

   // AASHTO Properties
   pStrSave->BeginUnit(_T("AASHTO"),1.0);
   pStrSave->put_Property(_T("EcK1"),             CComVariant(EcK1));
   pStrSave->put_Property(_T("EcK2"),             CComVariant(EcK2));
   pStrSave->put_Property(_T("CreepK1"),          CComVariant(CreepK1));
   pStrSave->put_Property(_T("CreepK2"),          CComVariant(CreepK2));
   pStrSave->put_Property(_T("ShrinkageK1"),      CComVariant(ShrinkageK1));
   pStrSave->put_Property(_T("ShrinkageK2"),      CComVariant(ShrinkageK2));

   if ( Type != pgsTypes::Normal )
   {
      pStrSave->put_Property(_T("HasFct"),CComVariant(bHasFct));
      if ( bHasFct )
      {
         pStrSave->put_Property(_T("Fct"),CComVariant(Fct));
      }
   }
   pStrSave->EndUnit(); // AASHTO

   pStrSave->EndUnit(); // Concrete

   return S_OK;
}

HRESULT xbrConcreteMaterial::Load(IStructuredLoad* pStrLoad)
{
   CComVariant var;
   USES_CONVERSION;

   CHRException hr;

   try
   {
      hr = pStrLoad->BeginUnit(_T("Concrete"));

      var.vt = VT_BSTR;
      hr = pStrLoad->get_Property(_T("Type"),&var);
      Type = (pgsTypes::ConcreteType)matConcrete::GetTypeFromName(OLE2T(var.bstrVal));

      var.vt = VT_R8;
      hr = pStrLoad->get_Property(_T("Fc"),&var);
      Fc = var.dblVal;

      var.vt = VT_BOOL;
      hr = pStrLoad->get_Property(_T("UserEc"),&var);
      bUserEc = (var.boolVal == VARIANT_TRUE ? true : false);

      if ( bUserEc )
      {
         var.vt = VT_R8;
         hr = pStrLoad->get_Property(_T("Ec"),&var);
         Ec = var.dblVal;
      }

      var.vt = VT_R8;
      hr = pStrLoad->get_Property(_T("WeightDensity"), &var);
      WeightDensity = var.dblVal;

      hr = pStrLoad->get_Property(_T("StrengthDensity"),  &var);
      StrengthDensity = var.dblVal;

      hr = pStrLoad->get_Property(_T("MaxAggregateSize"), &var);
      MaxAggregateSize = var.dblVal;

      // AASHTO
      hr = pStrLoad->BeginUnit(_T("AASHTO"));

      var.vt = VT_R8;

      hr = pStrLoad->get_Property(_T("EcK1"),&var);
      EcK1 = var.dblVal;

      hr = pStrLoad->get_Property(_T("EcK2"),&var);
      EcK2 = var.dblVal;

      hr = pStrLoad->get_Property(_T("CreepK1"),&var);
      CreepK1 = var.dblVal;

      hr = pStrLoad->get_Property(_T("CreepK2"),&var);
      CreepK2 = var.dblVal;

      hr = pStrLoad->get_Property(_T("ShrinkageK1"),&var);
      ShrinkageK1 = var.dblVal;

      hr = pStrLoad->get_Property(_T("ShrinkageK2"),&var);
      ShrinkageK2 = var.dblVal;

      if ( Type != pgsTypes::Normal )
      {
         var.vt = VT_BOOL;
         hr = pStrLoad->get_Property(_T("HasFct"),&var);
         bHasFct = (var.boolVal == VARIANT_TRUE ? true : false);

         if ( bHasFct )
         {
            var.vt = VT_R8;
            hr = pStrLoad->get_Property(_T("Fct"),&var);
            Fct = var.dblVal;
         }
      }

      hr = pStrLoad->EndUnit(); // AASHTO

      hr = pStrLoad->EndUnit(); // Concrete
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }

   return hr;
}
