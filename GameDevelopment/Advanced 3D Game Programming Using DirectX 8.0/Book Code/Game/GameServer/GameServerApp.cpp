// GameServer2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "GameServer.h"

#include <string>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
HWND g_ecWnd;
HWND g_lbWnd; // list box window
UINT g_wTimer;

cGameServer g_server;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Dialog(HWND, UINT, WPARAM, LPARAM);

bool bServerStarted = false;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	/**
	 * Ahhh... Now this is the kind of Win32 app I like.
	 */
	DialogBox(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)Dialog);

	return 0;
}



void AddText( char* text )
{
	static char buff[65536];

	SendMessage( g_ecWnd, WM_GETTEXT, 65536, (long)buff );

	string out = string(buff) + string("\r\n") + string(text);

	SendMessage( g_ecWnd, WM_SETTEXT, 0, (long)out.c_str() );

	int numLines = SendMessage( g_ecWnd, EM_GETLINECOUNT, 0, 0 );

	//SendMessage( g_ecWnd, EM_LINEINDEX, numLines, 0 );
	SendMessage( g_ecWnd, EM_LINESCROLL, 0, 100 );
}


/**
 * Edit-box printf.
 */
void EbPrintf( char* first, ... )
{
	char buff[1024];

	va_list marker;

	va_start( marker, first );     /* Initialize variable arguments. */

	vsprintf( buff, first, marker );

	va_end( marker );              /* Reset variable arguments.      */

	AddText( buff );
}


// Mesage handler for about box.
LRESULT CALLBACK Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			// Get the HWND for the edit box.
			g_ecWnd = GetDlgItem( hDlg, IDC_EDIT1 );
			g_lbWnd = GetDlgItem( hDlg, IDC_LIST );
			SendMessage( g_ecWnd, EM_LIMITTEXT, 0, 65536 );

			g_wTimer = 0;

			SendMessage( g_ecWnd, WM_SETTEXT, 0, (long)"Mobots ATTACK!\r\nServer Ver 0.5b\r\n==============" );

			SetWindowPos(
				hDlg, HWND_TOP, 
				700, 10, 0, 0, SWP_NOSIZE );

			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			if (LOWORD(wParam) == IDSTART ) 
			{
				if( !bServerStarted )
				{
					g_server.Initialize();
					bServerStarted = true;
					g_wTimer = SetTimer( hDlg, 0, 1000/40, NULL );
				}
			}
			break;

		case WM_TIMER:
			g_server.Tick();
			break;

		case WM_DESTROY:
			if( g_wTimer )
				KillTimer( hDlg, g_wTimer );
			break;
	}
    return FALSE;
}

void AddPlayer( char* playerName )
{
	SendMessage( g_lbWnd, LB_ADDSTRING, 0, (long)playerName);
}

void RemovePlayer( char* playerName )
{
	int ind = SendMessage( g_lbWnd, LB_FINDSTRING, 0, (long)playerName);

	if( ind != LB_ERR )
	{
		SendMessage( g_lbWnd, LB_DELETESTRING, ind, 0);
	}
}