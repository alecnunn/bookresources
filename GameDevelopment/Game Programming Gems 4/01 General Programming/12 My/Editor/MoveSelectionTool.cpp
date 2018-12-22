//--------------------------------------------------------------------------------------------------------------------//
//                                                MOVE SELECTION (MOUSE+KEYS)                                         //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "MoveSelectionTool.h"

#include  "../RTTIPropsDoc.h"
#include  "../3DView.h"
#include  "../CustomMsgs.h"
#include  "Logger.h"

#include  "EngineCamera.h"
#include  "EngineNode.h"
#include  "ListObj.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CMoveSelectionTool::CMoveSelectionTool(CView* pView) : CEditorTool(pView)
  {
  m_strName = "MoveSelection";
  m_fSpeed  = 1.f;
  m_v3DoneMove.Set(0.f);

  // save state for ctrl+Z

  m_boModified    = false;
  m_pObjSelected  = NULL;
  m_pNodeSelected = NULL;
  u32 u32Objs     = 0;

  C3DView*        p3DView = static_cast<C3DView*>(m_pView);
  CRTTIPropsDoc*  pDoc    = p3DView->GetDocument();
  GPG4::CListObj* pSelec  = pDoc->GetSelection();

  if(pSelec)
    {
    GPG4::CEngineObj* pEngineObj = pSelec->GetFirstObj();
    while(pEngineObj)
      {
      GPG4::CEngineNode* pEngineNode = DYNAMIC_CAST(GPG4::CEngineNode,pEngineObj);
      GPG4::CEditorObj*  pEditObj    = pEngineObj->GetEditorObj();
      if(pEditObj && pEngineNode)
        {
        pEditObj->Store();
        if(!u32Objs)
          {
          m_pObjSelected  = pEditObj;
          m_pNodeSelected = pEngineNode;
          }
        u32Objs++;
        }
      pEngineObj = pSelec->GetNextObj();
      }
    }

  if(u32Objs > 1)
    {                                                       // multi-sel
    m_pObjSelected  = NULL;
    m_pNodeSelected = NULL;
    }
  }

//

CMoveSelectionTool::~CMoveSelectionTool()
  {
  if(!m_boModified) return;

  // log result

  C3DView* pView = static_cast<C3DView*>(m_pView);
  CVect3D  v3Pos(m_v3DoneMove);
  CString  strMsg;
  if(m_pObjSelected && m_pNodeSelected)
    {
    v3Pos = m_pNodeSelected->GetPosition();
    strMsg.Format("move [%s] to : %.1f %.1f %.1f",m_pObjSelected->GetName().c_str(),v3Pos.x,v3Pos.y,v3Pos.z);
    }
  else
    {
    strMsg.Format("move selection : %.1f %.1f %.1f",v3Pos.x,v3Pos.y,v3Pos.z);
    }
  _DLOG2_((*pView->GetDocument()->GetLogger()),(LPCTSTR)strMsg,"tool op");
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OVERRIDABLES                                                        //
//--------------------------------------------------------------------------------------------------------------------//

u32 CMoveSelectionTool::Start()
  {
  if(CEditorTool::Start() != 0) return -1;

  C3DView*       pView = static_cast<C3DView*>(m_pView);
  CRTTIPropsDoc* pDoc  = pView->GetDocument();
  if(!pDoc->GetSelection()) return -2;

  pDoc->LockSelection(true);

  SetFocus(m_pView->m_hWnd);
  m_u32Cursor = CURSOR_SIZEALL;
  pDoc->UpdateAllViews(NULL,CHANGE_CURSOR,(CObject*)m_u32Cursor);

  m_nTimer = m_pView->SetTimer(1,20,NULL);
  UpdatePrompt();
  return 0;
  }

//

u32 CMoveSelectionTool::End()
  {
  m_pView->KillTimer(m_nTimer);
  m_pView->GetDocument()->UpdateAllViews(NULL,CHANGE_CURSOR,(CObject*)CURSOR_ARROW);
  m_pView->GetDocument()->UpdateAllViews(m_pView,PROP_UPDATE,NULL);

  C3DView* pView = static_cast<C3DView*>(m_pView);
  pView->GetDocument()->LockSelection(false);

  return CEditorTool::End();
  }

//

u32 CMoveSelectionTool::Cancel()
  {
  if(!m_pView) return -1;

  // reset state

  C3DView*        p3DView = static_cast<C3DView*>(m_pView);
  CRTTIPropsDoc*  pDoc    = p3DView->GetDocument();
  GPG4::CListObj* pSelec  = pDoc->GetSelection();

  if(pSelec)
    {
    GPG4::CEngineObj* pEngineObj = pSelec->GetFirstObj();
    while(pEngineObj)
      {
      GPG4::CEditorObj*  pEditObj    = pEngineObj->GetEditorObj();
      GPG4::CEngineNode* pEngineNode = DYNAMIC_CAST(GPG4::CEngineNode,pEngineObj);
      if(pEditObj && pEngineNode)
        {
        pEditObj->Reset();
        }
      pEngineObj = pSelec->GetNextObj();
      }
    }

  //

  m_v3DoneMove.Set(0.f);

  m_pView->GetDocument()->UpdateAllViews(m_pView,PROP_UPDATE,(CObject*)NULL);
  UpdatePrompt();

  m_boModified = false;
  return 0;
  }

// cancel keys used by timer in parent view

u32 CMoveSelectionTool::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
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

u32 CMoveSelectionTool::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
  {
  if(CEditorTool::OnMouseWheel(nFlags,zDelta,pt) != 0) return -1;
  return 1;                                                 // so 3DView can't handle the msg
  }

//

u32 CMoveSelectionTool::OnRButtonDown(UINT nFlags,CPoint point)
  {
  if(CEditorTool::OnRButtonDown(nFlags,point) != 0) return -1;
  return 1;
  }

//

u32 CMoveSelectionTool::OnTimer(UINT nIDEvent)
  {
  if(CEditorTool::OnTimer(nIDEvent) != 0) return -1;

  static bool boLastMove = false;
  CVect3D v3Move(0.f);

  // shift & ctrl => cursor

  bool boShift = AsyncKey(VK_SHIFT);
  bool boCtrl  = AsyncKey(VK_CONTROL);

  u32 u32Cursor = CURSOR_SIZEALL;
  if( boShift && !boCtrl) u32Cursor = CURSOR_VERTI;
  if(!boShift &&  boCtrl) u32Cursor = CURSOR_HORIZ;
  if(u32Cursor != m_u32Cursor)
    {
    m_u32Cursor = u32Cursor;
    m_pView->GetDocument()->UpdateAllViews(NULL,CHANGE_CURSOR,(CObject*)m_u32Cursor);
    }

  // mouse

  if(m_ptNew != m_ptOld)
    {
    if(m_boLeftBtn)
      {
      CPoint ptDiff = m_ptNew-m_ptOld;
      float  dx = float(ptDiff.x);
      float  dy = float(ptDiff.y);
      v3Move.x += dx/5.f;
      v3Move.y -= dy/5.f;
      }
    m_ptOld = m_ptNew;
    }

  if(m_s32Wheel)
    {
    v3Move.z -= m_s32Wheel/WHEEL_DELTA;
    m_s32Wheel = 0;
    }

  // keyboard

  if(AsyncKey(VK_LEFT))  v3Move.x -= 1.f;
  if(AsyncKey(VK_RIGHT)) v3Move.x += 1.f;
  if(AsyncKey(VK_UP))    v3Move.y += 1.f;
  if(AsyncKey(VK_DOWN))  v3Move.y -= 1.f;
  if(AsyncKey(VK_PRIOR)) v3Move.z -= 1.f;
  if(AsyncKey(VK_NEXT))  v3Move.z += 1.f;

  bool boAddSpeed = NoRepeatKey(VK_MULTIPLY);
  bool boSubSpeed = NoRepeatKey(VK_DIVIDE);

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

  if(v3Move != CVect3D(0.f)) MoveSelection(v3Move);
  else if(boAddSpeed || boSubSpeed) UpdatePrompt();
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                SPECIFIC                                                            //
//--------------------------------------------------------------------------------------------------------------------//

bool CMoveSelectionTool::MoveSelection(const CVect3D& v3Move0)
  {
  C3DView*         pView = static_cast<C3DView*>(m_pView);
  CRTTIPropsDoc*   pDoc  = pView->GetDocument();
  CEditorCamera* pEditor = pView->GetCurrentCamera();
  if(!pEditor) return false;
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,pEditor->GetEngineObj());
  if(!pCamera) return false;

  CVect3D v3Move(v3Move0);

  // calculate move

  CMat4x4  m4Cam2World = pCamera->GetWorldTrf();
  v3Move = m4Cam2World.RotateVector(v3Move);

  m_v3DoneMove += v3Move;

  // update positions

  GPG4::CListObj* pSelec = pDoc->GetSelection();
  if(!pSelec) return false;

  GPG4::CEngineObj* pEngineObj = pSelec->GetFirstObj();
  while(pEngineObj)
    {
    GPG4::CEngineNode* pEngineNode = DYNAMIC_CAST(GPG4::CEngineNode,pEngineObj);
    if(pEngineNode && !IsChildOfSelected(pEngineNode))
      {
      GPG4::CEngineCamera* pEngineCam = DYNAMIC_CAST(GPG4::CEngineCamera,pEngineNode);
      if(!pEngineCam)
        {
        pEngineNode->SetWorldPosition(pEngineNode->GetWorldPosition()+v3Move);
        }
      else
        {
        CEngineNode* pParent = pEngineCam->GetParentNode();
        if(!pParent) pEngineCam->SetRepere(pEngineCam->GetPosition()+v3Move,pEngineCam->GetTarget()+v3Move,pEngineCam->GetUp());
        else
          {
          CVect3D v3WorldPos   (pEngineCam->GetWorldPosition()+v3Move);
          CVect3D v3WorldTarget(pEngineCam->GetWorldTarget  ()+v3Move);
          CMat4x4 m4World2Cam  (pParent->GetWorldTrf());
          m4World2Cam.Invert();
          pEngineCam->SetRepere(CVect3D(m4World2Cam*v3WorldPos,W),CVect3D(m4World2Cam*v3WorldTarget,W),pEngineCam->GetUp());
          }
        }
      pDoc->UpdateAllViews(NULL,PROP_UPDATE,(CObject*)pEngineNode);
      }

    pEngineObj = pSelec->GetNextObj();
    }

  m_boModified = true;
  UpdatePrompt();
  return true;
}

//

void CMoveSelectionTool::MulSpeed()
  {
  m_fSpeed *= 2.f;
  if(m_fSpeed > 16.f) m_fSpeed = 16.f;
  }

//

void CMoveSelectionTool::DivSpeed()
  {
  m_fSpeed *= 0.5f;
  if(m_fSpeed < 0.125) m_fSpeed = 0.125f;
  }

//

bool CMoveSelectionTool::UpdatePrompt()
  {
  CVect3D v3Pos(m_v3DoneMove);
  CStdString strHelp;

  if(m_pObjSelected && m_pNodeSelected)
    {
    v3Pos = m_pNodeSelected->GetPosition();
    strHelp.Format("moving [%s] (speed=%.2f) : %.1f %.1f %.1f",m_pObjSelected->GetName().c_str(),m_fSpeed,v3Pos.x,v3Pos.y,v3Pos.z);
    }
  else
    {
    strHelp.Format("moving selection (speed=%.2f) : %.1f %.1f %.1f",m_fSpeed,v3Pos.x,v3Pos.y,v3Pos.z);
    }

  SetHelpText(strHelp);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
