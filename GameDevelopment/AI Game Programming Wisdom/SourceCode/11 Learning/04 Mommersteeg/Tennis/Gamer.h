//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Gamer.h
//----------------------------------------------------------------------------------------------

#ifndef __GAMER_H
#define __GAMER_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "paddle.h"

//----------------------------------------------------------------------------------------------
// Macro definitions
//----------------------------------------------------------------------------------------------

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define VK_Z			0x5A
#define VK_X			0x58

//----------------------------------------------------------------------------------------------
// CGamer: A paddle controlled by human player input (keyboard)
//----------------------------------------------------------------------------------------------

class CGamer : public CPaddle {
public:

	virtual void		Update();

protected:

	void				HandleUserInput();

};

#endif // __GAMER_H

