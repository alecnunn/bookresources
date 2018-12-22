// CommandView.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"
#include "CommandView.h"

#include "CustomMsgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommandView

IMPLEMENT_DYNCREATE(CCommandView, CFormView)

CCommandView::CCommandView()
	: CFormView(CCommandView::IDD)
  {
	//{{AFX_DATA_INIT(CCommandView)
	m_boReadOnlyProps = TRUE;
	//}}AFX_DATA_INIT
  m_boDragging   = false;
  m_pDragImgList = NULL;
	m_imgList.Create(IDB_SCENE_TREE,16,1,RGB(255,255,255));
  }

CCommandView::~CCommandView()
  {
  delete m_pDragImgList;
  m_imgList.DeleteImageList();
  }

void CCommandView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommandView)
	DDX_Control(pDX, IDC_GRAPH, m_SceneGraph);
	DDX_Control(pDX, IDC_MOVECAM, m_btnMoveCam);
	DDX_Control(pDX, IDC_MOVESEL, m_btnMoveSel);
	DDX_Check(pDX, IDC_READONLYPROPS, m_boReadOnlyProps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommandView, CFormView)
	//{{AFX_MSG_MAP(CCommandView)
	ON_BN_CLICKED(IDC_CLEARSEL, OnClearsel)
	ON_BN_CLICKED(IDC_MOVESEL, OnMovesel)
	ON_BN_CLICKED(IDC_MOVECAM, OnMovecamera)
	ON_BN_CLICKED(IDC_READONLYPROPS, OnReadonlyprops)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_GRAPH, OnBegindragTree)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_NOTIFY(TVN_KEYDOWN, IDC_GRAPH, OnKeydownTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommandView diagnostics

#ifdef _DEBUG
void CCommandView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCommandView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRTTIPropsDoc* CCommandView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCommandView message handlers

void CCommandView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  switch(lHint)
    {
    case TOOL_CHANGED    : UpdateControls(); break;
    case SCENE_NEWOBJECT : UpdateGraph();    break;
    }
  }

void CCommandView::OnInitialUpdate() 
  {
	CFormView::OnInitialUpdate();
  m_boReadOnlyProps = GetDocument()->GetReadOnlyProps();
  UpdateControls();

  m_SceneGraph.SetImageList(&m_imgList,TVSIL_NORMAL);
  UpdateGraph();
  }

void CCommandView::UpdateControls()
  {
  CRTTIPropsDoc* pDoc = GetDocument();
  if(!pDoc) return;
  if(!m_btnMoveCam.m_hWnd) return;

  m_btnMoveCam.SetCheck(pDoc->GetToolName() == "MoveCamera");
  m_btnMoveSel.SetCheck(pDoc->GetToolName() == "MoveSelection");
  UpdateData(false);
  }

//

void CCommandView::OnClearsel() 
  {
  GetDocument()->ClearSelection();
  }

void CCommandView::OnMovesel() 
  {
  GetDocument()->UpdateAllViews(this,m_btnMoveSel.GetCheck()==TRUE ? TOOL_MOVESELECTION : TOOL_NONE);
  }

void CCommandView::OnMovecamera() 
  {
  GetDocument()->UpdateAllViews(this,m_btnMoveCam.GetCheck()==TRUE ? TOOL_MOVECAMERA: TOOL_NONE);
  }

void CCommandView::OnReadonlyprops() 
  {
  UpdateData();
  GetDocument()->SetReadOnlyProps(m_boReadOnlyProps != FALSE);
  GetDocument()->UpdateAllViews(this,PROP_UPDATE,NULL);
  }

//

void CCommandView::OnSize(UINT nType, int cx, int cy) 
  {
	CFormView::OnSize(nType, cx, cy);

  // resize tree

  if(m_SceneGraph.m_hWnd)
    {                                                       // otherwise not created yet
    RECT rect,rect2;
    GetClientRect(&rect);
    m_SceneGraph.GetWindowRect(&rect2);
    ScreenToClient(&rect2);
    int iSizeX,iSizeY;
    iSizeX = rect.right -rect.left-15;
    iSizeY = rect.bottom-rect.top -5-rect2.top;
    if((iSizeX > 0) && (iSizeY > 0)) m_SceneGraph.SetWindowPos(NULL,0,0,iSizeX,iSizeY,SWP_NOMOVE);
    #undef BORDER
    }
  }

//

void CCommandView::UpdateGraph()
  {
  if(!m_SceneGraph.m_hWnd) return;
  m_SceneGraph.DeleteAllItems();
  BuildSubtree(GetDocument()->GetScene(),TVI_ROOT);
  }

// recursive

void CCommandView::BuildSubtree(GPG4::CEngineNode* pNode,HTREEITEM hParent)
  {
  if(!pNode) return;
  GPG4::CEditorNode* pEditor = DYNAMIC_CAST(GPG4::CEditorNode,pNode->GetEditorObj());
  if(!pEditor) return;
  HTREEITEM hNew = m_SceneGraph.InsertItem(pNode->GetName(),hParent);
  if(!hNew) return;

  TV_ITEM tvItem;
  tvItem.mask   = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  tvItem.iImage = tvItem.iSelectedImage = pEditor->GetIcon();
	tvItem.lParam = (LPARAM)pNode;
  tvItem.hItem  = hNew;
  m_SceneGraph.SetItem(&tvItem);

  GPG4::CEngineNode* pChild = pNode->GetFirstSubnode();
  while(pChild)
    {
    BuildSubtree(pChild,hNew);
    m_SceneGraph.Expand(hNew,TVE_EXPAND);
    pChild = pNode->GetNextSubnode();
    }
  }

//
// drag & drop
//

void CCommandView::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	TV_KEYDOWN*  pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
  WORD wVKey = pTVKeyDown->wVKey;

  if(wVKey == VK_CONTROL) OnMButtonDblClk(0,CPoint(0,0));   // collapse/expand in drag & drop mode
	
	*pResult = 0;
  }

//

void CCommandView::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  HTREEITEM    hItem       = pNMTreeView->itemNew.hItem;
  GPG4::CEngineNode* pNode = (GPG4::CEngineNode*)m_SceneGraph.GetItemData(hItem);
  m_SceneGraph.SelectItem(hItem);
  if(!pNode) return;

  m_hDragItem    = hItem;
  m_pDragImgList = m_SceneGraph.CreateDragImage(hItem);
  if(!m_pDragImgList) return;
  
  m_pDragImgList->BeginDrag(0,CPoint(0,0));
  m_pDragImgList->DragEnter(this,pNMTreeView->ptDrag);
  m_SceneGraph.m_pDragImgList = m_pDragImgList;
  m_boDragging = true;

  SetCapture();
  m_hDragTarget = NULL;

  m_nTimer = SetTimer(2,25,NULL);
	*pResult = 0;
  }

// end drag

void CCommandView::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  if(m_boDragging)
    {
    KillTimer(m_nTimer);

    ReleaseCapture();
    m_boDragging = false;

    m_pDragImgList->DragLeave(this);
    m_pDragImgList->EndDrag();
    m_SceneGraph.m_pDragImgList = NULL;
    delete m_pDragImgList;
    m_pDragImgList = NULL;

    m_SceneGraph.SelectDropTarget(NULL);
    m_SceneGraph.SelectItem(m_hDragItem);

    if(m_hDragTarget && (m_hDragTarget != m_hDragItem))
      {
      SuccessfulDrag(m_hDragTarget,m_hDragItem);
      }
    } 	

	CFormView::OnLButtonUp(nFlags, point);
  }

// cancel drag

void CCommandView::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  if(m_boDragging)
    {
    KillTimer(m_nTimer);

    ReleaseCapture();
    m_boDragging = false;

    m_pDragImgList->DragLeave(this);
    m_pDragImgList->EndDrag();
    m_SceneGraph.m_pDragImgList = NULL;
    delete m_pDragImgList;
    m_pDragImgList = NULL;

    m_SceneGraph.SelectDropTarget(NULL);
    m_SceneGraph.SelectItem(m_hDragItem);
    m_SceneGraph.EnsureVisible(m_hDragItem);
    } 	

	CFormView::OnRButtonDown(nFlags, point);
  }

// collapse/expand item

void CCommandView::OnMButtonDown(UINT nFlags, CPoint point) 
  {
	CFormView::OnMButtonDown(nFlags, point);
  }


void CCommandView::OnMButtonDblClk(UINT nFlags, CPoint point) 
  {	
  if(m_boDragging) 
    {
    if(m_hDragTarget)
      {
      m_pDragImgList->DragShowNolock(false);
      m_SceneGraph.Expand(m_hDragTarget,TVE_TOGGLE);
      m_SceneGraph.RedrawWindow();
      m_pDragImgList->DragShowNolock(true);
      }
    }

	CFormView::OnMButtonDblClk(nFlags, point);
  }

// highlight & scroll

void CCommandView::OnTimer(UINT nIDEvent) 
  {
  if(m_boDragging)
    {
    POINT point,pt2;
    GetCursorPos(&point);
    pt2 = point;
    ScreenToClient(&pt2);
    m_SceneGraph.ScreenToClient(&point);

    // highlight target

    TVHITTESTINFO tvHit;
    tvHit.pt = point;
    HTREEITEM hTarget = m_SceneGraph.HitTest(&tvHit);

    if(hTarget)
      {
      if(hTarget && (hTarget != m_hDragTarget))
        {
        m_pDragImgList->DragShowNolock(false);
        m_SceneGraph.EnsureVisible(hTarget);
        m_SceneGraph.SelectDropTarget(hTarget);
        m_pDragImgList->DragShowNolock(true);
        m_hDragTarget = hTarget;
        }
      }

    // scroll tree

    else
      {
      RECT rect;
      m_SceneGraph.GetClientRect(&rect);

      int iMaxV = m_SceneGraph.GetScrollLimit(SB_VERT);
 	    int iPosV = m_SceneGraph.GetScrollPos  (SB_VERT);

      // up
      if((point.y < rect.top -10) && iPosV)
        {
        HTREEITEM hPrev = m_SceneGraph.GetPrevVisibleItem(m_SceneGraph.GetFirstVisibleItem());
        m_pDragImgList->DragShowNolock(false);
        m_SceneGraph.EnsureVisible(hPrev);
        m_pDragImgList->DragShowNolock(true);
        }

      // down
      if((point.y > rect.bottom +10) && (iPosV != iMaxV))
        {
        UINT Nb = m_SceneGraph.GetVisibleCount();
        if(Nb != -1)
          {
          HTREEITEM hNext = m_SceneGraph.GetFirstVisibleItem();
          for(UINT i = 0; i < Nb; i++) hNext = m_SceneGraph.GetNextVisibleItem(hNext);
          m_pDragImgList->DragShowNolock(false);
          m_SceneGraph.EnsureVisible(hNext);
          m_pDragImgList->DragShowNolock(true);
          }
        }

      int iPosH = m_SceneGraph.GetScrollPos  (SB_HORZ);
    	int iMaxH = m_SceneGraph.GetScrollLimit(SB_HORZ);

      // left
      if((point.x < rect.left) && iPosH)
        {
        m_pDragImgList->DragShowNolock(false);
        m_SceneGraph.SendMessage(WM_HSCROLL,SB_LINELEFT);
        m_pDragImgList->DragShowNolock(true);
        }

      // right
      if((point.x > rect.right) && (iPosH != iMaxH))
        {
        m_pDragImgList->DragShowNolock(false);
        m_SceneGraph.SendMessage(WM_HSCROLL,SB_LINERIGHT);
        m_pDragImgList->DragShowNolock(true);
        }
      }

    m_pDragImgList->DragMove(pt2);
    } 
	
	CFormView::OnTimer(nIDEvent);
  }

//

void CCommandView::SuccessfulDrag(HTREEITEM hDest,HTREEITEM hSrc)
  {
  // test : no op if hSrc is already a child of hDest

  if(m_SceneGraph.GetParentItem(hSrc) == hDest) return;

  // test : if hDest is a child of hSrc, we're going to have a stack overflow ! (infinite loop)

  HTREEITEM hParent = hDest;
  while(hParent)
    {
    if(hParent == hSrc)
      {
      _DLOG2_((*GetDocument()->GetLogger()),"invalid drag & drop : destination can't be a child of source (infinite loop)","cancelled");
      return;
      }
    hParent = m_SceneGraph.GetParentItem(hParent);
    }

  //

  HTREEITEM hNew = m_SceneGraph.InsertItemAndSubtree(hDest,hSrc);
  if(!hNew) return;

  CString strTmp("drag & drop : ");
  strTmp += "[" + m_SceneGraph.GetItemText(hSrc) + "] becomes a child of [" + m_SceneGraph.GetItemText(hDest) +"]";
  _DLOG2_((*GetDocument()->GetLogger()),(LPCTSTR)strTmp,"graph op");

  GetDocument()->SetModifiedFlag();

  // same changes in scene graph

  GPG4::CEngineNode* pSrc  = (GPG4::CEngineNode*)m_SceneGraph.GetItemData(hSrc);
  GPG4::CEngineNode* pDest = (GPG4::CEngineNode*)m_SceneGraph.GetItemData(hDest);
  GPG4::CEngineNode* pSrcParent = (pSrc ? pSrc->GetParentNode() : NULL);

  if(pSrc && pDest && pSrcParent)
    {
    GPG4::CVect3D     v3Pos = pSrc->GetWorldPosition();
    GPG4::CQuaternion qRot  = pSrc->GetWorldRotation();

    pDest->AddSubnode(pSrc);                                // parent is set automatically
    pSrcParent->RemoveSubnode(pSrc);

    pSrc->SetWorldRotation(qRot);
    pSrc->SetWorldPosition(v3Pos);
    }
  else
    {
    _DERR_((*GetDocument()->GetLogger()),"pSrc, pDest or pSrcParent is NULL");
    }

  //

  m_SceneGraph.SelectItem   (hNew);
  m_SceneGraph.EnsureVisible(hNew);
  m_SceneGraph.DeleteItem   (hSrc);
  }
