
#include "stdafx.h"
#include <XBeamRateExt\ShearRatingArtifact.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************
CLASS
   xbrShearRatingArtifact
****************************************************************************/
xbrShearRatingArtifact::xbrShearRatingArtifact() :
m_strVehicleName(_T("Unknown"))
{
   m_bRFComputed = false;
   m_RF = 0;

   m_RatingType = pgsTypes::lrDesign_Inventory;

   m_VehicleIndex = INVALID_INDEX;
   m_VehicleWeight = -999999;

   m_SystemFactor = 1.0;
   m_ConditionFactor = 1.0;
   m_CapacityRedutionFactor = 0.9;
   m_Vn = 0;
   m_gDC = 1;
   m_gDW = 1;
   m_gLL = 1;
   m_Vdc = 0;
   m_Vdw = 0;
   m_Vllim = 0;
   m_AdjVllim = 0;
}

xbrShearRatingArtifact::xbrShearRatingArtifact(const xbrShearRatingArtifact& rOther)
{
   MakeCopy(rOther);
}

xbrShearRatingArtifact::~xbrShearRatingArtifact()
{
}

xbrShearRatingArtifact& xbrShearRatingArtifact::operator=(const xbrShearRatingArtifact& rOther)
{
   if ( this != &rOther )
   {
      MakeAssignment(rOther);
   }

   return *this;
}

void xbrShearRatingArtifact::SetPointOfInterest(const xbrPointOfInterest& poi)
{
   m_POI = poi;
}

const xbrPointOfInterest& xbrShearRatingArtifact::GetPointOfInterest() const
{
   return m_POI;
}

void xbrShearRatingArtifact::SetRatingType(pgsTypes::LoadRatingType ratingType)
{
   m_RatingType = ratingType;
}

pgsTypes::LoadRatingType xbrShearRatingArtifact::GetLoadRatingType() const
{
   return m_RatingType;
}

void xbrShearRatingArtifact::SetVehicleIndex(VehicleIndexType vehicleIdx)
{
   m_VehicleIndex = vehicleIdx;
}

VehicleIndexType xbrShearRatingArtifact::GetVehicleIndex() const
{
   return m_VehicleIndex;
}

void xbrShearRatingArtifact::SetVehicleWeight(Float64 W)
{
   m_VehicleWeight = W;
}

Float64 xbrShearRatingArtifact::GetVehicleWeight() const
{
   return m_VehicleWeight;
}

void xbrShearRatingArtifact::SetVehicleName(LPCTSTR str)
{
   m_strVehicleName = str;
}

std::_tstring xbrShearRatingArtifact::GetVehicleName() const
{
   return m_strVehicleName;
}

void xbrShearRatingArtifact::SetSystemFactor(Float64 systemFactor)
{
   m_SystemFactor = systemFactor;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetSystemFactor() const
{
   return m_SystemFactor;
}

void xbrShearRatingArtifact::SetConditionFactor(Float64 conditionFactor)
{
   m_ConditionFactor = conditionFactor;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetConditionFactor() const
{
   return m_ConditionFactor;
}

void xbrShearRatingArtifact::SetCapacityReductionFactor(Float64 phi)
{
   m_CapacityRedutionFactor = phi;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetCapacityReductionFactor() const
{
   return m_CapacityRedutionFactor;
}

void xbrShearRatingArtifact::SetNominalShearCapacity(Float64 Vn)
{
   m_Vn = Vn;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetNominalShearCapacity() const
{
   return m_Vn;
}

void xbrShearRatingArtifact::SetDeadLoadFactor(Float64 gDC)
{
   m_gDC = gDC;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetDeadLoadFactor() const
{
   return m_gDC;
}

void xbrShearRatingArtifact::SetDeadLoadShear(Float64 Vdc)
{
   m_Vdc = Vdc;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetDeadLoadShear() const
{
   return m_Vdc;
}

void xbrShearRatingArtifact::SetWearingSurfaceFactor(Float64 gDW)
{
   m_gDW = gDW;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetWearingSurfaceFactor() const
{
   return m_gDW;
}

void xbrShearRatingArtifact::SetWearingSurfaceShear(Float64 Vdw)
{
   m_Vdw = Vdw;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetWearingSurfaceShear() const
{
   return m_Vdw;
}

void xbrShearRatingArtifact::SetLiveLoadFactor(Float64 gLL)
{
   m_gLL = gLL;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetLiveLoadFactor() const
{
   return m_gLL;
}

void xbrShearRatingArtifact::SetLiveLoadShear(Float64 Vllim)
{
   m_Vllim = Vllim;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetLiveLoadShear() const
{
   return m_Vllim;
}

void xbrShearRatingArtifact::SetAdjacentLaneLiveLoadShear(Float64 Vllim)
{
   m_AdjVllim = Vllim;
   m_bRFComputed = false;
}

Float64 xbrShearRatingArtifact::GetAdjacentLaneLiveLoadShear() const
{
   return m_AdjVllim;
}

Float64 xbrShearRatingArtifact::GetRatingFactor() const
{
   if ( m_bRFComputed )
   {
      return m_RF;
   }


   if ( IsZero(m_Vllim) || IsZero(m_gLL) )
   {
      m_RF = DBL_MAX;
   }
   else
   {
      Float64 p = m_SystemFactor * m_ConditionFactor;
      if ( p < 0.85 )
      {
         p = 0.85; // 6A.4.2.1-3)
      }

      Float64 C = p * m_CapacityRedutionFactor * m_Vn;
      Float64 RFtop = C - m_gDC*m_Vdc - m_gDW*m_Vdw;
      if ( IsPermitRatingType(m_RatingType) )
      {
         RFtop -= m_gLL*m_AdjVllim; // BDM Eqn. 13.1.1A-2
      }
      Float64 RFbot = m_gLL*m_Vllim;

      if ( RFtop < 0 )
      {
         // There isn't any capacity remaining for live load
         m_RF = 0;
      }
      else if ( ::BinarySign(RFtop) != ::BinarySign(RFbot) && !IsZero(RFtop) )
      {
         // (fr - DL) and LL have opposite signs
         // this case probably shouldn't happen, but if does,
         // the rating is great
         m_RF = DBL_MAX;
      }
      else
      {
         m_RF = RFtop/RFbot;
      }
   }

   m_bRFComputed = true;
   return m_RF;
}

void xbrShearRatingArtifact::MakeCopy(const xbrShearRatingArtifact& rOther)
{
   m_POI                        = rOther.m_POI;
   m_RatingType                 = rOther.m_RatingType;
   m_VehicleIndex               = rOther.m_VehicleIndex;
   m_VehicleWeight              = rOther.m_VehicleWeight;
   m_strVehicleName             = rOther.m_strVehicleName;
   m_bRFComputed                = rOther.m_bRFComputed;
   m_RF                         = rOther.m_RF;
   m_SystemFactor               = rOther.m_SystemFactor;
   m_ConditionFactor            = rOther.m_ConditionFactor;
   m_CapacityRedutionFactor     = rOther.m_CapacityRedutionFactor;
   m_Vn                         = rOther.m_Vn;
   m_gDC                        = rOther.m_gDC;
   m_gDW                        = rOther.m_gDW;
   m_gLL                        = rOther.m_gLL;
   m_Vdc                        = rOther.m_Vdc;
   m_Vdw                        = rOther.m_Vdw;
   m_Vllim                      = rOther.m_Vllim;
   m_AdjVllim                   = rOther.m_AdjVllim;
}

void xbrShearRatingArtifact::MakeAssignment(const xbrShearRatingArtifact& rOther)
{
   MakeCopy( rOther );
}
