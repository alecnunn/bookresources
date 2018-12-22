// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "ChildFrm.h"

#include "3DView.h"
#include "CommandView.h"
#include "LogReportView.h"
#include "PropView.h"
#include "SceneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	return TRUE;
}

void CChildFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Modify this function to change how the frame is activated.

	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
  {
  RECT Rect;
  GetParent()->GetClientRect(&Rect);                        // so we have the correct size even if the frame is not maximized yet
  SIZE csTaille;
	csTaille.cy = Rect.bottom;

  if     (Rect.right > 1400) csTaille.cx = 1024;            // 1600*1200
  else if(Rect.right > 1200) csTaille.cx =  800;            // 1280*1024 (very good)
  else if(Rect.right > 1100) csTaille.cx =  640;            // 1152* 864 (     good)
  else if(Rect.right >  900) csTaille.cx =  512;            // 1024* 768
  else if(Rect.right >  700) csTaille.cx =  400;            //  800* 600
  else                       csTaille.cx =  320;            //  640* 480
  csTaille.cx += 4*0;
  csTaille.cy  = long(csTaille.cx*0.75f+4*0);

  SIZE Size = csTaille;

  // splitters

	m_splitGeneral.CreateStatic(this,1,2);
  m_splitLeft   .CreateStatic(&m_splitGeneral,2,1,WS_CHILD|WS_VISIBLE,m_splitGeneral.IdFromRowCol(0,0));
  m_splitRight  .CreateStatic(&m_splitGeneral,2,1,WS_CHILD|WS_VISIBLE,m_splitGeneral.IdFromRowCol(0,1));
  m_splitRightUp.CreateStatic(&m_splitRight,  1,2,WS_CHILD|WS_VISIBLE,m_splitRight  .IdFromRowCol(0,0));

  // views

  m_splitLeft.CreateView(0,0,RUNTIME_CLASS(C3DView),csTaille,pContext);
  csTaille.cy = Rect.bottom-csTaille.cy;
  m_splitLeft.CreateView(1,0,RUNTIME_CLASS(CPropView),csTaille,pContext);

  csTaille.cx = Rect.right-csTaille.cx;
  csTaille.cy = Rect.bottom/2;
  m_splitRight.CreateView(1,0,RUNTIME_CLASS(CLogReportView),csTaille,pContext);

  csTaille.cx /= 2;
  m_splitRightUp.CreateView(0,0,RUNTIME_CLASS(CSceneView),  csTaille,pContext);
  m_splitRightUp.CreateView(0,1,RUNTIME_CLASS(CCommandView),csTaille,pContext);

  // resize

  m_splitGeneral.SetColumnInfo(0,Size.cx,0);
  m_splitGeneral.RecalcLayout();
  m_splitRight  .SetRowInfo(0,Rect.bottom/2,0);
  m_splitRight  .RecalcLayout();

  return TRUE;
  }
  