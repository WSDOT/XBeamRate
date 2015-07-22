#pragma once

#include <XBeamRateExt\XBRExtExp.h>

class XBREXTCLASS xbrBearingLineData
{
public:
   xbrBearingLineData();
   xbrBearingLineData(const xbrBearingLineData& rOther);
   virtual ~xbrBearingLineData();

   xbrBearingLineData& operator = (const xbrBearingLineData& rOther);

   void SetBearingLineOffset(Float64 offset);
   Float64 GetBearingLineOffset() const;
   Float64& GetBearingLineOffset();

   void SetReferenceBearing(pgsTypes::OffsetMeasurementType datum,IndexType refBrgIdx,Float64 refBrgOffset);
   void GetReferenceBearing(pgsTypes::OffsetMeasurementType* pdatum,IndexType* prefBrgIdx,Float64* prefBrgOffset) const;
   pgsTypes::OffsetMeasurementType& GetRefBearingDatum();
   IndexType& GetRefBearingIndex();
   Float64& GetRefBearingOffset();

   void SetSpacing(const std::vector<Float64>& vSpacing);
   const std::vector<Float64>& GetSpacing() const;

   void SetSpacing(IndexType spaceIdx,Float64 s);
   Float64 GetSpacing(IndexType spaceIdx) const;

   void SetBearingCount(IndexType nBearings);
   IndexType GetBearingCount() const;

   HRESULT Load(IStructuredLoad* pStrLoad);
	HRESULT Save(IStructuredSave* pStrSave);

protected:
   void MakeCopy(const xbrBearingLineData& rOther);
   virtual void MakeAssignment(const xbrBearingLineData& rOther);

protected:
   // Offset from CL Pier to the bearing line, measure normal to the pier
   Float64 m_BrgLineOffset;

   // dimensions are measured in the plane of the pier, along the CL pier
   pgsTypes::OffsetMeasurementType m_RefBearingDatum;
   IndexType m_RefBearingIndex;
   Float64 m_RefBearingOffset; // offset from the datum point ot the reference bearing
   std::vector<Float64> m_vSpacing; // spacing between bearings
};
