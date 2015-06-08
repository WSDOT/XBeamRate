#pragma once

#include <XBeamRateExt\XBRExtExp.h>
#include <MFCTools\RebarMaterialComboBox.h>
#include <PgsExt\ConcreteMaterial.h>
#include <PgsExt\PierData2.h>

interface IEditPierData;
class CLongitudinalRebarGrid;
class CStirrupGrid;
class xbrLongitudinalRebarData;
class xbrStirrupData;

interface IReinforcementPageParent
{
   // Returns data for the pier that is being edited
   // Return NULL if a pier object (part of a bridge object) is not being edited
   virtual CPierData2* GetPierData() = 0;
   virtual CConcreteMaterial& GetConcrete() = 0;
   virtual xbrTypes::SuperstructureConnectionType GetSuperstructureConnectionType() = 0;

   virtual matRebar::Type& GetRebarType() = 0;
   virtual matRebar::Grade& GetRebarGrade() = 0;
   virtual CConcreteMaterial& GetConcreteMaterial() = 0;
   virtual xbrLongitudinalRebarData& GetLongitudinalRebar() = 0;
   virtual xbrStirrupData& GetLowerXBeamStirrups() = 0;
   virtual xbrStirrupData& GetFullDepthStirrups() = 0;
};

// CReinforcementPage dialog

// This property page does double duty... it provides a single UI element for
// both standalone XBeam models and for XBeam models that are part of PGSuper/PGSplice piers
//
// When used with standalone XBeamRate, it is a normal property page. When used with
// PGSuper/PGSplice piers, this property page is an extension to Edit Pier dialog.

class XBREXTCLASS CReinforcementPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CReinforcementPage)

public:
   CReinforcementPage(IReinforcementPageParent* pParent);
	virtual ~CReinforcementPage();

   IReinforcementPageParent* GetPageParent();

// Dialog Data

   void GetRebarMaterial(matRebar::Type* pType,matRebar::Grade* pGrade);

   void OnEnableDelete(UINT nIDC,bool bEnableDelete);

protected:
   IReinforcementPageParent* m_pParent;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CEdit	m_ctrlEc;
	CButton m_ctrlEcCheck;
	CEdit	m_ctrlFc;

	//{{AFX_MSG(CPierLayoutPage)
	virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   afx_msg void OnMoreProperties();
   afx_msg void OnBnClickedEc();
   afx_msg void OnAddRebar();
   afx_msg void OnRemoveRebar();
   afx_msg void OnAddLowerXBeam();
   afx_msg void OnRemoveLowerXBeam();
   afx_msg void OnAddFullDepth();
   afx_msg void OnRemoveFullDepth();
   afx_msg void OnChangeFc();
   afx_msg void OnLowerXBeamSymmetry();
   afx_msg void OnFullDepthSymmetry();
   afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CRebarMaterialComboBox m_cbRebar;
   CString m_strUserEc;

   CLongitudinalRebarGrid* m_pRebarGrid;

   CStirrupGrid* m_pLowerXBeamGrid;
   CStirrupGrid* m_pFullDepthGrid;

   void UpdateConcreteTypeLabel();
   void UpdateEc();
   void UpdateStirrupGrids();
};
