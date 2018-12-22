// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "TreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//

ItemCallbackEx CTreeCtrlEx::m_pItemCallback = NULL;
HTREEITEM      CTreeCtrlEx::m_hParent       = TVI_ROOT;

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
  m_pDragImgList = NULL;
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
	ON_WM_MOUSEWHEEL()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

void CTreeCtrlEx::ParseBranch(HTREEITEM hItem,bool boRecurse)
	{
	if(!hItem) return;

	HTREEITEM hChild = GetChildItem(hItem);
  if(m_pItemCallback) (*m_pItemCallback)(this,hChild);
	if(boRecurse) ParseBranch(hChild);

	while(hChild)
		{
		hChild = GetNextSiblingItem(hChild);
    if(m_pItemCallback) (*m_pItemCallback)(this,hChild);
		if(boRecurse) ParseBranch(hChild);
		}
	}

//

void CTreeCtrlEx::CopyItem(HTREEITEM hDest,HTREEITEM hSrc)
  {
  TV_ITEM tvSrc;
  tvSrc.mask  = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
  tvSrc.hItem = hSrc;
  if(!GetItem(&tvSrc)) return;

  tvSrc.hItem = hDest;
  SetItem(&tvSrc);
  SetItemText(hDest,GetItemText(hSrc));
  SetCheck   (hDest,GetCheck   (hSrc));

  if(tvSrc.state & TVIS_EXPANDED)
    Expand(hDest,TVE_EXPAND);
  }

//

void CTreeCtrlEx::CopySubtree(HTREEITEM hDest,HTREEITEM hSrc)
  {
  if(!hDest || !hSrc) return;

  HTREEITEM hChildSrc = GetChildItem(hSrc);
  while(hChildSrc)
    {
    HTREEITEM   hChildDest = InsertItem("dest child",hDest);
    CopySubtree(hChildDest,hChildSrc);
    CopyItem   (hChildDest,hChildSrc);

    hChildSrc = GetNextSiblingItem(hChildSrc);
    }
  }

//

HTREEITEM CTreeCtrlEx::InsertItemAndSubtree(HTREEITEM hDest,HTREEITEM hSrc)
{
  if(!hDest || !hSrc) return NULL;

  HTREEITEM hNew = InsertItem("new",hDest);
  if(!hNew) return NULL;

  CopySubtree(hNew,hSrc);                                   // start with subtree so that item is correctly expanded
  CopyItem   (hNew,hSrc);
  return hNew;
}

//

void CTreeCtrlEx::CopyItem(HTREEITEM hDest,CTreeCtrlEx& treeSrc,HTREEITEM hSrc)
  {
  TV_ITEM tvSrc;
  tvSrc.mask  = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
  tvSrc.hItem = hSrc;
  if(!treeSrc.GetItem(&tvSrc)) return;

  tvSrc.hItem = hDest;
  SetItem(&tvSrc);
  SetItemText(hDest,treeSrc.GetItemText(hSrc));
  SetCheck   (hDest,treeSrc.GetCheck   (hSrc));

  if(tvSrc.state & TVIS_EXPANDED)
    Expand(hDest,TVE_EXPAND);
  }

//

void CTreeCtrlEx::CopySubtree(HTREEITEM hDest,CTreeCtrlEx& treeSrc,HTREEITEM hSrc)
  {
  if(!hDest || !hSrc) return;

  HTREEITEM hChildSrc = treeSrc.GetChildItem(hSrc);
  while(hChildSrc)
    {
    HTREEITEM   hChildDest = InsertItem("dest child",hDest);
    CopySubtree(hChildDest,treeSrc,hChildSrc);
    CopyItem   (hChildDest,treeSrc,hChildSrc);

    hChildSrc = treeSrc.GetNextSiblingItem(hChildSrc);
    }
  }

//
//

BOOL CTreeCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
  {
  if(m_pDragImgList) m_pDragImgList->DragShowNolock(false);
	BOOL Res = CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
  if(m_pDragImgList) m_pDragImgList->DragShowNolock(true);
  return Res;
  }

//

void CTreeCtrlEx::OnMButtonDown(UINT nFlags, CPoint point) 
  {	
	CWnd::OnMButtonDown(nFlags, point);
  }

//

void CTreeCtrlEx::OnMButtonDblClk(UINT nFlags, CPoint point) 
  {	
	CWnd::OnMButtonDblClk(nFlags, point);
  }
