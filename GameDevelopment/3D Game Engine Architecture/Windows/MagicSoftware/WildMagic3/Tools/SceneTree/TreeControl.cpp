// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2003.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "resource.h"
#include "TreeControl.h"

static int CLOSED_B, OPEN_B, CLOSED_Y, OPEN_Y, CLOSED_P, OPEN_P, ATTRIBUTE;

//----------------------------------------------------------------------------
TreeControl::TreeControl (HINSTANCE hInstance, HWND hParentWnd,
    Spatial* pkObject, const char* acName, int iX, int iY, int iW, int iH)
{
    m_hInstance = hInstance;
    m_hParentWnd = hParentWnd;

    m_acName = new char[strlen(acName)+1];
    strcpy(m_acName,acName);

    CreateWindows(iX,iY,iW,iH);
    CreateImageList();
    CreateTree(pkObject);

    ShowWindow(m_hTreeWnd,SW_SHOWDEFAULT);
    UpdateWindow(m_hTreeWnd);
}
//----------------------------------------------------------------------------
TreeControl::~TreeControl ()
{
    delete[] m_acName;

    DestroyTree();
    DestroyImageList();
    DestroyWindows();
}
//----------------------------------------------------------------------------
void TreeControl::CreateWindows (int iX, int iY, int iW, int iH)
{
    static char s_acWindowClassName[] = "SceneTree";

    // initialize tree view classes
    INITCOMMONCONTROLSEX kInitCC;
    kInitCC.dwSize = sizeof(INITCOMMONCONTROLSEX);
    kInitCC.dwICC = ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&kInitCC);

    // register the window class
    WNDCLASS kWClass;
    kWClass.style = CS_HREDRAW | CS_VREDRAW;
    kWClass.lpfnWndProc = HandleMessage;
    kWClass.cbClsExtra = 0;
    kWClass.cbWndExtra = 0;
    kWClass.hInstance = m_hInstance;
    kWClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    kWClass.hCursor = LoadCursor(NULL,IDC_ARROW);
    kWClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    kWClass.lpszClassName = s_acWindowClassName;
    kWClass.lpszMenuName = NULL;
    RegisterClass(&kWClass);

    // create the tree view window
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
        | WS_SIZEBOX;
    m_hTreeWnd = CreateWindow(s_acWindowClassName,m_acName,dwStyle,iX,iY,iW,
        iH,0,0,m_hInstance,NULL);

    // create the tree view
    dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD |
        TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;

    RECT kRect;
    GetClientRect(m_hTreeWnd,&kRect);

    m_hTreeView = CreateWindow(WC_TREEVIEW,"",dwStyle,0,0,
        kRect.right-kRect.left+1,kRect.bottom-kRect.top+1,m_hTreeWnd,0,
        m_hInstance,NULL);
}
//----------------------------------------------------------------------------
void TreeControl::CreateImageList ()
{
    const int iW = 16, iH = 15;
    HBITMAP hBitmap;

    m_hImageList = ImageList_Create(iW,iH,ILC_COLOR24,2,1);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_CLOSED_B),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    CLOSED_B = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_OPEN_B),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    OPEN_B = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_CLOSED_Y),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    CLOSED_Y = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_OPEN_Y),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    OPEN_Y = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_CLOSED_P),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    CLOSED_P = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_OPEN_P),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    OPEN_P = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = (HBITMAP) LoadImage(m_hInstance,
        MAKEINTRESOURCE(IDB_BMP_ATTRIBUTE),IMAGE_BITMAP,iW,iH,
        LR_DEFAULTCOLOR);
    ATTRIBUTE = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);
}
//----------------------------------------------------------------------------
void TreeControl::CreateTree (Spatial* pkObject)
{
    TreeView_SetImageList(m_hTreeView,m_hImageList,TVSIL_NORMAL);

    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // add root item
    tvs.hParent = TVI_ROOT;
    tvs.item.pszText = m_acName;
    tvs.item.iImage = CLOSED_B;
    tvs.item.iSelectedImage = OPEN_B;
    m_hTreeRoot = TreeView_InsertItem(m_hTreeView,&tvs);

    m_pkRoot = pkObject->SaveStrings();
    CreateTreeRecursive(m_hTreeRoot,m_pkRoot,NT_CLASS);
}
//----------------------------------------------------------------------------
void TreeControl::CreateTreeRecursive (HTREEITEM hParent,
    StringTree* pkTree, NodeType eType)
{
    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    assert( pkTree->GetStringQuantity() > 0 );
    if ( strncmp(pkTree->GetString(0),"Wm3",3) == 0 )
    {
        // string tree rooted at an Object-derived object

        // Add object.  Strip off the "Wm3." prefix (the "+ 4" term).
        tvs.hParent = hParent;
        tvs.item.pszText = pkTree->GetString(0) + 4;
        if ( eType == NT_CLASS )
        {
            tvs.item.iImage = CLOSED_B;
            tvs.item.iSelectedImage = OPEN_B;
        }
        else if ( eType == NT_SUBCLASS )
        {
            tvs.item.iImage = CLOSED_Y;
            tvs.item.iSelectedImage = OPEN_Y;
        }
        else
        {
            assert( false );
        }

        tvs.hParent = TreeView_InsertItem(m_hTreeView,&tvs);

        // add subclass of object
        int iStart;
        if ( strncmp(pkTree->GetString(0),"Wm3.Object",10) != 0 )
        {
            CreateTreeRecursive(tvs.hParent,pkTree->GetChild(0),NT_SUBCLASS);
            iStart = 1;
        }
        else
        {
            iStart = 0;
        }

        // add strings
        int i;
        for (i = 1; i < pkTree->GetStringQuantity(); i++)
        {
            tvs.item.pszText = pkTree->GetString(i);
            tvs.item.iImage = ATTRIBUTE;
            tvs.item.iSelectedImage = ATTRIBUTE;
            TreeView_InsertItem(m_hTreeView,&tvs);
        }

        // add children
        for (i = iStart; i < pkTree->GetChildQuantity(); i++)
        {
            StringTree* pkCTree = pkTree->GetChild(i);
            assert( pkCTree->GetStringQuantity() > 0 );

            if ( strncmp(pkCTree->GetString(0),"Wm3",3) == 0 )
                eType = NT_CLASS;
            else
                eType = NT_DATA;

            CreateTreeRecursive(tvs.hParent,pkCTree,eType);
        }
    }
    else
    {
        // string tree represents a native type (for example, an array)

        // add object
        tvs.hParent = hParent;
        tvs.item.pszText = pkTree->GetString(0);
        tvs.item.iImage = CLOSED_P;
        tvs.item.iSelectedImage = OPEN_P;
        tvs.hParent = TreeView_InsertItem(m_hTreeView,&tvs);

        // add strings
        int i;
        for (i = 1; i < pkTree->GetStringQuantity(); i++)
        {
            tvs.item.pszText = pkTree->GetString(i);
            tvs.item.iImage = ATTRIBUTE;
            tvs.item.iSelectedImage = ATTRIBUTE;
            TreeView_InsertItem(m_hTreeView,&tvs);
        }

        // add children
        for (i = 0; i < pkTree->GetChildQuantity(); i++)
        {
            StringTree* pkCTree = pkTree->GetChild(i);
            assert( pkCTree->GetStringQuantity() > 0 );

            if ( strncmp(pkCTree->GetString(0),"Wm3",3) == 0 )
                eType = NT_CLASS;
            else
                eType = NT_DATA;

            CreateTreeRecursive(tvs.hParent,pkCTree,eType);
        }
    }
}
//----------------------------------------------------------------------------
void TreeControl::DestroyWindows ()
{
    DestroyWindow(m_hTreeView);
    DestroyWindow(m_hTreeWnd);
}
//----------------------------------------------------------------------------
void TreeControl::DestroyImageList ()
{
    ImageList_Destroy(m_hImageList);
}
//----------------------------------------------------------------------------
void TreeControl::DestroyTree ()
{
    TreeView_DeleteAllItems(m_hTreeView);
}
//----------------------------------------------------------------------------
LRESULT CALLBACK TreeControl::HandleMessage (HWND hWnd, UINT uiMsg,
    WPARAM wParam, LPARAM lParam)
{
    if ( uiMsg == WM_DESTROY )
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
