// winMain.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "menuManager.h"

// Global Variables:
HINSTANCE hInst;									// current instance
HWND      hMainWnd;									// application window handle
TCHAR szTitle[]       = { "menuManager" };			// The title bar text
TCHAR szWindowClass[] = { "MenuClass" };			// the main window class name

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Forward declarations of functions included in this code module:
BOOL				InitWindow(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

static menuManager& pMenuMgr = menuManager::getInstance();

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	int rc;

	MessageBox(NULL, "Press the arrow keys to move the menu controls\nUse the backspace key to return to a previous menu\nPress the escape key to end", "Menu System", MB_OK);

	// Perform application initialization:
	if (!InitWindow(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	// initialize the menu system
	if (!pMenuMgr.init())
	{
		return FALSE;
	}

	// init the resourceHandler, handles images and double buffering
	resourceHandler::init(hMainWnd, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Main message loop:	
	while (1)		
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// hitting the Escape key ends the application
			if KEYDOWN(VK_ESCAPE)
				SendMessage (hMainWnd, WM_CLOSE, 0, 0);

			// update the menu system
			rc = pMenuMgr.update();
			if (!rc)
				break;

			// render the menus
			pMenuMgr.render();

			// flip the back/front buffers
			resourceHandler::flip();

			// let the rest of the system have a little bit of processor time
			Sleep(1);
		}
	}

	// shutdown the resource handler
	resourceHandler::shutdown(hMainWnd);

	// shutdown the menu system
	pMenuMgr.shutdown();

	return (int) msg.wParam;
}


BOOL InitWindow(HINSTANCE hInstance, int nCmdShow)
{	
	WNDCLASSEX wcex;

	// Store instance handle in our global variable	
	hInst = hInstance; 

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= 0;

	// register the window class
	RegisterClassEx(&wcex);

	// create the application window
	hMainWnd = CreateWindow(szWindowClass, szTitle, WS_BORDER,
      CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

	// make sure there is a valid window handle
	if (!hMainWnd)
	{
		return FALSE;
	}

	// display the application window
	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;  // used in WM_PAINT
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		
		case WM_PAINT: 
		{
			BeginPaint(hWnd, &ps); // validate the window            
			EndPaint(hWnd, &ps);

			return(0); // return success
		}  break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}