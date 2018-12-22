/******************************************************************************
tree.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:



******************************************************************************/

#include "stdafx.h"
#include "tree2.h"

 
void CLeaf::Init()
{
  ClearName();

  layer = 0;
  parent = 0;
  icon_number = 0;
  selected_icon_number = 0;
  id = 0;
  arg = 0;

  user_data = 0;


}

CTree::~CTree()
{
    int i;
    
    if (tree)
    {
        for(i=0; i<top; i++)
            tree[i].ReleaseName();

        delete [] tree;
        tree = 0;
    }
}






char * CTree::YesNo(int b)
{
  if (b)
    return "Yes";
  else
    return "No";
}


int CTree::Insert(int layer, int parent, int selected_icon_number, int icon_number, 
                  int type, void * data, bool bHasCheckBox, const char * __format, ...)
{
  int t;
  va_list arg;

  m_leaf->layer = layer;
  m_leaf->parent = parent;
  m_leaf->selected_icon_number = selected_icon_number;
  m_leaf->icon_number = icon_number;
  m_leaf->ClearName();
  m_leaf->user_data_type = type;
  m_leaf->user_data = data;
  m_leaf->bHasCheckBox = bHasCheckBox;

  //m_leaf->func = 0; // init to no function
  

  va_start(arg, __format);
  char temp[5000];

  vsprintf(temp,__format, arg);

  m_leaf->SetName(temp);


  t = top;
  m_leaf++; 
  top++;

  return t;

}



        
/////////////////////////

void CTree::Reset()
{
  top = 0;
  m_leaf = &tree[0];
}




void CTree::InitTree(CTreeCtrl* treectrl, CListCtrl	*listctrl, int max_nodes)
{
  int i;

  Reset();
  m_treectrl = treectrl;

  tree = new CLeaf[max_nodes];
  for(i=0; i<max_nodes; i++)
    tree[i].Init();




}
    /*
void CTree::create_icons(CIcon * icons, int n)
{
  int i;

  //int num_icons = sizeof(icons) / sizeof(Icon);
  
  m_imageList.Create(16,16, 0, 0, n * 2);

  for(i=0; i<n; i++)
  {
    m_imageList.Add( AfxGetApp()->LoadIcon(icons[i].selected));
    m_imageList.Add( AfxGetApp()->LoadIcon(icons[i].unselected));
  }
}     */


void CTree::BuildTree()
{  
  int i;
  CLeaf * leaf;  


  m_treectrl->DeleteAllItems();

  HTREEITEM hItem;


  m_treectrl->SetImageList(&m_imageList, TVSIL_NORMAL);


  // tree structure common values
  TV_INSERTSTRUCT tvinsert;
  memset(&tvinsert, 0, sizeof(TV_INSERTSTRUCT));

  tvinsert.hInsertAfter = TVI_LAST;
  tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM; 

  //tvinsert.state          = INDEXTOSTATEIMAGEMASK (TVIS_CHECK);
  //tvinsert.stateMask      = TVIS_STATEIMAGEMASK;


  tvinsert.item.hItem = NULL;
  tvinsert.item.cchTextMax = 12;



  // top level
  for(i=0; i<top; i++)
  {

    leaf = &tree[i];

    tvinsert.item.pszText = leaf->GetName();

    tvinsert.item.iSelectedImage = leaf->selected_icon_number;
    tvinsert.item.iImage = leaf->icon_number;

    tvinsert.item.lParam = i;


    if (leaf->layer)
    {
      if (leaf->parent < 0)
        tvinsert.hParent = tree[i + leaf->parent].id;
      else
        tvinsert.hParent = tree[leaf->parent].id;
    }
    else
      tvinsert.hParent = 0;

    hItem = tree[i].id = m_treectrl->InsertItem(&tvinsert);

    if (leaf->bHasCheckBox)
        SetCheckState(hItem, true);
  }

}


BOOL CTree::SetCheckState(HTREEITEM hItem, BOOL fCheck)
{
    TVITEM tvItem;

    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;

    /*
    Since state images are one-based, 1 in this macro turns the check off, and 
    2 turns it on.
    */
    tvItem.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));

    return TreeView_SetItem(*m_treectrl, &tvItem);
}

BOOL CTree::GetCheckState(HTREEITEM hItem)
{
    TVITEM tvItem;

    // Prepare to receive the desired information.
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;

    // Request the information.
    TreeView_GetItem(*m_treectrl, &tvItem);

    // Return zero if it's not checked, or nonzero otherwise.
    return ((BOOL)(tvItem.state >> 12) -1);
}








void CList::InitList(CListCtrl * listctrl)
{
  m_tmAveCharWidth = 8;
  m_listctrl = listctrl;

  HFONT       g_hFont;
  CDC *cdc;
  int PixelsPerInch;
  TEXTMETRIC tm;
  DWORD tmAveCharWidth;

  
  cdc = m_listctrl->GetDC();
  PixelsPerInch = cdc->GetDeviceCaps(LOGPIXELSX);
  g_hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
  cdc->SelectObject(g_hFont);
  cdc->GetTextMetrics(&tm);
  tmAveCharWidth = tm.tmAveCharWidth;
  m_listctrl->ReleaseDC(cdc);

  set_tree_char_width(tmAveCharWidth);

  ResetListViewRefresh();

  LVAddColumn(0, "Texture", 24);
  //LVAddColumn(1, "Status", 10);


}


int CList::LVAddText(int col, const char *sz, ...)
{
  LV_ITEM lvi;
  char    ach[1000];
  va_list vl;
  
  va_start(vl, sz );
  wvsprintf(ach,sz, vl);
  
  lvi.mask        = LVIF_TEXT;
  lvi.iSubItem    = 0;
  lvi.state       = 0;
  lvi.stateMask   = 0;
  lvi.pszText     = ach;
  lvi.cchTextMax  = 0;
  lvi.iImage      = 0;
  lvi.lParam      = 0;
  
  if (col == 0)
  {
    lvi.iItem    = 0x7FFF;
    lvi.iSubItem = 0;
    return ListView_InsertItem(*m_listctrl, &lvi);
  }
  else
  {
    lvi.iItem    = ListView_GetItemCount(*m_listctrl)-1;
    lvi.iSubItem = col;
    return ListView_SetItem(*m_listctrl, &lvi);
  }
  va_end(vl);
}


void CList::ResetListViewRefresh( )
{
  if (m_listctrl)
    ListView_DeleteAllItems(*m_listctrl);
}


void CList::set_tree_char_width(int w)
{
  m_tmAveCharWidth = w;
}


void CList::LVAddColumn(int i, char *name, int width)
{           
  if (m_listctrl == 0)
    return;
  LV_COLUMN col;
  
  if (i == 0)
  {
    while(ListView_DeleteColumn(*m_listctrl, 0))
      ;
  }
  
  col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  col.fmt  = LVCFMT_LEFT;
  col.pszText = name;
  col.cchTextMax = 0;
  col.cx = width * m_tmAveCharWidth;
  col.iSubItem = 0;
  ListView_InsertColumn(*m_listctrl, i, &col);
}
