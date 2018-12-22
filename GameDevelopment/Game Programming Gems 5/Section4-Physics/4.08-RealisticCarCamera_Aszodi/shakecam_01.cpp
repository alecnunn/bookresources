//************************************************************************* //
// Demonstrative OpenGL application presenting real movement of a camera
// which follows a moving car-like object
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //

//#include <stdafx.h>
#include <windows.h>							
#include <gl/glut.h>
#include <gl/glaux.h>
#include <stdarg.h>						
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include "camera.h"
#include "car.h"
#include "envrnmnt.h"
#include "tgaldr.h"


// Initialization of variables
HGLRC		hRC=NULL;							
HDC		hDC=NULL;							
HWND		hWnd=NULL;							
HINSTANCE	hInstance;							
bool	keys[256];							
bool	active=true;							
bool	fullscreen=true;
// end of Inic.


// Moving object in the virtual scene **********************
Car car(0.0, 0.0, -5.0);

// Environment ************************
Environment environment;

// Screen parameters
int wWidth= 0;
int wHeight = 0;

// Cameras *****************************
Camera camera(-5.0, 5.0, -20.0, 0.0, 0.0, -5.0, 0.0, 1.0, 0.0, 28.0);
Camera camera2(-3.0, 10.0, 20.0, 0.0, 0.0, -5.0, 0.0, 1.0, 0.0, 28.0);
bool directFollow = false;                        // In case of "direct follow" we don't take the human behavior into account
// end camera ******************


GLuint	texture[1];

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void LoadGLTexture(int idx, char * texturefilename) { // Load TGA images and convert to textures
	TGAImage tgaimg(256, 256, 255, 255, 255);     // Creating a white background 256x256 image by default
	tgaimg.loadFromFile(texturefilename);
	glGenTextures(1, &texture[idx]);
	glBindTexture(GL_TEXTURE_2D, texture[idx]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tgaimg.getWidth(), tgaimg.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tgaimg.getImageDataPointer());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ReSizeGLScene(GLsizei width, GLsizei height) {  // Resize and initialize the GL window
	if (height == 0) height = 1;											
	wWidth = width;
	wHeight = height;
	glViewport(0, 0, width, height);						
	glMatrixMode(GL_PROJECTION);							
	glLoadIdentity();										
	gluPerspective(28.0f, (GLfloat)wWidth / (GLfloat)wHeight, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);								
	glLoadIdentity();										
}

bool InitGL(GLvoid) {                              // Setup for OpenGL
	LoadGLTexture(0, "asphalt.tga");
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      // Black background
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);                    // Enable back-face culling (to enhance the performance)
	return true;
}

void Camera::buildScene() {                       // This function defines our virtual scene calling the drawInScnene() functions of the virtual objects. Is is called by the Render() function of the cameras
	car.drawInScene();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	environment.drawInScene();
}

bool DrawGLScene(int cam) {                       // Draw the whole virtual scene from one of the cameras' viewpoint, while the additional calculations are done in case of each camera.
	camera.recalculateTime();
	camera2.recalculateTime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.placeObjectHereToFollow(car.Position(), directFollow);
	camera2.placeObjectHereToFollow(car.Position(), directFollow);
	switch (cam) {
		case 0: camera.Render(wWidth, wHeight); break;
		case 1: camera2.Render(wWidth, wHeight); break;
	}
	return true;						
}

void KillGLWindow() {
	if (fullscreen) {
		ChangeDisplaySettings(NULL,0);						
		ShowCursor(true);									
	}
	if (hRC) {
		if (!wglMakeCurrent(NULL,NULL)) MessageBox(NULL,"Release Of DC and RC failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		if (!wglDeleteContext(hRC)) MessageBox(NULL,"Release Rendering Context failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hRC=NULL;							
	}
	if (hDC && !ReleaseDC(hWnd,hDC)) {
		MessageBox(NULL,"Release Device Context failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							
	}
	if (hWnd && !DestroyWindow(hWnd)) {
		MessageBox(NULL,"Could not release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							
	}
	if (!UnregisterClass("OpenGL",hInstance)) {
		MessageBox(NULL,"Could not unregister class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									
	}
}

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag) {
	wWidth = width;
	wHeight = height;
	GLuint PixelFormat;						
	WNDCLASS wc;									
	DWORD dwExStyle;							
	DWORD dwStyle;							
	RECT WindowRect;								
	WindowRect.left = (long)0;						
	WindowRect.right = (long)width;					
	WindowRect.top = (long)0;							
	WindowRect.bottom = (long)height;					
	fullscreen = fullscreenflag;						
	
	hInstance = GetModuleHandle(NULL);			
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) WndProc;			
	wc.cbClsExtra = 0;						
	wc.cbWndExtra = 0;						
	wc.hInstance = hInstance;				
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground = NULL;						
	wc.lpszMenuName = NULL;						
	wc.lpszClassName = "OpenGL";					
	
	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Failed to register the window class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;							
	}

	if (fullscreen) {
		DEVMODE dmScreenSettings;				
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);	
		dmScreenSettings.dmPelsWidth = width;			
		dmScreenSettings.dmPelsHeight = height;			
		dmScreenSettings.dmBitsPerPel = bits;				
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			if (MessageBox(NULL, "The requested fullscreen mode is not supported by\nYour video card. Use windowed mode instead?", "ERROR", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) fullscreen = false;				
			else {
				MessageBox(NULL, "Program will now close.", "ERROR", MB_OK | MB_ICONSTOP);
				return false;					
			}
		}
	}
	if (fullscreen) {
		dwExStyle=WS_EX_APPWINDOW;				
		dwStyle=WS_POPUP;						
		ShowCursor(false);						
	} else {
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;				
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);	
	if (!(hWnd = CreateWindowEx(dwExStyle, "OpenGL", title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 0, 0, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, hInstance, NULL))) {
		KillGLWindow();							
		MessageBox(NULL, "Window creation error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;							
	}
	static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, bits, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0							
	};
	if (!(hDC=GetDC(hWnd))) {
		KillGLWindow();							
		MessageBox(NULL, "Can't create a GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) {
		KillGLWindow();
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;							
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd)) {
		KillGLWindow();
		MessageBox(NULL, "Can't set the PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if (!(hRC=wglCreateContext(hDC))) {
		KillGLWindow();
		MessageBox(NULL, "Can't create a GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if(!wglMakeCurrent(hDC,hRC)) {
		KillGLWindow();
		MessageBox(NULL, "Can't activate the GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(width, height);
	if (!InitGL()) {
		KillGLWindow();
		MessageBox(NULL, "Initialization failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;						
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_ACTIVATE: {
			active = (!HIWORD(wParam));
			return 0;						
		}
		case WM_SYSCOMMAND: {
			switch (wParam) {
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}
		case WM_CLOSE: {
			PostQuitMessage(0);				
			return 0;						
		}
		case WM_KEYDOWN: {
			keys[wParam] = TRUE;			
			return 0;						
		}
		case WM_KEYUP: {
			keys[wParam] = FALSE;			
			return 0;						
		}
		case WM_SIZE: {
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			return 0;
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG	msg;
	BOOL done=false;
	// Temporary char buffer for constructing screen messages
	char message[256];
	// Saving actual parameters into the 'original' variables for the case of reset
	camera.saveParametersToOriginals();
	camera2.saveParametersToOriginals();
	int cam = 0;                       // "Which camera is used actually?"

	////////// STARTING THE APPLICATION /////////////////////////////////////////////////

	if (MessageBox(NULL, "Would You like to run in fullscreen mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO) {
		fullscreen=false;
	}
	if (!CreateGLWindow("ShakeCam v0.1", 640, 480, 16, fullscreen)) {
		return 0;
	}
	while(!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				done = true;	
			} else {
				TranslateMessage(&msg);	
				DispatchMessage(&msg);	
			}
		} else {
			// Animate moving objects
			car.animate(camera.getLastFrameTime());

			// Handling keyboard events
			if (active) {
				if (keys[VK_ESCAPE]) {  // Exit
					done = true;
				} else {
					// Define screen messages (Main title, Help screen, FPS value etc.)
					camera.setScreenMessage(0, "ShakeCam v0.1 - Keyboard Layout:\n\t[F1] - toggle this help screen ON/OFF\n\t[F2] - toggle full screen mode\n\t[UP] [END] [LEFT] [RIGHT] - accelerate ahead, break, steer left, steer right the car\n\t[DOWN] - accelerate backwards\n\t[SPACE] - hand break (sudden stop)\n\t[1]...[7] - show/hide screen message 1...7 (#7 is the DEBUG-graph by default)\n\t[R] - Reset scene\n\t[Q] - toggle direct follow / human camera control\n\t[A] [D] [W] [S] - rotate left/right, raise/sink camera's eye\n\t[O] - toggle auto zoom ON/OFF\n\t[I] [U] - zoom in/out\n\t[TAB] - switch camera\n\tNumpad\n\t\t[7] [8] [9] - huge amplitude\n\t\t[4] [5] [6] - moderate amplitude\n\t\t[1] [2] [3] - small amplitude\n\t\tSlow, fast, extreme fast shaking of the car (vertical movement)\n\t[ESC] - exit", 10, 460);
					camera2.setScreenMessage(0, "ShakeCam v0.1 - Keyboard Layout:\n\t[F1] - toggle this help screen ON/OFF\n\t[F2] - toggle full screen mode\n\t[UP] [END] [LEFT] [RIGHT] - accelerate ahead, break, steer left, steer right the car\n\t[DOWN] - accelerate backwards\n\t[SPACE] - hand break (sudden stop)\n\t[1]...[7] - show/hide screen message 1...7 (#7 is the DEBUG-graph by default)\n\t[R] - Reset scene\n\t[Q] - toggle direct follow / human camera control\n\t[A] [D] [W] [S] - rotate left/right, raise/sink camera's eye\n\t[O] - toggle auto zoom ON/OFF\n\t[I] [U] - zoom in/out\n\t[TAB] - switch camera\n\tNumpad\n\t\t[7] [8] [9] - huge amplitude\n\t\t[4] [5] [6] - moderate amplitude\n\t\t[1] [2] [3] - small amplitude\n\t\tSlow, fast, extreme fast shaking of the car (vertical movement)\n\t[ESC] - exit", 10, 460);
					sprintf(message, "[6] FPS: %f", (cam)?(camera.getActualFPS()):(camera2.getActualFPS()));
					camera.setScreenMessage(6, message, 10, 40);
					camera2.setScreenMessage(6, message, 10, 40);
					if (cam) sprintf(message, "[5] FOV dif: %f",camera2.getFov()-camera2.FovToReach());
					else sprintf(message, "[5] FOV dif: %f",camera.getFov()-camera.FovToReach());
					camera.setScreenMessage(5, message, 10, 60);
					camera2.setScreenMessage(5, message, 10, 60);
					

					// DEBUG-graph as the 8th (no.7) screen message. On the screen only
					// the coordinates of the ScreenMessage will be seen.
					camera.ScreenMessagePositionVector(7).X() = 10 * camera.Target().X();
					camera2.ScreenMessagePositionVector(7).X() = 10 * camera2.Target().X();
					camera.ScreenMessagePositionVector(7).Y() = 10 * car.Position().X();
					camera2.ScreenMessagePositionVector(7).Y() = 10 * car.Position().X();
					camera.ScreenMessagePositionVector(7).Z() = 30 * (camera.getFov() - camera.FovToReach());
					camera2.ScreenMessagePositionVector(7).Z() = 30 * (camera2.getFov() - camera2.FovToReach());

					// Continuing OpenGL drawing
					DrawGLScene(cam);             // Drawing the sight of one of the cameras
					SwapBuffers(hDC);
				}
			}
			if (keys[VK_F2]) {  // Toggle full screen mode
				keys[VK_F2] = false;
				KillGLWindow();
				fullscreen = !fullscreen;
				if (!CreateGLWindow("Windowed mode", 640, 480, 16, fullscreen)) {
					return 0;
				}
			}
			if (keys[VK_F1]) {  // Toggle this help screen ON/OFF
				keys[VK_F1] = false;
				camera.ScreenMessage() ^= 0x01;
				camera2.ScreenMessage() ^= 0x01;
			}
			for (char k = '1', bit = 1; k <= '7'; k++, bit++)  // Show/hide screen message 1...7
				if (keys[k]) {
					keys[k] = false;
					camera.ScreenMessage() ^= (1 << bit);
					camera2.ScreenMessage() ^= (1 << bit);
				}
			float oneBitOfChange = camera.getLastFrameTime() * 0.012;  // Animating the scene, this value makes the movements FPS-independent
			if (keys['A']) {  // Rotate right the camera's eye around the target
				Vector3f eye2target = camera.Eye() - camera.Target();     // For the first camera 
				Vector3f eye2target2 = camera2.Eye() - camera2.Target();  // For the second camera
				
				if (cam) { 
					eye2target2 = eye2target2.Rotate(Vector3f(0.0, 1.0, 0.0), -oneBitOfChange * 4);
					camera2.Eye() = camera2.Target() + eye2target2;
				} else {
					eye2target = eye2target.Rotate(Vector3f(0.0, 1.0, 0.0), -oneBitOfChange * 4); 
					camera.Eye() = camera.Target() + eye2target;
				}
			}
			if (keys['D']) {  // Rotate left the camera's eye around the target
				Vector3f eye2target = camera.Eye() - camera.Target();
				Vector3f eye2target2 = camera2.Eye() - camera2.Target();

				if (cam) { 
					eye2target2 = eye2target2.Rotate(Vector3f(0.0, 1.0, 0.0), oneBitOfChange * 4);
					camera2.Eye() = camera2.Target() + eye2target2;
				} else {
					eye2target = eye2target.Rotate(Vector3f(0.0, 1.0, 0.0), oneBitOfChange * 4); 
					camera.Eye() = camera.Target() + eye2target;
				}
			}
			if (keys['W']) {  // Raise the camera's eye
				if (cam) camera2.Eye().Y() += oneBitOfChange;
				else camera.Eye().Y() += oneBitOfChange;
			}
			if (keys['S']) {  // Sink the camera's eye
				if (cam) camera2.Eye().Y() -= oneBitOfChange;
				else camera.Eye().Y() -= oneBitOfChange;
			}
			if (keys[VK_UP]) {  // Accelerate the car (ahead)
				car.Speed().Z() -= oneBitOfChange;
			}
			if (keys[VK_DOWN]) {  // Accelerate the car (backwards)
				car.Speed().Z() += oneBitOfChange;
			}
			if (keys[VK_END]) {   // Break the car
				if (abs(car.Speed().Z()) < oneBitOfChange) car.Speed().Z() = 0.0;
				else {
					if (car.Speed().Z() < 0) car.Speed().Z() += oneBitOfChange;
					if (car.Speed().Z() > 0) car.Speed().Z() -= oneBitOfChange;
				}
			}
			if (keys[VK_LEFT]) {   // Steering left
				car.setSteeringAngle(car.getSteeringAngle() + (oneBitOfChange * 10));
			}
			if (keys[VK_RIGHT]) {   // Steering right
				car.setSteeringAngle(car.getSteeringAngle() - (oneBitOfChange * 10));
			}
	
			if (keys[VK_SPACE])	{  // Hand break (sudden stop)
				car.stopMotion();
				car.RotationSpeed().set(0.0, 0.0, 0.0);
			}
			if (keys['R']) {  // Reset the scene
				car.stopMotion();
				car.setPosition(0.0, 0.0, -5.0);
				car.setSteeringAngle(0.0);
				car.Orientation().set(0.0, 0.0, 0.0);
				camera.reset();
			}
			if (keys['Q']) {  // Toggle direct follow / human camera control
				keys['Q'] = false;
				directFollow = !(directFollow);
			}
			if (keys['U']) {  // Zoom out
				if (cam) {
					camera2.setFov(camera2.getFov() + (oneBitOfChange * 10));
					if (camera2.getFov() > 179) camera2.setFov(179);
				} else {
					camera.setFov(camera.getFov() + (oneBitOfChange * 10));
					if (camera.getFov() > 179) camera.setFov(179);
				}
			}
			if (keys['I']) {  // Zoom in
				if (cam) {
					camera2.setFov(camera2.getFov() - (oneBitOfChange * 10));
					if (camera2.getFov() < 1) camera2.setFov(1);
				} else {
					camera.setFov(camera.getFov() - (oneBitOfChange * 10));
					if (camera.getFov() < 1) camera.setFov(1);
				}
			}
			if (keys['O']) {  // Toggle auto zoom ON/OFF
				keys['O'] = false;
				camera.setAutoZoom(!camera.getAutoZoom(), car.AllAroundSize());
				camera2.setAutoZoom(!camera2.getAutoZoom(), car.AllAroundSize());
			}
			// Using the numpad keys, you can simulate the shaking of the followed object with different speed and different amplitude
			long int actualTime = camera.getCurrentTime();
			if (keys[VK_NUMPAD1]) car.simulateBumps(actualTime, 0.05, 2);
			if (keys[VK_NUMPAD2]) car.simulateBumps(actualTime, 0.05, 8);
			if (keys[VK_NUMPAD3]) car.simulateBumps(actualTime, 0.05, 20);
			if (keys[VK_NUMPAD4]) car.simulateBumps(actualTime, 0.1, 2);
			if (keys[VK_NUMPAD5]) car.simulateBumps(actualTime, 0.1, 8);
			if (keys[VK_NUMPAD6]) car.simulateBumps(actualTime, 0.1, 20);
			if (keys[VK_NUMPAD7]) car.simulateBumps(actualTime, 0.3, 2);
			if (keys[VK_NUMPAD8]) car.simulateBumps(actualTime, 0.3, 8);
			if (keys[VK_NUMPAD9]) car.simulateBumps(actualTime, 0.3, 20);

			if (keys[VK_TAB]) {  // Switch camera
				keys[VK_TAB] = false;
				cam++;
				if (cam > 1) cam = 0;
			}
		}
	}
	KillGLWindow();
	return (msg.wParam);
}
