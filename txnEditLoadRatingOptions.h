
#pragma once

class txnEditLoadRatingOptions :
   public txnTransaction
{
public:
   txnEditLoadRatingOptions(pgsTypes::AnalysisType oldAnalysisType,pgsTypes::AnalysisType newAnalysisType);
   ~txnEditLoadRatingOptions(void);

   virtual bool Execute();
   virtual void Undo();
   virtual txnTransaction* CreateClone() const;
   virtual std::_tstring Name() const;
   virtual bool IsUndoable();
   virtual bool IsRepeatable();

private:
   void Execute(int i);

   pgsTypes::AnalysisType m_AnalysisType[2];
};
