/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: potentialFunc.cpp
 *    Desc: code showing off potential functions.  Only uses math3d,
 *          does not use the game lib (uses GDI)
 * (C) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "..\math3d\point3.h"
#include <vector>
#include <list>
using namespace std;

#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;					// current instance
HWND g_hWnd;
TCHAR szTitle[MAX_LOADSTRING];		// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];// The title bar text

float g_virtWndWidth = 100.f;
float g_virtWndHeight = 100.f;
float g_creatureSpeed = 0.08f;

point3 g_goalLoc( 80, 80, 0 );

point3 g_mouseLoc;

class cObstacle
{
public:
	point3 m_loc;
	float m_rad;

	cObstacle( point3 loc, float rad ) 
	: m_loc( loc )
	, m_rad( rad )
	{
	}

	void Draw( HDC hdc )
	{
		static HPEN redPen = CreatePen( PS_SOLID, 2, RGB(0xff,00,00) );
		static LOGBRUSH redLog = { BS_SOLID, RGB(0xff,00,00), 0 };
		static HBRUSH redBrush = CreateBrushIndirect( &redLog );

		SelectObject(hdc, redPen );
		SelectObject(hdc, redBrush );

		RECT rc;
		GetClientRect( g_hWnd, &rc );

		// draw ourselves.
		int x0, y0;

		float xScale = rc.right / g_virtWndWidth;
		float yScale = rc.bottom / g_virtWndHeight;

		x0 = (int)(xScale * m_loc.x);
		y0 = (int)(yScale * m_loc.y);

		int xRadius = (int)(m_rad * xScale );
		int yRadius = (int)(m_rad * yScale );

		Ellipse( hdc, x0 - xRadius, y0 - yRadius, x0 + xRadius, y0 + yRadius );


	}
};
vector<cObstacle> g_obstacles;

class cCreature
{
public:
	point3 m_loc;
	float m_rad;

	HPEN m_pen;
	char m_r, m_g, m_b;

	cCreature( point3 loc, float rad ) 
	: m_loc( loc )
	, m_rad( rad )
	{
		m_pen = CreatePen( PS_SOLID, 2,  RGB(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF) );
	}
	~cCreature()
	{
		DeleteObject( m_pen );
	}
	cCreature( const cCreature &in )
	{
		m_loc = in.m_loc;
		m_rad = in.m_rad;
		m_pen = CreatePen( PS_SOLID, 2,  RGB(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF) );
	}
	cCreature& operator=( const cCreature &in )
	{
		m_loc = in.m_loc;
		m_rad = in.m_rad;
		m_pen = in.m_pen;
		return *this;
	}

	void Draw( HDC hdc )
	{
		static LOGBRUSH whiteLog = { BS_SOLID, RGB(0xff,0xff,0xff), 0 };
		static HBRUSH whiteBrush = CreateBrushIndirect( &whiteLog );

		RECT rc;
		GetClientRect( g_hWnd, &rc );

		// draw ourselves.
		int x0, y0;

		float xScale = rc.right / g_virtWndWidth;
		float yScale = rc.bottom / g_virtWndHeight;

		x0 = (int)(xScale * m_loc.x);
		y0 = (int)(yScale * m_loc.y);

		int xRadius = (int)(m_rad * xScale );
		int yRadius = (int)(m_rad * yScale );

		SelectObject(hdc, m_pen );
		SelectObject(hdc, whiteBrush );
		Ellipse( hdc, x0 - xRadius, y0 - yRadius, x0 + xRadius, y0 + yRadius );
	}

	bool Process()
	{

		point3 goalVec = g_goalLoc - m_loc;

		if( goalVec.Mag() < g_creatureSpeed )
			return false; // we reached the goal, destroy ourselves

		point3 dirVec = goalVec / goalVec.Mag();

		float k = .1f; 

		// for each obstacle
		for( int i=0; i<g_obstacles.size(); i++ )
		{
			// find the vector between the creature and the obstacle
			point3 obstacleVec = m_loc - g_obstacles[i].m_loc;

			// compute the Mag
			float dist = obstacleVec.Mag() - g_obstacles[i].m_rad - m_rad;

			// this is the vector pointing away from the obstacle
			obstacleVec.Normalize(); 

			dirVec += obstacleVec * ( k / (dist * dist) );
		}
		dirVec.Normalize();

		m_loc += g_creatureSpeed * dirVec;
		return true; // we should continue processing
	}
};

list<cCreature> g_creatures;


void DrawGoal( HDC hdc )
{
	static HPEN greenPen = CreatePen( PS_SOLID, 2,  RGB(00, 0xff,00) );
	static LOGBRUSH greenLog = { BS_SOLID, RGB(0,0xff,0), 0 };
	static HBRUSH greenBrush = CreateBrushIndirect( &greenLog );

	SelectObject(hdc, greenPen );
	SelectObject(hdc, greenBrush );

	RECT rc;
	GetClientRect( g_hWnd, &rc );

	// draw ourselves.
	int x0, y0;

	float xScale = rc.right / g_virtWndWidth;
	float yScale = rc.bottom / g_virtWndHeight;

	x0 = (int)(xScale * g_goalLoc.x);
	y0 = (int)(yScale * g_goalLoc.y);

	int xRadius = (int)(3.f * xScale );
	int yRadius = (int)(3.f * yScale );

	Ellipse( hdc, x0 - xRadius, y0 - yRadius, x0 + xRadius, y0 + yRadius );
}


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
	LoadString(hInstance, IDC_POTENTIALFUNC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_POTENTIALFUNC);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_POTENTIALFUNC);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_POTENTIALFUNC;
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
      40, 40, 400, 400, NULL, NULL, hInstance, NULL);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	static UINT uTimer; /* timer identifier */      


	switch (message) 
	{
	case WM_CREATE:
		uTimer = SetTimer(hWnd, 1, 33, NULL);   
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
	case WM_TIMER:  
		{
			list<cCreature>::iterator iter;
			for( iter = g_creatures.begin(); iter != g_creatures.end(); iter++ )
			{
				for( int i=0; i<10; i++ )
				{
					if( !(*iter).Process() )
					{
						iter = g_creatures.erase( iter );
						break;
					}
				}
			}
			InvalidateRect( hWnd, NULL, FALSE );
		}
		break;

    case WM_DESTROY:             
		if (uTimer) 
            KillTimer(hWnd, uTimer);             
		PostQuitMessage(0);
		break;


	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			vector<cObstacle>::iterator iter;
			for( iter = g_obstacles.begin(); iter < g_obstacles.end(); iter++ )
			{
				(*iter).Draw( hdc );
			}
			list<cCreature>::iterator iter2;
			for( iter2 = g_creatures.begin(); iter2 != g_creatures.end(); iter2++ )
			{
				(*iter2).Draw( hdc );
			}


			DrawGoal( hdc );
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_MOUSEMOVE:
		{
			RECT rc;
			GetClientRect( g_hWnd, &rc );

			float xScale = rc.right / g_virtWndWidth;
			float yScale = rc.bottom / g_virtWndHeight;

			g_mouseLoc.x = LOWORD(lParam) / xScale;
			g_mouseLoc.y = HIWORD(lParam) / yScale;
			g_mouseLoc.z = 0;
		}
		break;
	case WM_KEYUP:
		switch( (int) wParam )
		{
		case 'Z':
			g_obstacles.push_back( cObstacle( g_mouseLoc, 7.0 ) );
			InvalidateRect( hWnd, NULL, FALSE );
			break;
		case 'X':
			g_obstacles.push_back( cObstacle( g_mouseLoc, 5.0 ) );
			InvalidateRect( hWnd, NULL, FALSE );
			break;
		case 'C':
			g_obstacles.push_back( cObstacle( g_mouseLoc, 3.0 ) );
			InvalidateRect( hWnd, NULL, FALSE );
			break;
		case ' ':
			g_creatures.push_back( cCreature( g_mouseLoc, 2.f ) );
			InvalidateRect( hWnd, NULL, FALSE );
			break;

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
