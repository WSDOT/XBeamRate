#pragma once


// CReinforcementPage dialog

class CReinforcementPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CReinforcementPage)

public:
	CReinforcementPage();
	virtual ~CReinforcementPage();

// Dialog Data
	enum { IDD = IDD_REINFORCEMENT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
