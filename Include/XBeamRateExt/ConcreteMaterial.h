
#pragma once

#include <XBeamRateExt\XBRExtExp.h>

/*****************************************************************************
CLASS 
   xbrConcreteMaterial

   Utility class for concrete material input parameters.

DESCRIPTION
   Utility class for concrete material input parameters.


COPYRIGHT
   Copyright © 1997-2008
   Washington State Department Of Transportation
   All Rights Reserved

LOG
   rab : 06.23.2008 : Created file
*****************************************************************************/

class XBREXTCLASS xbrConcreteMaterial
{
public:
   pgsTypes::ConcreteType Type;

   // General Concrete Parameters
   Float64 Fc; // 28-day concrete strength
   Float64 WeightDensity;
   Float64 StrengthDensity;
   Float64 MaxAggregateSize;
   bool    bUserEc; // if true, Ec is a user input value
   Float64 Ec; // 28-day secant modulus

   // Parameters for the AASHTO LRFD Material Model
   Float64 EcK1;
   Float64 EcK2;
   Float64 CreepK1;
   Float64 CreepK2;
   Float64 ShrinkageK1;
   Float64 ShrinkageK2;
   bool    bHasFct; // if true, the concrete model is defined with Fct
   Float64 Fct;

   //------------------------------------------------------------------------
   // Default constructor
   xbrConcreteMaterial();

   //------------------------------------------------------------------------
   // Copy constructor
   xbrConcreteMaterial(const xbrConcreteMaterial& rOther);

   //------------------------------------------------------------------------
   // Destructor
   ~xbrConcreteMaterial();

   // GROUP: OPERATORS
   //------------------------------------------------------------------------
   // Assignment operator
   xbrConcreteMaterial& operator = (const xbrConcreteMaterial& rOther);

   //------------------------------------------------------------------------
   bool operator==(const xbrConcreteMaterial& rOther) const;

   //------------------------------------------------------------------------
   bool operator!=(const xbrConcreteMaterial& rOther) const;

	HRESULT Load(IStructuredLoad* pStrLoad);
	HRESULT Save(IStructuredSave* pStrSave);

protected:
   //------------------------------------------------------------------------
   void MakeCopy(const xbrConcreteMaterial& rOther);

   //------------------------------------------------------------------------
   virtual void MakeAssignment(const xbrConcreteMaterial& rOther);
};
