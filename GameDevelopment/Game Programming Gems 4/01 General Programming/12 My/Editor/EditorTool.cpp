//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR TOOL BASE CLASS                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "../MainFrm.h"
#include  "EditorTool.h"

#include  "MyAssert.h"
#include  "EngineNode.h"
#include  "EditorObj.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEditorTool::CEditorTool(CView* pView) : m_pView(pView)
  {
  MY_ASSERT(pView,"view can't be NULL");

  m_boRightBtn = false;
  m_boLeftBtn  = false;
  m_s32Wheel   = 0;

  m_strName    = "Tool";
  m_boModified = false;
  }

//

CEditorTool::~CEditorTool()
  {
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

const CStdString& CEditorTool::GetName() const
  {
  return m_strName;
  }

const CView* CEditorTool::GetView() const
  {
  return m_pView;
  }

const bool CEditorTool::Modified() const
  {
  return m_boModified;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OVERRIDABLES                                                        //
//--------------------------------------------------------------------------------------------------------------------//

u32 CEditorTool::Start()
  {
  GetCursorPos(&m_ptNew);
  m_pView->ScreenToClient(&m_ptNew);
  m_ptOld = m_ptNew;

  InitKeys();
  return 0;
  }

u32 CEditorTool::End()
  {
  SetHelpText("");
  return 0;
  }

u32 CEditorTool::Cancel()
  {
  return 0;
  }

//

u32 CEditorTool::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
  {
  return nChar;                                             // default: the key is not handled (and removed) by the tool
  }

u32 CEditorTool::OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags)
  {
  return nChar;
  }

//

u32 CEditorTool::OnMouseMove(UINT nFlags,CPoint point)
  {
  m_ptNew = point;
  return 0;
  }

u32 CEditorTool::OnMouseWheel(UINT nFlags,short zDelta,CPoint point)
  {
  m_s32Wheel += zDelta;
  return 0;
  }

//

u32 CEditorTool::OnLButtonDown(UINT nFlags,CPoint point)
  {
  m_boLeftBtn = true;
  return 0;
  }

u32 CEditorTool::OnRButtonDown(UINT nFlags,CPoint point)
  {
  m_boRightBtn = true;
  return 0;
  }

u32 CEditorTool::OnLButtonUp(UINT nFlags,CPoint point)
  {
  m_boLeftBtn = false;
  return 0;
  }

u32 CEditorTool::OnRButtonUp(UINT nFlags,CPoint point)
  {
  m_boRightBtn = false;
  return 0;
  }

u32 CEditorTool::OnLButtonDblClk(UINT nFlags,CPoint point)
  {
  return 0;
  }

u32 CEditorTool::OnRButtonDblClk(UINT nFlags,CPoint point)
  {
  return 0;
  }

//

u32 CEditorTool::OnTimer(UINT nIDEvent)
  {
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                HELPERS                                                             //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorTool::NoRepeatKey(u32 u32Key)
  {
  return((GetAsyncKeyState(u32Key) & 0x0001) != 0);
  }

bool CEditorTool::AsyncKey(u32 u32Key)
  {
  return((GetAsyncKeyState(u32Key) & 0x8000) != 0);
  }

//

void CEditorTool::InitKeys()
  {
  AsyncKey(VK_SHIFT);
  AsyncKey(VK_CONTROL);

  AsyncKey(VK_LEFT);
  AsyncKey(VK_RIGHT);
  AsyncKey(VK_UP);
  AsyncKey(VK_DOWN);
  AsyncKey(VK_PRIOR);
  AsyncKey(VK_NEXT);

  AsyncKey(VK_MULTIPLY);
  AsyncKey(VK_DIVIDE);
  AsyncKey('S');
  AsyncKey('Z');
  }

//

void CEditorTool::CenterCursor()
  {
  RECT rect;
  m_pView->GetClientRect(&rect);
  POINT pt;
  pt.x = rect.right /2;
  pt.y = rect.bottom/2;
  m_ptOld = m_ptNew = pt;
  m_pView->ClientToScreen(&pt);
  SetCursorPos(pt.x,pt.y);
  }

//

void CEditorTool::SetHelpText(const CStdString& strHelp)
  {
  CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
  if(!pMainFrm) return;
  pMainFrm->SetHelpText(strHelp);
  }

//

bool CEditorTool::IsChildOfSelected(CEngineNode* pNode)
  {
  if(!pNode) return false;
  CEngineNode* pParent = pNode->GetParentNode();
  if(!pParent) return false;

  CEditorObj* pEditor = pParent->GetEditorObj();
  if(pEditor)
    {
    if(pEditor->IsSelected()) return true;
    }

  return IsChildOfSelected(pParent);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
