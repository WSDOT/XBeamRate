#pragma once

#include "PierLayoutPage.h"
#include "txnEditPier.h"

// CPierDlg

class CPierDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CPierDlg)

public:
	CPierDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPierDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPierDlg();

   void SetPierData(const txnEditPierData& pierData);
   const txnEditPierData& GetPierData() const;

protected:
	DECLARE_MESSAGE_MAP()

   void Init();

   txnEditPierData m_PierData;

public:
   friend CPierLayoutPage;

   CPierLayoutPage m_LayoutPage;
};


