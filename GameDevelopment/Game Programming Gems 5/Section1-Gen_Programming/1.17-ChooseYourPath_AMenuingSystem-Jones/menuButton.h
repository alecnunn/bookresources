#pragma once
#include "menucontrol.h"

#include <string>

class resourceImage;

typedef struct _buttonMetrics {
	int XOffset;
	int YOffset;
	int width;
	int height;
} buttonMetrics;

typedef struct _buttonDetails {
	std::string imagePath;
	std::string destMenu;
}buttonDetails;

class menuButton : public menuControl
{
public:
	// button states
	static enum buttonStates { DEFAULT=0, CLICKED };

	menuButton(void);
	~menuButton(void);

	// overridden from parent
	void render(void);

	bool initButton(const std::string imageName, int XOffset1, int YOffset1, int width1, int height1,
										   int XOffset2, int YOffset2, int width2, int height2);

	// button events
	void clickButton(void);
	void unClickButton(void);

private:
	resourceImage *image;
	
	buttonMetrics bMetrics[2];

	int curButtonState;
};
