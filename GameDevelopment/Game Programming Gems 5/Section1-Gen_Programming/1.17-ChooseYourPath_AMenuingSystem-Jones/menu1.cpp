#include "StdAfx.h"
#include ".\menu1.h"
#include ".\menuSlider.h"

// name of the background for this menu
static char bkgrdName[] = "./images/menu1bkgrd.bmp";

menu1::menu1(void)
{
	slider = NULL;
}

menu1::~menu1(void)
{
	// delete the slider control
	if (slider)
		delete slider;
}

bool menu1::init(void)
{
	bool rc = true;

	// make sure the background loaded
	if (!(rc = loadBackground(bkgrdName)))
	{
		return rc;
	}

	// if the background load was successfull
	// create the slider control
	if (rc)
	{
		slider = new menuSlider();
		if (slider)
		{
			// init the slider
			rc = slider->initSlider("./images/slider.bmp", "./images/box.bmp");
			// set the position of the slider
			slider->setControlXY(14,250);			
			// the length of the line and the number of tics
			slider->setSliderLine(300, 10);
			// set the control extents (minValue, maxValue, startValue)
			slider->setSliderExtents(0, 10, 0);
		}
	}

	return rc;
}

// called each frame to update the menu
int menu1::update()
{
	// has the back button been hit?
	if (KEYDOWN(VK_BACK))
	{
		nextMenu = "";
		return PREV;
	}

	if (KEYDOWN(VK_RIGHT))
	{
		int value = slider->getSliderValue();

		slider->setSliderValue(value + 1);
	}
	if (KEYDOWN(VK_LEFT))
	{
		int value = slider->getSliderValue();

		slider->setSliderValue(value - 1);
	}

	return NONE;
}

void menu1::render(void)
{
	// draw the background image
	bkgrdImg->drawImage(0,0,0,0,640,480);

	slider->render();
}