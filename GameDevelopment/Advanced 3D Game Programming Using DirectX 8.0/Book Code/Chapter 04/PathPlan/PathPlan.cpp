/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: PathPlan.cpp
 *    Desc: main-loop code for the path-planning sample
 *          
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include <vector>
#include <list>
using namespace std;

#include "cell.h"
#include "resource.h"

cWorld* g_pWorld;
cCreature g_creature;
int xCenter, yCenter;
float scale;
HWND g_hWnd;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PATHPLAN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PATHPLAN);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PATHPLAN);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_PATHPLAN;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      40, 40, 500, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static UINT uTimer; /* timer identifier */      

	switch (message) 
	{
	case WM_CREATE:
		{
			g_creature.m_loc = point2(0.f,0.f);
			g_creature.m_bFollowingPath = false;
			uTimer = SetTimer(hWnd, 1, 50, NULL);   
			g_pWorld = new cWorld( "media\\rooms.wld" );
			g_hWnd = hWnd;
		}
		break;
	case WM_TIMER:  
		{
			InvalidateRect( hWnd, NULL, FALSE );
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_ABOUT:
			   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			   break;
			case IDM_EXIT:
			   DestroyWindow(hWnd);
			   break;
			default:
			   return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);  

			// create a node here, and make the unit go to it.
			point2 clickLoc( 
				(x - xCenter) / scale,
				(y - yCenter) / -scale );

			// 2 cases... if the user clicked inside the creature's cell, bee-line to it.

			int clickedIndex = g_pWorld->FindContainingCell( clickLoc );
			int creatureIndex = g_pWorld->FindContainingCell( g_creature.m_loc );
			if( creatureIndex < 0 ) 
				break; // creature wasn't in a cell

			cNode* clickNode = g_pWorld->AddTempNode( clickLoc );
			cNode* creatureNode = g_pWorld->AddTempNode( g_creature.m_loc );
			if( !clickNode || !creatureNode ) 
				break;

			if( clickedIndex == creatureIndex )
			{
				g_creature.m_path.Clear();
				g_creature.m_path.Add( clickNode );
				g_creature.m_bFollowingPath = true;
			}
			else
			{
				g_pWorld->ShortestPath( &g_creature.m_path, 
					clickNode, creatureNode);
				g_creature.m_bFollowingPath = true;
			}		
		}
		break;

    case WM_DESTROY:             
		if (uTimer) 
            KillTimer(hWnd, uTimer);             
		PostQuitMessage(0);
		break;

    //case WM_ERASEBKGND:             
	//	break;


	case WM_SIZE:
		{
			int nx = LOWORD(lParam);
			int ny = HIWORD(lParam); 
			xCenter = nx/2, yCenter = ny/2;
			scale = (float)min( (float)nx/20, (float)ny/20 );
			break;
		}

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(g_hWnd, &ps);
			RECT rt;
			GetClientRect(g_hWnd, &rt);

			int width = rt.right - rt.left, height = rt.bottom - rt.top;

			// since we're not letting windows erase the background, we need to do it here
			//Rectangle( hdc, rt.left-1, rt.top-1, rt.right+1, rt.bottom+1 );

			// draw the world
			g_pWorld->Draw( hdc, xCenter, yCenter, scale);

			g_creature.Erase( hdc, xCenter, yCenter, scale);
			g_creature.Walk();
			g_creature.Draw( hdc, xCenter, yCenter, scale);

			EndPaint(g_hWnd, &ps);

			//InvalidateRect( g_hWnd, NULL, false );
		}
		break;

	case WM_KEYUP:
		{
			int key = (int)wParam;

			if( key == 'Z' )
			{
				g_bDrawPortals = !g_bDrawPortals;

				if( !g_bDrawPortals )
				{
					InvalidateRect( g_hWnd, NULL, true );
				}
			}
			if( key == 'X' )
			{
				g_bDrawGraph = !g_bDrawGraph;

				if( !g_bDrawGraph )
				{
					InvalidateRect( g_hWnd, NULL, true );
				}
			}


		}
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
