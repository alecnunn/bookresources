#ifndef __APPLICATION_H_
#define __APPLICATION_H_

/** @file application.h
 *	Meshula framework application base classes
 */

/*
---------------------------------------------------------------------------------------------------
Meshula Physics Demo
Created for Games Programming Gems IV
Copyright (c) 2003 Nick Porcino, http://meshula.net

The MIT License: http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------------------
*/

#include <string>
#include "SDL.h"

typedef unsigned int uint32;

class VEventHandler {
public:
	enum eMouseButton { eLeft, eMiddle, eRight, eWheel };

	VEventHandler() : mbFullScreen(false) { }
	~VEventHandler() { }

	// input handlers
	virtual bool HandleKey(int key) { return false; }	//!< Keys are as defined in SDL_keysym.h
	virtual void MouseMotion(int x, int y) { }
	virtual void MouseWheelRollUp() { }
	virtual void MouseWheelRollDown() { }
	virtual void MouseClick(eMouseButton) { }
	virtual void MouseUnclick(eMouseButton) { }

	// event handlers
	virtual void SetWindowSize(int width, int height, bool fullScreen);

	int			mWindowWidth;
	int			mWindowHeight;
	bool		mbFullScreen;
};

class VMain : public VEventHandler {
public:
	VMain() { }
	virtual ~VMain() { }

	// run the main application
	virtual void Update(float dt) = 0;

	// window utility functions
	virtual void ClearWindow() = 0;
	virtual void SetWindowName(char* pName) { mName.assign(pName); }

protected:
	std::string	mName;
};

class VOpenGLMain : public VMain {
public:
	VOpenGLMain() : VMain() { }
	virtual ~VOpenGLMain() { }

	// window utility functions
	virtual	void	ClearWindow();

	// event handlers
	virtual void	SetWindowSize(int width, int height, bool fullScreen);

	virtual	void	GetWindowSize(int& width, int& height) { width = mWindowWidth; height = mWindowHeight; }

	// GL utility functions
			void	ConvertWindowCoordsToOrthoGL(float wX, float wY, float& x, float& y);

	float	mRight, mTop;		// parameters to gluOrtho
};

namespace MeshulaApp {
	int PollEvents(VMain&);
}


#endif
