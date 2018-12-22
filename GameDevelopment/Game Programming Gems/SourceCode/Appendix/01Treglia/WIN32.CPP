/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#include "win32.h"

/*
//----------------------------------------------------------------------------
// Sample!!
void main() {
	WinInit();

	glClearColor(0.3F, 0.0, 0.0, 0.0);

	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	while(1) {
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(10.0);
		glBegin(GL_POINTS);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();
		WinSwapBuffers();
	}
}
// Sample!!
//----------------------------------------------------------------------------
*/

//----------------------------------------------------------------------------
// Create Output Window
#define OUTPUT_WINDOW
#define APP_STRING "Text Module"

//----------------------------------------------------------------------------
// Global Variables
HINSTANCE	global_hInstance;
HWND		global_hWnd;
HDC			global_hDC;
HGLRC		global_hRC;
WinKey winKey;
HANDLE outputWinHandle = NULL;
HANDLE inputWinHandle = NULL;
unsigned int retraceCount;

char outStr[] = "Hello!\n";

//----------------------------------------------------------------------------
// Foward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				WinDestroy(void);
void 				main(void);


//----------------------------------------------------------------------------
// Win32 Main. 
// Note: Control is released to main()
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					 LPSTR lpCmdLine, int nCmdShow) {

	// Store instance handle in our global variable
	global_hInstance = hInstance;

	// Create Windows
	WinInit();

	// User's Main
	main();

	return 0;
}

//----------------------------------------------------------------------------
// Needs to be Called every frame to check Win32 status
void WinSwapBuffers(void) {
	MSG msg;
	RECT rect;

	winKey.downCnt = 0;
	winKey.upCnt = 0;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	SwapBuffers(global_hDC);

	GetWindowRect(global_hWnd, &rect);

	retraceCount ++;
}

//----------------------------------------------------------------------------
// Release Window Information
void WinDestroy(void) {
	DestroyWindow(global_hWnd);
}

//----------------------------------------------------------------------------
// Start the Output window
void WinStartConsole() {
	DWORD cCharsWritten;

	AllocConsole();
	outputWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	inputWinHandle = GetStdHandle(STD_INPUT_HANDLE);

	WriteConsole(outputWinHandle, outStr, strlen(outStr), &cCharsWritten, NULL);
}

//----------------------------------------------------------------------------
// Set Pixel Format
void WinSetDCPixelFormat(HDC hDC) {
	int fmtIndex;
	PIXELFORMATDESCRIPTOR 	pxlfmt;

	ZeroMemory(&pxlfmt, sizeof(pxlfmt));
	pxlfmt.nSize = sizeof(pxlfmt);
	pxlfmt.nVersion = 1;
	pxlfmt.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pxlfmt.iPixelType = PFD_TYPE_RGBA;
	pxlfmt.cColorBits = 24;
	pxlfmt.cDepthBits = 16;
	pxlfmt.iLayerType = PFD_MAIN_PLANE;

	// Set Pixel Format
	fmtIndex = ChoosePixelFormat(hDC, &pxlfmt);
	if (SetPixelFormat(hDC, fmtIndex, &pxlfmt) == FALSE) {
		MessageBox(global_hWnd, "The current pixel depth is not supported. Contact support@noa.com", 
		"Pixel Depth", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

//----------------------------------------------------------------------------
// Create the Window
void WinInit() { 
	DWORD error;
	WNDCLASS wc;
	RECT rect;
	DWORD style =  WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE;

	// Register Class
	wc.style			= CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= global_hInstance;
	// wc.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WIN32);
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= APP_STRING;
	RegisterClass(&wc);

	// Adjust Window Size
	rect.left = 0;
	rect.right = 640;
	rect.top = 0;
	rect.bottom = 480;
	AdjustWindowRect(&rect, style, FALSE);

#ifdef OUTPUT_WINDOW
	// Output Window
	WinStartConsole();
#endif

	// Create Window
	global_hWnd = CreateWindow(APP_STRING, APP_STRING, 
	    style,
		0, 0, 
		rect.right-rect.left, 
		rect.bottom-rect.top,
		NULL, NULL, 
		global_hInstance, NULL);

	// Make Sure
	if (!global_hWnd) {
		MessageBox(global_hWnd, "The Main Window could not be Created!", 
		"Initialize", MB_OK | MB_ICONERROR);
		error  = GetLastError();
		exit(0);
	}

	global_hDC = GetDC(global_hWnd);

	// Setup OpenGL
	WinSetDCPixelFormat(global_hDC);
	global_hRC = wglCreateContext(global_hDC);
	wglMakeCurrent(global_hDC, global_hRC);
	
	return;
}

//----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) 
	{
		case WM_CREATE:
		return 0;

		case WM_DESTROY:
			// Clean Up
			FreeConsole();
			wglDeleteContext(global_hRC);
			ReleaseDC(global_hWnd, global_hDC);
			PostQuitMessage(0);
			exit(0);
		return 0;

		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE:
					WinDestroy();
				break;
			}
			
			if (winKey.downCnt < 64) winKey.down[winKey.downCnt++] = wParam;
		return 0;

		case WM_KEYUP:
			if (winKey.upCnt < 64) winKey.up[winKey.upCnt++] = wParam;
		return 0;

		default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
