//----------------------------------------------------------------------------
//
//basecode.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )


#include "gl_wrap.h"
#include "win_wrap.h"
#include "demo.h"

#include "resource.h"

//··········································································//
//    Source :: Definitions
//··········································································//

#define CLS_NAME "OpenGL"
#define APP_TITLE "Natural Effects"
#define WM_DO_TGL (WM_USER + 2)

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

static bool id_cancel = false;
static bool looping = true;
static bool is_visible = false;

static HWND hWnd;

static int width = 800;
static int height = 600;
static int bpp = 32; 
static bool fullscr = false;

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		}
	case WM_CLOSE:
		{
			wnd_post_quit();
			return 0;
		}
	case WM_SIZE:
		{
			switch(wParam)
			{
			case SIZE_MINIMIZED:
				{
					is_visible = false;
					return 0;
				}
			case SIZE_MAXIMIZED:				
			case SIZE_RESTORED:
				{
					is_visible = true;
					wnd_set_scr_width(LOWORD(lParam));
					wnd_set_scr_height(HIWORD(lParam));
					gl_resize_wnd((GLsizei)LOWORD(lParam),(GLsizei)HIWORD(lParam));
					return 0;
				}
			}
			break;
		}
	case WM_KEYUP:
		{
			if(wnd_set_key_press(wParam, false))
				return 0;
			break;
		}
	case WM_KEYDOWN:
		{
			if(wnd_set_key_press(wParam, true))
				return 0;
			break;
		}	
	case WM_TGL_FULL_SCR:
		{
			PostMessage(hWnd,WM_DO_TGL,0,0);
			break;
		}
	case WM_MOUSEMOVE:
		{
			wnd_set_mouse_x(LOWORD(lParam));
			wnd_set_mouse_y(HIWORD(lParam));
			break;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	switch(uMsg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					//fullscreen
					if(IsDlgButtonChecked(hWnd,IDC_FULLSCREEN))
						fullscr = true;
					//bpp
					if(IsDlgButtonChecked(hWnd,IDC_BPP_16))
						bpp = 16;
					else
						bpp = 32;
					//resolution
					if(IsDlgButtonChecked(hWnd,IDC_RES_800_600))
					{
						width = 800;
						height = 600;
					}
					else if(IsDlgButtonChecked(hWnd,IDC_RES_1024_768))
					{
						width = 1024;
						height = 768;
					}
					else
					{					
						width = GetDlgItemInt(hWnd,IDC_CUS_WIDTH,NULL,FALSE);
						height = GetDlgItemInt(hWnd,IDC_CUS_HEIGHT,NULL,FALSE);
					}
					EndDialog(hWnd,wParam);
					return TRUE;
				}//case IDOK:
			case IDCANCEL:
				{
					id_cancel = true;
					EndDialog(hWnd,wParam);
					return TRUE;
				}//case IDCANCEL:
			}//switch(LOWORD(wParam))

			if(HIWORD(wParam) == BN_CLICKED)
			{
				CheckDlgButton(hWnd,lParam,BST_CHECKED);
				if(IsDlgButtonChecked(hWnd,IDC_RES_CUSTOM))
				{
					HWND dlg_item;
					dlg_item = GetDlgItem(hWnd, IDC_STAT_CUSTOM);
					EnableWindow(dlg_item,TRUE);
					dlg_item = GetDlgItem(hWnd, IDC_CUS_WIDTH);
					EnableWindow(dlg_item,TRUE);
					dlg_item = GetDlgItem(hWnd, IDC_CUS_HEIGHT);
					EnableWindow(dlg_item,TRUE);
				}
				else
				{
					HWND dlg_item;
					dlg_item = GetDlgItem(hWnd, IDC_STAT_CUSTOM);
					EnableWindow(dlg_item,FALSE);
					dlg_item = GetDlgItem(hWnd, IDC_CUS_WIDTH);
					EnableWindow(dlg_item,FALSE);
					dlg_item = GetDlgItem(hWnd, IDC_CUS_HEIGHT);
					EnableWindow(dlg_item,FALSE);
				}
			}

			break;
		}//case WM_COMMAND:
	case WM_INITDIALOG:
		{
			CheckDlgButton(hWnd,IDC_RES_1024_768,BST_CHECKED);
			CheckDlgButton(hWnd,IDC_BPP_32,BST_CHECKED);
			HWND dlg_item;
			dlg_item = GetDlgItem(hWnd, IDC_STAT_CUSTOM);
			EnableWindow(dlg_item,FALSE);
			dlg_item = GetDlgItem(hWnd, IDC_CUS_WIDTH);
			EnableWindow(dlg_item,FALSE);
			dlg_item = GetDlgItem(hWnd, IDC_CUS_HEIGHT);
			EnableWindow(dlg_item,FALSE);
			break;
		}		
	}//switch(uMsg)
    
    return FALSE; 
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Gather UI
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_INIT), 
                     HWND_DESKTOP, (DLGPROC)DlgProc);

	if(id_cancel)
		return 0;
	
	//Register Class
	if(!wnd_register_cls(hInstance, CLS_NAME, WinProc))
	{
		MessageBox(NULL,"There was a problem. \n Could not register window. \n  Windows is like that...","Haiku Error Message", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	HDC hDC; 
	HGLRC hRC;
	DWORD lst_tckcnt = GetTickCount();
	DWORD tckcnt;

	bool msg_pmp;
	MSG msg;
	
	while(looping)
	{
		is_visible = false;
		if(wnd_create_gl_wnd(hInstance, CLS_NAME,
					    hWnd, hDC, hRC,
						width, height, bpp, fullscr,
						APP_TITLE, is_visible))
		{
			if(dem_init())
			{
				msg_pmp = true;
				while(msg_pmp)
				{
					//check for quit
					if(PeekMessage(&msg,hWnd,0,0,PM_REMOVE))
					{
						if(msg.message == WM_QUIT)
						{
							msg_pmp = false;
							looping = false;							
						}
						else if(msg.message == WM_DO_TGL)
						{
							msg_pmp = false;
							fullscr = !fullscr;
						}
						else
							DispatchMessage(&msg);
					}
					else
					{
						if(is_visible)
						{
							tckcnt = GetTickCount();
							dem_update((int)(tckcnt - lst_tckcnt));
							lst_tckcnt = tckcnt;

							dem_draw();
							
							SwapBuffers(hDC);
						}
						else
							WaitMessage();
					}
				}//while(msg_pmp)
			}//if(dem_init())
			else
				looping = false;

			dem_deinit();
			wnd_destroy_gl_wnd(hWnd,hDC,hRC,fullscr);
		}//if(wnd_create_gl_wnd(hInstance, CLS_NAME...
		else
		{
			MessageBox(NULL,"OpenGL broke...\n Error Creating Window. \n  Nothing to see here.","Haiku Error Message", MB_OK | MB_ICONEXCLAMATION);
			looping = false;
		}		
	}//while(looping)

	//unregister class
	UnregisterClass(CLS_NAME, hInstance);
	return 0;
}

//----------------------------------------------------------------------------
//basecode.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
