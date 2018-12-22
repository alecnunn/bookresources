#pragma once
#include "menuscreen.h"

#include <vector>

#include "menuButton.h"

class mainMenu : public menuScreen
{
public:
	mainMenu(void);
	~mainMenu(void);

	bool init(void);
	int  update();
	void render(void);

private:
	static enum menuButtons { PURPLE_BUTTON=0, GREEN_BUTTON, MAX_BUTTONS };
	int curButton;
	
	menuButton *Buttons[4];

	// popup menu
	menuScreen *popupMenu;
};
