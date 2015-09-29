#pragma once

class CAgentCmdTarget : public CCmdTarget
{
public:
   CAgentCmdTarget();
   void Init(IBroker* pBroker);

   void OnEditProjectName();

private:
   IBroker* m_pBroker; // weak reference

   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnEditPier();
   afx_msg void OnEditOptions();
};