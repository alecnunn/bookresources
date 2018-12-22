/*********************************************************************NVMH1****
File:
tree2.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef TREE2_H
#define TREE2_H




class CLeaf: public ulName
{ 
public:
  int layer;
  int parent;
  int selected_icon_number;
  int icon_number;
  bool bHasCheckBox;

  HTREEITEM id;
  int arg;

  void Init();

  int user_data_type;
  void * user_data;


};


class CIcon
{
public:
  int selected, unselected;
};




class CTree
{
public:
    
    CLeaf * m_leaf;  // temp pointer
    
    int top;
    
    CLeaf * tree; 
    ~CTree();
    CTreeCtrl* m_treectrl;
    
    CImageList m_imageList;
    
    
    
    void Reset();
    void InitTree(CTreeCtrl* treectrl, CListCtrl	*listctrl, int max_nodes);
    
    void create_icons(CIcon * icons, int n);
    
    void BuildTree();
    
    char * YesNo(int b);
    
    
    int Insert(int layer, int parent, int selected_icon_number, int icon_number,
        int user_data_type, void * user_data, bool bHasCheckBox, const char * __format, ...);
    
    BOOL SetCheckState(HTREEITEM hItem, BOOL fCheck);
    BOOL GetCheckState(HTREEITEM hItem);

    
};





class CList
{
public:
  CListCtrl	*m_listctrl;

  void InitList(CListCtrl * listctrl);

  DWORD   m_tmAveCharWidth;
  void LVAddColumn(int i, char *name, int width);
  int LVAddText(int col, const char *sz, ...);
  void ResetListViewRefresh();
  void set_tree_char_width(int w);
};

#endif