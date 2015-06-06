
#pragma once

#include <WBFLCore.h>

class txnEditProject :
   public txnTransaction
{
public:
   txnEditProject(LPCTSTR strOldProjectName,LPCTSTR strNewProjectName);
   ~txnEditProject(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

	CString m_ProjectName[2];
};
