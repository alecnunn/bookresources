#pragma once
#include "menucontrol.h"

#include <string>

class menuSlider : public menuControl
{
public:
	menuSlider(void);
	~menuSlider(void);

	bool initSlider(const std::string sliderName, const std::string boxName);

	void setSliderExtents(int minValue, int maxValue, int startValue);
	void setSliderLine(int lineLength, int numTics);
	
	void setSliderValue(int newValue);	
	int getSliderValue(void) { return curValue; }

	void render(void);
private:
	resourceImage *BoxImage;
	resourceImage *SliderImage;

	int maxValue;
	int minValue;
	int curValue;
	int lineLength;
	int numTics;

	int boxLocX;
	int boxLocY;
};
