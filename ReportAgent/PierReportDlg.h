#pragma once

#include "resource.h"

class CPierReportDlg : public CDialog
{
	DECLARE_DYNAMIC(CPierReportDlg)

public:
	CPierReportDlg(IBroker* pBroker,const CReportDescription& rptDesc,boost::shared_ptr<CReportSpecification>& pRptSpec,CWnd* pParent = NULL);
	virtual ~CPierReportDlg();

// Dialog Data
	enum { IDD = IDD_PIERREPORT };

   PierIDType m_PierID;
   std::vector<std::_tstring> m_ChapterList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   virtual void UpdateChapterList();

   virtual void ClearChapterCheckMarks();
   virtual void InitChapterListFromSpec();
   virtual void InitFromRptSpec();

   CCheckListBox	m_ChList;

   const CReportDescription& m_RptDesc;
   IBroker* m_pBroker;

   boost::shared_ptr<CReportSpecification> m_pInitRptSpec; // report spec for initializing the dialog

public:
	// Generated message map functions
	//{{AFX_MSG(CPierReportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
