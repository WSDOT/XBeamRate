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
#include "txnEditProject.h"

#include <IFace\Project.h>

txnEditProject::txnEditProject(IBroker* pBroker,LPCTSTR strOldProjectName,LPCTSTR strNewProjectName) :
m_pBroker(pBroker)
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
   //GET_IFACE(IEvents, pEvents);
   //pEvents->HoldEvents(); // don't fire any changed events until all changes are done

   GET_IFACE(IProject,pProject);
   pProject->SetProjectName(m_ProjectName[i]);

   //pEvents->FirePendingEvents();
}

txnTransaction* txnEditProject::CreateClone() const
{
   return new txnEditProject(m_pBroker,m_ProjectName[0],m_ProjectName[1]);
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
