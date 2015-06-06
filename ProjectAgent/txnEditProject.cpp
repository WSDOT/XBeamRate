

#include "stdafx.h"
#include "txnEditProject.h"

#include <IFace\Project.h>

txnEditProject::txnEditProject(LPCTSTR strOldProjectName,LPCTSTR strNewProjectName)
{
   m_ProjectName[0] = strOldProjectName;
   m_ProjectName[1] = strNewProjectName;
}

txnEditProject::~txnEditProject(void)
{
}

bool txnEditProject::Execute()
{
   Execute(1);
   return true;
}

void txnEditProject::Undo()
{
   Execute(0);
}

void txnEditProject::Execute(int i)
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBREvents, pEvents);
   pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   //GET_IFACE2(pBroker,IXBRProject,pProject);
   //pProject->SetProjectName(m_ProjectName[i]);

   pEvents->FirePendingEvents();
}

txnTransaction* txnEditProject::CreateClone() const
{
   return new txnEditProject(m_ProjectName[0],m_ProjectName[1]);
}

std::_tstring txnEditProject::Name() const
{
   return _T("Edit Project Name");
}

bool txnEditProject::IsUndoable()
{
   return true;
}

bool txnEditProject::IsRepeatable()
{
   return false;
}
