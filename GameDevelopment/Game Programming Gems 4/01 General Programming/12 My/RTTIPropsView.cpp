// RTTIPropsView.cpp : implementation of the CRTTIPropsView class
//

#include "stdafx.h"
#include "RTTIProps.h"

#include "RTTIPropsDoc.h"
#include "RTTIPropsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsView

IMPLEMENT_DYNCREATE(CRTTIPropsView, CView)

BEGIN_MESSAGE_MAP(CRTTIPropsView, CView)
	//{{AFX_MSG_MAP(CRTTIPropsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsView construction/destruction

CRTTIPropsView::CRTTIPropsView()
{
	// TODO: add construction code here

}

CRTTIPropsView::~CRTTIPropsView()
{
}

BOOL CRTTIPropsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsView drawing

void CRTTIPropsView::OnDraw(CDC* pDC)
{
	CRTTIPropsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsView diagnostics

#ifdef _DEBUG
void CRTTIPropsView::AssertValid() const
{
	CView::AssertValid();
}

void CRTTIPropsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRTTIPropsDoc* CRTTIPropsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsView message handlers
