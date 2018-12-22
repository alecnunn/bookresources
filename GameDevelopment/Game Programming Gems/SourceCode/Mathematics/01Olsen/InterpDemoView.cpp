/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// InterpDemoView.cpp : implementation of the CInterpDemoView class
//

#include "stdafx.h"
#include "InterpDemo.h"

#include "InterpDemoDoc.h"
#include "InterpDemoView.h"

#include "interp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoView

IMPLEMENT_DYNCREATE(CInterpDemoView, CView)

BEGIN_MESSAGE_MAP(CInterpDemoView, CView)
	//{{AFX_MSG_MAP(CInterpDemoView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoView construction/destruction

CInterpDemoView::CInterpDemoView()
{
	// TODO: add construction code here

}

CInterpDemoView::~CInterpDemoView()
{
}

BOOL CInterpDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoView drawing

void CInterpDemoView::OnDraw(CDC* pDC)
{
	CInterpDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CString strMessage;
	int i; // for looping;
	int dataPointCount;

	strMessage = "100 samples";
	pDC->TextOut(800, 70, strMessage);
	strMessage = "10 samples";
	pDC->TextOut(800, 240, strMessage);

	// Linear Interpolation
	strMessage = "Linear Interpolation";
	CLinearInterpolation linear;
	dataPointCount = 100;
	linear.Setup(0.0, 100.0, 10.0);
	pDC->TextOut(10,5,strMessage);
	pDC->MoveTo(10, 30 + (int)linear.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		linear.Interpolate( 10.0f/dataPointCount );
		pDC->LineTo(10 + i, 30 + (int)linear.GetValue());
	}

	dataPointCount = 10;
	linear.Setup(0.0, 100.0, 10.0);
	pDC->MoveTo(10, 200 + (int)linear.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		linear.Interpolate( 10.0f/dataPointCount );
		pDC->LineTo(10 + i*10, 200 + (int)linear.GetValue());
	}

	// Ease Out Shift Interpolation (scaled up since we lose resolution with ints)
	strMessage = "Ease Out Shift";
	pDC-> TextOut(200, 5, strMessage);
	CEaseOutShiftInterpolation easeOutShift;
	dataPointCount = 100;
	easeOutShift.Setup(0, 1000, 5);
	pDC->MoveTo(200, 30 + easeOutShift.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeOutShift.Interpolate();
		pDC->LineTo(200 + i, 30 + (int)easeOutShift.GetValue()/10);
	}

	dataPointCount = 10;
	easeOutShift.Setup(0, 1000, 2); // Must reduce the shift value to get an equivalent curve.
	pDC->MoveTo(200, 200 + easeOutShift.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeOutShift.Interpolate();
		pDC->LineTo(200 + i*10, 200 + (int)easeOutShift.GetValue()/10);
	}

	// Ease out with divide and floats instead of integer with shifts.
	strMessage = "Ease Out Divide";
	pDC-> TextOut(400, 5, strMessage);
	CEaseOutDivideInterpolation easeOutDivide;
	dataPointCount = 100;
	easeOutDivide.Setup(0.0f, 100.0f, 25.0f);
	pDC->MoveTo(400, 30 + (int)easeOutDivide.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeOutDivide.Interpolate();
		pDC->LineTo(400 + i, 30 + (int)easeOutDivide.GetValue());
	}

	dataPointCount = 10;
	easeOutDivide.Setup(0.0f, 100.0f, 3.5f);
	pDC->MoveTo(400, 200 + (int)easeOutDivide.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeOutDivide.Interpolate();
		pDC->LineTo(400 + i*10, 200 + (int)easeOutDivide.GetValue());
	}

	// Physics based ease-in, ease-out which calculates optimal acceleration rate.
	strMessage = "Ease in/out Interpolation";
	pDC->TextOut(600,5,strMessage);
	CEaseInOutInterpolation easeInOut;
	dataPointCount = 100;
	easeInOut.Setup(0.0, 100.0, 10.0);
	pDC->MoveTo(600, 30 + (int)easeInOut.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeInOut.Interpolate(10.0f/dataPointCount);
		pDC->LineTo(600 + i, 30 + (int)easeInOut.GetValue());
	}

	dataPointCount = 10;
	easeInOut.Setup(0.0, 100.0, 10.0);
	pDC->MoveTo(600, 200 + (int)easeInOut.GetValue());
	for(i = 1; i < dataPointCount; i++)
	{
		easeInOut.Interpolate(10.0f/dataPointCount);
		pDC->LineTo(600 + i*10, 200 + (int)easeInOut.GetValue());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoView diagnostics

#ifdef _DEBUG
void CInterpDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CInterpDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CInterpDemoDoc* CInterpDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CInterpDemoDoc)));
	return (CInterpDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoView message handlers
