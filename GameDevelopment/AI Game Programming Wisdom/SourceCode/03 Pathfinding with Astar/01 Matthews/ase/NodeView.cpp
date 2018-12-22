// LeftView.cpp : implementation of the CNodeView class
//

#include "stdafx.h"
#include "ase.h"

#include "aseDoc.h"
#include "NodeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodeView

IMPLEMENT_DYNCREATE(CNodeView, CTreeView)

BEGIN_MESSAGE_MAP(CNodeView, CTreeView)
	//{{AFX_MSG_MAP(CNodeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeView construction/destruction

TVSORTCB CNodeView::m_tvSortData;

CNodeView::CNodeView()
{
	m_cImageList.Create(IDB_NODEICONS,16,16,RGB(255,0,255));

	m_cTahoma.CreatePointFont(80, "Verdana");
}

CNodeView::~CNodeView()
{
}

BOOL CNodeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNodeView drawing

void CNodeView::OnDraw(CDC* pDC)
{
	CAseDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


void CNodeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	CTreeCtrl &ctrl = GetTreeCtrl();

	ctrl.SetFont(&m_cTahoma, true);
	ctrl.SetImageList(&m_cImageList, TVSIL_NORMAL);
	ctrl.DeleteAllItems();

	m_tvSortData.hParent = NULL;
	m_tvSortData.lpfnCompare = SortByF;
	m_tvSortData.lParam = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNodeView diagnostics

#ifdef _DEBUG
void CNodeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNodeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CAseDoc* CNodeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAseDoc)));
	return (CAseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNodeView message handlers

BOOL CNodeView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CNodeView::OnNotifyChild(_asNode *parent, _asNode *child, int data, void *pThis)
{
	CNodeView *me = reinterpret_cast<CNodeView *>(pThis);
	CTreeCtrl &treectrl = me->GetTreeCtrl();

	CString str;
	str.Format("%d,%d: f=%d, g=%d, h=%d", child->x, child->y, child->f, child->g, child->h);

	HTREEITEM hNew;
	HTREEITEM hTree = (parent) ? HTREEITEM(parent->dataptr) : treectrl.GetRootItem();

	me->GetDocument()->NodeAdded(child, data);

	int icon = data;
	hNew = treectrl.InsertItem(str, icon, icon, hTree);

	child->dataptr = (void *)(hNew);
	treectrl.SetItemData(hNew, DWORD(child));

	return 0;
}

int CNodeView::OnNotifyList(_asNode *previous, _asNode *addnode, int data, void *pThis)
{
	CNodeView *me = reinterpret_cast<CNodeView *>(pThis);
	
	HTREEITEM hItem;
	HTREEITEM hPrevious = (previous) ? HTREEITEM(previous->dataptr) : NULL;

	CString str;
	CTreeCtrl &treectrl = me->GetTreeCtrl();
	
	if (data < 3) {
		hItem = treectrl.GetNextItem(treectrl.GetRootItem(), TVGN_NEXT);
		str.Format("%d,%d: f = %d", addnode->x, addnode->y, addnode->f);

		if (data == 0) {
			hItem = treectrl.InsertItem(str, 2, 2, hItem, hPrevious);
			treectrl.SetItemData(hItem, DWORD(addnode));
		} else if (data == 1) {
			hItem = treectrl.InsertItem(str, 2, 2, hItem, hItem);
			treectrl.SetItemData(hItem, DWORD(addnode));
		} else if (data == 2) {
			hItem = treectrl.GetChildItem(hItem);
			if (hItem) treectrl.DeleteItem(hItem);
		}
	} else if (data == 3) {
		hItem = treectrl.GetRootItem();
		hItem = treectrl.GetNextItem(hItem, TVGN_NEXT);
		hItem = treectrl.GetNextItem(hItem, TVGN_NEXT);

		str.Format("%d,%d", addnode->x, addnode->y);

		hItem = treectrl.InsertItem(str, 4, 4, hItem, NULL);
		treectrl.SetItemData(hItem, DWORD(addnode));
	}

	return 0;
}

void CNodeView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	_asNode *node = (_asNode *)(GetTreeCtrl().GetItemData(hTreeItem));

	GetDocument()->DrawNode(node);

	*pResult = 0;
}

void CNodeView::OnPreAStar()
{
	m_hGoalNode = NULL;
	GetTreeCtrl().DeleteAllItems();
}

void CNodeView::OnPostAStar(_asNode *end)
{
	ASSERT(end);

	m_hGoalNode = HTREEITEM(end->dataptr);
	GetTreeCtrl().SetItemState(m_hGoalNode, TVIS_BOLD, TVIS_BOLD);
}

void CNodeView::DisplayGoalNode()
{
	if (m_hGoalNode) GetTreeCtrl().EnsureVisible(m_hGoalNode);
}

int CNodeView::SortByF(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	_asNode *item1 = reinterpret_cast<_asNode *>(lParam1);
	_asNode *item2 = reinterpret_cast<_asNode *>(lParam2);

	if (item1->f <= item2->f) return -1;

	return 1;
}

void CNodeView::SortOpen()
{
	HTREEITEM hItem;
	hItem = GetTreeCtrl().GetNextItem(GetTreeCtrl().GetRootItem(), TVGN_NEXT);

	if (hItem) {
		m_tvSortData.hParent = hItem;

		GetTreeCtrl().SortChildrenCB(&m_tvSortData);
	}
}
