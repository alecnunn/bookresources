/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      DynMenu.h
// Project:       EthoVision
// Module:        DynamicMenu
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 13-03-2000
//----------------------------------------------------------------------------
// Description:   dynamic submenus
//----------------------------------------------------------------------------
// Revision history:
// 13-03-2000 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------

struct MenuItemData {
	UINT cmdID;
	const char *string;
};

// appends item to menu
void AddSubMenuItem(CMenu *pMenu, UINT cmdID, const char *string);

// inserts item in menu
void InsertSubMenuItem(CMenu *pMenu, UINT itemIndex, UINT cmdID, const char *string);

// removes item from menu
void RemoveSubMenuItem(CMenu *pMenu, UINT cmdID);

CMenu *AddSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex, 
                  const char *string, MenuItemData *data = NULL);

// removes item from menu
void RemoveSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex);

// emty submenu of all items
void ClearSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex);
