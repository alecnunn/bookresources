//----------------------------------------------------------------------------
//
//win_wrap.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

#include "win_wrap.h"
#include "gl_wrap.h"

//··········································································//
//    Source :: Definitions
//··········································································//

#define NUM_KEYS 256

#ifndef CDS_FULLSCREEN
	#define CDS_FULLSCREEN 4
#endif

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

static bool wnd_set_flscr(void);
static void wnd_kill_wnd(HWND & hWnd, HDC & hDC);
//··········································································//
//    Source :: Private Vars
//··········································································//

static int wnd_width = 0;
static int wnd_height = 0;
static int wnd_bpp = 0;

static HWND wnd_hWnd = 0;

static bool keys[NUM_KEYS];
static int mouse_x = 0;
static int mouse_y = 0;

//··········································································//
//    Source :: External Function Implementation
//··········································································//

bool wnd_register_cls( const HINSTANCE & hInstance, const char * cls_name, const void * wnd_proc)
{
	WNDCLASSEX wnd_cls;
	ZeroMemory (&wnd_cls, sizeof (WNDCLASSEX));
	wnd_cls.cbSize			= sizeof (WNDCLASSEX);
	wnd_cls.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wnd_cls.lpfnWndProc		= (WNDPROC)(wnd_proc);
	wnd_cls.hInstance		= hInstance;
	wnd_cls.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);
	wnd_cls.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wnd_cls.lpszClassName	= cls_name;
	wnd_cls.hIcon			= LoadIcon(hInstance,(char *)IDI_ICON);
	if (RegisterClassEx (&wnd_cls) == 0)
		return false;
	return true;
}

bool wnd_create_gl_wnd( const HINSTANCE & hInstance, const char * cls_name,
					    HWND & hWnd, HDC & hDC, HGLRC & hRC,
						const int width, const int height, const int bpp, bool & full_scr,
						const char * app_title, bool & is_visible)
{
	wnd_width = width;
	wnd_height = height;
	wnd_bpp = bpp;
	RECT wnd_rect = {0,0,width,height};

	//windowed values
	DWORD wnd_style = WS_OVERLAPPEDWINDOW;
	DWORD wnd_ex_style = WS_EX_APPWINDOW;

	//Setup for wnd creation
	if(full_scr)
	{
		if(wnd_set_flscr())
		{
			ShowCursor(FALSE);
			wnd_style = WS_POPUP;
			wnd_ex_style |= WS_EX_TOPMOST;
		}
		else
		{
			MessageBox(NULL,"Fullscreen didn't work. \n Will try to load windowed mode. \n  Not sure why it broke","Haiku Error Message", MB_OK | MB_ICONEXCLAMATION);
			full_scr = false;
		}
	}
	else
		AdjustWindowRectEx(&wnd_rect, wnd_style, 0, wnd_ex_style);

	//Create window
	hWnd = CreateWindowEx(wnd_ex_style,
						  cls_name,
						  app_title,
						  wnd_style,
						  0,0,
						  wnd_rect.right - wnd_rect.left,
						  wnd_rect.bottom - wnd_rect.top,
						  HWND_DESKTOP,
						  0,
						  hInstance,
						  NULL);
	if(!hWnd)
		return false;
	hDC = GetDC(hWnd);
	if(!hDC)
	{
		DestroyWindow(hWnd);
		hWnd = 0;
		return false;
	}

	//Select Pixel Format
	PIXELFORMATDESCRIPTOR pfd = gl_get_pfd(bpp);
	GLuint pf = ChoosePixelFormat(hDC, &pfd);
	if(!pf)
	{
		wnd_kill_wnd(hWnd,hDC);
		return false;
	}

	if(SetPixelFormat(hDC,pf, &pfd) == FALSE)
	{
		wnd_kill_wnd(hWnd,hDC);
		return false;
	}

	if(!gl_create_wnd(hWnd,hDC,hRC))
	{
		wnd_kill_wnd(hWnd,hDC);
		return false;
	}

	ShowWindow(hWnd, SW_NORMAL);
	is_visible = true;
	wnd_hWnd = hWnd;
	ZeroMemory(keys, sizeof(bool) * NUM_KEYS);

	gl_resize_wnd((GLsizei)width,(GLsizei)height);	
	
	return true;
}

void wnd_destroy_gl_wnd(HWND & hWnd, HDC & hDC, HGLRC & hRC,const bool & full_scr)
{
	gl_destroy_wnd( hWnd, hDC, hRC);
	wnd_kill_wnd( hWnd, hDC);
	ChangeDisplaySettings(NULL,0);
	if(full_scr)
		ShowCursor(TRUE);	
}

void wnd_post_quit(void)
{
	PostMessage(wnd_hWnd,WM_QUIT,0,0);
}
void wnd_post_tgl_full_scr(void)
{
	PostMessage(wnd_hWnd,WM_TGL_FULL_SCR,0,0);
}

bool wnd_set_key_press(int i, bool val)
{
	if((i >= 0) && (i < NUM_KEYS))
	{
		keys[i] = val;
		return true;
	}
	return false;
}
bool wnd_get_key_press(int i)
{
	if((i >= 0) && (i < NUM_KEYS))
		return keys[i];
	return false;
}

void wnd_set_mouse_x(int x)
{
	mouse_x = x;
}
void wnd_set_mouse_y(int y)
{
	mouse_y = y;
}
int wnd_get_mouse_x(void)
{
	return mouse_x;
}
int wnd_get_mouse_y(void)
{
	return mouse_y;
}

//Accessors
void wnd_set_scr_width(int width)
{
	wnd_width = width;
}
void wnd_set_scr_height(int height)
{
	wnd_height = height;
}
int wnd_get_scr_width(void)
{
	return wnd_width;
}
int wnd_get_scr_height(void)
{
	return wnd_height;
}
int wnd_get_bpp(void)
{
	return wnd_bpp;
}

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

bool wnd_set_flscr(void)
{
	DEVMODE dm_scr;
	ZeroMemory(&dm_scr, sizeof(DEVMODE));

	dm_scr.dmSize = sizeof(DEVMODE);
	dm_scr.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	
	dm_scr.dmPelsWidth = wnd_width;
	dm_scr.dmPelsHeight = wnd_height;
	dm_scr.dmBitsPerPel = wnd_bpp;

	if(ChangeDisplaySettings(&dm_scr,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		return false;

	return true;
}

void wnd_kill_wnd(HWND & hWnd, HDC & hDC)
{
	ReleaseDC(hWnd,hDC);
	hDC = 0;
	DestroyWindow(hWnd);
	hWnd = 0;
}

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//win_wrap.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
