//GPExample
//Copyright John Manslow
//29/09/2001

// GPExampleView.cpp : implementation of the CGPExampleView class
//

#include "stdafx.h"
#include "GPExample.h"

#include "GPExampleDoc.h"
#include "GPExampleView.h"

#include "CWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGPExampleView

IMPLEMENT_DYNCREATE(CGPExampleView, CView)

BEGIN_MESSAGE_MAP(CGPExampleView, CView)
	//{{AFX_MSG_MAP(CGPExampleView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CWorld *pWorld;
extern void Evaluate(void);
CGPExampleView *pView;

/////////////////////////////////////////////////////////////////////////////
// CGPExampleView construction/destruction

CGPExampleView::CGPExampleView()
{
	//This is used in the document to draw to the window
	pView=this;
}

CGPExampleView::~CGPExampleView()
{
}

BOOL CGPExampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGPExampleView drawing

void CGPExampleView::OnDraw(CDC* pDC)
{
	CGPExampleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//Redraw the world
	pWorld->Draw(pDC);

	//Set the timer that calls the main loop
	SetTimer(0,0,NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CGPExampleView diagnostics

#ifdef _DEBUG
void CGPExampleView::AssertValid() const
{
	CView::AssertValid();
}

void CGPExampleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGPExampleDoc* CGPExampleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGPExampleDoc)));
	return (CGPExampleDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGPExampleView message handlers

void CGPExampleView::OnTimer(UINT nIDEvent) 
{
	KillTimer(0);	
	CView::OnTimer(nIDEvent);
	Evaluate();
	SetTimer(0,0,NULL);
}
