
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

#include "GraphObj.h"
#include "SDL.h"
#include "application.h"

int MeshulaApp::PollEvents(VMain& handler)
{
	int done = 0;
	int key;

	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		switch (event.type) {
			case SDL_QUIT:			done = true; break;

			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if (!handler.HandleKey(key)) {
					if (key == SDLK_ESCAPE) {
						done = 1;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button) {
					case SDL_BUTTON_WHEELUP:	handler.MouseWheelRollUp();					break;
					case SDL_BUTTON_WHEELDOWN:	handler.MouseWheelRollDown();				break;
					case SDL_BUTTON_LEFT:		handler.MouseClick(VEventHandler::eLeft);	break;
					case SDL_BUTTON_MIDDLE:		handler.MouseClick(VEventHandler::eMiddle);	break;
					case SDL_BUTTON_RIGHT:		handler.MouseClick(VEventHandler::eRight);	break;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				switch (event.button.button) {
					//case SDL_BUTTON_WHEELUP:	handler.MouseWheelRollUp();						break;
					//case SDL_BUTTON_WHEELDOWN:	handler.MouseWheelRollDown();				break;
					case SDL_BUTTON_LEFT:		handler.MouseUnclick(VEventHandler::eLeft);		break;
					case SDL_BUTTON_MIDDLE:		handler.MouseUnclick(VEventHandler::eMiddle);	break;
					case SDL_BUTTON_RIGHT:		handler.MouseUnclick(VEventHandler::eRight);	break;
				}
				break;

			case SDL_VIDEORESIZE:
				handler.SetWindowSize(event.resize.w, event.resize.h, false);
				break;
		}
	}

	int x, y;
	int buttons = SDL_GetMouseState(&x, &y);
	handler.MouseMotion(x, y);
	return done;
}

void VEventHandler::SetWindowSize(int width, int height, bool fullScreen) {
	mWindowWidth = width;
	mWindowHeight = height;
	mbFullScreen = fullScreen;
}

void VOpenGLMain::ClearWindow() {
	glDrawBuffer(GL_BACK);
	glViewport ( 0, 0, mWindowWidth, mWindowHeight);
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();

	float wWidth = (float) mWindowWidth;
	float wHeight = (float) mWindowHeight;
	float right, top;

	if (mWindowWidth > mWindowHeight) {
		right = wWidth / wHeight;
		top = 1.0f;
	}
	else {
		right = 1.0f;
		top = wHeight / wWidth;
	}

	mRight = right;
	mTop = top;

	gluOrtho2D (0.0f, right, 0.0f, top);			// left right, bottom top
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
}

bool InitGL(int Width, int Height) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	
	glClearDepth(1.0);					
	glDepthFunc(GL_LESS);					
	glEnable(GL_DEPTH_TEST);				
	glShadeModel(GL_SMOOTH);	

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	return true;
}

void VOpenGLMain::ConvertWindowCoordsToOrthoGL(float wX, float wY, float& x, float& y) {
	x = (mRight * (float) wX) / (float) mWindowWidth;
	y = (mTop * ((float) mWindowHeight - (float) wY)) / (float) mWindowHeight;
}

void VOpenGLMain::SetWindowSize(int width, int height, bool fullScreen) {
	VMain::SetWindowSize(width, height, fullScreen);

	uint32 flags = 0;
	if (fullScreen) {
		flags |= SDL_FULLSCREEN;
	}

	flags |= /*SDL_DOUBLEBUF | */SDL_RESIZABLE;

	int bits = 0;	// natural bit depth
	if ( SDL_SetVideoMode(width, height, bits, SDL_OPENGL | flags) == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// Set the title bar in environments that support it
	SDL_WM_SetCaption(mName.c_str(), NULL);

	if (!InitGL(width, height))						// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
	}
}
