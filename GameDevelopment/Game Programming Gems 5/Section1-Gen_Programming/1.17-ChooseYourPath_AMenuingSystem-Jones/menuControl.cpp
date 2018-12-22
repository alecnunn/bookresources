#include "StdAfx.h"
#include ".\menucontrol.h"

menuControl::menuControl(void)
{
	locX = 0;
	locY = 0;
	type = NONE;
	
	state = controlState::ACTIVE;
	view  = controlView::VISIBLE;
}

menuControl::~menuControl(void)
{
}
