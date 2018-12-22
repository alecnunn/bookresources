#pragma once

#include <string>
#include <vector>
#include <map>

class menuScreen;

////////////////////////////////////////////////////////////////
// menu factory
typedef menuScreen *(*menuCreateFunc)();
typedef std::map<std::string, menuCreateFunc>::iterator Iterator;

template<typename ClassType>
menuScreen *CreateObject()
{
   return new ClassType;
}

////////////////////////////////////////////////////////////////
// menu System
class menuManager
{
public:		
	// singleton to ensure only one instance of the 
	// menu system
	static menuManager& getInstance() 
	{ 
		if (pInstance == NULL) pInstance = new menuManager();
			return *pInstance;
	};

	// initialize the menu system
	bool init(void);

	// close and release the menu system
	void shutdown(void);

	// passes down update messages to the menus
	int update();

	// draws the menus
	void render(void);
		
private:
	static menuManager *pInstance;

	menuManager(void);
	~menuManager(void);
	
	menuScreen *popupMenu;

	// flag to keep track of whether a popup window
	// is active
	bool popupActive;

	// the menu trail keeps track of the menus that the
	// user has gone through. This gives the system the 
	// ability to track back the path the user took through
	// the system
	std::vector<menuScreen*> menuTrail;

	// menu factory implementation	
	#define REGISTERMENU(a) registerMenu(#a, &CreateObject<a>);
	std::map<std::string, menuCreateFunc> menuList;
	void registerMenu(std::string menuName, menuCreateFunc menuFunc);
	
	// creates a new menu
	menuScreen* createMenu(std::string menuName);

	// removes a menu from the system
	void destroyMenu(menuScreen* menu);
};
