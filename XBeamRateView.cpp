// XBeamRateView.cpp : implementation of the CXBeamRateView class
//

#include "stdafx.h"

#include "XBeamRateDoc.h"
#include "XBeamRateView.h"

#include <IFace\Project.h>
#include <IFace\AnalysisResults.h>
#include <IFace\LoadRating.h>
#include <MFCTools\Format.h>

#include <EAF\EAFDisplayUnits.h>

#include <Colors.h>
#define SELECTED_OBJECT_LINE_COLOR     RED4
#define SELECTED_OBJECT_FILL_COLOR     BLUE
#define COLUMN_LINE_COLOR              DARKSLATEGRAY4
#define XBEAM_LINE_COLOR               DARKSLATEGRAY4

#define COLUMN_LINE_WEIGHT             3
#define XBEAM_LINE_WEIGHT              3

#define DISPLAY_LIST_ID 0

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
// CXBeamRateView drawing

//void CXBeamRateView::OnDraw(CDC* pDC)
//{
//	CXBeamRateDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
//	// TODO: add draw code for native data here
//
//   CComPtr<IBroker> pBroker;
//   EAFGetBroker(&pBroker);
//   GET_IFACE2(pBroker,IXBRProject,pProject);
//   CString strProjectName = pProject->GetProjectName();
//
//   GET_IFACE2(pBroker,IAnalysisResults,pResults);
//   Float64 value = pResults->GetResult();
//
//   GET_IFACE2(pBroker,ILoadRating,pRating);
//   Float64 RF = pRating->GetRatingFactor();
//
//   CString strMsg;
//   strMsg.Format(_T("%s\nM = %f\nRF = %f"),strProjectName,value,RF);
//
//   MultiLineTextOut(pDC,0,0,strMsg);
//}

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
   displayList->SetID(DISPLAY_LIST_ID);
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
   CWaitCursor wait;

   CComPtr<iDisplayMgr> dispMgr;
   GetDisplayMgr(&dispMgr);

   CDManipClientDC dc(this);

   dispMgr->ClearDisplayObjects();

   CComPtr<iDisplayList> displayList;
   dispMgr->FindDisplayList(DISPLAY_LIST_ID,&displayList);

   CXBeamRateDoc* pDoc = (CXBeamRateDoc*)GetDocument();
   CComPtr<IBroker> pBroker;
   pDoc->GetBroker(&pBroker);

   GET_IFACE2(pBroker,IXBRProject,pProject);

   Float64 leftOverhang = pProject->GetLeftOverhang();
   Float64 rightOverhang = pProject->GetRightOverhang();
   IndexType nColumns = pProject->GetColumnCount();

   // Start point at left end of cross beam
   CComPtr<IPoint2d> point1;
   point1.CoCreateInstance(CLSID_Point2d);
   point1->Move(0,0);

   CComPtr<iPointDisplayObject> doPnt1;
   doPnt1.CoCreateInstance(CLSID_PointDisplayObject);
   doPnt1->SetID(m_DisplayObjectID++);
   doPnt1->SetPosition(point1,FALSE,FALSE);

   CComQIPtr<iConnectable> connectable1(doPnt1);
   CComPtr<iSocket> socket1;
   connectable1->AddSocket(0,point1,&socket1);

   displayList->AddDisplayObject(doPnt1);

   // create point at top of first column
   CComPtr<IPoint2d> point2;
   point2.CoCreateInstance(CLSID_Point2d);
   point2->Move(leftOverhang,0);

   CComPtr<iPointDisplayObject> doPnt2;
   doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
   doPnt2->SetID(m_DisplayObjectID++);
   doPnt2->SetPosition(point2,FALSE,FALSE);

   CComQIPtr<iConnectable> connectable2(doPnt2);
   CComPtr<iSocket> socket2;
   connectable2->AddSocket(0,point2,&socket2);

   displayList->AddDisplayObject(doPnt2);

   // create left overhang member
   CComPtr<iLineDisplayObject> doLine1;
   doLine1.CoCreateInstance(CLSID_LineDisplayObject);
   doLine1->SetID(m_DisplayObjectID++);

   CComPtr<iDrawLineStrategy> dlStrategy;
   doLine1->GetDrawLineStrategy(&dlStrategy);
   CComQIPtr<iSimpleDrawLineStrategy> strategy(dlStrategy);
   strategy->SetColor(XBEAM_LINE_COLOR);
   strategy->SetWidth(XBEAM_LINE_WEIGHT);
   dlStrategy.Release();
   strategy.Release();

   CComQIPtr<iConnector> connector1(doLine1);
   CComQIPtr<iPlug> startPlug,endPlug;
   connector1->GetStartPlug(&startPlug);
   connector1->GetEndPlug(&endPlug);
   DWORD dwCookie;
   connectable1->Connect(0,atByID,startPlug,&dwCookie);
   connectable2->Connect(0,atByID,endPlug,  &dwCookie);

   displayList->AddDisplayObject(doLine1);

   BuildDimensionLine(displayList,point1,point2,leftOverhang);

   point1 = point2;
   doPnt1 = doPnt2;
   connectable1 = connectable2;
   socket1 = socket2;

   Float64 x = leftOverhang;
   for ( IndexType colIdx = 0; colIdx < nColumns; colIdx++ )
   {
      Float64 space = (colIdx < nColumns-1 ? pProject->GetSpacing(colIdx) : rightOverhang);
      Float64 columnHeight = pProject->GetColumnHeight(colIdx);

      // create point at bottom of column
      point2.Release();
      point2.CoCreateInstance(CLSID_Point2d);
      point2->Move(x,-columnHeight);

      doPnt2.Release();
      doPnt2.CoCreateInstance(CLSID_PointDisplayObject);
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

      CComQIPtr<iConnector> connector(doColumn);
      startPlug.Release();
      endPlug.Release();
      connector->GetStartPlug(&startPlug);
      connector->GetEndPlug(&endPlug);
      connectable1->Connect(0,atByID,startPlug,&dwCookie);
      connectable2->Connect(0,atByID,endPlug,  &dwCookie);

      doColumn->GetDrawLineStrategy(&dlStrategy);
      dlStrategy.QueryInterface(&strategy);
      strategy->SetColor(COLUMN_LINE_COLOR);
      strategy->SetWidth(COLUMN_LINE_WEIGHT);
      dlStrategy.Release();
      strategy.Release();

      displayList->AddDisplayObject(doColumn);

      BuildDimensionLine(displayList,point1,point2,columnHeight);

      x += space;

      // add joint at top of next column
      CComPtr<IPoint2d> point3;
      point3.CoCreateInstance(CLSID_Point2d);
      point3->Move(x,0);

      CComPtr<iPointDisplayObject> doPnt3;
      doPnt3.CoCreateInstance(CLSID_PointDisplayObject);
      doPnt3->SetID(m_DisplayObjectID++);
      doPnt3->SetPosition(point3,FALSE,FALSE);

      CComQIPtr<iConnectable> connectable3(doPnt3);
      CComPtr<iSocket> socket3;
      connectable3->AddSocket(0,point3,&socket3);

      displayList->AddDisplayObject(doPnt3);

      // create cross beam member
      CComPtr<iLineDisplayObject> doXBeam;
      doXBeam.CoCreateInstance(CLSID_LineDisplayObject);
      doXBeam->SetID(m_DisplayObjectID++);

      doXBeam->GetDrawLineStrategy(&dlStrategy);
      dlStrategy.QueryInterface(&strategy);
      strategy->SetColor(XBEAM_LINE_COLOR);
      strategy->SetWidth(XBEAM_LINE_WEIGHT);
      dlStrategy.Release();
      strategy.Release();

      CComQIPtr<iConnector> connector3(doXBeam);
      startPlug.Release();
      endPlug.Release();
      connector3->GetStartPlug(&startPlug);
      connector3->GetEndPlug(&endPlug);
      connectable1->Connect(0,atByID,startPlug,&dwCookie);
      connectable3->Connect(0,atByID,endPlug,  &dwCookie);

      displayList->AddDisplayObject(doXBeam);

      BuildDimensionLine(displayList,point1,point3,space);

      point1 = point3;
      doPnt1 = doPnt3;
      connectable1 = connectable3;
      socket1 = socket3;
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
