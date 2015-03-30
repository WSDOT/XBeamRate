#pragma once


// CSuperstructureLayoutPage dialog

class CSuperstructureLayoutPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSuperstructureLayoutPage)

public:
	CSuperstructureLayoutPage();
	virtual ~CSuperstructureLayoutPage();

// Dialog Data
	enum { IDD = IDD_SUPERSTRUCTURE_LAYOUT_PAGE };

protected:
   CMetaFileStatic m_Dimensions;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   afx_msg void OnCurbLineDatumChanged();
   afx_msg void OnPierTypeChanged();

   CString GetImageName();

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();

private:
   void UpdateImage();
   void FillCurbLineMeasureComboBox();
   void FillPierTypeComboBox();
};
