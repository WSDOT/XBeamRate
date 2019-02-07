///////////////////////////////////////////////////////////////////////
// XBeamRate - Cross Beam Load Rating
// Copyright © 1999-2019  Washington State Department of Transportation
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

// XBeamRateDoc.h : interface of the CXBeamRateDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_)
#define AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <EAF\EAFBrokerDocument.h>
#include <EAF\EAFAutoCalcDoc.h>
#include <WBFLUnitServer.h>

class CXBeamRateDocProxyAgent;

class CXBeamRateDoc : public CEAFBrokerDocument, public CEAFAutoCalcDocMixin
{
protected: // create from serialization only
	CXBeamRateDoc();
	DECLARE_DYNCREATE(CXBeamRateDoc)

// CEAFAutoCalcDocMixin over-rides
public:
   virtual bool IsAutoCalcEnabled() const override;
   virtual void EnableAutoCalc(bool bEnable) override;

// CEBrokerDocument over-rides
public:
   virtual BOOL Init() override;
   virtual BOOL LoadSpecialAgents(IBrokerInitEx2* pBrokerInit) override;

   virtual void OnChangedFavoriteReports(BOOL bIsFavorites, BOOL bFromMenu) override;
   virtual void ShowCustomReportHelp(eafTypes::CustomReportHelp helpType) override;
   virtual void ShowCustomReportDefinitionHelp() override;

// Attributes
public:
   virtual CString GetRootNodeName() override;
   virtual Float64 GetRootNodeVersion() override;

// Operations
public:
   void GetDocUnitSystem(IDocUnitSystem** ppDocUnitSystem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateDoc)
	public:
	virtual BOOL OnNewDocument() override;
   virtual void OnCloseDocument() override;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL	//}}AFX_VIRTUAL

   virtual void LoadToolbarState() override;
   virtual void SaveToolbarState() override;
   virtual CString GetToolbarSectionName() override;
   virtual BOOL GetStatusBarMessageString(UINT nID,CString& rMessage) const override;
   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const override;

// Implementation
public:
	virtual ~CXBeamRateDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:
   virtual CATID GetAgentCategoryID() override;
   virtual HINSTANCE GetResourceInstance() override;
   virtual HRESULT LoadTheDocument(IStructuredLoad* pStrLoad) override;
   virtual HRESULT WriteTheDocument(IStructuredSave* pStrSave) override;
   virtual void CreateReportView(CollectionIndexType rptIdx,BOOL bPrompt) override;
   virtual void CreateGraphView(CollectionIndexType graphIdx) override;

   virtual void LoadDocumentSettings() override;
   virtual void SaveDocumentSettings() override;

   virtual CString GetDocumentationSetName() override;
   virtual CString GetDocumentationRootLocation() override;

// Generated message map functions
protected:
	//{{AFX_MSG(CXBeamRateDoc)
   afx_msg void OnAbout();
	//}}AFX_MSG
   afx_msg void OnUpdateViewGraphs(CCmdUI* pCmdUI);
   afx_msg BOOL OnViewGraphs(NMHDR* pnmhdr,LRESULT* plr);
   afx_msg void OnUpdateViewReports(CCmdUI* pCmdUI);
   afx_msg BOOL OnViewReports(NMHDR* pnmhdr,LRESULT* plr);
   afx_msg void OnViewPier();
   afx_msg void OnAutoCalc();
   afx_msg void OnUpdateAutoCalc(CCmdUI* pCmdUI);
   afx_msg void OnUpdateNow();
	afx_msg void OnUpdateUpdateNow(CCmdUI* pCmdUI);
   afx_msg void OnHelpFinder();
	DECLARE_MESSAGE_MAP()

   // callback IDs for any status callbacks we register
   StatusCallbackIDType m_scidInformationalError;
   StatusGroupIDType m_StatusGroupID;

   friend CXBeamRateDocProxyAgent;
   CXBeamRateDocProxyAgent* m_pMyDocProxyAgent;

   CComPtr<IDocUnitSystem> m_DocUnitSystem;

   bool m_bAutoCalcEnabled;

   virtual BOOL OpenTheDocument(LPCTSTR lpszPathName) override;
   virtual void OnCreateFinalize() override;
   virtual void BrokerShutDown() override;

   // called when the UI Hints have been reset
   virtual void ResetUIHints() override;

   void PopulateReportMenu();
   void PopulateGraphMenu();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_)
