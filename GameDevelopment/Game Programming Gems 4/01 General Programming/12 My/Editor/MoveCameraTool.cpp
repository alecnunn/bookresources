//--------------------------------------------------------------------------------------------------------------------//
//                                                MOVE CAMERA (MOUSE+KEYS)                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "MoveCameraTool.h"

#include  "../RTTIPropsDoc.h"
#include  "../3DView.h"
#include  "../CustomMsgs.h"
#include  "EngineCamera.h"
#include  "Logger.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CMoveCameraTool::CMoveCameraTool(CView* pView) : CEditorTool(pView)
  {
  m_strName = "MoveCamera";
  m_fSpeed  = 1.f;

  // save state for ctrl+Z

  m_boModified  = false;

  C3DView*       p3DView = static_cast<C3DView*>(m_pView);
  CEditorCamera* pEditor = p3DView->GetCurrentCamera();
  if(!pEditor) return;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return;

  m_fScaleOrtho = pCamera->GetScaleOrtho();
  m_v3Pos       = pCamera->GetPosition();
  m_v3Target    = pCamera->GetTarget();
  m_v3Up        = pCamera->GetUp();
  }

//

CMoveCameraTool::~CMoveCameraTool()
  {
  if(!m_boModified) return;

  // log result

  C3DView*       p3DView = static_cast<C3DView*>(m_pView);
  CEditorCamera* pEditor = p3DView->GetCurrentCamera();
  if(!pEditor) return;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return;

  CVect3D v3Pos(pCamera->GetPosition());
  CString strMsg;
  strMsg.Format("move camera [%s] to : %.1f %.1f %.1f , scale ortho = %.2f",pCamera->GetName().c_str(),
                v3Pos.x,v3Pos.y,v3Pos.z,pCamera->GetScaleOrtho());
  _DLOG2_((*p3DView->GetDocument()->GetLogger()),(LPCTSTR)strMsg,"tool op");
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OVERRIDABLES                                                        //
//--------------------------------------------------------------------------------------------------------------------//

u32 CMoveCameraTool::Start()
  {
  if(CEditorTool::Start() != 0) return -1;

  C3DView* pView = static_cast<C3DView*>(m_pView);
  if(!pView->GetCurrentCamera())
    {
    AfxMessageBox("no camera currently selected; you need to choose one to show the view",MB_OK|MB_ICONINFORMATION);
    GetCursorPos(&m_ptSaveCursor);
    return -2;
    }

  GetCursorPos(&m_ptSaveCursor);
  SetFocus  (m_pView->m_hWnd);
  SetCapture(m_pView->m_hWnd);
  ShowCursor(false);
  m_nTimer = pView->SetTimer(1,20,NULL);

  UpdatePrompt();
  return 0;
  }

//

u32 CMoveCameraTool::End()
  {
  m_pView->KillTimer(m_nTimer);
  ReleaseCapture();

  SetCursorPos(m_ptSaveCursor.x,m_ptSaveCursor.y);
  ShowCursor(true);
  return CEditorTool::End();
  }

//

u32 CMoveCameraTool::Cancel()
  {
  if(!m_pView) return -1;
  C3DView* pView = static_cast<C3DView*>(m_pView);
  CEditorCamera* pEditor = pView->GetCurrentCamera();
  if(!pEditor) return -2;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return -3;

  pCamera->SetScaleOrtho(m_fScaleOrtho);
  pCamera->SetRepere(m_v3Pos,m_v3Target,m_v3Up);
  m_pView->GetDocument()->UpdateAllViews(m_pView,PROP_UPDATE,(CObject*)pEditor);
  UpdatePrompt();

  m_boModified = false;
  return 0;
  }

// cancel keys used by timer in parent view

u32 CMoveCameraTool::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
  {
  switch(nChar)
    {
    case VK_MULTIPLY :
    case VK_DIVIDE   :
    case 'S'         :
    case 'Z'         : return 0;
    }
  return nChar;                                             // default: the key is not handled (and removed) by the tool
  }

//

u32 CMoveCameraTool::OnRButtonDown(UINT nFlags,CPoint point)
  {
  if(CEditorTool::OnRButtonDown(nFlags,point) != 0) return -1;
  return 1;
  }

//

u32 CMoveCameraTool::OnLButtonDblClk(UINT nFlags,CPoint point)
  {
  if(CEditorTool::OnLButtonDblClk(nFlags,point) != 0) return -1;
  MulSpeed();
  UpdatePrompt();
  return 0;
  }

//

u32 CMoveCameraTool::OnRButtonDblClk(UINT nFlags,CPoint point)
  {
  if(CEditorTool::OnRButtonDblClk(nFlags,point) != 0) return -1;
  DivSpeed();
  UpdatePrompt();
  return 0;
  }

//

u32 CMoveCameraTool::OnTimer(UINT nIDEvent)
  {
  if(CEditorTool::OnTimer(nIDEvent) != 0) return -1;

  CVect3D v3Move(0.f);

  // keyboard

  bool boShift = AsyncKey(VK_SHIFT);
  bool boCtrl  = AsyncKey(VK_CONTROL);

  if(AsyncKey(VK_LEFT))  v3Move.x -= 1.f;
  if(AsyncKey(VK_RIGHT)) v3Move.x += 1.f;
  if(AsyncKey(VK_UP))    v3Move.y += 1.f;
  if(AsyncKey(VK_DOWN))  v3Move.y -= 1.f;
  if(AsyncKey(VK_PRIOR)) v3Move.z -= 1.f;
  if(AsyncKey(VK_NEXT))  v3Move.z += 1.f;

  bool boAddSpeed = NoRepeatKey(VK_MULTIPLY);
  bool boSubSpeed = NoRepeatKey(VK_DIVIDE);

  // mouse

  if(m_boLeftBtn)  v3Move.z -= 4.f;
  if(m_boRightBtn) v3Move.z += 4.f;

  if(m_ptNew != m_ptOld)
    {
    CPoint ptDiff = m_ptNew-m_ptOld;
    float  dx = float(ptDiff.x);
    float  dy = float(ptDiff.y);
    v3Move.x += dx/2.f;
    v3Move.y -= dy/2.f;
    m_ptOld   = m_ptNew;

    CenterCursor();                                         // so we're sure the cursor never reaches a border of the screen
    }

  // ctrl+Z

  if(boCtrl && AsyncKey('Z'))
    {
    Cancel();
    return 0;
    }

  //

  if(boAddSpeed) MulSpeed();
  if(boSubSpeed) DivSpeed();

  v3Move *= m_fSpeed*0.1f;

  if(boShift) v3Move.x = v3Move.z = 0.f;
  if(boCtrl)  v3Move.y = v3Move.z = 0.f;

  if(v3Move != CVect3D(0.f)) MoveCamera(v3Move);
  else if(boAddSpeed || boSubSpeed) UpdatePrompt();
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                SPECIFIC                                                            //
//--------------------------------------------------------------------------------------------------------------------//

bool CMoveCameraTool::MoveCamera(const CVect3D& v3Move0)
  {
  C3DView*       pView   = static_cast<C3DView*>(m_pView);
  CEditorCamera* pEditor = pView->GetCurrentCamera();
  if(!pEditor) return false;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return false;

  // ortho

  CVect3D v3Move(v3Move0);

  if(!pCamera->IsPerspective() && (v3Move.z != 0.f))
    {
    if(v3Move.z > 0.f) while(v3Move.z > 0.f) { MulScale(pCamera); v3Move.z -= 1.f; }
    else               while(v3Move.z < 0.f) { DivScale(pCamera); v3Move.z += 1.f; }
    v3Move.z = 0.f;
    }

  // update pos

  CMat4x4  m4Cam2World = pCamera->GetWorldTrf();
  v3Move = m4Cam2World.RotateVector(v3Move);

  CVect3D v3WorldPos   (pCamera->GetWorldPosition()+v3Move);
  CVect3D v3WorldTarget(pCamera->GetWorldTarget  ()+v3Move);
  CMat4x4 m4World2Cam  (pCamera->GetParentNode() ? pCamera->GetParentNode()->GetWorldTrf() : CMat4x4::Identity);
  m4World2Cam.Invert();

  pCamera->SetRepere(CVect3D(m4World2Cam*v3WorldPos,W),CVect3D(m4World2Cam*v3WorldTarget,W),pCamera->GetUp());
  m_pView->GetDocument()->UpdateAllViews(m_pView,PROP_UPDATE,(CObject*)pCamera);

  m_boModified = true;
  UpdatePrompt();
  return true;
}

//

void CMoveCameraTool::MulSpeed()
  {
  m_fSpeed *= 2.f;
  if(m_fSpeed > 16.f) m_fSpeed = 16.f;
  }

//

void CMoveCameraTool::DivSpeed()
  {
  m_fSpeed *= 0.5f;
  if(m_fSpeed < 0.125) m_fSpeed = 0.125f;
  }

//

void CMoveCameraTool::MulScale(CEngineCamera* pCam)
  {
  if(!pCam) return;
  float fOrtho = pCam->GetScaleOrtho();

  if(fOrtho > 50.f) fOrtho = 50.f;
  pCam->SetScaleOrtho(fOrtho*1.05f);
  m_boModified = true;
  }

//

void CMoveCameraTool::DivScale(CEngineCamera* pCam)
  {
  if(!pCam) return;
  float fOrtho = pCam->GetScaleOrtho();

  if(fOrtho < 0.5f) fOrtho = 0.5f;
  pCam->SetScaleOrtho(fOrtho*0.95f);
  m_boModified = true;
  }

//

bool CMoveCameraTool::UpdatePrompt()
  {
  C3DView*       pView   = static_cast<C3DView*>(m_pView);
  CEditorCamera* pEditor = pView->GetCurrentCamera();
  if(!pEditor) return false;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return false;

  CVect3D v3Pos(pCamera->GetPosition());
  CVect3D v3At (pCamera->GetTarget());
  CVect3D v3Up (pCamera->GetUp());

  CStdString strHelp;
  strHelp.Format("moving camera [%s] (speed=%.2f) : %.1f  %.1f  %.1f [target=%.1f  %.1f  %.1f] [up=%.2f  %.2f  %.2f]",
                  pEditor->GetName().c_str(),m_fSpeed,v3Pos.x,v3Pos.y,v3Pos.z,v3At.x,v3At.y,v3At.z,v3Up.x,v3Up.y,v3Up.z);
  SetHelpText(strHelp);

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
