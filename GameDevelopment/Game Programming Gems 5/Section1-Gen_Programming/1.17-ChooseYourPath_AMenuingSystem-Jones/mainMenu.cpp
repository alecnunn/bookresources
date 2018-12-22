#include "StdAfx.h"
#include "menuManager.h"
#include ".\mainmenu.h"
#include ".\menuButton.h"

// button action details
static buttonDetails bDetails[] = {
	{ "./images/purplebutton.bmp", "menu1" },
	{ "./images/greenbutton.bmp", "menu2" },
};

// name of the background for this menu
static char bkgrdName[] = "./images/mainbkgrd.bmp";


mainMenu::mainMenu(void) : menuScreen()
{
	// Set the current button
	curButton = PURPLE_BUTTON;

	for (int i=0; i < menuButtons::MAX_BUTTONS; i++)
		Buttons[i] = NULL;

	popupMenu = NULL;
}

mainMenu::~mainMenu(void)
{
	// allow the destructor to do clean up
	for (int i=0; i < menuButtons::MAX_BUTTONS; i++)
	{
		delete[] Buttons[i];
		Buttons[i] = NULL;
	}
}

bool mainMenu::init(void)
{
	bool rc = true;

	// make sure the background loaded
	if (!(rc = loadBackground(bkgrdName)))
	{
		return false;		
	}
	
	for (int i=0; i < menuButtons::MAX_BUTTONS; i++)
	{
		Buttons[i] = new menuButton();
		if (Buttons[i])
		{
			rc = Buttons[i]->initButton(bDetails[i].imagePath, 0, 0, 213, 33, 213, 0, 213, 33);

			// set the position of the button
			// the position is hardcoded here for simplicity
			Buttons[i]->setControlXY(350, 130 + (i*45));
		}
		
		// confirm that the images are loading properly
		if (!rc)
		{
			rc = false;
			break;
		}
	}
		
	return rc;
}

int mainMenu::update()
{
	if KEYDOWN(VK_DOWN)
	{
		curButton++;
	}
	else if KEYDOWN(VK_UP)
	{
		curButton--;
	}

	// allow the button selection to loop
	if (curButton > menuButtons::GREEN_BUTTON)
		curButton = menuButtons::PURPLE_BUTTON;	
	else if (curButton < menuButtons::PURPLE_BUTTON)
		curButton = menuButtons::GREEN_BUTTON;
	
	// check if a menu option has been selected
	if KEYDOWN(VK_RETURN)
	{
		// handle popup menu
		if (curButton == menuButtons::GREEN_BUTTON)
		{
			nextMenu = "menu2";
			return POPUP;
		}
		else
		{
			nextMenu = bDetails[curButton].destMenu;
			return NEXT;
		}
	}
	
	// handle hilighting the correct menu button
	for (int i=0; i<MAX_BUTTONS;i++)
		Buttons[i]->unClickButton();
	
	Buttons[curButton]->clickButton();

	return NONE;
}

void mainMenu::render(void)
{
	// draw the background image
	bkgrdImg->drawImage(0,0,0,0,640,480);

	// draw each of the buttons
	for (int i=0; i<MAX_BUTTONS; i++)
	{
		if (Buttons[i])
			Buttons[i]->render();
	}
}