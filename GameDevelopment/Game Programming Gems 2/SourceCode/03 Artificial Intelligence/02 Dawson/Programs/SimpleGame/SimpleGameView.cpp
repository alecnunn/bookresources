/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// SimpleGameView.cpp : implementation of the CSimpleGameView class
//

#include "stdafx.h"
#include "SimpleGame.h"

#include "SimpleGameDoc.h"
#include "SimpleGameView.h"

#include <imagelibrary/csbitmap.h>
#include "gamelibrary/objectfactories.h"
#include "gameworld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView

IMPLEMENT_DYNCREATE(CSimpleGameView, CView)

BEGIN_MESSAGE_MAP(CSimpleGameView, CView)
	//{{AFX_MSG_MAP(CSimpleGameView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView construction/destruction

CSimpleGameView::CSimpleGameView()
{
	// TODO: add construction code here

}

CSimpleGameView::~CSimpleGameView()
{
}

BOOL CSimpleGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView drawing

void CSimpleGameView::OnDraw(CDC* pDC)
{
	CSimpleGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	const CSBitmap* pBitmap = GetGameWorld().GetBitmap();
	CRect clientRect;
	GetClientRect(&clientRect);
	const int bitmapWidth = pBitmap->GetWidth();
	const int bitmapHeight = pBitmap->GetHeight();
	const int xOffset = (clientRect.Width() - bitmapWidth) / 2;
	const int yOffset = (clientRect.Height() - bitmapHeight) / 2;
	pBitmap->Draw(*pDC, xOffset, yOffset, bitmapWidth, bitmapHeight, 0, 0);

	// Clear the areas of the window that are not covered by the game bitmap.
	CRgn clientRegion, bitmapRegion;
	clientRegion.CreateRectRgnIndirect(&clientRect);
	bitmapRegion.CreateRectRgn(xOffset, yOffset, xOffset + bitmapWidth, yOffset + bitmapHeight);
	clientRegion.CombineRgn(&clientRegion, &bitmapRegion, RGN_DIFF);
	pDC->SelectClipRgn(&clientRegion);
	CBrush	outline(RGB(128, 128, 128));
	pDC->FillRect(&clientRect, &outline);
	pDC->SelectClipRgn(0);
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView printing

BOOL CSimpleGameView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSimpleGameView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSimpleGameView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView diagnostics

#ifdef _DEBUG
void CSimpleGameView::AssertValid() const
{
	CView::AssertValid();
}

void CSimpleGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSimpleGameDoc* CSimpleGameView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSimpleGameDoc)));
	return (CSimpleGameDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameView message handlers

int CSimpleGameView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetGameWorld().StartRecordingInput(*this);
	SetTimer(10, 1000 / k_updateFrameRate, 0);
	
	return 0;
}

void CSimpleGameView::OnDestroy() 
{
	GameInputManager::GetSingleton().StopRecordingInput();
	CView::OnDestroy();
}

/*
The timer is just a simple way of cooperating with MFC. I prefer it to
using idle times. It's not really appropriate for games, but neither is
MFC, and that's the only time I use it.

The frequency of the timer events dictates the maximum frame rate, but
not the maximum game state update rate. The two can be quite
different.
*/
void CSimpleGameView::OnTimer(UINT nIDEvent) 
{
	static bool InHere = false;
	if (InHere)
		return;	// Avoid nasty recursion - happens during asserts.

	// A lot of this code would go away if I didn't try to center my game world
	// in a sizeable window. Oh well - I've done it now!
	CRect clientRect;
	GetClientRect(&clientRect);
	const CSBitmap* pBitmap = GetGameWorld().GetBitmap();
	const int bitmapWidth = pBitmap->GetWidth();
	const int bitmapHeight = pBitmap->GetHeight();
	const int xOffset = (clientRect.Width() - bitmapWidth) / 2;
	const int yOffset = (clientRect.Height() - bitmapHeight) / 2;
	GetGameWorld().SetOffset(xOffset, yOffset);

	CSimpleGameDoc* pDoc = GetDocument();
	// Process all queued up input.
#ifdef	USING_MULTIMEDIA_TIMERS
	while (GameInputManager::GetSingleton().NextInput())
		GetGameWorld().ProcessFrame();
#else
	GameInputManager::GetSingleton().NextInput();
	GetGameWorld().ProcessFrame();
#endif
	// Then draw the frame once.
	GetGameWorld().RenderFrame();

	InHere = true;
	static double lastFrameTime;
	static double lastFrameRate;
	double currentTime = GetSeconds();
	if (lastFrameTime)
	{
		char	buffer[100];
		double frameRate = 1 / (currentTime - lastFrameTime);
		if (lastFrameRate == 0)
			lastFrameRate = frameRate;
		const double currentWeight = 0.05;
		frameRate = lastFrameRate + (frameRate - lastFrameRate) * currentWeight;
		sprintf(buffer, "%1.1f fps", frameRate);
		lastFrameRate = frameRate;
		CRect clientRect;
		GetClientRect(&clientRect);
		SetBkMode(pBitmap->GetDrawSurface(), TRANSPARENT);
		DrawText(pBitmap->GetDrawSurface(), buffer, -1, &clientRect, DT_LEFT);
		//tempDC.DrawText(buffer, -1, &clientRect, DT_LEFT);
		GdiFlush();
	}
	lastFrameTime = currentTime;
	ObjectFrameEnd();

	CClientDC tempDC(this);
	OnDraw(&tempDC);

	CView::OnTimer(nIDEvent);
	InHere = false;
}

BOOL CSimpleGameView::OnEraseBkgnd(CDC* pDC) 
{
	// Please do not erase the background!
	return TRUE;
}
