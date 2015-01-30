// XBeamRateDoc.h : interface of the CXBeamRateDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_)
#define AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <EAF\EAFBrokerDocument.h>

class CXBeamRateDocProxyAgent;

class CXBeamRateDoc : public CEAFBrokerDocument
{
protected: // create from serialization only
	CXBeamRateDoc();
	DECLARE_DYNCREATE(CXBeamRateDoc)

   // CEBrokerDocument over-rides
   virtual BOOL LoadSpecialAgents(IBrokerInitEx2* pBrokerInit);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXBeamRateDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

   virtual HRESULT WriteTheDocument(IStructuredSave* pStrSave);
   virtual HRESULT LoadTheDocument(IStructuredLoad* pStrLoad);
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

// Generated message map functions
protected:
	//{{AFX_MSG(CXBeamRateDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CXBeamRateDocProxyAgent* m_pMyDocProxyAgent;
   virtual void BrokerShutDown();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBeamRateDoc_H__A5D5FE52_139D_46C7_84CF_35817D298084__INCLUDED_)
