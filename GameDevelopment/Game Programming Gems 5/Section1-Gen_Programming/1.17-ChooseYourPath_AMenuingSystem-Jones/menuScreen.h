#pragma once

class menuScreen
{
public:
	// menuReturn code
	// The update method will return one of these codes to inform
	// the menuManager of its status.
	// NONE - no action, continue showing current menu
	// NEXT - current menu should end and go to the next menu
	// PREV - current menu ends, display the previous menu
	// POPUP - the menu is requesting a popup menu be displayed
	// END  - This is the last menu and the menus are done
	static enum menuReturn { NONE=0, NEXT, PREV, POPUP, END };

	menuScreen(void);
	virtual ~menuScreen(void);

	// load all the resources needed for this menu
	virtual bool init(void)   = 0;

	// called each frame to update the menu
	virtual int update() = 0;

	// called each frame to draw the current menu
	virtual void render(void) = 0;

	// returns a string that represents the name
	// of the next menu
	std::string& getNextMenu(void);

protected:
	// loads the background image
	bool loadBackground(std::string imageName);

	// all menus have a background image associated with them
	resourceImage *bkgrdImg;

	// the name of the next menu
	std::string nextMenu;
};
