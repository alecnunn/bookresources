/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Real Time Universe Generator
// (c) 2000 Guy W. Lecky-Thompson

#include <windows.h>
#include <commdlg.h>

// Application specific includes
#include "unigen.h" 

// Globally defined objects, those which have a lifespan for the entire
// application session

long FAR PASCAL WndProc (HWND, UINT, UINT, LONG);

HINSTANCE hInst;

int __stdcall WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
			 LPSTR lpszCmdParam, int nCmdShow) {
	static char szAppName[] = "RTUG";
	HWND			hwnd;
	MSG			msg;
	WNDCLASS		wndclass;

	if (!hPrevInstance)
	{
		wndclass.style				= CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc		= WndProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= 0;
		wndclass.hInstance		= (HINSTANCE)hInstance;
		wndclass.hIcon			= LoadIcon (NULL,IDI_APPLICATION);
		wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground	= (HBRUSH)GetStockObject (WHITE_BRUSH);
		wndclass.lpszMenuName	= NULL;
		wndclass.lpszClassName	= szAppName;

		RegisterClass (&wndclass);
	}

	hwnd = CreateWindow (szAppName,
								"RealTime Universe Generator v1.0",
								WS_CAPTION|WS_BORDER|WS_SYSMENU,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								NULL,
								NULL,
								(HINSTANCE)hInstance,
								NULL);

	ShowWindow (hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow (hwnd);
	hInst = (HINSTANCE)hInstance;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage (&msg);
	}

	return msg.wParam;
}

long FAR PASCAL WndProc (HWND hwnd, UINT message,
											UINT wParam, LONG lParam)
{
	HDC 			hdc;
	PAINTSTRUCT ps;
	static RECT			rect, rcMapInfo, rcMapWindow, rcSolarSystemWindow;
	static int mouse_x, mouse_y;
	static long universe_number;

	switch (message)
	{
		case WM_CREATE:
			universe_number = 1;
		return 0;

		case WM_SIZE:
			GetClientRect(hwnd,&rect);
			// The display area is divided up, and we should clip it, too
			rcMapWindow.left = rect.left;
			rcMapWindow.right = rect.right - ((rect.right - rect.left) / 4);
			rcMapWindow.top = rect.top;
			rcMapWindow.bottom = rect.bottom;
			rcMapInfo.left = rcMapWindow.right + 1;
			rcMapInfo.top = rect.top + 1;
			rcMapInfo.right = rect.right - 1;
			rcMapInfo.bottom = rcMapInfo.top + (rect.bottom / 4);
			rcSolarSystemWindow.top = rcMapInfo.bottom + 1;
			rcSolarSystemWindow.left = rcMapInfo.left;
			rcSolarSystemWindow.right = rcMapInfo.right;
			rcSolarSystemWindow.bottom = rcMapWindow.bottom-10;
		break;

		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			HPEN hOldPen,hPen;
			HBRUSH hOldBrush,hBrush;

			hBrush = CreateSolidBrush(PALETTERGB(200,200,200));
			hPen = CreatePen(PS_SOLID,1,PALETTERGB(0,0,0));

			DisplayUniverse(universe_number,&rcMapWindow,hdc);
			hOldPen = (HPEN)SelectObject(hdc,hPen);
			hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);
			RoundRect(hdc,rcMapInfo.left, rcMapInfo.top,
							  rcMapInfo.right,rcMapInfo.bottom,
							  (rcMapInfo.right - rcMapInfo.left) / 8,
							  (rcMapInfo.bottom - rcMapInfo.top) / 8);
			SelectObject(hdc,hOldPen);
			SelectObject(hdc,hOldBrush);
			DeleteObject(hPen);
			DeleteObject(hBrush);
			EndPaint (hwnd, &ps);
		return 0;

		case WM_MOUSEMOVE:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			hdc = GetDC(hwnd);
			DisplayFeatureInformation(universe_number,&rcMapWindow,mouse_x,mouse_y,
											  &rcMapInfo,hdc);
			ReleaseDC(hwnd,hdc);
		return 0;

		case WM_LBUTTONUP:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			hdc = GetDC(hwnd);
			DisplaySolarSystem(universe_number,&rcMapWindow,
												mouse_x,mouse_y,
											  &rcSolarSystemWindow,hdc);
			ReleaseDC(hwnd,hdc);
		return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}
