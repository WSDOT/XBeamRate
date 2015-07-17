#pragma once


// CLoadRatingOptionsPage dialog

class CLoadRatingOptionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLoadRatingOptionsPage)

public:
	CLoadRatingOptionsPage();
	virtual ~CLoadRatingOptionsPage();

// Dialog Data
	enum { IDD = IDD_RATING_OPTIONS };
   virtual BOOL OnInitDialog();

   pgsTypes::AnalysisType m_AnalysisType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   void FillAnalysisModeComboBox();

	DECLARE_MESSAGE_MAP()
};
