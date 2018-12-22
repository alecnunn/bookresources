// 3DView.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"
#include "3DView.h"

#include "CustomMsgs.h"

#include "Engine.h"
#include "EngineCamera.h"

#include "Renderer.h"
#include "SwapChain.h"

#include "EditorTool.h"
#include "MoveCameraTool.h"
#include "MoveSelectionTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DView

IMPLEMENT_DYNCREATE(C3DView, CView)

C3DView::C3DView()
  {
  m_u32Renderer = 0;

  m_hArrow      = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
  m_hSizeAll    = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
  m_hSizeNS     = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
  m_hSizeWE     = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
  m_hCursor     = m_hArrow;
  }

C3DView::~C3DView()
  {
  if(m_u32Renderer) GPG4::CEngine::GetEngine()->GetRenderer()->ReleaseSwapChain(m_u32Renderer);

  DestroyCursor(m_hArrow);
  DestroyCursor(m_hSizeAll);
  DestroyCursor(m_hSizeNS);
  DestroyCursor(m_hSizeWE);
  }

void C3DView::OnDestroy() 
  {
  SetTool(NULL);
	CView::OnDestroy();
  }

BEGIN_MESSAGE_MAP(C3DView, CView)
	//{{AFX_MSG_MAP(C3DView)
  ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_3D_MOVECAMERA, On3dMovecamera)
	ON_UPDATE_COMMAND_UI(ID_3D_MOVECAMERA, OnUpdate3dMovecamera)
	ON_COMMAND(ID_3D_PERSPECTIVE, On3dPerspective)
	ON_UPDATE_COMMAND_UI(ID_3D_PERSPECTIVE, OnUpdate3dPerspective)
	ON_COMMAND(ID_3D_MOVESEL, On3dMovesel)
	ON_UPDATE_COMMAND_UI(ID_3D_MOVESEL, OnUpdate3dMovesel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DView drawing

void C3DView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// C3DView diagnostics

#ifdef _DEBUG
void C3DView::AssertValid() const
{
	CView::AssertValid();
}

void C3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRTTIPropsDoc* C3DView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C3DView message handlers

BOOL C3DView::OnEraseBkgnd(CDC* pDC) 
  {
  DrawFrame();
  return TRUE;
  //return CView::OnEraseBkgnd(pDC);
  }

//
// contextual menu
//

void C3DView::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  u32 u32Res = 0;
  CRTTIPropsDoc* pDoc = GetDocument();
  if(pDoc->GetTool()) u32Res = pDoc->GetTool()->OnRButtonDown(nFlags,point);

  // commands menu

  if(!u32Res)
    {
    CMenu	Menu;
    Menu.LoadMenu(IDR_CONTEXT_3DCOMMANDS);
    CMenu* pMenu = Menu.GetSubMenu(0);
    POINT	Pt;
    GetCursorPos(&Pt);

    if(pMenu) pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,Pt.x,Pt.y,/*this*/AfxGetMainWnd(),NULL);
    }

	CView::OnRButtonDown(nFlags, point);
  }

//

void C3DView::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  if(GetDocument()->GetTool()) GetDocument()->GetTool()->OnRButtonUp(nFlags,point);
	CView::OnRButtonUp(nFlags, point);
  }

//

void C3DView::OnRButtonDblClk(UINT nFlags, CPoint point) 
  {
  if(GetDocument()->GetTool()) GetDocument()->GetTool()->OnRButtonDblClk(nFlags,point);
	CView::OnRButtonDblClk(nFlags, point);
  }

//

void C3DView::On3dMovecamera() 
  {
  if(!m_spCurrentCam) return;
  SetTool(new GPG4::CMoveCameraTool(this));
  }


void C3DView::OnUpdate3dMovecamera(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(false);

  if(!m_spCurrentCam) return;
  if(!m_spCurrentCam->GetEngineObj()) return;

  pCmdUI->Enable(true);
  }

//

void C3DView::On3dPerspective() 
  {
  GetDocument()->TogglePerspective(m_spCurrentCam);
  }


void C3DView::OnUpdate3dPerspective(CCmdUI* pCmdUI) 
  {
  GetDocument()->UpdateCameraPerspective(pCmdUI,m_spCurrentCam);
  }

//

void C3DView::On3dMovesel() 
  {
  SetTool(new GPG4::CMoveSelectionTool(this));
  }


void C3DView::OnUpdate3dMovesel(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(true);
  }

//
// keys
//

void C3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  UINT nChar2 = nChar;
  if(pDoc->GetTool()) nChar2 = pDoc->GetTool()->OnKeyDown(nChar,nRepCnt,nFlags);

  switch(nChar2)
    {
    // ortho scale

    case VK_ADD :
      {
      if(!m_spCurrentCam) break;
      GPG4::CEngineCamera* pCam = DYNAMIC_CAST(GPG4::CEngineCamera,m_spCurrentCam->GetEngineObj());
      if(!pCam || pCam->IsPerspective()) break;

      GPG4::CMoveCameraTool MoveCam(this);
      MoveCam.MoveCamera(GPG4::CVect3D(0.f,0.f,-1.f));
      pDoc->SetModifiedFlag();
      break;
      }

    case VK_SUBTRACT :
      {
      if(!m_spCurrentCam) break;
      GPG4::CEngineCamera* pCam = DYNAMIC_CAST(GPG4::CEngineCamera,m_spCurrentCam->GetEngineObj());
      if(!pCam || pCam->IsPerspective()) break;

      GPG4::CMoveCameraTool MoveCam(this);
      MoveCam.MoveCamera(GPG4::CVect3D(0.f,0.f,+1.f));
      pDoc->SetModifiedFlag();
      break;
      }

    // perspective

    case 'P' : On3dPerspective(); break;

    // tools

    case VK_RETURN : 
      {
      if(pDoc->GetTool()) SetTool(NULL);                    // stop tool, validate action
      else
        {
        GPG4::CEngineCamera* pCamera = NULL;
        if(m_spCurrentCam)   pCamera = DYNAMIC_CAST(GPG4::CEngineCamera,m_spCurrentCam->GetEngineObj());

        SetTool(new GPG4::CMoveCameraTool(this));
        }
      break;
      }

    case VK_ESCAPE :
      {
      if(pDoc->GetTool())
        {
        pDoc->GetTool()->Cancel();                          // cancel action
        SetTool(NULL);                                      // stop tool
        }
      break;
      }

    // selection

    case VK_SPACE :
      {
      if(pDoc->GetTool())
        {
        if(pDoc->GetTool()->GetName() == "MoveSelection") SetTool(NULL);
        }
      else SetTool(new GPG4::CMoveSelectionTool(this));
      break;
      }
    }

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
  }

//
// msgs
//

void C3DView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  // ! be careful: pHint is NOT necessarily a CEditorSceneItem derived object !

  switch(lHint)
    {
    case SCENE_DBLCLK :
      {
      GPG4::CEditorSceneItem* pItem = (GPG4::CEditorSceneItem*)pHint;
      GPG4::CEditorObj*       pObj  = (pItem ? pItem->GetObject() : NULL);
      GPG4::CEditorCamera*    pCam = DYNAMIC_CAST(GPG4::CEditorCamera,pObj);
      if(pCam) SetCurrentCamera(pCam); 
      break;
      }

    case CHANGE_CURSOR :
      {
      u32 u32Cursor = (u32)pHint;
      switch(u32Cursor)
        {
        case CURSOR_ARROW   :
        case IDC_ARROW      : m_hCursor = m_hArrow;   break;
        case CURSOR_SIZEALL :
        case IDC_SIZEALL    : m_hCursor = m_hSizeAll; break;
        case CURSOR_VERTI   :
        case IDC_SIZENS     : m_hCursor = m_hSizeNS;  break;
        case CURSOR_HORIZ   :
        case IDC_SIZEWE     : m_hCursor = m_hSizeWE;  break;
        }
      SetCursor(m_hCursor);
      break;
      }

    case IDLE :
      {
      DrawFrame();
      break;
      }

    // tools

    case TOOL_NONE          : SetTool(NULL); break;
    case TOOL_MOVECAMERA    : SetTool(new GPG4::CMoveCameraTool   (this)); break;
    case TOOL_MOVESELECTION : SetTool(new GPG4::CMoveSelectionTool(this)); break;
    }
  }

//

void C3DView::SetCurrentCamera(GPG4::CEditorCamera* pCamera)
  {
  if(m_spCurrentCam == pCamera) return;
  m_spCurrentCam = pCamera;

  CString strMsg("display view from [");
  strMsg += (pCamera ? pCamera->GetName() : "NO camera");
  strMsg += "]";

  CRTTIPropsDoc* pDoc = GetDocument();
  _DLOG2_((*pDoc->GetLogger()),(LPCTSTR)strMsg,"3d op");
  }


GPG4::CEditorCamera* C3DView::GetCurrentCamera() const
  {
  return m_spCurrentCam;
  }

//
// draw
//

bool ObjIsSelected(GPG4::CEngineObj* pEngineObj)
  {                                                                   // draw 3D selected obj
  if(!pEngineObj) return false;

  GPG4::CEditorObj* pEditObj = pEngineObj->GetEditorObj();
  if(!pEditObj) return false;
  return pEditObj->IsSelected();
  }

  //

bool ObjIsNotSelected(GPG4::CEngineObj* pEngineObj)
  {                                                                   // draw 3D non selected obj
  if(!pEngineObj) return false;

  GPG4::CEditorObj* pEditObj = pEngineObj->GetEditorObj();
  if(!pEditObj) return false;
  return !pEditObj->IsSelected();
  }

//

void C3DView::DrawFrame()
  {
  // renderer

  if(!m_u32Renderer && !CreateRenderer())
    {
    MY_ASSERT(false,"CreateRenderer failed");
    return;
    }

  GPG4::IRenderer* pRenderer = GPG4::CEngine::GetEngine()->GetRenderer();
  if(!pRenderer->SelectSwapChain(m_u32Renderer))
    {
    MY_ASSERT(false,"SelectSwapChain failed");
    return;
    }

  GPG4::CEngineNode* pScene = GetDocument()->GetScene();

  // camera

  GPG4::CEngineCamera* pCam = NULL;
  if(m_spCurrentCam)   pCam = DYNAMIC_CAST(GPG4::CEngineCamera,m_spCurrentCam->GetEngineObj());

  if(pCam) pRenderer->SetCamera(pCam);

  // draw

  MY_ASSERT(pRenderer->BeginScene(),"BeginScene failed");
  MY_ASSERT(pRenderer->Clear(),     "Clear failed");

  // 3D

  if(pScene)
    {
    pScene->Draw(pRenderer,GPG4::CEngineNode::NONE,    &ObjIsNotSelected);
    pScene->Draw(pRenderer,GPG4::CEngineNode::SELECTED,&ObjIsSelected);
    }

  // swap

  MY_ASSERT(pRenderer->EndScene(),"EndScene failed");
  MY_ASSERT(pRenderer->Swap(),    "Swap failed");
  }

//

bool C3DView::CreateRenderer()
  {
  GPG4::IRenderer* pRenderer = GPG4::CEngine::GetEngine()->GetRenderer();
  MY_ASSERT(pRenderer,"no renderer");
  if(!pRenderer) return false;

  m_u32Renderer = pRenderer->CreateSwapChain(m_hWnd);
  if(!m_u32Renderer) return false;
  if(!pRenderer->SelectSwapChain(m_u32Renderer)) return true;

  pRenderer->SetClearColor(GPG4::CVect3D(0.35f,0.f,0.f));
  return true;
  }

//
//

void C3DView::OnMouseMove(UINT nFlags, CPoint point) 
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(pDoc->GetTool()) pDoc->GetTool()->OnMouseMove(nFlags,point);
	CView::OnMouseMove(nFlags, point);
  }

//

void C3DView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
  {
  UINT nChar2 = nChar;
  if(GetDocument()->GetTool()) nChar2 = GetDocument()->GetTool()->OnKeyUp(nChar,nRepCnt,nFlags);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
  }

//

void C3DView::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(pDoc->GetTool()) pDoc->GetTool()->OnLButtonDown(nFlags,point);
	CView::OnLButtonDown(nFlags, point);
  }

//

void C3DView::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  if(GetDocument()->GetTool()) GetDocument()->GetTool()->OnLButtonUp(nFlags,point);
	CView::OnLButtonUp(nFlags, point);
  }

//

void C3DView::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  if(GetDocument()->GetTool()) GetDocument()->GetTool()->OnLButtonDblClk(nFlags,point);
	CView::OnLButtonDblClk(nFlags, point);
  }

//

BOOL C3DView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
  {
  if(GetDocument()->GetTool())
    {
    if(GetDocument()->GetTool()->OnMouseWheel(nFlags,zDelta,pt)) return 0;
    }                                                       // !0 : the tool has handled the msg
	
  GPG4::CMoveCameraTool MoveCam(this);
  MoveCam.MoveCamera(GPG4::CVect3D(0.f,0.f,(float)zDelta/WHEEL_DELTA));
	return CView::OnMouseWheel(nFlags, zDelta, pt);
  }

//
// tools
//

GPG4::CEditorTool* C3DView::SetTool(GPG4::CEditorTool* pTool)
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(!pDoc) return NULL;

  if(pDoc->GetTool())
    {
    pDoc->GetTool()->End();
    if(pDoc->GetTool()->Modified()) pDoc->SetModifiedFlag();
    delete pDoc->GetTool();
    pDoc->SetTool(NULL);
    }

  if(pTool)
    {
    pDoc->SetTool(pTool);                                   // Start can use it
    if(pTool->Start() != 0)
      {                                                     // init error
      pTool->End();
      delete pTool;
      pTool = NULL;
      }
    }

  pDoc->SetTool(pTool);
  pDoc->UpdateAllViews(this,TOOL_CHANGED);
  return pTool;
  }

//

void C3DView::OnTimer(UINT nIDEvent) 
  {
  UINT uRes = 0;
  if(GetDocument()->GetTool()) uRes = GetDocument()->GetTool()->OnTimer(nIDEvent);
	CView::OnTimer(nIDEvent);
  }

//

BOOL C3DView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
  {
  SetCursor(m_hCursor);
  return true;
	//return CView::OnSetCursor(pWnd, nHitTest, message);
  }
