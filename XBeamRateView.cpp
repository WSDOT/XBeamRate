// XBeamRateView.cpp : implementation of the CXBeamRateView class
//

#include "stdafx.h"

#include "XBeamRateDoc.h"
#include "XBeamRateView.h"
#include "XBeamRateChildFrame.h"
#include "SectionCut.h"
#include "DisplayObjectFactory.h"

#include <IFace\XBeamRateAgent.h>
#include <IFace\Project.h>

#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\Pier.h>
#include <IFace\EditByUI.h>
#include <\ARP\PGSuper\Include\IFace\Project.h>
#include <\ARP\PGSuper\Include\IFace\Bridge.h>
#include <\ARP\PGSuper\Include\IFace\PointOfInterest.h>
#include <\ARP\PGSuper\Include\IFace\Intervals.h>
#include <MFCTools\Format.h>

#include <EAF\EAFDisplayUnits.h>
#include <MFCTools\WsdotCalculationSheet.h>

#include <PgsExt\BridgeDescription2.h>

#include <WBFLGenericBridge.h>

#include <PGSuperColors.h>
#define COLUMN_LINE_COLOR              GREY50
#define COLUMN_FILL_COLOR              GREY70
#define XBEAM_LINE_COLOR               GREY50
#define XBEAM_FILL_COLOR               GREY70

#define COLUMN_LINE_WEIGHT             1
//#define XBEAM_LINE_WEIGHT              3
#define REBAR_LINE_WEIGHT              1

#define ROADWAY_DISPLAY_LIST_ID        0
#define GIRDER_DISPLAY_LIST_ID         1
#define BEARING_DISPLAY_LIST_ID        2
#define XBEAM_DISPLAY_LIST_ID          3
#define COLUMN_DISPLAY_LIST_ID         4
#define DIMENSIONS_DISPLAY_LIST_ID     5
#define REBAR_DISPLAY_LIST_ID          6
#define STIRRUP_DISPLAY_LIST_ID        7
#define SECTION_CUT_DISPLAY_LIST_ID    8


#define SECTION_CUT_ID                500

// The End/Section view of the pier is offset from the Elevation
// view by this amount.
const Float64 EndOffset = ::ConvertToSysUnits(10,unitMeasure::Feet);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView

IMPLEMENT_DYNCREATE(CXBeamRateView, CDisplayView)

BEGIN_MESSAGE_MAP(CXBeamRateView, CDisplayView)
	//{{AFX_MSG_MAP(CXBeamRateView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView construction/destruction

CXBeamRateView::CXBeamRateView()
{
	// TODO: add construction code here
   m_DisplayObjectID = 0;
}

CXBeamRateView::~CXBeamRateView()
{
}

BOOL CXBeamRateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDisplayView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView printing
void CXBeamRateView::OnDraw(CDC* pDC)
{
   if ( m_bIsIdealized )
   {
      pDC->TextOut(0,0,_T("Pier is idealized"));
   }
   else
   {
      CDisplayView::OnDraw(pDC);
   }
}

BOOL CXBeamRateView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CXBeamRateView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
   // get paper size
   WsdotCalculationSheet border;

   CDocument* pdoc = GetDocument();
   CString path = pdoc->GetPathName();
   border.SetFileName(path);
   CRect rcPrint = border.Print(pDC, 1);

   // want to offset picture away from borders - get device units for 10mm
   int oldmode = pDC->SetMapMode(MM_LOMETRIC);
   POINT offset[2] = { {0,0}, {100,-100}};
   pDC->LPtoDP(offset,2);
   int offsetx = offset[1].x - offset[0].x;
   int offsety = offset[1].y - offset[0].y;
   rcPrint.DeflateRect(offsetx,offsety);
   pDC->SetMapMode(oldmode);

   if (rcPrint.IsRectEmpty())
   {
      CHECKX(0,_T("Can't print border - page too small?"));
      rcPrint = pInfo->m_rectDraw;
   }

   CDisplayView::OnBeginPrinting(pDC, pInfo, rcPrint);
   OnPrepareDC(pDC);
   ScaleToFit();
   OnDraw(pDC);
   OnEndPrinting(pDC, pInfo);
}

void CXBeamRateView::OnSize(UINT nType, int cx, int cy) 
{
	CDisplayView::OnSize(nType, cx, cy);

   CRect rect;
   GetClientRect(&rect);
   rect.DeflateRect(15,15,15,15);

   CSize size = rect.Size();
   size.cx = Max(0L,size.cx);
   size.cy = Max(0L,size.cy);

   SetLogicalViewRect(MM_TEXT,rect);

   SetScrollSizes(MM_TEXT,size,CScrollView::sizeDefault,CScrollView::sizeDefault);

   ScaleToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView diagnostics

#ifdef _DEBUG
void CXBeamRateView::AssertValid() const
{
 //  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CDisplayView::AssertValid();
}

void CXBeamRateView::Dump(CDumpContext& dc) const
{
	CDisplayView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView message handlers
int CXBeamRateView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if ( CDisplayView::OnCreate(lpCreateStruct) == -1 )
   {
      return -1;
   }

   EnableToolTips();

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDisplayObjectFactory* factory = new CDisplayObjectFactory();
   CComPtr<iDisplayObjectFactory> doFactory;
   doFactory.Attach((iDisplayObjectFactory*)factory->GetInterface(&IID_iDisplayObjectFactory));
   dispMgr->AddDisplayObjectFactory(doFactory);

   dispMgr->EnableLBtnSelect(TRUE);
   dispMgr->EnableRBtnSelect(TRUE);
   dispMgr->SetSelectionLineColor(SELECTED_OBJECT_LINE_COLOR);
   dispMgr->SetSelectionFillColor(SELECTED_OBJECT_FILL_COLOR);

   CDisplayView::SetMappingMode(DManip::Isotropic);

   // Setup display lists
   CComPtr<iDisplayList> displayList;
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(ROADWAY_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(SECTION_CUT_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(DIMENSIONS_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(REBAR_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(STIRRUP_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(BEARING_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(GIRDER_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(XBEAM_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(COLUMN_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   m_pFrame = (CXBeamRateChildFrame*)GetParent();
   ASSERT( m_pFrame != 0 );
   ASSERT( m_pFrame->IsKindOf( RUNTIME_CLASS( CXBeamRateChildFrame ) ) );

   return 0;
}

void CXBeamRateView::OnInitialUpdate()
{
   CDisplayView::OnInitialUpdate();
}

PierIDType CXBeamRateView::GetPierID()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   CXBeamRateChildFrame* pFrame = (CXBeamRateChildFrame*)GetParentFrame();
   return pFrame->GetPierID();
}

PierIndexType CXBeamRateView::GetPierIndex()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   CXBeamRateChildFrame* pFrame = (CXBeamRateChildFrame*)GetParentFrame();
   return pFrame->GetPierIndex();
}

xbrPointOfInterest CXBeamRateView::GetCutLocation()
{
   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CComPtr<iDisplayList> pDL;
   dispMgr->FindDisplayList(SECTION_CUT_DISPLAY_LIST_ID,&pDL);
   ATLASSERT(pDL);

   CComPtr<iDisplayObject> dispObj;
   pDL->FindDisplayObject(SECTION_CUT_ID,&dispObj);

   if ( dispObj == NULL )
   {
      return xbrPointOfInterest();
   }

   CComPtr<iDisplayObjectEvents> sink;
   dispObj->GetEventSink(&sink);

   CComQIPtr<iPointDisplayObject,&IID_iPointDisplayObject> point_disp(dispObj);
   CComQIPtr<iSectionCutDrawStrategy,&IID_iSectionCutDrawStrategy> sectionCutStrategy(sink);

   return sectionCutStrategy->GetCutPOI(m_pFrame->GetCurrentCutLocation());
}

void CXBeamRateView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
   m_pFrame->UpdateSectionCutExtents();

   CDisplayView::OnUpdate(pSender,lHint,pHint);

   CDManipClientDC dc2(this);
   UpdateDisplayObjects();
   ScaleToFit();
   Invalidate();
}

void CXBeamRateView::UpdateDisplayObjects()
{
   CWaitCursor wait;

   m_bIsIdealized = false;
   PierIndexType pierIdx = GetPierIndex();
   if ( pierIdx != INVALID_INDEX )
   {
      CComPtr<IBroker> pBroker;
      EAFGetBroker(&pBroker);
      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      const CPierData2* pPier = pIBridgeDesc->GetPier(pierIdx);
      if ( pPier->GetPierModelType() == pgsTypes::pmtIdealized )
      {
         m_bIsIdealized = true;
         return;
      }
   }


   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   // Capture the current selection before blasting all the 
   // display objects
   DisplayObjectContainer vCurSel;
   dispMgr->GetSelectedObjects(&vCurSel);
   ATLASSERT(vCurSel.size() < 2);
   IDType curSel = INVALID_ID;
   IDType listID = INVALID_ID;
   if ( vCurSel.size() == 1 )
   {
      CComPtr<iDisplayObject> pDO = vCurSel[0];
      curSel = pDO->GetID();
      CComPtr<iDisplayList> pDL;
      pDO->GetDisplayList(&pDL);
      listID = pDL->GetID();
   }

   dispMgr->ClearDisplayObjects();
   m_DisplayObjectID = 0;

   UpdateRoadwayDisplayObjects();
   UpdateXBeamDisplayObjects();
   UpdateColumnDisplayObjects();
   UpdateBearingDisplayObjects();
   UpdateGirderDisplayObjects();
   UpdateRebarDisplayObjects();
   UpdateStirrupDisplayObjects();
   UpdateDimensionsDisplayObjects();
   UpdateSectionCutDisplayObjects();

   // Re-instate the current selection
   if ( curSel != INVALID_ID )
   {
      CComPtr<iDisplayObject> doSel;
      dispMgr->FindDisplayObject(curSel,listID,atByID,&doSel);
      dispMgr->SelectObject(doSel,TRUE);
   }
}

void CXBeamRateView::UpdateRoadwayDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(ROADWAY_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   PierIDType pierID = GetPierID();

   GET_IFACE2(pBroker,IXBRPier,pPier);
   Float64 skew = pPier->GetSkewAngle(pierID);

   GET_IFACE2(pBroker,IXBRProject,pProject);

   // Model a vertical line for the alignment
   // The alignment is at X = 0 in Pier coordinates
   Float64 X = 0;
   Float64 Ydeck = pProject->GetDeckElevation(pierID); // deck elevation at alignment
   Float64 Yt = Ydeck + ::ConvertToSysUnits(1.0,unitMeasure::Feet); // add a little so it projects over the roadway surface
   CComPtr<IPoint2d> pnt1;
   pnt1.CoCreateInstance(CLSID_Point2d);
   pnt1->Move(X,Yt);

   Float64 Yb = Yt - pPier->GetMaxColumnHeight(pierID);
   CComPtr<IPoint2d> pnt2;
   pnt2.CoCreateInstance(CLSID_Point2d);
   pnt2->Move(X,Yb);

   CComPtr<iLineDisplayObject> doAlignment;
   CreateLineDisplayObject(pnt1,pnt2,&doAlignment);
   CComPtr<iDrawLineStrategy> drawStrategy;
   doAlignment->GetDrawLineStrategy(&drawStrategy);
   CComQIPtr<iSimpleDrawLineStrategy> drawAlignmentStrategy(drawStrategy);
   drawAlignmentStrategy->SetWidth(ALIGNMENT_LINE_WEIGHT);
   drawAlignmentStrategy->SetColor(ALIGNMENT_COLOR);
   drawAlignmentStrategy->SetLineStyle(lsCenterline);

   displayList->AddDisplayObject(doAlignment);

   // Draw the bridge line if different then the alignment
   Float64 BLO = pProject->GetBridgeLineOffset(pierID);
   if ( !IsZero(BLO) )
   {
      // Model a vertical line for the bridge line
      // Let X = BLO be at the alignment and Y = the alignment elevation
      Float64 X = BLO;
      pnt1.Release();
      pnt1.CoCreateInstance(CLSID_Point2d);
      pnt1->Move(X,Yt);

      pnt2.Release();
      pnt2.CoCreateInstance(CLSID_Point2d);
      pnt2->Move(X,Yb);

      CComPtr<iLineDisplayObject> doBridgeLine;
      CreateLineDisplayObject(pnt1,pnt2,&doBridgeLine);
      CComPtr<iDrawLineStrategy> drawStrategy;
      doBridgeLine->GetDrawLineStrategy(&drawStrategy);
      CComQIPtr<iSimpleDrawLineStrategy> drawBridgeLineStrategy(drawStrategy);
      drawBridgeLineStrategy->SetWidth(BRIDGELINE_LINE_WEIGHT);
      drawBridgeLineStrategy->SetColor(BRIDGE_COLOR);
      drawBridgeLineStrategy->SetLineStyle(lsCenterline);

      displayList->AddDisplayObject(doBridgeLine);
   }

   // Draw the top of roadway surface between curblines
   Float64 LCO, RCO;
   pProject->GetCurbLineOffset(pierID,&LCO,&RCO);

   Float64 Ylc = pPier->GetElevation(pierID,0);
   Float64 Yrc = pPier->GetElevation(pierID,RCO-LCO);
   CComPtr<IPoint2d> pnt3;
   pnt1.Release();
   pnt1.CoCreateInstance(CLSID_Point2d);
   LCO /= cos(skew); // skew adjust
   pnt1->Move(LCO,Ylc);

   pnt2.Release();
   pnt2.CoCreateInstance(CLSID_Point2d);
   pnt2->Move(0,Ydeck);

   pnt3.CoCreateInstance(CLSID_Point2d);
   RCO /= cos(skew); // skew adjust
   pnt3->Move(RCO,Yrc);

   CComPtr<iPolyLineDisplayObject> doDeck;
   doDeck.CoCreateInstance(CLSID_PolyLineDisplayObject);
   doDeck->SetID(m_DisplayObjectID++);
   doDeck->AddPoint(pnt1);
   doDeck->AddPoint(pnt2);
   doDeck->AddPoint(pnt3);
   doDeck->put_PointType(plpNone);
   doDeck->put_Color(PROFILE_COLOR);
   doDeck->put_Width(PROFILE_LINE_WEIGHT);
   doDeck->Commit();

   displayList->AddDisplayObject(doDeck);
}

void CXBeamRateView::UpdateXBeamDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(XBEAM_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   PierIDType pierID = GetPierID();

   GET_IFACE2(pBroker,IXBRProject,pProject);
   GET_IFACE2(pBroker,IXBRPier,pPier);
   GET_IFACE2(pBroker,IXBRSectionProperties,pSectProp);

   // Model Upper Cross Beam (Elevation)
   CComPtr<IPoint2d> point;
   point.CoCreateInstance(CLSID_Point2d);
   point->Move(0,0);

   if ( pProject->GetPierType(pierID) != xbrTypes::pctExpansion )
   {
      CComPtr<iPointDisplayObject> doUpperXBeam;
      doUpperXBeam.CoCreateInstance(CLSID_PointDisplayObject);
      doUpperXBeam->SetID(m_DisplayObjectID++);
      doUpperXBeam->SetPosition(point,FALSE,FALSE);
      doUpperXBeam->SetSelectionType(stNone);

      CComPtr<IShape> upperXBeamShape;
      pPier->GetUpperXBeamProfile(pierID,&upperXBeamShape);

      CComPtr<iShapeDrawStrategy> upperXBeamDrawStrategy;
      upperXBeamDrawStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
      upperXBeamDrawStrategy->SetShape(upperXBeamShape);
      upperXBeamDrawStrategy->SetSolidLineColor(XBEAM_LINE_COLOR);
      upperXBeamDrawStrategy->SetSolidFillColor(XBEAM_FILL_COLOR);
      upperXBeamDrawStrategy->DoFill(TRUE);

      doUpperXBeam->SetDrawingStrategy(upperXBeamDrawStrategy);

      CComPtr<iShapeGravityWellStrategy> upper_xbeam_gravity_well;
      upper_xbeam_gravity_well.CoCreateInstance(CLSID_ShapeGravityWellStrategy);
      upper_xbeam_gravity_well->SetShape(upperXBeamShape);
      doUpperXBeam->SetGravityWellStrategy(upper_xbeam_gravity_well);

      displayList->AddDisplayObject(doUpperXBeam);
   }

   // Model Lower Cross Beam (Elevation)
   CComPtr<iPointDisplayObject> doLowerXBeam;
   doLowerXBeam.CoCreateInstance(CLSID_PointDisplayObject);
   doLowerXBeam->SetID(m_DisplayObjectID++);
   doLowerXBeam->SetPosition(point,FALSE,FALSE);
   doLowerXBeam->SetSelectionType(stNone);

   CComPtr<IShape> lowerXBeamShape;
   pPier->GetLowerXBeamProfile(pierID,&lowerXBeamShape);

   CComPtr<iShapeDrawStrategy> lowerXBeamDrawStrategy;
   lowerXBeamDrawStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
   lowerXBeamDrawStrategy->SetShape(lowerXBeamShape);
   lowerXBeamDrawStrategy->SetSolidLineColor(XBEAM_LINE_COLOR);
   lowerXBeamDrawStrategy->SetSolidFillColor(XBEAM_FILL_COLOR);
   lowerXBeamDrawStrategy->DoFill(TRUE);

   doLowerXBeam->SetDrawingStrategy(lowerXBeamDrawStrategy);

   CComPtr<iShapeGravityWellStrategy> lower_xbeam_gravity_well;
   lower_xbeam_gravity_well.CoCreateInstance(CLSID_ShapeGravityWellStrategy);
   lower_xbeam_gravity_well->SetShape(lowerXBeamShape);
   doLowerXBeam->SetGravityWellStrategy(lower_xbeam_gravity_well);

   displayList->AddDisplayObject(doLowerXBeam);

   // Section Cut
   Float64 Lxb = pPier->GetXBeamLength(pierID);
   Lxb = pPier->ConvertCrossBeamToPierCoordinate(pierID,Lxb);

   Float64 XxbCut = pPier->ConvertPierToCrossBeamCoordinate(pierID,m_pFrame->GetCurrentCutLocation());

   CComPtr<iPointDisplayObject> doXBeamSection;
   doXBeamSection.CoCreateInstance(CLSID_PointDisplayObject);
   doXBeamSection->SetID(m_DisplayObjectID++);
   doXBeamSection->SetPosition(point,FALSE,FALSE);
   doXBeamSection->SetSelectionType(stNone);

   CComPtr<IShape> xbeamShape;
   pSectProp->GetXBeamShape(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,XxbCut),&xbeamShape);
   CComQIPtr<IXYPosition> position(xbeamShape);
   position->Offset(EndOffset+Lxb,0);

   CComPtr<iShapeDrawStrategy> xbeamDrawStrategy;
   xbeamDrawStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
   xbeamDrawStrategy->SetShape(xbeamShape);
   xbeamDrawStrategy->SetSolidLineColor(XBEAM_LINE_COLOR);
   xbeamDrawStrategy->SetSolidFillColor(XBEAM_FILL_COLOR);
   xbeamDrawStrategy->DoFill(TRUE);

   doXBeamSection->SetDrawingStrategy(xbeamDrawStrategy);

   CComPtr<iShapeGravityWellStrategy> xbeam_section_gravity_well;
   xbeam_section_gravity_well.CoCreateInstance(CLSID_ShapeGravityWellStrategy);
   xbeam_section_gravity_well->SetShape(xbeamShape);
   doXBeamSection->SetGravityWellStrategy(xbeam_section_gravity_well);

   displayList->AddDisplayObject(doXBeamSection);
}

void CXBeamRateView::UpdateColumnDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(COLUMN_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRPier,pPier);
   GET_IFACE2(pBroker,IXBRProject,pProject);

   PierIDType pierID = GetPierID();

   // Create a function that represents the bottom of the cross beam
   // We will use it to make the top of the column match the bottom of the
   // cross beam.
   mathPwLinearFunction2dUsingPoints fn;
   CComPtr<IPoint2dCollection> points;
   pPier->GetBottomSurface(pierID,xbrTypes::Stage1,&points);
   CComPtr<IEnumPoint2d> enumPoints;
   points->get__Enum(&enumPoints);
   CComPtr<IPoint2d> pnt;
   while ( enumPoints->Next(1,&pnt,NULL) != S_FALSE )
   {
      Float64 x,y;
      pnt->Location(&x,&y);
      fn.AddPoint(x,y);
      pnt.Release();
   }

   IndexType nColumns = pPier->GetColumnCount(pierID);
   for (IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 XxbCol = pPier->GetColumnLocation(pierID,colIdx);
      Float64 XpCol  = pPier->ConvertCrossBeamToPierCoordinate(pierID,XxbCol);
      Float64 Ytop = pPier->GetTopColumnElevation(pierID,colIdx);
      Float64 Ybot = pPier->GetBottomColumnElevation(pierID,colIdx);
      CColumnData::ColumnShapeType colShapeType;
      Float64 d1, d2;
      CColumnData::ColumnHeightMeasurementType columnHeightType;
      Float64 H;
      pProject->GetColumnProperties(pierID,colIdx,&colShapeType,&d1,&d2,&columnHeightType,&H);

      CComPtr<IPoint2d> pntTop, pntBot;
      pntTop.CoCreateInstance(CLSID_Point2d);
      pntTop->Move(XpCol,Ytop);

      pntBot.CoCreateInstance(CLSID_Point2d);
      pntBot->Move(XpCol,Ybot);
   
      CComPtr<iPointDisplayObject> doTop;
      doTop.CoCreateInstance(CLSID_PointDisplayObject);
      doTop->Visible(FALSE);
      doTop->SetPosition(pntTop,FALSE,FALSE);
      CComQIPtr<iConnectable> connectable1(doTop);
      CComPtr<iSocket> socket1;
      connectable1->AddSocket(0,pntTop,&socket1);

      CComPtr<iPointDisplayObject> doBot;
      doBot.CoCreateInstance(CLSID_PointDisplayObject);
      doBot->Visible(FALSE);
      doBot->SetPosition(pntBot,FALSE,FALSE);
      CComQIPtr<iConnectable> connectable2(doBot);
      CComPtr<iSocket> socket2;
      connectable2->AddSocket(0,pntBot,&socket2);

      // Create the shape of the column
      CComPtr<IPolyShape> columnShape;
      columnShape.CoCreateInstance(CLSID_PolyShape);
      Float64 X1,X2,X3;
      pntTop->get_X(&X2);
      X1 = X2-d1/2;
      X3 = X2+d1/2;
      Float64 Y1 = fn.Evaluate(X1);
      Float64 Y2 = fn.Evaluate(X2);
      Float64 Y3 = fn.Evaluate(X3);

      columnShape->AddPoint(X1,Y1);
      columnShape->AddPoint(X2,Y2);
      columnShape->AddPoint(X3,Y3);
      columnShape->AddPoint(X3,Ybot);
      columnShape->AddPoint(X1,Ybot);

      CComQIPtr<IShape> shape(columnShape);

      CComPtr<iPointDisplayObject> doColumn;
      doColumn.CoCreateInstance(CLSID_PointDisplayObject);
      doColumn->SetID(m_DisplayObjectID++);
      doColumn->SetPosition(pntTop,FALSE,FALSE);
      doColumn->SetSelectionType(stNone);

      CComPtr<iShapeDrawStrategy> drawColumnStrategy;
      drawColumnStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
      doColumn->SetDrawingStrategy(drawColumnStrategy);

      drawColumnStrategy->SetShape(shape);
      drawColumnStrategy->SetSolidLineColor(COLUMN_LINE_COLOR);
      drawColumnStrategy->SetSolidLineWidth(COLUMN_LINE_WEIGHT);
      drawColumnStrategy->SetSolidFillColor(COLUMN_FILL_COLOR);
      drawColumnStrategy->DoFill(TRUE);

      displayList->AddDisplayObject(doColumn);
   }
}

void CXBeamRateView::UpdateRebarDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(REBAR_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRPier,pPier);

   PierIDType pierID = GetPierID();

   Float64 CPO = pPier->GetCrownPointOffset(pierID);
   Float64 Xcrown = pPier->GetCrownPointLocation(pierID);
   Xcrown = pPier->ConvertCurbLineToCrossBeamCoordinate(pierID,Xcrown);
   Float64 Xoffset = CPO - Xcrown;

   GET_IFACE2(pBroker,IXBRRebar,pRebar);

   // Elevation View
   IndexType nRebarRows = pRebar->GetRebarRowCount(pierID);
   for ( IndexType rowIdx = 0; rowIdx < nRebarRows; rowIdx++ )
   {
      CComPtr<IPoint2dCollection> points;
      pRebar->GetRebarProfile(pierID,rowIdx,&points); // in Cross Beam Coordinates
      points->Offset(Xoffset,0);

      CComPtr<iPolyLineDisplayObject> doRebar;
      doRebar.CoCreateInstance(CLSID_PolyLineDisplayObject);
      doRebar->SetID(m_DisplayObjectID++);
      doRebar->AddPoints(points);
      doRebar->put_PointType(plpNone);
      doRebar->put_Color(REBAR_COLOR);
      doRebar->put_Width(REBAR_LINE_WEIGHT);
      doRebar->Commit();
      displayList->AddDisplayObject(doRebar);
   }


   // Section View
   Float64 Lxb = pPier->GetXBeamLength(pierID);
   Lxb = pPier->ConvertCrossBeamToPierCoordinate(pierID,Lxb);

   Float64 XxbCut = pPier->ConvertPierToCrossBeamCoordinate(pierID,m_pFrame->GetCurrentCutLocation());

   CComPtr<IRebarSection> rebarSection;
   pRebar->GetRebarSection(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,XxbCut),&rebarSection);

   CComPtr<IEnumRebarSectionItem> enumSectionItems;
   rebarSection->get__EnumRebarSectionItem(&enumSectionItems);
   CComPtr<IRebarSectionItem> sectionItem;
   while ( enumSectionItems->Next(1,&sectionItem,NULL) != S_FALSE )
   {
      CComPtr<IPoint2d> pntBar;
      sectionItem->get_Location(&pntBar);

      pntBar->Offset(EndOffset+Lxb,0);

      CComPtr<iPointDisplayObject> doBar;
      doBar.CoCreateInstance(CLSID_PointDisplayObject);
      doBar->SetID(m_DisplayObjectID++);
      doBar->SetPosition(pntBar,FALSE,FALSE);
      

      displayList->AddDisplayObject(doBar);

      sectionItem.Release();
   }

}

void CXBeamRateView::UpdateStirrupDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(STIRRUP_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRPier,pPier);
   GET_IFACE2(pBroker,IXBRSectionProperties,pSectProps);

   PierIDType pierID = GetPierID();

   GET_IFACE2(pBroker,IXBRProject,pProject);
   Float64 H,W;
   pProject->GetDiaphragmDimensions(pierID,&H,&W);

   xbrTypes::SuperstructureConnectionType pierType = pProject->GetPierType(pierID);

   GET_IFACE2(pBroker,IXBRStirrups,pStirrups);
   for ( int i = 0; i < 2; i++ )
   {
      xbrTypes::Stage stage = (xbrTypes::Stage)i;
      IndexType nStirrupZones = pStirrups->GetStirrupZoneCount(pierID,stage);
      for ( IndexType zoneIdx = 0; zoneIdx < nStirrupZones; zoneIdx++ )
      {
         Float64 Xstart, Xend;
         pStirrups->GetStirrupZoneBoundary(pierID,stage,zoneIdx,&Xstart,&Xend);
         
         IndexType nStirrups = pStirrups->GetStirrupCount(pierID,stage,zoneIdx);
         Float64 S = pStirrups->GetStirrupZoneSpacing(pierID,stage,zoneIdx);

         for ( IndexType stirrupIdx = 0; stirrupIdx < nStirrups; stirrupIdx++ )
         {
            Float64 Xxb = Xstart + stirrupIdx*S;
            Float64 Xcl = pPier->ConvertCrossBeamToCurbLineCoordinate(pierID,Xxb);

            Float64 Ytop = pPier->GetElevation(pierID,Xcl);

            Float64 D = pSectProps->GetDepth(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Xxb));
            Float64 Ybot = Ytop - D;

            if ( pierType != xbrTypes::pctIntegral )
            {
               // for non-integral, D is just the depth of the lower portion
               Ybot -= H;
            }

            if ( stage == xbrTypes::Stage1 || pierType != xbrTypes::pctIntegral )
            {
               Ytop -= H;
            }

            CComPtr<IPoint2d> pntTop;
            pntTop.CoCreateInstance(CLSID_Point2d);
            Float64 X = pPier->ConvertCrossBeamToPierCoordinate(pierID,Xxb);
            pntTop->Move(X,Ytop);

            CComPtr<iPointDisplayObject> doTopPnt;
            doTopPnt.CoCreateInstance(CLSID_PointDisplayObject);
            doTopPnt->SetID(m_DisplayObjectID++);
            doTopPnt->SetPosition(pntTop,FALSE,FALSE);

            CComQIPtr<iConnectable> startConnectable(doTopPnt);
            CComPtr<iSocket> startSocket;
            startConnectable->AddSocket(0,pntTop,&startSocket);

            CComPtr<iDrawPointStrategy> draw_point_strategy;
            doTopPnt->GetDrawingStrategy(&draw_point_strategy);
            CComQIPtr<iSimpleDrawPointStrategy> thePointDrawStrategy(draw_point_strategy);
            thePointDrawStrategy->SetColor(STIRRUP_COLOR);
            thePointDrawStrategy->SetPointType(ptNone);

            CComPtr<IPoint2d> pntBottom;
            pntBottom.CoCreateInstance(CLSID_Point2d);
            pntBottom->Move(X,Ybot);

            CComPtr<iPointDisplayObject> doBottomPnt;
            doBottomPnt.CoCreateInstance(CLSID_PointDisplayObject);
            doBottomPnt->SetID(m_DisplayObjectID++);
            doBottomPnt->SetPosition(pntBottom,FALSE,FALSE);

            CComQIPtr<iConnectable> endConnectable(doBottomPnt);
            CComPtr<iSocket> endSocket;
            endConnectable->AddSocket(0,pntBottom,&endSocket);

            draw_point_strategy.Release();
            doBottomPnt->GetDrawingStrategy(&draw_point_strategy);
            thePointDrawStrategy.Release();
            draw_point_strategy.QueryInterface(&thePointDrawStrategy);
            thePointDrawStrategy->SetColor(STIRRUP_COLOR);
            thePointDrawStrategy->SetPointType(ptNone);

            CComPtr<iLineDisplayObject> doLine;
            doLine.CoCreateInstance(CLSID_LineDisplayObject);
            doLine->SetID(m_DisplayObjectID++);
            CComQIPtr<iConnector> connector(doLine);
            CComPtr<iPlug> startPlug, endPlug;
            connector->GetStartPlug(&startPlug);
            connector->GetEndPlug(&endPlug);
            startPlug->SetSocket(startSocket);
            endPlug->SetSocket(endSocket);

            CComPtr<iDrawLineStrategy> strategy;
            doLine->GetDrawLineStrategy(&strategy);
            CComQIPtr<iSimpleDrawLineStrategy> theStrategy(strategy);
            theStrategy->SetColor(STIRRUP_COLOR);
            theStrategy->SetEndType(leNone);

            displayList->AddDisplayObject(doTopPnt);
            displayList->AddDisplayObject(doBottomPnt);
            displayList->AddDisplayObject(doLine);
         }
      }
   }
}

void CXBeamRateView::UpdateBearingDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(BEARING_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   PierIDType pierID = GetPierID();

   GET_IFACE2(pBroker,IXBRPier,pPier);

   IndexType nBearingLines = pPier->GetBearingLineCount(pierID);
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pPier->GetBearingCount(pierID,brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = pPier->GetBearingLocation(pierID,brgLineIdx,brgIdx);
         Float64 Xp   = pPier->ConvertCrossBeamToPierCoordinate(pierID,Xbrg);
         Float64 Y    = pPier->GetBearingElevation(pierID,brgLineIdx,brgIdx);

         CComPtr<IPoint2d> pnt;
         pnt.CoCreateInstance(CLSID_Point2d);
         pnt->Move(Xp,Y);

         CComPtr<iPointDisplayObject> doPnt;
         doPnt.CoCreateInstance(CLSID_PointDisplayObject);
         doPnt->SetID(m_DisplayObjectID++);
         doPnt->SetPosition(pnt,FALSE,FALSE);

         CComPtr<iDrawPointStrategy> strategy;
         doPnt->GetDrawingStrategy(&strategy);

         CComQIPtr<iSimpleDrawPointStrategy> theStrategy(strategy);
         theStrategy->SetPointType(ptSquare);
         theStrategy->SetColor(BLACK);

         displayList->AddDisplayObject(doPnt);
      }
   }
}

void CXBeamRateView::UpdateGirderDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(GIRDER_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   CComPtr<IXBeamRateAgent> pAgent;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent);
   if ( FAILED(hr) )
   {
      // not in PGSuper/PGSplice so we don't know anything about the shape of the girder
      return;
   }

   PierIndexType pierIdx = GetPierIndex();
   GET_IFACE2(pBroker,IBridge,pBridge);
   GroupIndexType backGroupIdx,aheadGroupIdx;
   pBridge->GetGirderGroupIndex(pierIdx,&backGroupIdx,&aheadGroupIdx);

   GET_IFACE2(pBroker,IShapes,pShapes);
   GET_IFACE2(pBroker,IPointOfInterest,pPoi);
   GET_IFACE2(pBroker,IIntervals,pIntervals);

   CComPtr<IAngle> objSkew;
   pBridge->GetPierSkew(pierIdx,&objSkew);

   Float64 skew;
   objSkew->get_Value(&skew);


   for ( GroupIndexType grpIdx = aheadGroupIdx; backGroupIdx <= grpIdx && grpIdx != INVALID_INDEX; grpIdx-- ) // draw in reverse order so back side girders cover ahead side girders
   {
      GirderIndexType nGirders = pBridge->GetGirderCount(grpIdx);
      for ( GirderIndexType gdrIdx = 0; gdrIdx < nGirders; gdrIdx++ )
      {
         CGirderKey girderKey(grpIdx,gdrIdx);
         pgsPointOfInterest poi = pPoi->GetPierPointOfInterest(girderKey,pierIdx);

         IntervalIndexType intervalIdx = pIntervals->GetErectSegmentInterval(poi.GetSegmentKey());

         CComPtr<IShape> shape;
         pShapes->GetSegmentShape(intervalIdx,poi,true,pgsTypes::scBridge,&shape);

         CComPtr<IShape> skewedShape;
         SkewGirderShape(skew,shape,&skewedShape);

         CComPtr<iPointDisplayObject> dispObj;
         dispObj.CoCreateInstance(CLSID_PointDisplayObject);

         CComQIPtr<IXYPosition> position(skewedShape);
         CComPtr<IPoint2d> topCenter;
         position->get_LocatorPoint(lpTopCenter,&topCenter);

         dispObj->SetPosition(topCenter,FALSE,FALSE);

         CComPtr<iShapeDrawStrategy> strategy;
         strategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
         strategy->SetShape(skewedShape);
         strategy->SetSolidLineColor(SEGMENT_BORDER_COLOR);
         strategy->SetVoidLineColor(VOID_BORDER_COLOR);
         strategy->SetVoidFillColor(GetSysColor(COLOR_WINDOW));
         if ( grpIdx == backGroupIdx )
         {
            strategy->SetSolidFillColor(SEGMENT_FILL_COLOR);
            strategy->SetSolidLineStyle(lsSolid);
            strategy->SetVoidLineStyle(lsSolid);
         }
         else
         {
            strategy->SetSolidFillColor(SEGMENT_FILL_GHOST_COLOR);
            strategy->SetSolidLineStyle(lsDash);
            strategy->SetVoidLineStyle(lsDash);
         }
         strategy->DoFill(true);

         dispObj->SetDrawingStrategy(strategy);

         dispObj->SetSelectionType(stNone);

         dispObj->SetID(m_DisplayObjectID++);

         displayList->AddDisplayObject(dispObj);
      }
   }
}

void CXBeamRateView::UpdateSectionCutDisplayObjects()
{
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CComPtr<iDisplayList> display_list;
   dispMgr->FindDisplayList(SECTION_CUT_DISPLAY_LIST_ID,&display_list);

   CComPtr<iDisplayObjectFactory> factory;
   dispMgr->GetDisplayObjectFactory(0, &factory);

   CComPtr<iDisplayObject> disp_obj;
   factory->Create(CSectionCutDisplayImpl::ms_Format,NULL,&disp_obj);

   CComPtr<iDisplayObjectEvents> sink;
   disp_obj->GetEventSink(&sink);

   disp_obj->SetSelectionType(stAll);

   CComQIPtr<iPointDisplayObject,&IID_iPointDisplayObject> point_disp(disp_obj);
   point_disp->SetMaxTipWidth(TOOLTIP_WIDTH);
   point_disp->SetToolTipText(_T("Drag me to move section cut.\r\nDouble click to enter the cut location\r\nPress CTRL + -> to move ahead\r\nPress CTRL + <- to move back"));
   point_disp->SetTipDisplayTime(TOOLTIP_DURATION);

   CComQIPtr<iSectionCutDrawStrategy,&IID_iSectionCutDrawStrategy> section_cut_strategy(sink);
   section_cut_strategy->Init(m_pFrame,point_disp, m_pFrame);
   section_cut_strategy->SetColor(CUT_COLOR);

   point_disp->SetID(SECTION_CUT_ID);

   display_list->Clear();
   display_list->AddDisplayObject(disp_obj);
}

void CXBeamRateView::UpdateDimensionsDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(DIMENSIONS_DISPLAY_LIST_ID,&displayList);

   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   PierIDType pierID = GetPierID();

   GET_IFACE2(pBroker,IXBRPier,pPier);
   GET_IFACE2(pBroker,IXBRSectionProperties,pSectProp);
   GET_IFACE2(pBroker,IXBRProject,pProject);

   xbrTypes::SuperstructureConnectionType pierType = pProject->GetPierType(pierID);

   Float64 CPO = pPier->GetCrownPointOffset(pierID);
   Float64 Xcrown = pPier->GetCrownPointLocation(pierID);
   Xcrown = pPier->ConvertCurbLineToCrossBeamCoordinate(pierID,Xcrown);
   Float64 Xoffset = CPO - Xcrown;

   Float64 H1, H2, H3, H4, X1, X2, X3, X4, W;
   pProject->GetLowerXBeamDimensions(pierID,&H1,&H2,&H3,&H4,&X1,&X2,&X3,&X4,&W);

   Float64 D, Hu;
   pProject->GetDiaphragmDimensions(pierID,&Hu,&D);

   // Upper Cross Beam - Top Left
   Float64 Xxb = 0;
   Float64 Xcl = pPier->ConvertCrossBeamToCurbLineCoordinate(pierID,Xxb);
   Float64 Xp  = pPier->ConvertCrossBeamToPierCoordinate(pierID,Xxb);
   Float64 Y   = pPier->GetElevation(pierID,Xcl);
   CComPtr<IPoint2d> uxbTL;
   uxbTL.CoCreateInstance(CLSID_Point2d);
   uxbTL->Move(Xp-X2,Y);

   // Upper Cross Beam - Bottom Left (Lower Cross Beam - Top Left)
   CComPtr<IPoint2d> uxbBL;
   uxbBL.CoCreateInstance(CLSID_Point2d);
   uxbBL->Move(Xp-X2,Y-Hu);

   // Lower Cross Beam - Bottom Left
   CComPtr<IPoint2d> lxbBL;
   lxbBL.CoCreateInstance(CLSID_Point2d);
   lxbBL->Move(Xp-X2,Y-Hu-H1);

   // Upper Cross Beam - Top Right
   Xxb = pPier->GetXBeamLength(pierID);
   Xcl = pPier->ConvertCrossBeamToCurbLineCoordinate(pierID,Xxb);
   Xp  = pPier->ConvertCrossBeamToPierCoordinate(pierID,Xxb);
   Y   = pPier->GetElevation(pierID,Xcl);
   CComPtr<IPoint2d> uxbTR;
   uxbTR.CoCreateInstance(CLSID_Point2d);
   uxbTR->Move(Xp+X4,Y);
   
   // Upper Cross Beam - Bottom Right (Lower Cross Beam - Top Right)
   CComPtr<IPoint2d> uxbBR;
   uxbBR.CoCreateInstance(CLSID_Point2d);
   uxbBR->Move(Xp+X4,Y-Hu);

   // Lower Cross Beam - Bottom Right
   CComPtr<IPoint2d> lxbBR;
   lxbBR.CoCreateInstance(CLSID_Point2d);
   lxbBR->Move(Xp+X4,Y-Hu-H1);

   CComPtr<IPoint2d> lxbBLC, lxbBRC;
   CComPtr<IPoint2d> lxbBL2, lxbBR2;
   lxbBLC.CoCreateInstance(CLSID_Point2d);
   lxbBRC.CoCreateInstance(CLSID_Point2d);
   lxbBL2.CoCreateInstance(CLSID_Point2d);
   lxbBR2.CoCreateInstance(CLSID_Point2d);
   Float64 x,y;
   lxbBL->Location(&x,&y);
   lxbBLC->Move(x,y-H2);
   lxbBL2->Move(x+X1,y-H2);

   lxbBR->Location(&x,&y);
   lxbBRC->Move(x,y-H4);
   lxbBR2->Move(x-X3,y-H4);

   // Horizontal Cross Beam Dimensions
   if ( pierType == xbrTypes::pctExpansion )
   {
      CComPtr<IPoint2d> lxbTLC, lxbTRC;
      lxbTLC.CoCreateInstance(CLSID_Point2d);
      lxbTRC.CoCreateInstance(CLSID_Point2d);
      Float64 x1,y1;
      uxbBL->Location(&x1,&y1);
      Float64 x2,y2;
      uxbBR->Location(&x2,&y2);
      lxbTLC->Move(x1,Max(y1,y2));
      lxbTRC->Move(x2,Max(y1,y2));
      BuildDimensionLine(displayList,lxbTLC,lxbTRC);
   }
   else
   {
      CComPtr<IPoint2d> uxbTLC, uxbTRC;
      uxbTLC.CoCreateInstance(CLSID_Point2d);
      uxbTRC.CoCreateInstance(CLSID_Point2d);
      Float64 x1,y1;
      uxbTL->Location(&x1,&y1);
      Float64 x2,y2;
      uxbTR->Location(&x2,&y2);
      uxbTLC->Move(x1,Max(y1,y2));
      uxbTRC->Move(x2,Max(y1,y2));
      BuildDimensionLine(displayList,uxbTLC,uxbTRC);
   }

   BuildDimensionLine(displayList,lxbBL2,lxbBLC);
   BuildDimensionLine(displayList,lxbBRC,lxbBR2);

   // Left Side of Cross Beam

   // Height of upper cross beam
   if ( pierType != xbrTypes::pctExpansion )
   {
      BuildDimensionLine(displayList,uxbBL,uxbTL);
      BuildDimensionLine(displayList,uxbTR,uxbBR);
   }

   // Height of lower cross beam
   BuildDimensionLine(displayList,lxbBL,uxbBL);
   BuildDimensionLine(displayList,uxbBR,lxbBR);

   BuildDimensionLine(displayList,lxbBLC,lxbBL);
   BuildDimensionLine(displayList,lxbBR,lxbBRC);

   // Column Dimensions

   // Column Height
   Float64 YbotColMin = DBL_MAX;
   ColumnIndexType nColumns = pPier->GetColumnCount(pierID);
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 XxbCol = pPier->GetColumnLocation(pierID,colIdx);
      Float64 XpCol = pPier->ConvertCrossBeamToPierCoordinate(pierID,XxbCol);
      Float64 YtopCol = pPier->GetTopColumnElevation(pierID,colIdx);
      Float64 YbotCol = pPier->GetBottomColumnElevation(pierID,colIdx);

      CComPtr<IPoint2d> pntTop, pntBot;
      pntTop.CoCreateInstance(CLSID_Point2d);
      pntBot.CoCreateInstance(CLSID_Point2d);
      pntTop->Move(XpCol,YtopCol);
      pntBot->Move(XpCol,YbotCol);
      BuildDimensionLine(displayList,pntTop,pntBot);

      YbotColMin = Min(YbotColMin,YbotCol);
   }

   // Column Spacing (starts with right cross beam cantilever and
   // than proceeds with the spacing between columns at their base)
   CComPtr<IPoint2d> pntLeft;
   pntLeft.CoCreateInstance(CLSID_Point2d);
   pntLeft->Move(Xoffset,YbotColMin);
   for ( ColumnIndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 XxbCol = pPier->GetColumnLocation(pierID,colIdx);
      Float64 XpCol = pPier->ConvertCrossBeamToPierCoordinate(pierID,XxbCol);

      CComPtr<IPoint2d> pntRight;
      pntRight.CoCreateInstance(CLSID_Point2d);
      pntRight->Move(XpCol,YbotColMin);
     
      BuildDimensionLine(displayList,pntRight,pntLeft);

      pntLeft = pntRight;
   }

   // Left cross beam cantilever
   Float64 Lxb = pPier->GetXBeamLength(pierID);
   Lxb = pPier->ConvertCrossBeamToPierCoordinate(pierID,Lxb);
   CComPtr<IPoint2d> pntRight;
   pntRight.CoCreateInstance(CLSID_Point2d);
   pntRight->Move(Lxb,YbotColMin);
   BuildDimensionLine(displayList,pntRight,pntLeft);

   Float64 Z = pPier->ConvertPierToCrossBeamCoordinate(pierID,m_pFrame->GetCurrentCutLocation());

   if ( pProject->GetPierType(pierID) != xbrTypes::pctExpansion )
   {
      // Upper Cross Beam (End View)
      CComPtr<IShape> upperXBeamShape;
      pSectProp->GetXBeamShape(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Z),&upperXBeamShape);

      CComQIPtr<ICompositeShape> composite(upperXBeamShape);
      CComPtr<IShape> shape;
      if ( composite )
      {
         CComPtr<ICompositeShapeItem> upperShapeItem;
         composite->get_Item(1,&upperShapeItem);

         upperShapeItem->get_Shape(&shape);
      }
      else
      {
         shape = upperXBeamShape;
      }

      CComQIPtr<IXYPosition> position(shape);
      pntLeft.Release();
      pntRight.Release();
      position->get_LocatorPoint(lpTopLeft,&pntLeft);
      position->get_LocatorPoint(lpTopRight,&pntRight);
      pntLeft->Offset(EndOffset+Lxb,0);
      pntRight->Offset(EndOffset+Lxb,0);
      BuildDimensionLine(displayList,pntLeft,pntRight);
   }

   // Lower Cross Beam (End View)
   CComPtr<IShape> lowerXBeamShape;
   pSectProp->GetXBeamShape(pierID,xbrTypes::Stage1,xbrPointOfInterest(INVALID_ID,Z),&lowerXBeamShape);

   CComQIPtr<IXYPosition> position(lowerXBeamShape);
   pntLeft.Release();
   pntRight.Release();
   position->get_LocatorPoint(lpBottomLeft,&pntLeft);
   position->get_LocatorPoint(lpBottomRight,&pntRight);
   pntLeft->Offset(EndOffset+Lxb,0);
   pntRight->Offset(EndOffset+Lxb,0);
   BuildDimensionLine(displayList,pntRight,pntLeft);

   // End View Height
   CComPtr<IShape> xbeamShape;
   pSectProp->GetXBeamShape(pierID,xbrTypes::Stage2,xbrPointOfInterest(INVALID_ID,Z),&xbeamShape);

   position.Release();
   xbeamShape->QueryInterface(&position);
   CComPtr<IPoint2d> pntTop;
   CComPtr<IPoint2d> pntBot;
   position->get_LocatorPoint(lpTopRight,&pntTop);
   position->get_LocatorPoint(lpBottomRight,&pntBot);
   pntTop->Offset(EndOffset+Lxb,0);
   pntBot->Offset(EndOffset+Lxb,0);
   BuildDimensionLine(displayList,pntTop,pntBot);

}

void CXBeamRateView::BuildDimensionLine(iDisplayList* pDL, IPoint2d* fromPoint,IPoint2d* toPoint,iDimensionLine** ppDimLine)
{
   Float64 distance;
   toPoint->DistanceEx(fromPoint,&distance);
   if ( !IsZero(distance) )
   {
      BuildDimensionLine(pDL,fromPoint,toPoint,distance,ppDimLine);
   }
}

void CXBeamRateView::BuildDimensionLine(iDisplayList* pDL, IPoint2d* fromPoint,IPoint2d* toPoint,Float64 dimension,iDimensionLine** ppDimLine)
{
   // put points at locations and make them sockets
   CComPtr<iPointDisplayObject> from_rep;
   ::CoCreateInstance(CLSID_PointDisplayObject,NULL,CLSCTX_ALL,IID_iPointDisplayObject,(void**)&from_rep);
   from_rep->SetPosition(fromPoint,FALSE,FALSE);
   from_rep->SetID(m_DisplayObjectID++);
   CComQIPtr<iConnectable,&IID_iConnectable> from_connectable(from_rep);
   CComPtr<iSocket> from_socket;
   from_connectable->AddSocket(0,fromPoint,&from_socket);
   from_rep->Visible(FALSE);
   pDL->AddDisplayObject(from_rep);

   CComPtr<iPointDisplayObject> to_rep;
   ::CoCreateInstance(CLSID_PointDisplayObject,NULL,CLSCTX_ALL,IID_iPointDisplayObject,(void**)&to_rep);
   to_rep->SetPosition(toPoint,FALSE,FALSE);
   to_rep->SetID(m_DisplayObjectID++);
   CComQIPtr<iConnectable,&IID_iConnectable> to_connectable(to_rep);
   CComPtr<iSocket> to_socket;
   to_connectable->AddSocket(0,toPoint,&to_socket);
   to_rep->Visible(FALSE);
   pDL->AddDisplayObject(to_rep);

   // Create the dimension line object
   CComPtr<iDimensionLine> dimLine;
   ::CoCreateInstance(CLSID_DimensionLineDisplayObject,NULL,CLSCTX_ALL,IID_iDimensionLine,(void**)&dimLine);

   dimLine->SetArrowHeadStyle(DManip::ahsFilled);

   // Attach connector (the dimension line) to the sockets 
   CComPtr<iConnector> connector;
   dimLine->QueryInterface(IID_iConnector,(void**)&connector);
   CComPtr<iPlug> startPlug;
   CComPtr<iPlug> endPlug;
   connector->GetStartPlug(&startPlug);
   connector->GetEndPlug(&endPlug);

   DWORD dwCookie;
   from_socket->Connect(startPlug,&dwCookie);
   to_socket->Connect(endPlug,&dwCookie);

   // Create the text block and attach it to the dimension line
   CComPtr<iTextBlock> textBlock;
   ::CoCreateInstance(CLSID_TextBlock,NULL,CLSCTX_ALL,IID_iTextBlock,(void**)&textBlock);

   // Format the dimension text
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);
   GET_IFACE2(pBroker,IEAFDisplayUnits,pDisplayUnits);
   CString strDimension = FormatDimension(dimension,pDisplayUnits->GetSpanLengthUnit());

   textBlock->SetText(strDimension);
   textBlock->SetBkMode(TRANSPARENT);

   dimLine->SetTextBlock(textBlock);

   // Assign the span id to the dimension line (so they are the same)
   dimLine->SetID(m_DisplayObjectID++);

   pDL->AddDisplayObject(dimLine);

   if ( ppDimLine )
   {
      (*ppDimLine) = dimLine;
      (*ppDimLine)->AddRef();
   }
}

void CXBeamRateView::HandleLButtonDblClk(UINT nFlags, CPoint logPoint)
{
   CDisplayView::HandleLButtonDblClk(nFlags,logPoint);
   CComPtr<IBroker> pBroker;
   EAFGetBroker(&pBroker);

   CComPtr<IXBeamRateAgent> pAgent;
   HRESULT hr = pBroker->GetInterface(IID_IXBeamRateAgent,(IUnknown**)&pAgent);
   if ( SUCCEEDED(hr) )
   {
      GET_IFACE2(pBroker,IEditByUI,pEditByUI);
      GET_IFACE2(pBroker,IBridgeDescription,pIBridgeDesc);
      const CBridgeDescription2* pBridgeDesc = pIBridgeDesc->GetBridgeDescription();
      PierIDType pierID = GetPierID();
      const CPierData2* pPier = pBridgeDesc->FindPier(pierID);
      pEditByUI->EditPierDescription(pPier->GetIndex(),0/*pageIdx*/);

   }
   else
   {
      GET_IFACE2(pBroker,IXBRProjectEdit,pProjectEdit);
      pProjectEdit->EditPier(INVALID_ID);
   }
}

void CXBeamRateView::CreateLineDisplayObject(IPoint2d* pntStart,IPoint2d* pntEnd,iLineDisplayObject** ppLineDO)
{
   CComPtr<iPointDisplayObject> doPntStart;
   doPntStart.CoCreateInstance(CLSID_PointDisplayObject);
   doPntStart->Visible(FALSE);
   doPntStart->SetPosition(pntStart,FALSE,FALSE);
   CComQIPtr<iConnectable> connectable1(doPntStart);
   CComPtr<iSocket> socket1;
   connectable1->AddSocket(0,pntStart,&socket1);

   CComPtr<iPointDisplayObject> doPntEnd;
   doPntEnd.CoCreateInstance(CLSID_PointDisplayObject);
   doPntEnd->Visible(FALSE);
   doPntEnd->SetPosition(pntEnd,FALSE,FALSE);
   CComQIPtr<iConnectable> connectable2(doPntEnd);
   CComPtr<iSocket> socket2;
   connectable2->AddSocket(0,pntEnd,&socket2);

   CComPtr<iLineDisplayObject> doLine;
   doLine.CoCreateInstance(CLSID_LineDisplayObject);

   CComQIPtr<iConnector> connector(doLine);
   CComPtr<iPlug> startPlug, endPlug;
   connector->GetStartPlug(&startPlug);
   connector->GetEndPlug(&endPlug);
   DWORD dwCookie;
   connectable1->Connect(0,atByID,startPlug,&dwCookie);
   connectable2->Connect(0,atByID,endPlug,  &dwCookie);

   doLine.CopyTo(ppLineDO);
}

DROPEFFECT CXBeamRateView::CanDrop(COleDataObject* pDataObject,DWORD dwKeyState,IPoint2d* point)
{
   // This override has to determine if the thing being dragged over it can
   // be dropped. In order to do that, it must unpackage the OleDataObject.
   //
   // The stuff in the data object is just from the display object. The display
   // objects need to be updated so that the client can attach an object to it
   // that knows how to package up domain specific information. At the same
   // time, this view needs to be able to get some domain specific hint 
   // as to the type of data that is going to be dropped.

   if ( pDataObject->IsDataAvailable(CSectionCutDisplayImpl::ms_Format) )
   {
      // need to peek at our object first and make sure it's coming from the local process
      // this is ugly because it breaks encapsulation of CBridgeSectionCutDisplayImpl
      CComPtr<iDragDataSource> source;               
      ::CoCreateInstance(CLSID_DragDataSource,NULL,CLSCTX_ALL,IID_iDragDataSource,(void**)&source);
      source->SetDataObject(pDataObject);
      source->PrepareFormat(CSectionCutDisplayImpl::ms_Format);

      CWinThread* thread = ::AfxGetThread( );
      DWORD threadid = thread->m_nThreadID;

      DWORD threadl;
      // know (by voodoo) that the first member of this data source is the thread id
      source->Read(CSectionCutDisplayImpl::ms_Format,&threadl,sizeof(DWORD));

      if (threadl == threadid)
      {
        return DROPEFFECT_MOVE;
      }
   }

   return DROPEFFECT_NONE;
}

void CXBeamRateView::OnDropped(COleDataObject* pDataObject,DROPEFFECT dropEffect,IPoint2d* point)
{
   AfxMessageBox(_T("CBridgePlanView::OnDropped"));
}

void CXBeamRateView::SkewGirderShape(Float64 skew,IShape* pShape,IShape** ppSkewedShape)
{
   if ( IsZero(skew) )
   {
      // Not skewed... nothing to do
      (*ppSkewedShape) = pShape;
      (*ppSkewedShape)->AddRef();
      return;
   }

   CComPtr<IPoint2dCollection> points;
   pShape->get_PolyPoints(&points);
   CComPtr<IPoint2d> pnt;
   CComPtr<IEnumPoint2d> enumPoints;
   points->get__Enum(&enumPoints);
   while ( enumPoints->Next(1,&pnt,NULL) != S_FALSE )
   {
      Float64 x;
      pnt->get_X(&x);
      x /= cos(skew);
      pnt->put_X(x);

      pnt.Release();
   }

   CComPtr<IPolyShape> polyShape;
   polyShape.CoCreateInstance(CLSID_PolyShape);
   polyShape->AddPoints(points);

   CComQIPtr<IShape> s(polyShape);
   s.CopyTo(ppSkewedShape);
}