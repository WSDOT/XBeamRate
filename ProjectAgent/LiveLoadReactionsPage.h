#pragma once

#include "LiveLoadReactionGrid.h"

// CLiveLoadReactionsPage dialog

class CLiveLoadReactionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLiveLoadReactionsPage)

public:
	CLiveLoadReactionsPage();
	virtual ~CLiveLoadReactionsPage();

// Dialog Data
	enum { IDD = IDD_LIVE_LOAD_REACTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   afx_msg void OnDesignAdd();
   afx_msg void OnDesignRemove();

   afx_msg void OnLegalRoutineAdd();
   afx_msg void OnLegalRoutineRemove();

   afx_msg void OnLegalSpecialAdd();
   afx_msg void OnLegalSpecialRemove();

   afx_msg void OnPermitRoutineAdd();
   afx_msg void OnPermitRoutineRemove();

   afx_msg void OnPermitSpecialAdd();
   afx_msg void OnPermitSpecialRemove();

	DECLARE_MESSAGE_MAP()

   CLiveLoadReactionGrid m_DesignGrid;
   CLiveLoadReactionGrid m_LegalRoutineGrid;
   CLiveLoadReactionGrid m_LegalSpecialGrid;
   CLiveLoadReactionGrid m_PermitRoutineGrid;
   CLiveLoadReactionGrid m_PermitSpecialGrid;

public:
   virtual BOOL OnInitDialog();
};
