// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// builds the Win32 application (based on WinMain())

#include <time.h>
#include "Metropolis.hpp"
#include "MetropolisRenderer.hpp"
#include "KDTree.hpp"
#include <windows.h>
#include <commdlg.h>
#include "resource.h"

// Global Variables
HINSTANCE	hInst		= NULL;			// current instance of the appplication
HWND		gHwnd		= NULL;			// the identifier of the window
HDC			gMemoryDC	= NULL;			// a memory device context contains the rendered image
TCHAR		szTitle[128];				// the title bar text
TCHAR		szWindowClass[128];			// the main window class name
short		gProgress	= 0;			// progression percent of the rendering (0..100)
long		gSamplePerPixel = 250;		// approximated number of rays per pixel of the Metropolis Ligth Transport

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE hInstance);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
void				LoadFile		(void);

// entry point of the MsWindows application
//-----------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, 128);
	LoadString(hInstance, IDC_METROPOLIS, szWindowClass, 128);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_METROPOLIS);
	// Main message loop
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

// register the Windows window type
//-----------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_METROPOLIS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_METROPOLIS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	return RegisterClassEx(&wcex);
}

// create an istance of the Window class 
//-----------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT rect = {0.0, 0.0, DefaultScreenHeight, DefaultScreenWidth};
   AdjustWindowRectEx (&rect, WS_OVERLAPPEDWINDOW, true, 0);

   gHwnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
   if (!gHwnd)
      return FALSE;

   ShowWindow(gHwnd, nCmdShow);
   UpdateWindow(gHwnd);
   return TRUE;
}

// event processing function
//-----------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_OPEN:
			LoadFile();
			break;
		case ID_CONTROL_SETSAMPLEPERPIXEL1:
			gSamplePerPixel = 30;
			break;
		case ID_CONTROL_SETSAMPLEPERPIXEL2:
			gSamplePerPixel = 250;
			break;
		case ID_CONTROL_SETSAMPLEPERPIXEL3:
			gSamplePerPixel = 1000;
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (gProgress >= 100) {	// rendering is finished (100%) --> draw the rendered image (gMemoryDC)
			RECT rect;
			GetClientRect (hWnd, &rect);
			if (gMemoryDC != NULL)
				long error = StretchBlt (hdc, 0, 0, rect.right, rect.bottom, gMemoryDC, 0, 0, DefaultScreenWidth, DefaultScreenHeight, SRCCOPY);
			EndPaint(hWnd, &ps);
		} else {			// if rendering is not finished --> draw progress percent
			char s[64];
			sprintf (s, "Progress: %d percent", gProgress);
			RECT rect = {0, 0, DefaultScreenWidth, DefaultScreenHeight};
			DrawText(hdc, s, -1, &rect, DT_CENTER);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// message handler for about box
//-----------------------------------------------------------------
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// callback function of the File/Load menu
//-----------------------------------------------------------------
void LoadFile(void)
{
	char szFile[260]	= "";    // buffer for file name
	OPENFILENAME ofn;			 // common dialog box structure

	// 0. initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= NULL;			// owner window
	ofn.lpstrFile		= szFile;
	ofn.nMaxFile		= sizeof(szFile);
	ofn.lpstrFilter		= "Sce Files\0*.sce\0All\0*.*\0";
	ofn.nFilterIndex	= 1;
	ofn.lpstrFileTitle	= NULL;
	ofn.nMaxFileTitle	= 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&ofn))	// Display the Open dialog box. 
		return;

	// 1. cread the scene
	Scene* scene = new Scene();
	scene->Read(szFile);
	scene->kdtree = new KDTree(&(scene->objects[0]), scene->objects.size());

	// 2. create gMemoryDC that contains the rendered image
	RECT rect;
	GetClientRect (gHwnd, &rect);
	scene->camera.vres	 = rect.bottom - rect.top;
	scene->camera.hres	 = rect.right - rect.left;
	scene->camera.CompleteCamera ();

	HDC hdc = GetDC(gHwnd); 
	gMemoryDC = CreateCompatibleDC (hdc);
	HBITMAP hbmScreen = CreateCompatibleBitmap(hdc, scene->camera.hres, scene->camera.vres); 
	if (hbmScreen == NULL) 
		return; 
	if (!SelectObject(gMemoryDC, hbmScreen))  // Select the bitmaps into the compatible DC. 
		return; 
	ReleaseDC (gHwnd, hdc);

	Image image(scene->camera.hres, scene->camera.vres);

	// 3. Render
	gProgress	= 0;
	long start	= clock();
	MetropolisRenderer renderer(scene, gSamplePerPixel, &image);
	if (!renderer.RenderMLT())
		throw "Error in rendering.";
	
	gProgress	= 100;
	long finish = clock();
	
	// 4. convert image radiances to pixel color
	double avgPixelLum = image.GetAvgPixelLum();
	for (short x = 0; x < scene->camera.hres; x++) {
		for (short y = 0; y < scene->camera.vres; y++) {
			Color col = image.GetPixelRad(x, y) / avgPixelLum / 2.6;
			SetPixel (gMemoryDC, x, y, RGB(min(255*col.r ,255), min(255*col.g,255), min(255*col.b,255)));
		}
	}
	
	double duration = (double)(finish - start) / 1000;
	char s[256];
	sprintf(s, "Rendering time is %3.3f seconds\n", duration);
	::MessageBox(NULL, s, "Time", MB_OK);

	delete scene->kdtree;
	delete scene;
	RedrawWindow(gHwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}