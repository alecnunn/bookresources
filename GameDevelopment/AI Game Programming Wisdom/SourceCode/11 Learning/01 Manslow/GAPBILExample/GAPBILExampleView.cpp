//GAPBILExample
//Copyright John Manslow
//29/09/2001

// GAPBILExampleView.cpp : implementation of the CGAPBILExampleView class
//

#include "stdafx.h"
#include "GAPBILExample.h"

#include "GAPBILExampleDoc.h"
#include "GAPBILExampleView.h"

#include "CWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleView

IMPLEMENT_DYNCREATE(CGAPBILExampleView, CView)

BEGIN_MESSAGE_MAP(CGAPBILExampleView, CView)
	//{{AFX_MSG_MAP(CGAPBILExampleView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CWorld *pWorld;
extern void Evaluate(void);
CGAPBILExampleView *pView;

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleView construction/destruction

CGAPBILExampleView::CGAPBILExampleView()
{
	//This is used in the document to draw to the window
	pView=this;
}

CGAPBILExampleView::~CGAPBILExampleView()
{
}

BOOL CGAPBILExampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleView drawing

void CGAPBILExampleView::OnDraw(CDC* pDC)
{
	CGAPBILExampleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//Redraw the world
	pWorld->Draw(pDC);

	//Set the timer that calls the main loop
	SetTimer(0,0,NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleView diagnostics

#ifdef _DEBUG
void CGAPBILExampleView::AssertValid() const
{
	CView::AssertValid();
}

void CGAPBILExampleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGAPBILExampleDoc* CGAPBILExampleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGAPBILExampleDoc)));
	return (CGAPBILExampleDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleView message handlers

void CGAPBILExampleView::OnTimer(UINT nIDEvent) 
{
	KillTimer(0);	
	CView::OnTimer(nIDEvent);
	Evaluate();
	SetTimer(0,0,NULL);
}
