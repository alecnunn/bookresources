#pragma once
#include "menuscreen.h"

class menu2 : public menuScreen
{
public:
	menu2(void);
	~menu2(void);

	bool init(void);

	// called each frame to update the menu
	int update();

	void render(void);
};
