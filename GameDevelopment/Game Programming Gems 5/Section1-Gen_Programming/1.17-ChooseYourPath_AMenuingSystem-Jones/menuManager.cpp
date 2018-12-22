#include "StdAfx.h"
#include ".\menuManager.h"

#include ".\menus.h"

menuManager *menuManager::pInstance = NULL;

menuManager::menuManager(void)
{
	popupMenu = NULL;
	popupActive = false;
}

menuManager::~menuManager(void)
{
}

bool menuManager::init(void)
{	
	menuScreen *menu = NULL;

	// register all the menus, each new menu
	// must call register before it can be used
	REGISTERMENU(mainMenu);
	REGISTERMENU(menu1);
	REGISTERMENU(menu2);
	
	// set the first menu to the mainMenu
	menu = createMenu("mainMenu");	
	if (menu == NULL)
		return false;

	// initialize the menu
	if (!menu->init())
	{
		// shutdown the menu system
		shutdown();

		return false;
	}

	// add the menu to the menu trail
	menuTrail.push_back(menu);

	return true;
}

// close and release the menu system
void menuManager::shutdown(void)
{
	// remove all the dangling menus from the trail
	for (unsigned int i=0; i<menuTrail.size(); i++)
	{
		destroyMenu(menuTrail[i]);
	}
	// clear the menu trail
	menuTrail.clear();

	// clear the menuList
	menuList.clear();

	// kill off the menuManager
	delete pInstance;
	pInstance = NULL;
}

int menuManager::update()
{
	// menu update return code
	int rc = 0;

	// update the current menu
	
	// if there is an active popup
	// send the popup the update
	if (popupActive)
	{
		rc = popupMenu->update();
	}
	// otherwise, send the update
	// to the base menu
	else
	{
		rc = menuTrail.back()->update();
	}

	// should the menuManager end?
	if (rc == menuScreen::menuReturn::END)
		return 0;

	// if the next button was hit
	else if (rc == menuScreen::menuReturn::NEXT)
	{
		menuScreen *newMenu = NULL;

		// create the next menu
		newMenu = createMenu(menuTrail.back()->getNextMenu());
		if (newMenu == NULL)
		{
			return 0;
		}
		// init the new current menu
		if (!newMenu->init())
			return 0;

		// add the menu to the menu trail
		menuTrail.push_back(newMenu);
	}

	// if the back button is hit
	else if (rc == menuScreen::menuReturn::PREV)
	{
		// if a popup is being displayed
		if (popupActive)
		{
			// kill off the popup
			destroyMenu(popupMenu);
			popupMenu = NULL;
			popupActive = false;
		}
		// otherwise, the current base menu is 
		// going away
		else
		{
			// kill off the current menu
			destroyMenu((menuScreen*)menuTrail.back());

			// remove the now dead menu from the list
			menuTrail.pop_back();
		}
	}

	// if the menu is requesting a popup
	else if (rc == menuScreen::menuReturn::POPUP)
	{		
		// create the popup menu
		popupMenu = createMenu(menuTrail.back()->getNextMenu());
		if (popupMenu == NULL)
		{
			return 0;
		}
		// init the popup
		if (!popupMenu->init())
			return 0;

		// the menu system keeps track that a popup
		// is active
		popupActive = true;
	}

	return 1;
}

void menuManager::render(void)
{
	// draw the current base menu
	menuTrail.back()->render();

	// if a popup menu is active
	// draw that one too
	if (popupActive)
	{
		popupMenu->render();
	}
}

///////////////////////////////////////////////////////////////////////////
// menu factory implementation
void menuManager::registerMenu(std::string menuName, menuCreateFunc menuFunc)
{
	menuList[menuName] = menuFunc;
}

menuScreen* menuManager::createMenu(std::string menuName) 
{
	// find the menu being requested in the list
	// of registered menus
	Iterator iter = menuList.find(menuName);

	// if the menu is not in the list, return null
	if (iter == menuList.end())
		return NULL;

	// second is the value, first would be the key
	// this should generate the constructor
	return ((*iter).second)();
}

void menuManager::destroyMenu(menuScreen* menu)
{
	if (menu)
	{
		delete menu;
		menu = NULL;
	}
}
