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

// main.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include "SDL.h"

#include "PhysicsEngine.h"
#include "PhysicsDemo.h"

using namespace PMath;


int xmain(int argc, char* argv[])
//int _tmain(int argc, _TCHAR* argv[])
{


	for (int i = 0; i < 50; ++i) {
	}


	return 0;
}

/**	Application main. SDL.h uses a macro to redefine main as SDL_main,
 *	which is invoked from the SDL distribution's SDL_main.c on start up.
 *
 * \param argc 
 * \param argv[] 
 * \return 
 */

int main(int argc, char* argv[])
{
	int done = 0;
	uint32 prevTime;
	uint32 newTime;

	fprintf(stderr, "Starting PhysicsEngine demo\n");

	// Initialize SDL for video output
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 ) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	fprintf(stderr, "SDL initialized\n");

	prevTime = SDL_GetTicks();

	int width = 800;
	int height = 600;

	PhysicsDemo* pDemo = new PhysicsDemo();
	pDemo->SetWindowName("Simple PhysicsEngine - by Nick Porcino");
	pDemo->SetWindowSize(width, height, false);
	pDemo->Reset();

	//glfInit();
	//font = glfLoadFont( "courier1.glf");
	//fprintf(stderr, "glf initialized\n");

	while ( !done ) {
		newTime = SDL_GetTicks();
		Real dt = ((Real)(newTime - prevTime)) * k1 / k1000;	// convert elapsed ms to elapsed seconds
		prevTime = newTime;

		// clamp at 20Hz simulation rate
		if (dt > k1 / Real(20.f)) {
			dt = k1 / Real(20.f);
		}

		pDemo->Update(dt);

		done = MeshulaApp::PollEvents(*pDemo);
		SDL_GL_SwapBuffers();
	}

	SDL_Quit();
	return 1;
}
