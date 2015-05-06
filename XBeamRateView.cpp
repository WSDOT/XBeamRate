// XBeamRateView.cpp : implementation of the CXBeamRateView class
//

#include "stdafx.h"

#include "XBeamRateDoc.h"
#include "XBeamRateView.h"

#include <IFace\Project.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <IFace\Pier.h>
#include <MFCTools\Format.h>

#include <EAF\EAFDisplayUnits.h>

#include <Colors.h>
#define SELECTED_OBJECT_LINE_COLOR     RED4
#define SELECTED_OBJECT_FILL_COLOR     RED2
#define COLUMN_LINE_COLOR              GREY50
#define COLUMN_FILL_COLOR              GREY70
#define XBEAM_LINE_COLOR               GREY50
#define XBEAM_FILL_COLOR               GREY70

#define COLUMN_LINE_WEIGHT             1
//#define XBEAM_LINE_WEIGHT              3

#define BEARING_DISPLAY_LIST_ID 0
#define XBEAM_DISPLAY_LIST_ID   1
#define COLUMN_DISPLAY_LIST_ID  2

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

BOOL CXBeamRateView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CXBeamRateView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CXBeamRateView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
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
	CDisplayView::AssertValid();
}

void CXBeamRateView::Dump(CDumpContext& dc) const
{
	CDisplayView::Dump(dc);
}

CXBeamRateDoc* CXBeamRateView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXBeamRateDoc)));
	return (CXBeamRateDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXBeamRateView message handlers
void CXBeamRateView::OnInitialUpdate()
{
   EnableToolTips();

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);
   dispMgr->EnableLBtnSelect(TRUE);
   dispMgr->EnableRBtnSelect(TRUE);
   dispMgr->SetSelectionLineColor(SELECTED_OBJECT_LINE_COLOR);
   dispMgr->SetSelectionFillColor(SELECTED_OBJECT_FILL_COLOR);

   CDisplayView::SetMappingMode(DManip::Isotropic);

   // Setup display lists
   CComPtr<iDisplayList> displayList;
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(BEARING_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(XBEAM_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   displayList.Release();
   ::CoCreateInstance(CLSID_DisplayList,NULL,CLSCTX_ALL,IID_iDisplayList,(void**)&displayList);
   displayList->SetID(COLUMN_DISPLAY_LIST_ID);
   dispMgr->AddDisplayList(displayList);

   CDisplayView::OnInitialUpdate();
}

void CXBeamRateView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
   CDisplayView::OnUpdate(pSender,lHint,pHint);
   UpdateDisplayObjects();
   ScaleToFit();
}

void CXBeamRateView::UpdateDisplayObjects()
{
   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);
   dispMgr->ClearDisplayObjects();

   UpdateXBeamDisplayObjects();
   UpdateColumnDisplayObjects();
   UpdateBearingDisplayObjects();
   //CWaitCursor wait;

   //CComPtr<iDisplayMgr> dispMgr;
   //GetDisplayMgr(&dispMgr);

   //CDManipClientDC dc(this);

   //dispMgr->ClearDisplayObjects();

   //CComPtr<iDisplayList> displayList;
   //dispMgr->FindDisplayList(DISPLAY_LIST_ID,&displayList);

   //CXBeamRateDoc* pDoc = (CXBeamRateDoc*)GetDocument();
   //CComPtr<IBroker> pBroker;
   //pDoc->GetBroker(&pBroker);

   //GET_IFACE2(pBroker,IXBRProject,pProject);

   //Float64 leftOverhang = pProject->GetXBeamOverhang(pgsTypes::pstLeft);
   //Float64 rightOverhang = pProject->GetXBeamOverhang(pgsTypes::pstRight);
   //IndexType nColumns = pProject->GetColumnCount();

   //Float64 H,W;
   //pProject->GetDiaphragmDimensions(&H,&W);

   //Float64 H1, H2, X1;
   //pProject->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1);

   //CComPtr<IPoint2d> pntXBeam;
   //pntXBeam.CoCreateInstance(CLSID_Point2d);
   //pntXBeam->Move(0,0);

   //CComPtr<iPointDisplayObject> doUpperXBeam;
   //doUpperXBeam.CoCreateInstance(CLSID_PointDisplayObject);
   //doUpperXBeam->SetID(m_DisplayObjectID++);
   //doUpperXBeam->SetPosition(pntXBeam,FALSE,FALSE);
   //doUpperXBeam->SetSelectionType(stAll);

   //GET_IFACE2(pBroker,IXBRPier,pPier);
   //CComPtr<IShape> upperXBeamShape;
   //pPier->GetUpperXBeamProfile(&upperXBeamShape);

   //CComPtr<iShapeDrawStrategy> upperXBeamDrawStrategy;
   //upperXBeamDrawStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
   //upperXBeamDrawStrategy->SetShape(upperXBeamShape);
   //upperXBeamDrawStrategy->SetSolidLineColor(XBEAM_LINE_COLOR);
   //upperXBeamDrawStrategy->SetSolidFillColor(XBEAM_FILL_COLOR);
   //upperXBeamDrawStrategy->DoFill(TRUE);

   //doUpperXBeam->SetDrawingStrategy(upperXBeamDrawStrategy);

   //CComPtr<iShapeGravityWellStrategy> upper_xbeam_gravity_well;
   //upper_xbeam_gravity_well.CoCreateInstance(CLSID_ShapeGravityWellStrategy);
   //upper_xbeam_gravity_well->SetShape(upperXBeamShape);
   //doUpperXBeam->SetGravityWellStrategy(upper_xbeam_gravity_well);

   //displayList->AddDisplayObject(doUpperXBeam);

   //CComPtr<iPointDisplayObject> doPnt1;
   //doPnt1.CoCreateInstance(CLSID_PointDisplayObject);
   //doPnt1->SetID(m_DisplayObjectID++);
   //doPnt1->SetPosition(pntXBeam,FALSE,FALSE);
   //doPnt1->SetSelectionType(stAll);

   //CComPtr<IShape> xbeamShape;
   //pPier->GetLowerXBeamProfile(&xbeamShape);

   //CComPtr<iShapeDrawStrategy> xbeamDrawStrategy;
   //xbeamDrawStrategy.CoCreateInstance(CLSID_ShapeDrawStrategy);
   //xbeamDrawStrategy->SetShape(xbeamShape);
   //xbeamDrawStrategy->SetSolidLineColor(XBEAM_LINE_COLOR);
   //xbeamDrawStrategy->SetSolidFillColor(XBEAM_FILL_COLOR);
   //xbeamDrawStrategy->DoFill(TRUE);

   //doPnt1->SetDrawingStrategy(xbeamDrawStrategy);

   //CComPtr<iShapeGravityWellStrategy> gravity_well;
   //gravity_well.CoCreateInstance(CLSID_ShapeGravityWellStrategy);
   //gravity_well->SetShape(xbeamShape);
   //doPnt1->SetGravityWellStrategy(gravity_well);

   //displayList->AddDisplayObject(doPnt1);

   //CComQIPtr<iConnectable> connectable1(doPnt1);
   //CComPtr<iSocket> socket1;
   //connectable1->AddSocket(0,pntXBeam,&socket1);

   //CComPtr<IPoint2d> point1;
   //point1.CoCreateInstance(CLSID_Point2d);
   //point1->Move(0,-(H+H1+H2));

   //// create point at top of first column
   //CComPtr<IPoint2d> point2;
   //point2.CoCreateInstance(CLSID_Point2d);
   //point2->Move(leftOverhang,-(H+H1+H2));

   //CComPtr<iPointDisplayObject> doPnt2;
   //doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
   //doPnt2->Visible(FALSE);
   //doPnt2->SetID(m_DisplayObjectID++);
   //doPnt2->SetPosition(point2,FALSE,FALSE);

   //CComQIPtr<iConnectable> connectable2(doPnt2);
   //CComPtr<iSocket> socket2;
   //connectable2->AddSocket(0,point2,&socket2);

   //displayList->AddDisplayObject(doPnt2);

   //// create left overhang member
   //BuildDimensionLine(displayList,point1,point2,leftOverhang);

   //point1 = point2;
   //doPnt1 = doPnt2;
   //connectable1 = connectable2;
   //socket1 = socket2;

   //Float64 x = leftOverhang;
   //for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   //{
   //   Float64 space = (colIdx < nColumns-1 ? pProject->GetSpacing(colIdx) : rightOverhang);
   //   Float64 columnHeight = pProject->GetColumnHeight(colIdx);
   //   CColumnData::ColumnShapeType colShapeType;
   //   Float64 d1, d2;
   //   pProject->GetColumnShape(&colShapeType,&d1,&d2);

   //   // create point at bottom of column
   //   point2.Release();
   //   point2.CoCreateInstance(CLSID_Point2d);
   //   point2->Move(x,-(H+H1+H2+columnHeight));

   //   doPnt2.Release();
   //   doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
   //   doPnt2->Visible(FALSE);
   //   doPnt2->SetID(m_DisplayObjectID++);
   //   doPnt2->SetPosition(point2,FALSE,FALSE);

   //   connectable2.Release();
   //   doPnt2.QueryInterface(&connectable2);
   //   socket2.Release();
   //   connectable2->AddSocket(0,point2,&socket2);

   //   displayList->AddDisplayObject(doPnt2);

   //   // create column member
   //   CComPtr<iLineDisplayObject> doColumn;
   //   doColumn.CoCreateInstance(CLSID_LineDisplayObject);
   //   doColumn->SetID(m_DisplayObjectID++);
   //   doColumn->SetSelectionType(stAll);

   //   CComPtr<iRectangleDrawLineStrategy> drawColumnStrategy;
   //   drawColumnStrategy.CoCreateInstance(CLSID_RectangleDrawLineStrategy);
   //   doColumn->SetDrawLineStrategy(drawColumnStrategy);

   //   drawColumnStrategy->SetWidth(d1);
   //   drawColumnStrategy->SetColor(COLUMN_LINE_COLOR);
   //   drawColumnStrategy->SetLineWidth(COLUMN_LINE_WEIGHT);
   //   drawColumnStrategy->SetFillColor(COLUMN_FILL_COLOR);
   //   drawColumnStrategy->SetDoFill(TRUE);

   //   drawColumnStrategy->PerimeterGravityWell(TRUE);
   //   CComQIPtr<iGravityWellStrategy> gravity_well(drawColumnStrategy);
   //   doColumn->SetGravityWellStrategy(gravity_well);


   //   CComQIPtr<iConnector> connector(doColumn);
   //   CComQIPtr<iPlug> startPlug, endPlug;
   //   connector->GetStartPlug(&startPlug);
   //   connector->GetEndPlug(&endPlug);
   //   DWORD dwCookie;
   //   connectable1->Connect(0,atByID,startPlug,&dwCookie);
   //   connectable2->Connect(0,atByID,endPlug,  &dwCookie);

   //   displayList->AddDisplayObject(doColumn);

   //   // column dimension line
   //   BuildDimensionLine(displayList,point1,point2,columnHeight);

   //   x += space;

   //   // add joint at top of next column
   //   CComPtr<IPoint2d> point3;
   //   point3.CoCreateInstance(CLSID_Point2d);
   //   point3->Move(x,-(H+H1+H2));

   //   CComPtr<iPointDisplayObject> doPnt3;
   //   doPnt3.CoCreateInstance(CLSID_PointDisplayObject);
   //   doPnt3->Visible(FALSE);
   //   doPnt3->SetID(m_DisplayObjectID++);
   //   doPnt3->SetPosition(point3,FALSE,FALSE);

   //   CComQIPtr<iConnectable> connectable3(doPnt3);
   //   CComPtr<iSocket> socket3;
   //   connectable3->AddSocket(0,point3,&socket3);

   //   displayList->AddDisplayObject(doPnt3);

   //   // cross beam dimension
   //   if ( nColumns-1 <= colIdx )
   //   {
   //      Float64 y;
   //      point1->get_Y(&y);
   //      point3->put_Y(y);
   //   }
   //   BuildDimensionLine(displayList,point1,point3,space);

   //   point1 = point3;
   //   doPnt1 = doPnt3;
   //   connectable1 = connectable3;
   //   socket1 = socket3;
   //}
}

void CXBeamRateView::UpdateXBeamDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(XBEAM_DISPLAY_LIST_ID,&displayList);

   CXBeamRateDoc* pDoc = (CXBeamRateDoc*)GetDocument();
   CComPtr<IBroker> pBroker;
   pDoc->GetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRProject,pProject);

   Float64 leftOverhang = pProject->GetXBeamOverhang(pgsTypes::pstLeft);
   Float64 rightOverhang = pProject->GetXBeamOverhang(pgsTypes::pstRight);
   IndexType nColumns = pProject->GetColumnCount();

   Float64 H,W;
   pProject->GetDiaphragmDimensions(&H,&W);

   Float64 H1, H2, X1;
   pProject->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1);

   CComPtr<IPoint2d> point;
   point.CoCreateInstance(CLSID_Point2d);
   point->Move(0,0);

   CComPtr<iPointDisplayObject> doUpperXBeam;
   doUpperXBeam.CoCreateInstance(CLSID_PointDisplayObject);
   doUpperXBeam->SetID(m_DisplayObjectID++);
   doUpperXBeam->SetPosition(point,FALSE,FALSE);
   doUpperXBeam->SetSelectionType(stAll);

   GET_IFACE2(pBroker,IXBRPier,pPier);
   CComPtr<IShape> upperXBeamShape;
   pPier->GetUpperXBeamProfile(&upperXBeamShape);

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

   CComPtr<iPointDisplayObject> doLowerXBeam;
   doLowerXBeam.CoCreateInstance(CLSID_PointDisplayObject);
   doLowerXBeam->SetID(m_DisplayObjectID++);
   doLowerXBeam->SetPosition(point,FALSE,FALSE);
   doLowerXBeam->SetSelectionType(stAll);

   CComPtr<IShape> lowerXBeamShape;
   pPier->GetLowerXBeamProfile(&lowerXBeamShape);

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
}

void CXBeamRateView::UpdateColumnDisplayObjects()
{
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(COLUMN_DISPLAY_LIST_ID,&displayList);

   CXBeamRateDoc* pDoc = (CXBeamRateDoc*)GetDocument();
   CComPtr<IBroker> pBroker;
   pDoc->GetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRProject,pProject);

   Float64 leftOverhang = pProject->GetXBeamOverhang(pgsTypes::pstLeft);
   Float64 rightOverhang = pProject->GetXBeamOverhang(pgsTypes::pstRight);
   IndexType nColumns = pProject->GetColumnCount();

   Float64 H,W;
   pProject->GetDiaphragmDimensions(&H,&W);

   Float64 H1, H2, X1;
   pProject->GetXBeamDimensions(pgsTypes::pstLeft,&H1,&H2,&X1);

   CComPtr<IPoint2d> point1;
   point1.CoCreateInstance(CLSID_Point2d);
   point1->Move(0,-(H+H1+H2));

   CComPtr<iPointDisplayObject> doPnt1;
   doPnt1.CoCreateInstance(CLSID_PointDisplayObject);
   doPnt1->Visible(FALSE);
   doPnt1->SetID(m_DisplayObjectID++);
   doPnt1->SetPosition(point1,FALSE,FALSE);

   displayList->AddDisplayObject(doPnt1);

   CComQIPtr<iConnectable> connectable1(doPnt1);
   CComPtr<iSocket> socket1;
   connectable1->AddSocket(0,point1,&socket1);


   // create point at top of first column
   CComPtr<IPoint2d> point2;
   point2.CoCreateInstance(CLSID_Point2d);
   point2->Move(leftOverhang,-(H+H1+H2));

   CComPtr<iPointDisplayObject> doPnt2;
   doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
   doPnt2->Visible(FALSE);
   doPnt2->SetID(m_DisplayObjectID++);
   doPnt2->SetPosition(point2,FALSE,FALSE);

   CComQIPtr<iConnectable> connectable2(doPnt2);
   CComPtr<iSocket> socket2;
   connectable2->AddSocket(0,point2,&socket2);

   displayList->AddDisplayObject(doPnt2);

   point1 = point2;
   doPnt1 = doPnt2;
   connectable1 = connectable2;
   socket1 = socket2;

   Float64 x = leftOverhang;
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 space = (colIdx < nColumns-1 ? pProject->GetSpacing(colIdx) : rightOverhang);
      Float64 columnHeight = pProject->GetColumnHeight(colIdx);
      CColumnData::ColumnShapeType colShapeType;
      Float64 d1, d2;
      pProject->GetColumnShape(&colShapeType,&d1,&d2);

      // create point at bottom of column
      point2.Release();
      point2.CoCreateInstance(CLSID_Point2d);
      point2->Move(x,-(H+H1+H2+columnHeight));

      doPnt2.Release();
      doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
      doPnt2->Visible(FALSE);
      doPnt2->SetID(m_DisplayObjectID++);
      doPnt2->SetPosition(point2,FALSE,FALSE);

      connectable2.Release();
      doPnt2.QueryInterface(&connectable2);
      socket2.Release();
      connectable2->AddSocket(0,point2,&socket2);

      displayList->AddDisplayObject(doPnt2);

      // create column member
      CComPtr<iLineDisplayObject> doColumn;
      doColumn.CoCreateInstance(CLSID_LineDisplayObject);
      doColumn->SetID(m_DisplayObjectID++);
      doColumn->SetSelectionType(stAll);

      CComPtr<iRectangleDrawLineStrategy> drawColumnStrategy;
      drawColumnStrategy.CoCreateInstance(CLSID_RectangleDrawLineStrategy);
      doColumn->SetDrawLineStrategy(drawColumnStrategy);

      drawColumnStrategy->SetWidth(d1);
      drawColumnStrategy->SetColor(COLUMN_LINE_COLOR);
      drawColumnStrategy->SetLineWidth(COLUMN_LINE_WEIGHT);
      drawColumnStrategy->SetFillColor(COLUMN_FILL_COLOR);
      drawColumnStrategy->SetDoFill(TRUE);

      drawColumnStrategy->PerimeterGravityWell(TRUE);
      CComQIPtr<iGravityWellStrategy> gravity_well(drawColumnStrategy);
      doColumn->SetGravityWellStrategy(gravity_well);


      CComQIPtr<iConnector> connector(doColumn);
      CComQIPtr<iPlug> startPlug, endPlug;
      connector->GetStartPlug(&startPlug);
      connector->GetEndPlug(&endPlug);
      DWORD dwCookie;
      connectable1->Connect(0,atByID,startPlug,&dwCookie);
      connectable2->Connect(0,atByID,endPlug,  &dwCookie);

      displayList->AddDisplayObject(doColumn);

      x += space;

      // add joint at top of next column
      CComPtr<IPoint2d> point3;
      point3.CoCreateInstance(CLSID_Point2d);
      point3->Move(x,-(H+H1+H2));

      CComPtr<iPointDisplayObject> doPnt3;
      doPnt3.CoCreateInstance(CLSID_PointDisplayObject);
      doPnt3->Visible(FALSE);
      doPnt3->SetID(m_DisplayObjectID++);
      doPnt3->SetPosition(point3,FALSE,FALSE);

      CComQIPtr<iConnectable> connectable3(doPnt3);
      CComPtr<iSocket> socket3;
      connectable3->AddSocket(0,point3,&socket3);

      displayList->AddDisplayObject(doPnt3);

      point1 = point3;
      doPnt1 = doPnt3;
      connectable1 = connectable3;
      socket1 = socket3;
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

   CXBeamRateDoc* pDoc = (CXBeamRateDoc*)GetDocument();
   CComPtr<IBroker> pBroker;
   pDoc->GetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRProject,pProject);

   Float64 H,W;
   pProject->GetDiaphragmDimensions(&H,&W);

   GET_IFACE2(pBroker,IXBRPier,pPier);

   IndexType nBearingLines = pPier->GetBearingLineCount();
   for ( IndexType brgLineIdx = 0; brgLineIdx < nBearingLines; brgLineIdx++ )
   {
      IndexType nBearings = pPier->GetBearingCount(brgLineIdx);
      for ( IndexType brgIdx = 0; brgIdx < nBearings; brgIdx++ )
      {
         Float64 Xbrg = pPier->GetBearingLocation(brgLineIdx,brgIdx);

         CComPtr<IPoint2d> pnt;
         pnt.CoCreateInstance(CLSID_Point2d);
         pnt->Move(Xbrg,-H);

         CComPtr<iPointDisplayObject> doPnt;
         doPnt.CoCreateInstance(CLSID_PointDisplayObject);
         //doPnt->Visible(FALSE);
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

   GET_IFACE2(pBroker,IXBRProjectEdit,pProjectEdit);
   pProjectEdit->EditPier(0);
}
