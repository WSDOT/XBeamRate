#include "stdafx.h"
#include <XBeamRateExt\ColumnData.h>

xbrColumnData::xbrColumnData()
{
   m_TransverseFixity = pgsTypes::cftFixed;
}

xbrColumnData::xbrColumnData(const xbrColumnData& rOther) :
CColumnData(rOther)
{
   MakeCopy(rOther);
}

xbrColumnData::~xbrColumnData()
{
}

xbrColumnData& xbrColumnData::operator=(const xbrColumnData& rOther)
{
   MakeAssignment(rOther);
   return *this;
}

xbrColumnData& xbrColumnData::operator=(const CColumnData& rOther)
{
   CColumnData::MakeAssignment(rOther);
   return *this;
}

bool xbrColumnData::operator == (const xbrColumnData& rOther) const
{
   return (CColumnData::operator==(rOther) && m_TransverseFixity == rOther.m_TransverseFixity) ? true : false;
}

bool xbrColumnData::operator != (const xbrColumnData& rOther) const
{
   return !operator==(rOther);
}

void xbrColumnData::SetTransverseFixity(pgsTypes::ColumnFixityType columnFixity)
{
   m_TransverseFixity = columnFixity;
}

pgsTypes::ColumnFixityType xbrColumnData::GetTransverseFixity() const
{
   return m_TransverseFixity;
}

HRESULT xbrColumnData::Save(IStructuredSave* pStrSave,IProgress* pProgress)
{
   HRESULT hr = S_OK;

   pStrSave->BeginUnit(_T("Column"),1.0);

   CColumnData::Save(pStrSave,pProgress);
   pStrSave->put_Property(_T("TransverseFixity"),CComVariant(m_TransverseFixity));

   pStrSave->EndUnit(); // Column

   return hr;
}

HRESULT xbrColumnData::Load(IStructuredLoad* pStrLoad,IProgress* pProgress)
{
   CHRException hr;

   try
   {
      CComVariant var;
      hr = pStrLoad->BeginUnit(_T("Column"));

      hr = CColumnData::Load(pStrLoad,pProgress);

      var.vt = VT_I4;
      hr = pStrLoad->get_Property(_T("TransverseFixity"),&var);
      m_TransverseFixity = (pgsTypes::ColumnFixityType)var.lVal;

      hr = pStrLoad->EndUnit(); // Column
   }
   catch (HRESULT)
   {
      ATLASSERT(false);
      THROW_LOAD(InvalidFileFormat,pStrLoad);
   }

   return S_OK;
}

void xbrColumnData::MakeCopy(const xbrColumnData& rOther)
{
   m_TransverseFixity = rOther.m_TransverseFixity;
}

void xbrColumnData::MakeAssignment(const xbrColumnData& rOther)
{
   CColumnData::MakeAssignment(rOther);
   MakeCopy(rOther);
}

