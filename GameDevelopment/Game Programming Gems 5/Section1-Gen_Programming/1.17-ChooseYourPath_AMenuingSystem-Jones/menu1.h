#pragma once
#include "menuscreen.h"

#include "menuSlider.h"

class menu1 : public menuScreen
{
public:
	menu1(void);
	~menu1(void);

	bool init(void);

	// called each frame to update the menu
	int update();

	void render(void);

private:
	menuSlider *slider;
};
