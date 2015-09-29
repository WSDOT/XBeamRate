

#include "stdafx.h"
#include "txnEditLoadRatingOptions.h"

#include <IFace\Project.h>
#include <IFace\RatingSpecification.h>

txnEditLoadRatingOptions::txnEditLoadRatingOptions(pgsTypes::AnalysisType oldAnalysisType,pgsTypes::AnalysisType newAnalysisType,
                            xbrTypes::PermitRatingMethod oldMethod,xbrTypes::PermitRatingMethod newMethod)
{
   m_AnalysisType[0] = oldAnalysisType;
   m_AnalysisType[1] = newAnalysisType;

   m_PermitRatingMethod[0] = oldMethod;
   m_PermitRatingMethod[1] = newMethod;
}

txnEditLoadRatingOptions::~txnEditLoadRatingOptions(void)
{
}

bool txnEditLoadRatingOptions::Execute()
{
   Execute(1);
   return true;
}

void txnEditLoadRatingOptions::Undo()
{
   Execute(0);
}

void txnEditLoadRatingOptions::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE2(pBroker,IXBRRatingSpecification,pSpec);
   pSpec->SetAnalysisMethodForReactions(m_AnalysisType[i]);
   pSpec->SetPermitRatingMethod(m_PermitRatingMethod[i]);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditLoadRatingOptions::CreateClone() const
{
   return new txnEditLoadRatingOptions(m_AnalysisType[0],m_AnalysisType[1],m_PermitRatingMethod[0],m_PermitRatingMethod[1]);
}

std::_tstring txnEditLoadRatingOptions::Name() const
{
   return _T("Edit Cross Beam Load Rating Options");
}

bool txnEditLoadRatingOptions::IsUndoable()
{
   return true;
}

bool txnEditLoadRatingOptions::IsRepeatable()
{
   return false;
}
