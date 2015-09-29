#pragma once


// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

   xbrTypes::PermitRatingMethod m_PermitRatingMethod;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   void FillPermitFactorList();

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
