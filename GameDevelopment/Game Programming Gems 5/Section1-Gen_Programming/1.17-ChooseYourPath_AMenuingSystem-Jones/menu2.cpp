#include "StdAfx.h"
#include ".\menu2.h"

// name of the background for this menu
static char bkgrdName[] = "./images/menu2bkgrd.bmp";

menu2::menu2(void)
{
}

menu2::~menu2(void)
{
}

bool menu2::init(void)
{
	bool rc = true;

	// make sure the background loaded
	if (!(rc = loadBackground(bkgrdName)))
	{
		return rc;
	}

	return rc;
}

// called each frame to update the menu
int menu2::update()
{
	// has the back button been hit?
	if (KEYDOWN(VK_BACK))
	{
		nextMenu = "";
		return PREV;
	}	

	return NONE;
}

void menu2::render(void)
{
	// draw the background image
	bkgrdImg->drawImage(75,75,0,0,452,264);
}