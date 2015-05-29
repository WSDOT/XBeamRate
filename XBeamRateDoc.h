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
   virtual bool IsAutoCalcEnabled() const;
   virtual void EnableAutoCalc(bool bEnable);

// CEBrokerDocument over-rides
public:
   virtual BOOL Init();
   virtual BOOL LoadSpecialAgents(IBrokerInitEx2* pBrokerInit);

// Attributes
public:
   virtual CString GetRootNodeName();
   virtual Float64 GetRootNodeVersion();

// Operations
public:
   void GetDocUnitSystem(IDocUnitSystem** ppDocUnitSystem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL	//}}AFX_VIRTUAL

   virtual void LoadToolbarState();
   virtual void SaveToolbarState();
   virtual CString GetToolbarSectionName();
   virtual BOOL GetStatusBarMessageString(UINT nID,CString& rMessage) const;
   virtual BOOL GetToolTipMessageString(UINT nID, CString& rMessage) const;

// Implementation
public:
	virtual ~CXBeamRateDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   virtual CATID GetAgentCategoryID();
   virtual HRESULT LoadTheDocument(IStructuredLoad* pStrLoad);
   virtual HRESULT WriteTheDocument(IStructuredSave* pStrSave);
   virtual void CreateReportView(CollectionIndexType rptIdx,bool bPrompt);
   virtual void CreateGraphView(CollectionIndexType graphIdx);

// Generated message map functions
protected:
	//{{AFX_MSG(CXBeamRateDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


   friend CXBeamRateDocProxyAgent;
   CXBeamRateDocProxyAgent* m_pMyDocProxyAgent;

   CComPtr<IDocUnitSystem> m_DocUnitSystem;

   bool m_bAutoCalcEnabled;

   virtual void OnCreateFinalize();
   virtual void BrokerShutDown();

   void PopulateReportMenu();
   void PopulateGraphMenu();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_)
