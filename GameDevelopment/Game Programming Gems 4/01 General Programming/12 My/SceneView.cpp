// SceneView.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"
#include "SceneView.h"

#include "CustomMsgs.h"
#include "ImgEnum.h"
#include "MenuEnum.h"

#include "DlgItemName.h"
#include "EditorTool.h"
#include "EditorCamera.h"

#include "Engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSceneView

IMPLEMENT_DYNCREATE(CSceneView, CFormView)

CSceneView::CSceneView()
	: CFormView(CSceneView::IDD)
{
	//{{AFX_DATA_INIT(CSceneView)
	//}}AFX_DATA_INIT
  m_boDragging   = false;
  m_pDragImgList = NULL;
	m_imgList.Create(IDB_SCENE_TREE,16,1,RGB(255,255,255));

  m_strInitSel = "default camera";
}

CSceneView::~CSceneView()
{
  delete m_pDragImgList;
  m_imgList.DeleteImageList();
}

void CSceneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneView)
  DDX_Control(pDX, IDC_TREE, m_Scene);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneView, CFormView)
	//{{AFX_MSG_MAP(CSceneView)
  ON_WM_SIZE()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, OnItemexpandedTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE, OnKeydownTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE, OnBegindragTree)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_COMMAND(ID_MENU_RENAME, OnMenuRename)
	ON_UPDATE_COMMAND_UI(ID_MENU_RENAME, OnUpdateMenuRename)
	ON_COMMAND(ID_MENU_EDIT, OnMenuEdit)
	ON_UPDATE_COMMAND_UI(ID_MENU_EDIT, OnUpdateMenuEdit)
	ON_COMMAND(ID_CAMERA_PERSPECTIVE, OnCameraPerspective)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_PERSPECTIVE, OnUpdateCameraPerspective)
	ON_COMMAND(ID_CREATE_CAMERA, OnCreateCamera)
	ON_COMMAND(ID_CREATE_FOLDER, OnCreateFolder)
	ON_COMMAND(ID_CREATE_BOX, OnCreateBox)
	ON_COMMAND(ID_CREATE_SPHERE, OnCreateSphere)
	ON_COMMAND(ID_CREATE_NODE, OnCreateNode)
	ON_COMMAND(ID_CREATE_MATERIAL, OnCreateMaterial)
	ON_COMMAND(ID_CREATE_TEXTURE, OnCreateTexture)
	ON_UPDATE_COMMAND_UI(ID_CREATE_CAMERA, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_FOLDER, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_BOX, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_SPHERE, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_NODE, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_MATERIAL, OnUpdateCreateItem)
	ON_UPDATE_COMMAND_UI(ID_CREATE_TEXTURE, OnUpdateCreateItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneView diagnostics

#ifdef _DEBUG
void CSceneView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSceneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRTTIPropsDoc* CSceneView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSceneView message handlers

void CSceneView::OnSize(UINT nType, int cx, int cy) 
  {
  CFormView::OnSize(nType, cx, cy);
  
  // resize tree

  if(m_Scene.m_hWnd)
    {                                                       // otherwise not created yet
    RECT rect;
    GetClientRect(&rect);
    #define BORDER 5
    int iSizeX,iSizeY;
    iSizeX = rect.right-rect.left-BORDER*2;
    iSizeY = rect.bottom-rect.top-BORDER*2;
    if((iSizeX > 0) && (iSizeY > 0)) m_Scene.SetWindowPos(NULL,BORDER,BORDER,iSizeX,iSizeY,SWP_NOZORDER);
    #undef BORDER
    }
  }

//

void CSceneView::OnInitialUpdate() 
  {
  CFormView::OnInitialUpdate();
  m_Scene.SetImageList(&m_imgList,TVSIL_NORMAL);
  BuildTreeFromScene();
  SelectItem(m_strInitSel);

  GetDocument()->UpdateAllViews(this,SCENE_NEWOBJECT,NULL);
  }

//

bool CSceneView::BuildTreeFromScene()
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(!pDoc)  return false;
  GPG4::CEditorSceneItem* pRoot = GetDocument()->GetEditorScene()->GetRootItem();
  if(!pRoot) return true;

  m_Scene.DeleteAllItems();
  return AddChildItems(pRoot,TVI_ROOT);
  }

  //

bool CSceneView::AddChildItems(GPG4::CEditorSceneItem* pItem,HTREEITEM hParent)
  {
  if(!pItem) return false;
  GPG4::CEditorSceneItem* pChild = pItem->GetFirstChild();

  while(pChild)
    {
    HTREEITEM hChild = AddSceneItem(pChild,hParent);
    AddChildItems(pChild,hChild);
    m_Scene.Expand(hChild,pChild->IsExpanded() ? TVE_EXPAND : TVE_COLLAPSE);

    pChild = pItem->GetNextChild();
    }
  return true;
  }

  //

HTREEITEM CSceneView::AddSceneItem(GPG4::CEditorSceneItem* pItem,HTREEITEM hParent)
  {
  if(!pItem || !hParent) return NULL;
  GPG4::CEditorObj* pObj = pItem->GetObject();
  return CreateItem(pItem,pObj ? pObj->GetName() : "**** no object *****",hParent);
  }

//
// items
//

HTREEITEM CSceneView::CreateItem(GPG4::CEditorSceneItem* pItem,const CStdString& strName,HTREEITEM hParent)
  {
  GPG4::CEditorObj* pObj = NULL;
  if(pItem) pObj = pItem->GetObject();

  CStdString strFinalName(strName);
  if(strFinalName.empty() && pObj) strFinalName = pObj->GetName();

  // tree item

  if(!hParent) hParent = m_Scene.GetSelectedItem();
  if(!hParent) hParent = TVI_ROOT;
  HTREEITEM hNew = m_Scene.InsertItem(strFinalName,hParent);
  if(!hNew) return NULL;

  TV_ITEM tvItem;
  tvItem.mask   = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  tvItem.iImage = tvItem.iSelectedImage = (pObj ? pObj->GetIcon() : GPG4::_IMG_EMPTY_);
	tvItem.lParam = (LPARAM)pItem;
  tvItem.hItem  = hNew;
  m_Scene.SetItem(&tvItem);

  //

  if(pObj) m_Scene.SetCheck(hNew,pObj->IsSelected());
  pItem->SetTreeHandle(hNew);
  return hNew;
  }

//

GPG4::CEditorSceneItem* CSceneView::GetItemData(HTREEITEM hItem)
  {
  if(!hItem) return NULL;
  GPG4::CEditorSceneItem* pItem = reinterpret_cast<GPG4::CEditorSceneItem*>(m_Scene.GetItemData(hItem));
  return pItem;
  }

//

GPG4::CEditorObj* CSceneView::GetTreeItemData(HTREEITEM hItem)
  {
  GPG4::CEditorSceneItem* pItem = GetItemData(hItem);
  if(!pItem) return NULL;
  return pItem->GetObject();
  }

//

bool CSceneView::SelectItem(GPG4::CEditorSceneItem* pItem)
  {
  if(!pItem) return false;
  HTREEITEM hItem = pItem->GetTreeHandle();
  if(!hItem)
    {
    m_strInitSel = pItem->GetName();
    return false;
    }

  m_Scene.SelectItem   (hItem);
  m_Scene.EnsureVisible(hItem);
  return true;
  }

//

bool CSceneView::SelectItem(const CStdString& strItem)
  {
  if(!GetDocument()) return false;
  return SelectItem(GetDocument()->GetEditorScene()->GetItem(strItem));
  }

//

void CSceneView::ClearSelection(HTREEITEM hItem)
  {
  if(!hItem) return;
  if(hItem != TVI_ROOT)
    {
    m_Scene.SetCheck(hItem,false);

    GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
    if(pObj) pObj->Select(false);
    }

  HTREEITEM hChild = m_Scene.GetChildItem(hItem);
  while(hChild)
    {
    ClearSelection(hChild);
    hChild = m_Scene.GetNextSiblingItem(hChild);
    }
  }

//

void CSceneView::ChangeSelection(HTREEITEM hItem,GPG4::CEditorObj* pObj)
  {
  if(!hItem) return;
  if(hItem != TVI_ROOT)
    {
    if(GetTreeItemData(hItem) == pObj)
      m_Scene.SetCheck(hItem,pObj->IsSelected());
    }

  HTREEITEM hChild = m_Scene.GetChildItem(hItem);
  while(hChild)
    {
    ChangeSelection(hChild,pObj);
    hChild = m_Scene.GetNextSiblingItem(hChild);
    }
  }

//
// click on tree
//

void CSceneView::OnItemexpandedTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  HTREEITEM    hItem       = pNMTreeView->itemNew.hItem;
  if(!hItem)   return;

  int nImage,nSelected;
  if(m_Scene.GetItemImage(hItem,nImage,nSelected))
    {
    if((nImage == GPG4::_IMG_FOLDER_) || (nImage == GPG4::_IMG_OPEN_FOLDER_))
      {
      if(pNMTreeView->action == TVE_EXPAND)   m_Scene.SetItemImage(hItem,GPG4::_IMG_OPEN_FOLDER_,GPG4::_IMG_OPEN_FOLDER_);
      if(pNMTreeView->action == TVE_COLLAPSE) m_Scene.SetItemImage(hItem,GPG4::_IMG_FOLDER_,     GPG4::_IMG_FOLDER_);
      }
    }

  GPG4::CEditorSceneItem* pItem = GetItemData(hItem);
  if(pItem)
    {
    if(pNMTreeView->action == TVE_EXPAND)   pItem->SetExpanded(true);
    if(pNMTreeView->action == TVE_COLLAPSE) pItem->SetExpanded(false);
    }

	*pResult = 0;
  }

//

void CSceneView::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  HTREEITEM    hItem       = pNMTreeView->itemNew.hItem;
  if(!hItem)   return;
  GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
  CRTTIPropsDoc*    pDoc = GetDocument();

  if(pObj)
    {
    CString strMsg;
    strMsg.Format("select item [%s]",pObj->GetName().c_str());
    _DLOG2_((*pDoc->GetLogger()),strMsg,"scene op");
    }

  // update other views

  pDoc->UpdateAllViews(this,SCENE_SELECTION,(CObject*)m_Scene.GetItemData(hItem));
	*pResult = 0;
  }

//

void CSceneView::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  POINT Pt;
  GetCursorPos(&Pt);
  m_Scene.ScreenToClient(&Pt);

  UINT uFlags;
  HTREEITEM hItem = m_Scene.HitTest(Pt,&uFlags);

  if(hItem)
    {
    GetDocument()->UpdateAllViews(this,SCENE_DBLCLK,(CObject*)m_Scene.GetItemData(hItem));
    }

  *pResult = 0;
  }

//

void CSceneView::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  POINT Pt;
  GetCursorPos(&Pt);
  m_Scene.ScreenToClient(&Pt);

  UINT uFlags;
  HTREEITEM hItem = m_Scene.HitTest(Pt,&uFlags);

  if(hItem)
    {
    CRTTIPropsDoc*    pDoc = GetDocument();
    GPG4::CEditorObj* pObj = GetTreeItemData(hItem);

    if(pObj && (uFlags & TVHT_ONITEMSTATEICON))
      {                                                     // check box
      if(pDoc->IsSelectionLocked())
        {
        AfxMessageBox(CString("selection can't be modified during a ")+pDoc->GetTool()->GetName().c_str(),MB_OK);
        m_Scene.SetCheck(hItem,!pObj->IsSelected());        // a msg is going to invert the check, so we'll go back to the initial state
        return;
        }

      pObj->Select(FALSE == m_Scene.GetCheck(hItem));
      if(!pDoc->ModifySelection(pObj)) pDoc->UpdateAllViews(this,PROP_UPDATE,(CObject*)pObj);

      if(!pObj->GetEngineObj()) pDoc->UpdateAllViews(NULL,PROP_UPDATE,(CObject*)pObj);
      }
    }

	*pResult = 0;
  }

// SPACE => modifies check box contents

void CSceneView::OnCheckBox()
  {
  HTREEITEM hItem = m_Scene.GetSelectedItem();
  if(!hItem) return;

  CRTTIPropsDoc*    pDoc = GetDocument();
  GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
  if(!pObj)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : item has no data");
    return;
    }

  if(pDoc->IsSelectionLocked())
    {
    AfxMessageBox(CString("selection can't be modified during a ")+pDoc->GetTool()->GetName().c_str(),MB_OK);
    m_Scene.SetCheck(hItem,!pObj->IsSelected());            // a msg is going to invert the check, so we'll go back to the initial state
    return;
    }

  pObj->Select(FALSE == m_Scene.GetCheck(hItem));
  if(!pDoc->ModifySelection(pObj)) pDoc->UpdateAllViews(this,PROP_UPDATE,(CObject*)pObj);
  }

//
// keys
//

void CSceneView::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	TV_KEYDOWN*  pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
  WORD wVKey = pTVKeyDown->wVKey;

  if(wVKey == VK_F2)      OnMenuRename();
  if(wVKey == VK_CONTROL) OnMButtonDblClk(0,CPoint(0,0));   // collapse/expand in drag & drop mode
  if(wVKey == VK_SPACE)   OnCheckBox();
	
	*pResult = 0;
  }

//
// drag & drop
// see http://www.fairyengine.com/articles.htm : "tree drag & drop"

void CSceneView::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  HTREEITEM    hItem       = pNMTreeView->itemNew.hItem;
  GPG4::CEditorObj* pObj   = GetTreeItemData(hItem);
  m_Scene.SelectItem(hItem);
  if(!pObj) return;
  if(!pObj->IsDraggable()) return;

  m_hDragItem    = hItem;
  m_pDragImgList = m_Scene.CreateDragImage(hItem);
  if(!m_pDragImgList) return;
  
  m_pDragImgList->BeginDrag(0,CPoint(0,0));
  m_pDragImgList->DragEnter(this,pNMTreeView->ptDrag);
  m_Scene.m_pDragImgList = m_pDragImgList;
  m_boDragging = true;

  //ShowCursor(false);
  SetCapture();
  m_hDragTarget = NULL;

  m_nTimer = SetTimer(3,25,NULL);
	*pResult = 0;
  }

// end drag

void CSceneView::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  if(m_boDragging)
    {
    KillTimer(m_nTimer);

    ReleaseCapture();
    //ShowCursor(true);
    m_boDragging = false;

    m_pDragImgList->DragLeave(this);
    m_pDragImgList->EndDrag();
    m_Scene.m_pDragImgList = NULL;
    delete m_pDragImgList;
    m_pDragImgList = NULL;

    m_Scene.SelectDropTarget(NULL);
    m_Scene.SelectItem(m_hDragItem);

    if(m_hDragTarget && (m_hDragTarget != m_hDragItem))
      {
      SuccessfulDrag(m_hDragTarget,m_hDragItem);
      }
    } 	

	CFormView::OnLButtonUp(nFlags, point);
  }

// cancel drag

void CSceneView::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  if(m_boDragging)
    {
    KillTimer(m_nTimer);

    ReleaseCapture();
    //ShowCursor(true);
    m_boDragging = false;

    m_pDragImgList->DragLeave(this);
    m_pDragImgList->EndDrag();
    m_Scene.m_pDragImgList = NULL;
    delete m_pDragImgList;
    m_pDragImgList = NULL;

    m_Scene.SelectDropTarget(NULL);
    m_Scene.SelectItem(m_hDragItem);
    m_Scene.EnsureVisible(m_hDragItem);
    } 	

	CFormView::OnRButtonDown(nFlags, point);
  }

// collapse/expand item

void CSceneView::OnMButtonDown(UINT nFlags, CPoint point) 
  {
	CFormView::OnMButtonDown(nFlags, point);
  }


void CSceneView::OnMButtonDblClk(UINT nFlags, CPoint point) 
  {	
  if(m_boDragging) 
    {
    if(m_hDragTarget)
      {
      m_pDragImgList->DragShowNolock(false);
      m_Scene.Expand(m_hDragTarget,TVE_TOGGLE);
      m_Scene.RedrawWindow();
      m_pDragImgList->DragShowNolock(true);
      }
    }

	CFormView::OnMButtonDblClk(nFlags, point);
  }

// highlight & scroll

void CSceneView::OnTimer(UINT nIDEvent) 
  {
  if(m_boDragging)
    {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    // highlight target

    TVHITTESTINFO tvHit;
    tvHit.pt = point;
    HTREEITEM hTarget = m_Scene.HitTest(&tvHit);

    if(hTarget)
      {
      GPG4::CEditorObj* pObj = GetTreeItemData(hTarget);
      if(pObj && pObj->IsDropTarget())
        {}
      else
        {                                                   // look for a parent target
        while(true)
          {
          hTarget = m_Scene.GetParentItem(hTarget);
          if(!hTarget) break;
          GPG4::CEditorObj* pObj = GetTreeItemData(hTarget);
          if(pObj && pObj->IsDropTarget()) break;
          }
        }

      if(hTarget && (hTarget != m_hDragTarget))
        {
        m_pDragImgList->DragShowNolock(false);
        m_Scene.EnsureVisible(hTarget);
        m_Scene.SelectDropTarget(hTarget);
        m_pDragImgList->DragShowNolock(true);
        m_hDragTarget = hTarget;
        }
      }

    // scroll tree

    else
      {
      RECT rect;
      m_Scene.GetClientRect(&rect);

      int iMaxV = m_Scene.GetScrollLimit(SB_VERT);
 	    int iPosV = m_Scene.GetScrollPos  (SB_VERT);

      // up
      if((point.y < rect.top -10) && iPosV)
        {
        HTREEITEM hPrev = m_Scene.GetPrevVisibleItem(m_Scene.GetFirstVisibleItem());
        m_pDragImgList->DragShowNolock(false);
        m_Scene.EnsureVisible(hPrev);
        m_pDragImgList->DragShowNolock(true);
        }

      // down
      if((point.y > rect.bottom +10) && (iPosV != iMaxV))
        {
        UINT Nb = m_Scene.GetVisibleCount();
        if(Nb != -1)
          {
          HTREEITEM hNext = m_Scene.GetFirstVisibleItem();
          for(UINT i = 0; i < Nb; i++) hNext = m_Scene.GetNextVisibleItem(hNext);
          m_pDragImgList->DragShowNolock(false);
          m_Scene.EnsureVisible(hNext);
          m_pDragImgList->DragShowNolock(true);
          }
        }

      int iPosH = m_Scene.GetScrollPos  (SB_HORZ);
    	int iMaxH = m_Scene.GetScrollLimit(SB_HORZ);

      // left
      if((point.x < rect.left) && iPosH)
        {
        m_pDragImgList->DragShowNolock(false);
        m_Scene.SendMessage(WM_HSCROLL,SB_LINELEFT);
        m_pDragImgList->DragShowNolock(true);
        }

      // right
      if((point.x > rect.right) && (iPosH != iMaxH))
        {
        m_pDragImgList->DragShowNolock(false);
        m_Scene.SendMessage(WM_HSCROLL,SB_LINERIGHT);
        m_pDragImgList->DragShowNolock(true);
        }
      }

    m_pDragImgList->DragMove(point);
    } 
	
	CFormView::OnTimer(nIDEvent);
  }

//

void CSceneView::SuccessfulDrag(HTREEITEM hDest,HTREEITEM hSrc)
  {
  // test : no op if hSrc is already a child of hDest

  if(m_Scene.GetParentItem(hSrc) == hDest) return;

  // test : if hDest is a child of hSrc, we're going to have a stack overflow ! (infinite loop)

  HTREEITEM hParent = hDest;
  while(hParent)
    {
    if(hParent == hSrc)
      {
      _DLOG2_((*GetDocument()->GetLogger()),"invalid drag & drop : destination can't be a child of source (infinite loop)","cancelled");
      return;
      }
    hParent = m_Scene.GetParentItem(hParent);
    }

  //

  HTREEITEM hNew = m_Scene.InsertItemAndSubtree(hDest,hSrc);
  if(!hNew) return;
  SetTreeHandles(hNew);

  CString strTmp("drag & drop : ");
  strTmp += "[" + m_Scene.GetItemText(hSrc) + "] becomes a child of [" + m_Scene.GetItemText(hDest) +"]";
  _DLOG2_((*GetDocument()->GetLogger()),(LPCTSTR)strTmp,"scene op");

  GetDocument()->SetModifiedFlag();

  // same changes in SceneItem

  GPG4::CEditorSceneItem* pSrc       = GetItemData(hSrc);
  GPG4::CEditorSceneItem* pDest      = GetItemData(hDest);
  GPG4::CEditorSceneItem* pSrcParent = pSrc->GetParent();

  if(pSrc && pDest && pSrcParent)
    {
    pDest->AddChild(pSrc);                                  // pSrc->SetParent(pDest) is called automatically
    pSrcParent->RemoveChild(pSrc);

    if(pSrcParent->GetObject())
      pSrc->SetPrevParent(pSrcParent->GetObject()->GetName());
    else
      pSrc->SetPrevParent("");
    }
  else
    {
    _DERR_((*GetDocument()->GetLogger()),"pSrc, pDest or pSrcParent is NULL");
    }

  //

  m_Scene.SelectItem   (hNew);
  m_Scene.EnsureVisible(hNew);
  m_Scene.DeleteItem   (hSrc);
  }

// resynchronize tree handles in scene items

void CSceneView::SetTreeHandles(HTREEITEM hItem)
  {
  if(!hItem) return;
  SetTreeHandle(hItem);

  HTREEITEM hChild = m_Scene.GetChildItem(hItem);
  while(hChild)
    {
    SetTreeHandle(hChild);
    hChild = m_Scene.GetNextSiblingItem(hChild);
    }
  }

  //

void CSceneView::SetTreeHandle(HTREEITEM hItem)
  {
  if(!hItem) return;
  GPG4::CEditorSceneItem* pItem = GetItemData(hItem);
  if(!pItem) return;
  pItem->SetTreeHandle(hItem);
  }

//
// msgs
//

void CSceneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(!pDoc) return;

  switch(lHint)
    {
    case SCENE_SELECT : SelectItem((GPG4::CEditorSceneItem*)pHint); break;

    // selection => check boxes

    case DOC_SELECTION_CHANGED : ChangeSelection(TVI_ROOT,(GPG4::CEditorObj*)pHint); break;
    case DOC_SELECTION_CLEARED : ClearSelection (TVI_ROOT); break;
    }
  }

//
// contextual menu
//

void CSceneView::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	*pResult = 0;

  POINT Pt;
  GetCursorPos(&Pt);
  m_Scene.ScreenToClient(&Pt);

  UINT uFlags;
  HTREEITEM hItem = m_Scene.HitTest(Pt,&uFlags);

  if(hItem)
    {
    m_Scene.SelectItem(hItem);
    GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
    if(!pObj) return;
    u32 u32Menu = pObj->GetMenu();
    if(u32Menu == GPG4::_MENU_NONE_) return;

    CMenu Menu,*pMenu;
    Menu.LoadMenu(IDR_CONTEXT_SCENE);
    GetCursorPos(&Pt);
    pMenu = Menu.GetSubMenu(u32Menu+1);
    if(!pMenu) return;

    // "Create" submenu : replace 1st item if it's a separator

    UINT uState = pMenu->GetMenuState(0,MF_BYPOSITION);
    if(uState & MF_SEPARATOR)
      {
      CMenu* pPopup = Menu.GetSubMenu(0);                     // "Create" submenu
      HMENU  hPopup = pPopup->GetSafeHmenu();
      pMenu->RemoveMenu(0,MF_BYPOSITION);
      pMenu->InsertMenu(0,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT)hPopup,"Create");
      }

    pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,Pt.x,Pt.y,/*this*/AfxGetMainWnd(),NULL);
    }
  }

//
// rename

void CSceneView::OnMenuRename() 
  {
  CRTTIPropsDoc* pDoc = GetDocument();

  HTREEITEM hItem = m_Scene.GetSelectedItem();
  if(!hItem)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : rename failed");
    return;
    }
  GPG4::CEditorSceneItem* pItem = GetItemData(hItem);
  GPG4::CEditorObj*       pObj  = GetTreeItemData(hItem);
  if(!pItem || !pObj)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : item has no data");
    return;
    }

  if(!pObj->IsRenamable()) return;

  //

  CDlgItemName dlgName;
  dlgName.m_pEditorScene = pDoc->GetEditorScene();
  dlgName.m_strName      = pObj->GetName().c_str();

  if(dlgName.DoModal() != IDOK) return;

  //

  CString strTmp("rename [");
  strTmp += pObj->GetName().c_str();
  strTmp += "] to [" + dlgName.m_strName + "]";
  _DLOG2_((*pDoc->GetLogger()),(LPCTSTR)strTmp,"scene op");

  pDoc->SetModifiedFlag();

  //

  m_Scene.SetItemText(hItem,(LPCTSTR)dlgName.m_strName);

  pDoc->GetEditorScene()->UnregisterItem(pObj->GetName(),pItem);
  pObj->SetName((LPCTSTR)dlgName.m_strName);
  pDoc->GetEditorScene()->RegisterItem  (pObj->GetName(),pItem);

    // rem: this is OK only if 1 item <-> 1 obj
  pItem->SetName(pObj->GetName());
  if(pObj->GetEngineObj()) pObj->GetEngineObj()->SetName(pObj->GetName());
    //

  pDoc->UpdateAllViews(this,PROP_UPDATE,(CObject*)pItem);
  }

//

void CSceneView::OnUpdateMenuRename(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(false);
  HTREEITEM hItem = m_Scene.GetSelectedItem();
  if(!hItem) return;
  GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
  if(!pObj) return;
  pCmdUI->Enable(pObj->IsRenamable());
  }

//
// edit

void CSceneView::OnMenuEdit()
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  HTREEITEM     hItem = m_Scene.GetSelectedItem();
  if(!hItem)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : edit failed");
    return;
    }
  GPG4::CEditorSceneItem* pItem = GetItemData(hItem);
  GPG4::CEditorObj*       pObj  = GetTreeItemData(hItem);
  if(!pItem || !pObj)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : item has no data");
    return;
    }

  if(pObj->EditExisting(GetDocument(),GPG4::CEngine::GetEngine()->m_spRenderer))
    {
    pDoc->SetModifiedFlag();
    pDoc->UpdateAllViews(this,PROP_UPDATE,(CObject*)pObj);
    }
  }

//

void CSceneView::OnUpdateMenuEdit(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(false);
  HTREEITEM hItem = m_Scene.GetSelectedItem();
  if(!hItem) return;
  GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
  if(!pObj) return;
  pCmdUI->Enable(pObj->IsRenamable());
  }

//
// create

GPG4::CEditorSceneItem* CSceneView::OnCreateItem(u32 u32Type,CStdString strItemType)
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  HTREEITEM   hParent = m_Scene.GetSelectedItem();
  if(!hParent)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : create " +strItemType+ " failed (no selected item)");
    return NULL;
    }
  GPG4::CEditorSceneItem* pParent = GetItemData(hParent);
  if(!pParent)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : parent item has no data");
    return NULL;
    }

  // editor's item & object

  GPG4::CEditorSceneItem* pItem = pDoc->CreateObject(u32Type,"",true);
  if(!pItem)
    {
    _DWAR_((*pDoc->GetLogger()),"scene op : error creating " +strItemType+ " OR cancel");
    return NULL;
    }
  GPG4::CEditorObj* pEditorObj = pItem->GetObject();
  pItem->SetName(pEditorObj->GetName());

  // engine's object

  if(!pEditorObj->CreateEngineObj())
    {
    pDoc->GetEditorScene()->UnregisterItem(pEditorObj->GetName(),pItem);  // this destroys pItem,pEditorObj,pEngineObj
    _DWAR_((*pDoc->GetLogger()),"scene op : error creating "+strItemType);
    return NULL;
    }
  GPG4::CEngineObj* pEngineObj = pEditorObj->GetEngineObj();

  // init

  if(!pEditorObj->Edit(pDoc,GPG4::CEngine::GetEngine()->m_spRenderer))
    {                                                       // editing probably cancelled
    pDoc->GetEditorScene()->UnregisterItem(pEditorObj->GetName(),pItem);
    return NULL;
    }

  // tree item

  HTREEITEM hNew = CreateItem(pItem,"",hParent);
  if(!hNew)
    {
    pDoc->GetEditorScene()->UnregisterItem(pEditorObj->GetName(),pItem);
    _DWAR_((*pDoc->GetLogger()),"scene op : error inserting item in tree");
    return NULL;
    }
  m_Scene.Expand(hParent,TVE_EXPAND);

  // link

  pParent->AddChild(pItem);

  GPG4::CEngineNode* pEngineNode = DYNAMIC_CAST(GPG4::CEngineNode,pEngineObj);
  if(pEngineNode) pDoc->GetScene()->AddSubnode(pEngineNode);

  // log

  CString strTmp("create ");
  strTmp += strItemType+" [";
  strTmp += m_Scene.GetItemText(hNew) + "] child of [" + m_Scene.GetItemText(hParent) + "]";
  _DLOG2_((*pDoc->GetLogger()),(LPCTSTR)strTmp,"scene op");

  pDoc->UpdateAllViews(this,SCENE_NEWOBJECT,(CObject*)pItem);
  pDoc->SetModifiedFlag();
  return pItem;
  }

//

void CSceneView::OnUpdateCreateItem(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(false);
  HTREEITEM hItem = m_Scene.GetSelectedItem();
  if(!hItem) return;
  GPG4::CEditorObj* pObj = GetTreeItemData(hItem);
  if(!pObj) return;
  pCmdUI->Enable(pObj->IsDropTarget());
  }

//

void CSceneView::OnCreateCamera() 
  {
  OnCreateItem(GPG4::_MENU_CAMERA_,"camera");
  }

//

void CSceneView::OnCreateFolder() 
  {
  OnCreateItem(GPG4::_MENU_FOLDER_,"folder");
  }

//

void CSceneView::OnCreateBox() 
  {
  OnCreateItem(GPG4::_MENU_BOX_,"box");
  }

//

void CSceneView::OnCreateMaterial() 
  {
  OnCreateItem(GPG4::_MENU_MATERIAL_,"material");
  }

//

void CSceneView::OnCreateNode() 
  {
  OnCreateItem(GPG4::_MENU_NODE_,"node");
  }

//

void CSceneView::OnCreateTexture() 
  {
  OnCreateItem(GPG4::_MENU_TEXTURE_,"texture");
  }

//

void CSceneView::OnCreateSphere() 
  {
	OnCreateItem(GPG4::_MENU_SPHERE_,"sphere");
  }

//
// camera

GPG4::CEditorCamera* CSceneView::GetTreeItemCamera(HTREEITEM hItem)
  {
  return DYNAMIC_CAST(GPG4::CEditorCamera,GetTreeItemData(hItem));
  }

void CSceneView::OnCameraPerspective() 
  {
  GetDocument()->TogglePerspective(GetTreeItemCamera(m_Scene.GetSelectedItem()));
  }

void CSceneView::OnUpdateCameraPerspective(CCmdUI* pCmdUI) 
  {
  GetDocument()->UpdateCameraPerspective(pCmdUI,GetTreeItemCamera(m_Scene.GetSelectedItem()));
  }
