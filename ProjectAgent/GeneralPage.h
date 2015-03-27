#pragma once


// CGeneralPage dialog

class CGeneralPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGeneralPage)

public:
	CGeneralPage();
	virtual ~CGeneralPage();

// Dialog Data
	enum { IDD = IDD_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   afx_msg void OnPierTypeChanged();

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();

private:
   void FillPierTypeComboBox();
   void FillTransverseMeasureComboBox();
};
