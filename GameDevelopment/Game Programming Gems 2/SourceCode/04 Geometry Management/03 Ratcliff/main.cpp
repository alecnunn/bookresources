/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "main.h"
#include "circle.h"



HDC				backbuffer = 0;
HBITMAP		backbufferbitmap = 0;
char			textbuffer[1024];
CircleFactory *Application::mCircleFactory=0;

//======================================================================================
//======================================================================================
int WINAPI WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
{
	MSG			msg;

  int count = atoi( cmdLine );
  if ( count < 20 )    count = 1000; // default value is 1000
  if ( count > 10000 ) count = 10000;
	
	Application* App = new Application(count);

	if (App->Startup(appInstance))
	{
		delete (App);
		exit(-1);
	}

	while (!App->ExitApp())
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		App->Process();
	}

	App->Shutdown();

	delete(App);

	return 0;
}


//======================================================================================
//======================================================================================
Application::Application(int simcount)
{
  mSimulationCount  = simcount;
	mExitApplication	= false;
	mAppInstance			= NULL;
	mWindow						= NULL;
	mFrameRate				= 0;
	mUpdateFrameRate	= timeGetTime();
	mStep							= false;
	mPause						= false;
}


//======================================================================================
Application::~Application(void)
{
	delete (mCircleFactory);
}


//======================================================================================
int Application::Startup(HINSTANCE hinstance)
{
	srand(timeGetTime());

	mAppInstance									= hinstance;

	mWindowClass.cbSize						= sizeof(WNDCLASSEX);
	mWindowClass.cbClsExtra				= 0;
	mWindowClass.cbWndExtra				= 0;
	mWindowClass.hbrBackground		= (HBRUSH)GetStockObject(COLOR_BACKGROUND);
	mWindowClass.hCursor					= LoadCursor(NULL, IDC_ARROW);
	mWindowClass.hIcon						= NULL;
	mWindowClass.hIconSm					= NULL;
	mWindowClass.hInstance				= hinstance;
	mWindowClass.style						= NULL;
	mWindowClass.lpfnWndProc			= Application::WindowProc;
	mWindowClass.lpszMenuName			= NULL;
	mWindowClass.lpszClassName		= "spheretest_window_class";

	ATOM atom = RegisterClassEx(&mWindowClass);

	mWindow = CreateWindow(	"spheretest_window_class", 
													"Sphere Tree for Game Programming Gems 2 : by John W. Ratcliff.",
													WS_OVERLAPPEDWINDOW, 30, 30, SCREEN_WIDTH+8, SCREEN_HEIGHT+28, 
													NULL, NULL, mAppInstance, NULL);

	SetWindowLong(mWindow, GWL_USERDATA, (long)this);
	ShowWindow(mWindow, SW_SHOW);

	mCircleFactory = new CircleFactory(mSimulationCount);

	HDC hdc = GetDC(mWindow);
	backbuffer = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	SelectObject(backbuffer, bitmap);
	ReleaseDC(mWindow, hdc);

	SetTextColor(backbuffer, 0x00ffff00);
	SetBkMode(backbuffer, TRANSPARENT);

	return 0;
}


//======================================================================================
int Application::Shutdown(void)
{
	DeleteObject(backbufferbitmap);
	DeleteDC(backbuffer);
	backbuffer = 0;

	return 0;
}


//======================================================================================
int Application::Process(void)
{	
	HDC hdc = GetDC(mWindow);
	Render(hdc);
	ReleaseDC(mWindow, hdc);
	Sleep(1);

	return 0;
}


//======================================================================================
int Application::Render(HDC hdc)
{	
	BitBlt(backbuffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backbuffer, 0, 0, BLACKNESS);

	mFrameRate++;
	if (timeGetTime() > mUpdateFrameRate)
	{
		mUpdateFrameRate += 1000;
		itoa(mFrameRate, mBuffer, 10);
		mFrameRate = 0;
	}
	TextOut(backbuffer, 2, 2, mBuffer, strlen(mBuffer)); 

	//  Here is where we put in the code to render the spheres
	if (!mPause)
	{
		mCircleFactory->Process();
	}
	else if (mStep)
	{
		mPause = true;
		mStep = false;
		mCircleFactory->Process();
	}

	mCircleFactory->Render();

	BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backbuffer, 0, 0, SRCCOPY);

	return 0;
}


//======================================================================================
int DrawCircle(int locx, int locy, int radius, int color)
{
	if (backbuffer)
	{
		HPEN pen = CreatePen(PS_SOLID, 0, color);
		HGDIOBJ oldpen = SelectObject(backbuffer, pen);

		int left, top, right, bottom;
		int	startx, starty, endx, endy;

		left		= locx - radius;
		top			= locy - radius;
		right		= locx + radius;
		bottom	= locy + radius;
		startx	= locx;
		starty  = locy - radius;
		endx	= locx;
		endy  = locy - radius;

		Arc(backbuffer, left, top, right, bottom, startx, starty, endx, endy);
	
		SelectObject(backbuffer, oldpen);
		DeleteObject(pen);
	}

	return 0;
}


//======================================================================================
int PrintText(int x, int y, int color, char* output, ...)
{
	vsprintf(textbuffer, output, (char *)(&output+1));
	int oldcolor = SetTextColor(backbuffer, color);
	TextOut(backbuffer, x, y, textbuffer, strlen(textbuffer));
	SetTextColor(backbuffer, oldcolor);
	return 0;
}


//======================================================================================
int DrawLine(int x1, int y1, int x2, int y2, int color)
{
	HPEN pen = CreatePen(PS_SOLID, 0, color);
	HGDIOBJ oldpen = SelectObject(backbuffer, pen);

	MoveToEx(backbuffer, x1, y1, NULL);
	LineTo(backbuffer, x2, y2);
	
	SelectObject(backbuffer, oldpen);
	DeleteObject(pen);

	return 0;
}


//======================================================================================
LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Application* app = (Application*)GetWindowLong(hwnd, GWL_USERDATA);

	switch (msg)
	{
		case WM_CLOSE:
		{
			app->CloseApp(true);
			return 0;
		}

		case WM_CHAR:
		{
		  switch (wparam)
			{
        case 'a':
        case 'A':
  	  	  mCircleFactory->SetState(CS_SHOW_ALL);
          break;
	  	  case 't':
    		case 'T':
  	  	  mCircleFactory->SetState(CS_SHOW_RAYTRACE);
          break;
        case 'f':
        case 'F':
  	  	  mCircleFactory->SetState(CS_SHOW_FRUSTUM);
          break;
        case 'r':
        case 'R':
          mCircleFactory->SetState(CS_SHOW_RANGE_TEST);
          break;
				case VK_SPACE:
					app->PauseApp();
					return 0;
				case 13:
					app->RunApp();
					return 0;
			}
		}
	} // end switch(msg)

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

