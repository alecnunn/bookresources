//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Gamer.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "gamer.h"

//----------------------------------------------------------------------------------------------
// HandleUserInput(): forwards commands from the keyboard to the paddle
//----------------------------------------------------------------------------------------------

void CGamer::HandleUserInput() {

	if KEY_DOWN(VK_LEFT) {
		Left();
	}
	if KEY_DOWN(VK_RIGHT) {
		Right();
	}
	if KEY_DOWN(VK_UP) {
		if (direction==-1) Up(); else Down();
	}
	if KEY_DOWN(VK_DOWN) {
		if (direction==-1) Down(); else Up();
	}
	if KEY_DOWN(VK_Z) {
		Rotate(-(int)NormSpeed(ROTATE_SPEED));
	}
	if KEY_DOWN(VK_X) {
		Rotate((int)NormSpeed(ROTATE_SPEED));
	}
}

//----------------------------------------------------------------------------------------------
// Update(): override of CPaddle::Update() to handle user input
//----------------------------------------------------------------------------------------------

void CGamer::Update() {
	HandleUserInput();
	CPaddle::Update();
}
