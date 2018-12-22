/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved

	ESC Quits
	"F" Shows the polygon scanline fills
	"L" Shows the bounding polygon lines
	"P" Shows the terrain planes
	"C" Toggles 3D Y clipping
	1-3 selects the plane to show info about
	
	The mouse moves the camera around with the button moving forward/backwards
*/
#include <stdlib.h>
#include <string.h>
#include "Sky.hpp"
#include "Waves.hpp"
#include "Desert.hpp"

// Defines the dimensions & bi depth of the screen
// The default setting gives ~60fps on a GeForce2 MX without VSync
#define WIDTH  800
#define HEIGHT 600
#define BPP    32
#define SDL_FLAGS	SDL_HWSURFACE | SDL_HWACCEL | SDL_OPENGL// | SDL_FULLSCREEN

// Common debug variables 
unsigned short  ShowID(0);
bool			ShowFill(true);
bool			ShowLines(true);
bool			ShowNormal(true);
bool			ShowPlanes(true);
bool			ClipPlanes(true);

// OpenGL virtual functions
PFNGLLOCKARRAYSEXTPROC		glLockArraysEXT		= NULL;
PFNGLUNLOCKARRAYSEXTPROC	glUnlockArraysEXT	= NULL;

#define RANDOM(MIN, MAX) ((rand() / 32768.0) * ((MAX) - (MIN)) + (MIN))


// Function to verify that the OpenGL implementation has the required extensions
bool HasExtension(char *Name)
{
	const char *Extensions	= (const char *)glGetString (GL_EXTENSIONS);
	unsigned short Length	= strlen(Name);

	do {
		unsigned short n = strcspn(Extensions, " ");

		if ((n == Length) && !memcmp(Name, Extensions, Length))
			return true;

		Extensions += n;
	} while (*Extensions++);

	return false;
}



// Initialises SDL & OpenGL
bool InitSDL(unsigned short Width, unsigned short Height, 
			 unsigned short Bpp, unsigned short Flags)
{
	static const Vector4D<float> BackColor(30/255.0f, 44/255.0f, 64/255.0f, 1.0f);
	SDL_Surface *Screen;

	// Initialise the SDL platform
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Error initializing SDL\n");
		return false;
	}

	SDL_GL_SetAttribute	(SDL_GL_RED_SIZE,		8);
	SDL_GL_SetAttribute	(SDL_GL_GREEN_SIZE,		8);
	SDL_GL_SetAttribute	(SDL_GL_BLUE_SIZE,		8);
	SDL_GL_SetAttribute	(SDL_GL_DEPTH_SIZE,		16);
	SDL_GL_SetAttribute	(SDL_GL_DOUBLEBUFFER,	1);

	// Initialise the video mode
	if (!(Screen = SDL_SetVideoMode(Width, Height, Bpp, Flags))) {
		printf("Error initializing the video mode\n");
		return false;
	}

	if (HasExtension("GL_EXT_compiled_vertex_array")) {
		glLockArraysEXT		= (PFNGLLOCKARRAYSEXTPROC)	SDL_GL_GetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT	= (PFNGLUNLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glUnlockArraysEXT");
	}

	// Initialize OpenGL
	glClearDepth	(1.0);
	glMatrixMode	(GL_PROJECTION);
	glLoadIdentity	();
	gluPerspective	(60.0, (float)Width / Height, 1.0, 350.0);
	glMatrixMode	(GL_MODELVIEW);
	glEnable		(GL_DEPTH_TEST);
	glEnable		(GL_CULL_FACE);
	glDepthFunc		(GL_LESS);			// Enable Z Buffering
	glDisable		(GL_NORMALIZE);

	glEnable		(GL_FOG);
	glFogi			(GL_FOG_MODE, GL_EXP);
	glFogfv			(GL_FOG_COLOR, (const float *)&BackColor);
	glFogf			(GL_FOG_DENSITY, 0.007f);

	glBlendFunc		(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup the clearing color
	glClearColor	(BackColor.X, BackColor.Y, BackColor.Z, BackColor.W);		// Clear using default color

	return true;
}



// Deinitialises SDL
void DeInitSDL(void)
{
	SDL_Quit();
}



// Need I say more?
int main(int ArgC, char *Args[])
{
	Vector3D<float>		 Dir;					// Camera's directional vector
	bool				 DemoOn(true);			// Are we quitting yet?
	Camera				 Cam;					// Camera object
	Sky	   				*Cloud;					// Sky dynamic terrain
	Desert				*Des;					// Desert static terrain
	Waves				*Wave;					// Alpha waves dynamic terrain
	unsigned long		 FirstTime, Frames = 0;	// FPS variables

	// Initialise the code
	if (!InitSDL(WIDTH, HEIGHT, BPP, SDL_FLAGS))
		return -1;

	Cloud	= new Sky();
	Des		= new Desert();
	Wave	= new Waves();

	// Let's get rolling...
	do {
		SDL_Event		Event;
		bool			HasMove(false);
		float			Time = (float)SDL_GetTicks();

		// We'll sample the time for our FPS if it's a new cycle
		if (!Frames)
			FirstTime = SDL_GetTicks();

		// Prepare the screen to render
		glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode	(GL_MODELVIEW);
		glLoadIdentity	();

		// First, rotate the camera
		glRotatef		(Cam.Rotation.X, 1, 0, 0);
		glRotatef		(Cam.Rotation.Y, 0, 1, 0);
		glRotatef		(Cam.Rotation.Z, 0, 0, 1);

		// Process the set of possible events
 		while (SDL_PollEvent(&Event)) {
			switch (Event.type) {
				case SDL_QUIT:
					DemoOn = false;
					break;



				case SDL_KEYDOWN:
					switch (Event.key.keysym.sym) {
						// ESCAPE quits
						case SDLK_ESCAPE:
							DemoOn = false;
							break;

						// "F" Shows the polygon scanline fills
						case SDLK_f:
							ShowFill ^= true;
							break;

						// "L" Shows the bounding polygon lines
						case SDLK_l:
							ShowLines ^= true;
							break;

						// "P" Shows the terrain planes
						case SDLK_p:
							ShowPlanes ^= true;
							break;

						// "C" Toggles 3D Y clipping
						case SDLK_c:
							ClipPlanes ^= true;
							break;

						// 1-3 selects the plane to show info about
						case SDLK_1:
						case SDLK_2:
						case SDLK_3:
							ShowID = Event.key.keysym.sym - SDLK_1;
							break;
					}

					break;



				// Click makes the camera move forward/backward
				case SDL_MOUSEBUTTONDOWN:
					Cam.GetDirection(Dir);

					if (Event.button.button == SDL_BUTTON_RIGHT)
						Dir *= -1;
					break;


				// Mouse button release stops the movement
				case SDL_MOUSEBUTTONUP:
					Dir = Vector3D<float>(0, 0, 0);
					break;


				// Moving hte mouse, moves the camera
				case SDL_MOUSEMOTION:
					if (!HasMove) {
						Cam.Rotation.Y += Event.motion.xrel / 5.0f;
						Cam.Rotation.X += Event.motion.yrel / 5.0f;

						SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
						HasMove = true;	// Mouse Warp generates a mouse motion event :S

					}
					break;
			}
		}

		// Update our position
		Cam.Position += Dir;

		// Update&Render everything
		Cloud->Update	(Cam, Time);
		Des->Update		(Cam);
		Wave->Update	(Cam, Time);

		Des->Draw		(Cam);
		Cloud->Draw		(Cam);
		Wave->Draw		(Cam);


		// Update our FPS calculation every 20 frames
		if (++Frames == 20) {
			char Title[255];
			sprintf(Title, "VTGDUF (FPS:%3.2f) Camera (%3.2f, %3.2f, %3.2f) °(%3.2f, %3.2f, %3.2f)", (float)Frames * 1000.0 / (SDL_GetTicks() - FirstTime),
					Cam.Position.X, Cam.Position.Y, Cam.Position.Z, Cam.Rotation.X, Cam.Rotation.Y, Cam.Rotation.Z);
			Frames = 0;

			SDL_WM_SetCaption(Title, NULL);
		}

		// Show the changes
		SDL_GL_SwapBuffers	();
	} while (DemoOn);


	// Exit cleanly
	delete Cloud;
	delete Des;
	delete Wave;
	DeInitSDL();

	return 0;
}
