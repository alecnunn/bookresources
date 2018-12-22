/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Game Scheduler Gem sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/


//
// This app is intended to demonstrate how to interface a scheduler into
// a GUI app, as well as dynamic adding and removing of tasks. Normally
// you probably wouldn't just store tasks in an array.
//
// Spacebar pauses/unpauses the window
//   '+' or 'a' adds a new ball
//   '-' or 'd' deletes a ball
//   clicking on a ball delets it
//

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_CLASS "MainWndClass"

#define FPS 60

#define MINX 0
#define MINY 0
#define MAXX 400
#define MAXY 400
#define BALLSIZE 40

#define MAX_BALLS 50
int BALLS = 3;

HWND hwndMain;

struct Ball {
	double x;
	double y;
	double xd;
	double yd;
	HBRUSH hbr;
	HBRUSH old;
	int task_id;
} ball[MAX_BALLS];

int x = MAXX/2, y = MAXY/2;
int xd = 2;
int yd = 3;

HBITMAP offscreen;
RECT offscreen_rect;

//---------------------------------------------------------------------
// customer tasks (see below for definitions)
//---------------------------------------------------------------------

#include "Scheduler.h"
#include "ITask.h"

class AnimateTask : public ITask {
public:
	virtual void Execute(int id, int time, void * arg);
};

class RenderTask : public ITask {
public:
	virtual void Execute(int id, int time, void * arg);
};

Scheduler sched;
RenderTask render;
AnimateTask animate;

//---------------------------------------------------------------------
// Paint
//---------------------------------------------------------------------

// color array
unsigned clr[] = { 0xFF0000,0x00FF00,0x0000FF,0xFFFF00,0xFF00FF,0x00FFFF };

void Add_Ball()
{
	// add a new ball to the window and schedule a task for it
	if (BALLS < MAX_BALLS)
	{
		ball[BALLS].x = 100;
		ball[BALLS].y = 100;
		ball[BALLS].xd = 1 * (double(rand()) / double(RAND_MAX)) + 1;
		ball[BALLS].xd *= double((rand()%2)==0?1:-1);
		ball[BALLS].yd = 1 * (double(rand()) / double(RAND_MAX)) + 1;
		ball[BALLS].yd *= double((rand()%2)==0?1:-1);

		int r = ((clr[BALLS%6]>>16) & 0xFF);
		int g = ((clr[BALLS%6]>>8) & 0xFF);
		int b = ((clr[BALLS%6]) & 0xFF);
		ball[BALLS].hbr = CreateSolidBrush(RGB(r,g,b));

		sched.Schedule(TASK_TIME, sched.GetClock().GetTime(), 15, 0, &animate, (void*) BALLS, &(ball[BALLS].task_id));
		BALLS++;
	}
}

void Delete_Ball(int n)
{
	// delete ball from array entry [n]
	sched.Terminate(ball[n].task_id);
	DeleteObject(ball[n].hbr);
	for (int i=n+1; i<BALLS; i++) {
		ball[i-1] = ball[i];
	}
	BALLS--;
}

void Delete_Ball()
{
	// delete last ball
	if (BALLS)
		Delete_Ball(BALLS-1);
}

void Delete_Ball(int x, int y)
{
	// delete ball at (x,y) if any
	for (int i=0; i<BALLS; i++)
	{
		if (x >= ball[i].x && x <= ball[i].x + BALLSIZE &&
			y >= ball[i].y && y <= ball[i].y + BALLSIZE )
		{
			Delete_Ball(i);
			break;
		}
	}
}

void Animate_Init()
{
	BALLS=0;

	// randomize things
	srand(GetTickCount());

	// create three balls to start off
	for (int i=0; i<3; i++)
		Add_Ball();

	// start the scheduler
	sched.Schedule(TASK_RENDER, 0, 0, 0, &render, NULL, NULL);
	sched.Run();
}

void Animate_Term()
{
	// delete our array
	sched.Stop();
	for (int i=0; i<BALLS; i++) {
		sched.Terminate(ball[i].task_id);
		DeleteObject(ball[i].hbr);
	}
	BALLS=0;
}

//---------------------------------------------------------------------
// these functions perform offscreen graphics buffering
//---------------------------------------------------------------------

void InitPaintBuffer(HWND hwnd)
{
	offscreen_rect.left = MINX;
	offscreen_rect.right = MAXX+1;
	offscreen_rect.top = MINY;
	offscreen_rect.bottom = MAXY+1;

	RECT rc;
	GetClientRect(hwnd, &rc);
	HDC hdc = GetDC(hwnd);
	offscreen = CreateCompatibleBitmap(hdc,
		offscreen_rect.right - offscreen_rect.left,
		offscreen_rect.bottom - offscreen_rect.top);
	ReleaseDC(hwnd, hdc);
}
void DeletePaintBuffer()
{
	DeleteObject(offscreen);
}

void PaintDC(HDC hdc, RECT * rect)
{
	//
	// paint the balls onto the offscreen buffer
	//
	HPEN hpen, hpenold;

	HBRUSH hbr = CreateSolidBrush(RGB(255,255,255));
	FillRect(hdc, rect, hbr);
	DeleteObject(hbr);

	hpen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	hpenold = (HPEN) SelectObject(hdc, hpen);

	MoveToEx(hdc, MINX, MINY, NULL);
	LineTo(hdc, MAXX, MINY);
	LineTo(hdc, MAXX, MAXY);
	LineTo(hdc, MINX, MAXY);
	LineTo(hdc, MINX, MINY);

	LineTo(hdc, MAXX, MAXY);
	MoveToEx(hdc, MINX, MAXY, NULL);
	LineTo(hdc, MAXX, MINY);

	// animate a bouncing circle
	for (int i=0; i<BALLS; i++)
	{
		ball[i].old = (HBRUSH) SelectObject(hdc, ball[i].hbr);
		Ellipse(hdc, (int) ball[i].x, (int) ball[i].y, (int) ball[i].x+BALLSIZE, (int) ball[i].y+BALLSIZE);
		SelectObject(hdc, ball[i].old);
	}

	SelectObject(hdc, hpenold);
	DeleteObject(hpen);
}

void PaintBuffer(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP old = (HBITMAP) SelectObject(memdc, offscreen);

	PaintDC(memdc, &offscreen_rect);

	SelectObject(memdc,old);
	DeleteDC(memdc);
	ReleaseDC(hwnd, hdc);
}

void PaintFromMemory(HDC hdc, RECT * rect)
{
	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP old = (HBITMAP) SelectObject(memdc, offscreen);
	BitBlt(hdc,
		rect->left,
		rect->top,
		rect->right - rect->left,
		rect->bottom - rect->top,
		memdc,
		rect->left,
		rect->top, SRCCOPY);
	SelectObject(memdc, old);
	DeleteDC(memdc);
}

void Paint(HWND hwnd, PAINTSTRUCT * ps)
{
	PaintFromMemory(ps->hdc, &offscreen_rect);
}

//---------------------------------------------------------------------
// Animation function
// animate a single ball
//---------------------------------------------------------------------

void Animate(int arg)
{
	int i = arg;
	ball[i].x += ball[i].xd;
	ball[i].y += ball[i].yd;

	if (ball[i].x < MINX) {
		ball[i].x -= ball[i].xd;
		ball[i].xd = -ball[i].xd;
	} else if (ball[i].x >= (MAXX-BALLSIZE)) {
		ball[i].x -= ball[i].xd;
		ball[i].xd = -ball[i].xd;
	}

	if (ball[i].y < MINY) {
		ball[i].y -= ball[i].yd;
		ball[i].yd = -ball[i].yd;
	} else if (ball[i].y >= (MAXY-BALLSIZE)) {
		ball[i].y -= ball[i].yd;
		ball[i].yd = -ball[i].yd;
	}
}

//---------------------------------------------------------------------
// task definitions
//---------------------------------------------------------------------

void RenderTask::Execute(int id, int time, void * arg)
{
	PaintBuffer(hwndMain);
	InvalidateRect(hwndMain, &offscreen_rect, false);
}

void AnimateTask::Execute(int id, int time, void * arg)
{
	int ball_index = (int) arg;

//	printf("{%d}\n",ball_index);
	Animate(ball_index);
}

//---------------------------------------------------------------------
// Window Proc
//---------------------------------------------------------------------

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			// setup offscreen buffer
			InitPaintBuffer(hwnd);

			// setup the scheduler
			SetTimer(hwnd, 1, 1000/FPS, NULL);
			Animate_Init();
			return 0;
		}

		case WM_PAINT:
		{
			// paint client area
			PAINTSTRUCT ps;
        	BeginPaint(hwnd, &ps);
        	Paint(hwnd, &ps);
        	EndPaint(hwnd, &ps);
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			// delete a ball by clicking on it
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);
			Delete_Ball(x,y);
			return 0;
		}

		case WM_CHAR:
		{
			switch (wparam)
			{
				case ' ': // spacebar toggles pause
				{
					if (sched.IsRunning())
						sched.Stop();
					else
						sched.Run();
					break;
				}
				case '+': case '=':
				case 'a': case 'A': // add a ball
				{
					Add_Ball();
					break;
				}
				case '-': case '_':
				case 'd': case 'D': // delete a ball
				{
					Delete_Ball();
					break;
				}
			}
			return 0;
		}

		case WM_TIMER:
		{
			// activate scheduler
			sched.ExecuteFrame();
			return 0;
		}

		case WM_CLOSE:
		{
			if (IDYES == MessageBox(hwnd, "Really Quit?", "Closing Window",
				MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL))
				DestroyWindow(hwnd);
			return 0;
		}

		case WM_DESTROY:
		{
			// shutdown scheduler
			Animate_Term();
			KillTimer(hwnd,1);

			// cleanup buffering
			DeletePaintBuffer();

			PostQuitMessage(0); // post WM_QUIT
			return 0;
		}

		default:
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}

	return 0;
}


//---------------------------------------------------------------------
// Main Proc
//---------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL retval;
	WNDCLASS wc;

	// create a console so we can use printf() to debug
	AllocConsole();
	freopen("CONOUT$", "a", stdout); // redirect printf to console

	if (!hPrev)
	{
		wc.style = 0;
		wc.lpfnWndProc = (WNDPROC) MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = "MainMenu";
		wc.lpszClassName = WINDOW_CLASS;

		if (!RegisterClass(&wc))
			return FALSE;
	}

	hwndMain = CreateWindow(
		WINDOW_CLASS, "Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 450, 450,
		(HWND) NULL, (HMENU) NULL, hInst, (LPVOID) NULL);

	if (!hwndMain)
		return FALSE;

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	while ((retval = GetMessage( &msg, NULL, 0, 0)) != 0)
	{
		if (retval != -1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



