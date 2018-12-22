// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2003.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

#ifndef TREECONTROL_H
#define TREECONTROL_H

#include <windows.h>
#include <commctrl.h>
#include "Wm3Spatial.h"
#include "Wm3StringTree.h"
using namespace Wm3;

class TreeControl
{
public:
    TreeControl (HINSTANCE hInstance, HWND hParentWnd, Spatial* pkObject,
        const char* acName, int iX, int iY, int iW, int iH);

    ~TreeControl ();

protected:
    void CreateWindows (int iX, int iY, int iW, int iH);
    void CreateImageList ();
    void CreateTree (Spatial* pkObject);

    enum NodeType
    {
        NT_CLASS,
        NT_SUBCLASS,
        NT_DATA
    };

    void CreateTreeRecursive (HTREEITEM hParent, StringTree* pkTree,
        NodeType eType);

    void DestroyWindows ();
    void DestroyImageList ();
    void DestroyTree ();

    static LRESULT CALLBACK HandleMessage (HWND hWnd, UINT uiMsg,
        WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hParentWnd;
    HWND m_hTreeWnd;
    HWND m_hTreeView;
    HTREEITEM m_hTreeRoot;
    HIMAGELIST m_hImageList;
    char* m_acName;
    StringTree* m_pkRoot;
};

#endif



