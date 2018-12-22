#include "StdAfx.h"
#include ".\menuslider.h"

static const int boxOffsetX = 10;
static const int boxOffsetY = 5;

menuSlider::menuSlider(void)
{
	this->minValue = 0;
	this->maxValue = 0;
	this->curValue = 0;

	BoxImage = NULL;
	SliderImage = NULL;
}

menuSlider::~menuSlider(void)
{
	resourceHandler::releaseImage(BoxImage);
	resourceHandler::releaseImage(SliderImage);
}

bool menuSlider::initSlider(const std::string sliderName, const std::string boxName)
{
	bool rc = true;

	SliderImage = new resourceImage();
	if (SliderImage)
	{
		rc = resourceHandler::loadImageFromDisk(sliderName, SliderImage);
	}
	if (rc)
	{
		BoxImage = new resourceImage();
		if (BoxImage)
		{
			rc = resourceHandler::loadImageFromDisk(boxName, BoxImage);
		}
	}
	
	return rc;
}

void menuSlider::setSliderExtents(int minValue, int maxValue, int startValue)
{
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->curValue = startValue;
}

void menuSlider::setSliderLine(int lineLength, int numTics)
{
	this->lineLength = lineLength;
	this->numTics = numTics;
}

void menuSlider::setSliderValue(int newValue)
{
	if (newValue <= minValue)
		curValue = minValue;

	else if (newValue >= (maxValue-1))
		curValue = (maxValue-1);
	else
		curValue = newValue;
}

void menuSlider::render(void)
{
	SliderImage->drawImage(locX,locY, 0, 0, 300, 50);

	// handles drawing the slider box in the correct spot on the line
	BoxImage->drawImage((locX + boxOffsetX + (curValue * (lineLength/numTics))),(locY+boxOffsetY), 0, 0, 20, 40);
}