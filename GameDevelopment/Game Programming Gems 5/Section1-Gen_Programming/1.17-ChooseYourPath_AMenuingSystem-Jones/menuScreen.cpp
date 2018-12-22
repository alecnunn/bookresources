#include "StdAfx.h"
#include ".\menuscreen.h"

menuScreen::menuScreen(void)
{
	bkgrdImg = NULL;
	nextMenu = "";
}

menuScreen::~menuScreen(void)
{
	// release the background image
	resourceHandler::releaseImage(bkgrdImg);
}

// returns the name of the next menu in 
// the sequence
std::string& menuScreen::getNextMenu(void)
{
	return nextMenu;
}

// loads a background image into bkgrdImg
// since most menus will need a background image, this
// method resides in the parent class
bool menuScreen::loadBackground(std::string imageName)
{
	bool rc = true;

	// load in the background bitmap
	bkgrdImg = new resourceImage();
	if (bkgrdImg)
	{
		rc = resourceHandler::loadImageFromDisk(imageName, bkgrdImg);
	}
	// make sure the background loaded
	if (!rc || (bkgrdImg == NULL))
	{
		rc = false;
	}

	return rc;
}