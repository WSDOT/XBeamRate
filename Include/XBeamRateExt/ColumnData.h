
#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <PgsExt\ColumnData.h>


/*****************************************************************************
CLASS 
   xbrColumnData

   Extends CColumnData by adding transverse column fixity
*****************************************************************************/

class XBREXTCLASS xbrColumnData : public CColumnData
{
public:
   xbrColumnData();
   xbrColumnData(const xbrColumnData& rOther);
   ~xbrColumnData();

   xbrColumnData& operator=(const xbrColumnData& rOther);
   xbrColumnData& operator=(const CColumnData& rOther);
   bool operator == (const xbrColumnData& rOther) const;
   bool operator != (const xbrColumnData& rOther) const;

   void SetTransverseFixity(pgsTypes::ColumnFixityType columnFixity);
   pgsTypes::ColumnFixityType GetTransverseFixity() const;

	HRESULT Load(IStructuredLoad* pStrLoad,IProgress* pProgress);
	HRESULT Save(IStructuredSave* pStrSave,IProgress* pProgress);

protected:
   void MakeCopy(const xbrColumnData& rOther);
   virtual void MakeAssignment(const xbrColumnData& rOther);

   pgsTypes::ColumnFixityType m_TransverseFixity; // fixity used for pier analysis
};
