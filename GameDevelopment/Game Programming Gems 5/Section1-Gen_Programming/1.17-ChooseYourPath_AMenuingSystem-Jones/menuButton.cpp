#include "StdAfx.h"
#include "resourceHandler.h"
#include ".\menubutton.h"

menuButton::menuButton(void) : menuControl()
{
	curButtonState = menuButton::buttonStates::DEFAULT;
	image = NULL;
}

menuButton::~menuButton(void)
{
	resourceHandler::releaseImage(image);
}

void menuButton::render(void)
{
	image->drawImage(locX,locY, bMetrics[curButtonState].XOffset, bMetrics[curButtonState].YOffset,
		bMetrics[curButtonState].width, bMetrics[curButtonState].height);
}

bool menuButton::initButton(const std::string imageName, int XOffset1, int YOffset1, int width1, int height1,
												   int XOffset2, int YOffset2, int width2, int height2)
{
	bool rc = true;

	// metrics for the default button state
	bMetrics[0].XOffset = XOffset1;
	bMetrics[0].YOffset = YOffset1;
	bMetrics[0].width = width1;
	bMetrics[0].height = height1;

	// metrics for the clicked button state
	bMetrics[1].XOffset = XOffset2;
	bMetrics[1].YOffset = YOffset2;
	bMetrics[1].width = width2;
	bMetrics[1].height = height2;
	
	type = menuControl::controlType::BUTTON;

	image = new resourceImage();
	if (image)
	{
		rc = resourceHandler::loadImageFromDisk(imageName, image);
	}

	// confirm that the images are loading properly
	if (!rc || (image == NULL))
	{
		rc = false;
	}

	return rc;
}

// handle button events
void menuButton::clickButton(void)
{
	curButtonState = menuButton::buttonStates::CLICKED;
}

void menuButton::unClickButton(void)
{
	curButtonState = menuButton::buttonStates::DEFAULT;
}
