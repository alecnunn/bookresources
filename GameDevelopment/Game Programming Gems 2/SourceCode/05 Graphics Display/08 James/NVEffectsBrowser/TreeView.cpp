/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "stdafx.h"
#include "NVEffectsBrowser.h"

#include "Doc.h"
#include "TreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace nv_objects;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CShaderTree

IMPLEMENT_DYNCREATE(CShaderTree, CTreeView)

BEGIN_MESSAGE_MAP(CShaderTree, CTreeView)
	//{{AFX_MSG_MAP(CShaderTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CShaderTree construction/destruction

#define ILI_CLOSED_FOLDER     0
#define ILI_OPEN_FOLDER       1
#define ILI_D3D_SHADER        2
#define ILI_D3D_SHADER_GREYED 3
#define ILI_OGL_SHADER        4
#define ILI_OGL_SHADER_GREYED 5

CShaderTree::CShaderTree()
: m_bPopulating(false)
{
}

CShaderTree::~CShaderTree()
{
}

BOOL CShaderTree::PreCreateWindow(CREATESTRUCT& cs)
{
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CShaderTree drawing

void CShaderTree::OnDraw(CDC* pDC)
{
	CNVEffectsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

void CShaderTree::OnInitialUpdate()
{
	m_ImageList.Create(IDB_IMAGELIST, 16, 1, CLR_DEFAULT);
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	TreeCtrl.ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, 0);

	CTreeView::OnInitialUpdate();
}

void CShaderTree::PopulateTree()
{
	const std::list<EBEffect*>* pEffectList;
	std::list<EBEffect*>::const_iterator itrEffects;
	EBEffect* pCurrentEffect;

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.DeleteAllItems();

	CNVEffectsDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	m_bPopulating = true;

	m_hRoot = TreeCtrl.InsertItem("Effects", ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER);

	pEffectList = pDoc->GetEffectsList();
	if (pEffectList)
	{

		for (itrEffects = pEffectList->begin(); itrEffects != pEffectList->end(); itrEffects++)
		{
			pCurrentEffect = *itrEffects;
			if (!pCurrentEffect)
				continue;

			string strName;
			EBProperty* pName = pCurrentEffect->GetProperty("EffectName");
			if (pName && pName->IsKindOf(EBTYPE_STRING_PROP))
			{
				strName = pName->ReadValue(pCurrentEffect);
			}
			else
			{
				strName = "No EffectName property in Effect!";
			}

			string strLocation;
			EBProperty* pLocation = pCurrentEffect->GetProperty("EffectLocation");
			if (pLocation && pLocation->IsKindOf(EBTYPE_STRING_PROP))
			{
				strLocation = pLocation->ReadValue(pCurrentEffect);
			}
			else
			{
				strLocation = "No EffectLocation property in Effect!";
			}

			string::size_type StartPos = 0;
			string::size_type EndPos;

			HTREEITEM hParent = m_hRoot;
			HTREEITEM hCurrent;
			HTREEITEM hNewInsert;

			// Walk down the tree, building it if we need to
			do
			{

				EndPos = strLocation.find_first_of("\\", StartPos);
				if (EndPos == StartPos)
				{
					break;
				}
				
				string strInsert = strLocation.substr(StartPos, EndPos);
				hCurrent = FindNamedEntry(hParent, strInsert);

				// Keep walking the tree
				if (hCurrent)
				{
					hNewInsert = hCurrent;
				}
				else
				{
					hNewInsert = TreeCtrl.InsertItem(strInsert.c_str(), ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER, hParent);
				}
				hParent = hNewInsert;
				StartPos = EndPos + 1;
			}
			while(EndPos != strLocation.npos);

			// Insert the actual effect
			BOOL fActive = pDoc->ConfirmDevice(pCurrentEffect) == S_OK;
			BOOL fOGLAPI = pCurrentEffect->API() == GFXAPI_OPENGL;
			UINT nIconID = 2 + (fOGLAPI ? 2 : 0) + (fActive ? 0 : 1);

			string strTitle = strName;
			if (!fActive)
				strTitle = strTitle + " (Not supported)";

			HTREEITEM hItem = TreeCtrl.InsertItem(strTitle.c_str(), nIconID, nIconID, hNewInsert);
			
			// Record the test pointer in the item
			TVITEM TreeItem;
			TreeItem.mask = TVIF_HANDLE | TVIF_PARAM;
			TreeItem.hItem = hItem;
			TreeItem.lParam = (DWORD)pCurrentEffect;
			TreeCtrl.SetItem(&TreeItem); 
		}
	}

	
	// Get the currently selected test
	pCurrentEffect = pDoc->GetCurrentEffect();

	// Expand tree based on user options
	if (theApp.GetExpandAll())
		ExpandAll(m_hRoot);

	m_bPopulating = false;
	FindAndSelect(m_hRoot, pCurrentEffect);

}
/////////////////////////////////////////////////////////////////////////////
// CShaderTree diagnostics

#ifdef _DEBUG
void CShaderTree::AssertValid() const
{
	CTreeView::AssertValid();
}

void CShaderTree::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CNVEffectsDoc* CShaderTree::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNVEffectsDoc)));
	return (CNVEffectsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShaderTree message handlers
void CShaderTree::ExpandAll(HTREEITEM hParent)
{
	HTREEITEM hNewItem;
	HTREEITEM hCurrent;
	CTreeCtrl& TreeCtrl = GetTreeCtrl();

	hCurrent = hParent;
	while(hCurrent != NULL)
	{
		// Check the name of this branch
		TRACE((LPCSTR)TreeCtrl.GetItemText(hCurrent));
		TRACE("\n");
		TreeCtrl.Expand(hCurrent, TVE_EXPAND);
	
		// Get the child of this opened branch
		hNewItem = TreeCtrl.GetChildItem(hCurrent);
		if (hNewItem != NULL)
			ExpandAll(hNewItem);

		// Get the next item down the list
		hCurrent = TreeCtrl.GetNextSiblingItem(hCurrent);
	};
}

// Finds an entry in the tree.  Doesn't walk down the hierarchy
HTREEITEM CShaderTree::FindNamedEntry(HTREEITEM hParent, const string& strName)
{
	HTREEITEM hCurrent;
	char Name[1024];
	CTreeCtrl& TreeCtrl = GetTreeCtrl();

	hCurrent = TreeCtrl.GetChildItem(hParent);
	while (hCurrent != NULL)
	{
		TVITEM TreeItem;
		TreeItem.hItem = hCurrent;
		TreeItem.mask = TVIF_TEXT | TVIF_HANDLE;
		TreeItem.pszText = Name;
		TreeItem.cchTextMax = 1024;
		BOOL bOK = TreeCtrl.GetItem(&TreeItem);
		if (bOK)
		{
			if (strName.compare(TreeItem.pszText) == 0)
			{
				return hCurrent;
			}
		}		
	
		hCurrent = TreeCtrl.GetNextItem(hCurrent, TVGN_NEXT);

	};

	return NULL;
}

bool CShaderTree::FindAndSelect(HTREEITEM hParent, EBEffect* pCurrentTest)
{
	HTREEITEM hNewItem;
	HTREEITEM hCurrent;
	CTreeCtrl& TreeCtrl = GetTreeCtrl();

	hCurrent = hParent;
	while(hCurrent != NULL)
	{
		// Get the test pointer in the item
		TVITEM TreeItem;
		TreeItem.mask = TVIF_HANDLE | TVIF_PARAM;
		TreeItem.hItem = hCurrent;
		TreeCtrl.GetItem(&TreeItem); 
		if ((EBEffect*)TreeItem.lParam == pCurrentTest)
		{
			TreeCtrl.SelectItem(hCurrent);
			return true;
		}

		// Get the child of this opened branch
		hNewItem = TreeCtrl.GetChildItem(hCurrent);
		if (hNewItem != NULL)
		{
			if (FindAndSelect(hNewItem, pCurrentTest))
				return true;
		}

		// Get the next item down the list
		hCurrent = TreeCtrl.GetNextSiblingItem(hCurrent);
	};

	return false;
}

void CShaderTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = 0;
}

void CShaderTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	EBEffect* pCurrentEffect;
	CNVEffectsDoc* pDoc = GetDocument();

	if (pNMTreeView->action == TVE_EXPAND)
	{
		GetTreeCtrl().SetItemImage(hItem, ILI_OPEN_FOLDER, ILI_OPEN_FOLDER);
		
		// Get the currently selected test
		pCurrentEffect = pDoc->GetCurrentEffect();
		FindAndSelect(hItem, pCurrentEffect);
	}
	else if (pNMTreeView->action == TVE_COLLAPSE)
	{
		GetTreeCtrl().SetItemImage(hItem, ILI_CLOSED_FOLDER, ILI_CLOSED_FOLDER);
	}
	
	*pResult = 0;

}

// If the test being selected for this device can't run,
// we fail the selection
void CShaderTree::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	
	// Record the test pointer in the item
	TVITEM TreeItem;
	TreeItem.mask = TVIF_HANDLE | TVIF_PARAM;
	TreeItem.hItem = pNMTreeView->itemNew.hItem;
	TreeCtrl.GetItem(&TreeItem); 

	EBEffect* pEffect = (EBEffect*)TreeItem.lParam;

	if (pEffect && !m_bPopulating)
	{
		CNVEffectsDoc* pDoc = GetDocument();
		pDoc->SetCurrentEffect(pEffect);
	}		
	
	*pResult = 0;
}
