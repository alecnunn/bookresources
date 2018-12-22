/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// Downloaded from http://www.codeproject.com/menu/dynmenu.asp

//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      DynMenu.cpp
// Project:       EthoVision
// Module:        DynamicMenu
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 13-03-2000
//----------------------------------------------------------------------------
// Description:   dynamic submenus
//                See DynMenu.h
//----------------------------------------------------------------------------
// Revision history:
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "DynMenu.h"

//----------------------------------------------------------------------------
// Function AddSubMenuItem
// @func    appends item to menu
// @rdesc   nothing
// @parm    CMenu*      | pMenu  | menu to add item to
// @parm    UINT        | cmdID  | command ID of item
// @parm    const char* | string | item caption
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void AddSubMenuItem(CMenu *pMenu, UINT cmdID, const char *string)
{
	ASSERT_VALID(pMenu);
	ASSERT(string);
	VERIFY(pMenu->AppendMenu(MF_STRING | MF_ENABLED, cmdID, string));
}

//----------------------------------------------------------------------------
// Function InsertSubMenuItem
// @func    inserts item in menu
// @rdesc   nothing
// @parm    CMenu*      | pMenu     | menu to add item to
// @parm    UINT        | itemIndex | index at which to insert (-1 is at end)
// @parm    UINT        | cmdID     | command ID of item
// @parm    const char* | string    | item caption
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void InsertSubMenuItem(CMenu *pMenu, UINT itemIndex, UINT cmdID, const char *string)
{
	ASSERT_VALID(pMenu);
	ASSERT(string);
	VERIFY(pMenu->InsertMenu(itemIndex, MF_STRING | MF_ENABLED | MF_BYPOSITION, cmdID, string));
}

//----------------------------------------------------------------------------
// Function RemoveSubMenuItem
// @func    removes item from menu
// @rdesc   nothing
// @parm    CMenu | *pMenu | menu to remove item from
// @parm    UINT  | cmdID  | command ID of item
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void RemoveSubMenuItem(CMenu *pMenu, UINT cmdID)
{
	ASSERT_VALID(pMenu);
	VERIFY(pMenu->RemoveMenu(cmdID, MF_BYCOMMAND));
}

//----------------------------------------------------------------------------
// Function CreateSubMenu
// @func    adds submenu to menu
// @rdesc   pointer to created menu (CMenu*)
// @parm    CFrameWnd    | *pWnd        | frame window to use
// @parm    UINT         | mainMenuItem | main menu item in which's menu submenu is to be added
// @parm    UINT         | itemIndex    | index of submenu in menu
// @parm    const char   | *string      | item caption (next to > sign)
// @parm    MenuItemData | *data        | pointer to null terminated list of menu items (default NULL)
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
CMenu *AddSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex, 
			const char *string, MenuItemData *data /* = NULL */)
{
	ASSERT_VALID(pWnd);
	ASSERT(string);

	CMenu *pMenuBar = pWnd->GetMenu();
	ASSERT_VALID(pMenuBar);

	CMenu *pSubMenu = pMenuBar;
	if (mainMenuItem != -1)
	{
		pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
		assert(pSubMenu);
	}
/*	// If the menu is empty we must create it.
	if (!pSubMenu)
	{
		pSubMenu = new CMenu;
		VERIFY(pSubMenu->CreatePopupMenu());
	}*/

	CMenu* pMenu = pSubMenu;
	if (itemIndex != -1)
	{
		pMenu = new CMenu;
		VERIFY(pMenu->CreatePopupMenu());
		VERIFY(pSubMenu->InsertMenu(itemIndex, MF_BYPOSITION | MF_POPUP, 
									(UINT)pMenu->m_hMenu, string));
	}

	if (data) 
		for (int i = 0; data[i].string; i++)
		  AddSubMenuItem(pMenu, data[i].cmdID, data[i].string);

	return pMenu;
}

//----------------------------------------------------------------------------
// Function RemoveSubMenu
// @func    removes submenu from menu
// @rdesc   nothing
// @parm    CFrameWnd    | *pWnd        | frame window to use
// @parm    UINT         | mainMenuItem | main menu item in which's menu submenu is to be added
// @parm    UINT         | itemIndex    | index of submenu in menu
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void RemoveSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex)
{
	assert(!"Needs updating to reflect changes made to AddSubMenu");
	ASSERT_VALID(pWnd);

	CMenu *pMenuBar = pWnd->GetMenu();
	ASSERT_VALID(pMenuBar);

	CMenu *pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
	ASSERT_VALID(pSubMenu);

	CMenu *pMenu = pSubMenu->GetSubMenu(itemIndex);
	ASSERT_VALID(pMenu);

	VERIFY(pSubMenu->RemoveMenu(itemIndex, MF_BYPOSITION));
	delete pMenu;
}

//----------------------------------------------------------------------------
// Function ClearSubMenu
// @func    emty submenu of all items
// @rdesc   nothing
// @parm    CFrameWnd    | *pWnd        | frame window to use
// @parm    UINT         | mainMenuItem | main menu item in which's menu submenu is to be added
// @parm    UINT         | itemIndex    | index of submenu in menu
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void ClearSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex)
{
	assert(!"Needs updating to reflect changes made to AddSubMenu");
	ASSERT_VALID(pWnd);

	CMenu *pMenuBar = pWnd->GetMenu();
	ASSERT_VALID(pMenuBar);

	CMenu *pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
	ASSERT_VALID(pSubMenu);

	CMenu *pMenu = pSubMenu->GetSubMenu(itemIndex);
	ASSERT_VALID(pMenu);

	while (pSubMenu->RemoveMenu(0, MF_BYPOSITION))
		;
}

//----------------------------------------------------------------------------
