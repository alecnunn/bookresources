/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
#include <windows.h>     // we always need to include the windows header file
#include <commdlg.h>	// common dialog boxes

#include "main.h"
#include "tgen.h"

// define the name of our Window Class
static const char szWndClassName[]={ "TEGEN" };

// Global variables
// Note:  use globals only for constants which are valid thoughout
//        the whole time of program execution

HINSTANCE hInstance;     // Global Handle to Program Instance
HWND      hAppWnd;       // Global Handle to Application Window

// Prototypes of exported functions called by Windows

LRESULT FAR PASCAL AppWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL FnHelpAbout(HWND hWndAbout,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL FAR PASCAL FnTerrainProperties(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

// Internal Function Prototoyes

static BOOL RegisterWindowClasses(HINSTANCE hFirstInstance);
long GetFileLength(HFILE hFile);
void MakeMetaFile(HWND hWnd);

OPENFILENAME oOFN;
HGLOBAL hGlobal;
LPVOID hGlobalLock;

FARPROC lpfnAboutDlgProc, lpfnTerrainPropertiesProc;

// Local varables
// e.g. static char szFileName[120];

TerrainGenerator * the_terrain;
char szAppTitle[] = "Terrain Generator 1.0";


//***********************************************************************/

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{ MSG  msg;

  // Store Instance Handle in the global variable hInstance
  hInstance=hInst;

  // If there is no Instance of our program in memory then register
  // the window class
  if (hPrevInst == NULL)
      if (!RegisterWindowClasses(hInstance))
           return FALSE;  // registration failed!

  // Create our Main Application Window
  
hAppWnd=CreateWindow(szWndClassName,szAppTitle, // Class Name and Window Title
					 WS_OVERLAPPEDWINDOW, // Style of Window
					 CW_USEDEFAULT,CW_USEDEFAULT,400,300,       // Position and Size of Window
					 NULL,NULL,hInstance,					    // Handle of Parent Window, Menu and Instance
					 NULL // User data passed to WM_CREATE
					 );
if (!hAppWnd) return FALSE; // Create Window failed

  // Show the Window as specified in the nCmdShow parameter
  ShowWindow(hAppWnd,nCmdShow);
  UpdateWindow(hAppWnd);

  // Enter the Message Loop
  while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); /* translates virtual key codes    */
        DispatchMessage(&msg);  /* dispatches message to window    */
    }

  // Program was terminated
  return (int) msg.wParam;    /* return value of PostQuitMessage */
}
//***********************************************************************/

//***********************************************************************/

long GetFileLength(HFILE hFile)
{
	long lLength;

	lLength =  _llseek(
    hFile,	// handle to file 
    0L,	// number of bytes to move  
    2 	// position to move from 
   );

	_llseek (hFile,0L,1);
	return lLength;
}

//***********************************************************************/

static BOOL RegisterWindowClasses(HINSTANCE hFirstInstance)
{ WNDCLASS wc;

  // Register the window class.
  wc.lpszClassName = szWndClassName;  // Name of the Window Class
  wc.hInstance     = hFirstInstance;  // Handle of program instance
  wc.style         = CS_HREDRAW|CS_VREDRAW;  // Combination of Class Styles
  wc.lpfnWndProc   = AppWndProc; // Adress of Window Procedure
  wc.cbClsExtra    = 0;   // Extra Bytes allocated for this Class
  wc.cbWndExtra    = 0;   // Extra Bytes allocated for each Window
  wc.hIcon         = LoadIcon(hFirstInstance, MAKEINTRESOURCE(ICON_APPWND));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName  = "main_menu";

  if (!RegisterClass(&wc)) return FALSE;  // Register Class failed

  // Register other Window Classes if necessary
  // ....

  // All Window Classes Registered successfully
  return TRUE;
}
//***********************************************************************/

LRESULT FAR PASCAL AppWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ // this is where we receive all message concerning this window
  // we can either process a message or pass it on to the default
  // message handler of windows

  static short cxChar, cyChar; // Character size based on default font
  static RECT rcClient;
  static HDC hDCSmooth;

	switch(msg)
	{
		case WM_CREATE:
		// Initialize Window
		// this message is received before the window is displayed
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		// Re-size window to 80x25
		MoveWindow(hWnd, 0,0,cxChar * 81, cyChar * 28, TRUE);
		// Create the main editor
		lpfnAboutDlgProc = MakeProcInstance ((FARPROC) FnHelpAbout,
			  hInstance) ;
		lpfnTerrainPropertiesProc = MakeProcInstance ((FARPROC) FnTerrainProperties,
			  hInstance) ;

		GetClientRect(hWnd,&rcClient);
		the_terrain = new TerrainGenerator;
		the_terrain->SetSeed(1);
		the_terrain->SetMaximumHeight(255);
      the_terrain->SetQuadrantSize(1);
      the_terrain->BuildBlankTerrain();
	  the_terrain->SetWaterLevel(20);
	  the_terrain->SetPlainsLevel(45);
	  the_terrain->SetHillsLevel(30);
	  the_terrain->SetMountainsLevel(5);
	  the_terrain->SetDimension(80);
		break;

		case WM_SIZE:
		{
      	GetClientRect(hWnd,&rcClient);
		}
		break;

	  case WM_PAINT:
          { PAINTSTRUCT ps;
				HDC hdc;

            hdc=BeginPaint(hWnd,&ps);
				 // Now Paint the Windows's contents

             the_terrain->DisplayTerrain(hdc,&rcClient);
            
            // Call EndPaint to release the DC and validate the client area
            EndPaint(hWnd,&ps);
          }
          break;

     case WM_DESTROY:
          PostQuitMessage(0);  // Terminate Application
          break;
	
	 case WM_COMMAND:
		 // Menu, button, or something else
		 switch (wParam)
		 {
		 case IDM_QUIT:
			 PostQuitMessage(0); // Exit
			 break;

		 case IDM_QUAD_AVE:
			 the_terrain->SmoothTerrain(2);
			 hDCSmooth = GetDC(hWnd);
			 SelectObject(hDCSmooth,GetStockObject(WHITE_PEN));
			 SelectObject(hDCSmooth,GetStockObject(WHITE_BRUSH));
			 Rectangle(hDCSmooth,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
          the_terrain->DisplayTerrain(hDCSmooth,&rcClient);
          ReleaseDC(hWnd,hDCSmooth);
          break;

		  case IDM_QUAD:
			 the_terrain->QuadTerrain(the_terrain->GetQuadrantSize());
			 hDCSmooth = GetDC(hWnd);
			 SelectObject(hDCSmooth,GetStockObject(WHITE_PEN));
			 SelectObject(hDCSmooth,GetStockObject(WHITE_BRUSH));
			 Rectangle(hDCSmooth,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
          the_terrain->DisplayTerrain(hDCSmooth,&rcClient);
          ReleaseDC(hWnd,hDCSmooth);
          break;

		 case IDM_FUZZY:
			the_terrain->BuildFuzzyTerrain();
			hDCSmooth = GetDC(hWnd);
			 SelectObject(hDCSmooth,GetStockObject(WHITE_PEN));
			 SelectObject(hDCSmooth,GetStockObject(WHITE_BRUSH));
			 Rectangle(hDCSmooth,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
			 the_terrain->DisplayTerrain(hDCSmooth,&rcClient);
			 ReleaseDC(hWnd,hDCSmooth);
			break;

		 case IDM_FAULTLINES:
			the_terrain->BuildFaultLines(the_terrain->GetDimension() / 2);
			 hDCSmooth = GetDC(hWnd);
			 SelectObject(hDCSmooth,GetStockObject(WHITE_PEN));
			 SelectObject(hDCSmooth,GetStockObject(WHITE_BRUSH));
			 Rectangle(hDCSmooth,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
			 the_terrain->DisplayTerrain(hDCSmooth,&rcClient);
			 ReleaseDC(hWnd,hDCSmooth);
			break;

		 case IDM_HELP_ABOUT:
			 DialogBox (hInstance, "about_dlg", hWnd, FnHelpAbout) ;
			 break;

		 case IDM_TERRAIN_PROPERTIES:
			 DialogBox (hInstance, "TerrainProperties", hWnd, FnTerrainProperties) ;
			 hDCSmooth = GetDC(hWnd);
			 SelectObject(hDCSmooth,GetStockObject(WHITE_PEN));
			 SelectObject(hDCSmooth,GetStockObject(WHITE_BRUSH));
			 Rectangle(hDCSmooth,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
			 the_terrain->DisplayTerrain(hDCSmooth,&rcClient);
			 ReleaseDC(hWnd,hDCSmooth);
		 break;

		 case IDM_METAFILE:
			 MakeMetaFile(hWnd);
			 break;
		 }

     default:
          // We didn't process the message so let Windows do it
          return DefWindowProc(hWnd,msg,wParam,lParam);
   }
  // We processed the message and there
  // is no processing by Windows necessary
  return 0L;
}
//***********************************************************************/

BOOL FAR PASCAL FnHelpAbout(HWND hWndAbout,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{	
	case WM_INITDIALOG:
		return TRUE ;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDD_OK:
			EndDialog(hWndAbout,0);
			return TRUE;
		}
		break;

	}
	return FALSE;
}

BOOL FAR PASCAL FnTerrainProperties(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{	
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg,IDC_WATER,the_terrain->GetWaterLevel(),FALSE);
		SetDlgItemInt(hDlg,IDC_PLAINS,the_terrain->GetPlainsLevel(),FALSE);
		SetDlgItemInt(hDlg,IDC_HILLS,the_terrain->GetHillsLevel(),FALSE);
		SetDlgItemInt(hDlg,IDC_MOUNTAINS,the_terrain->GetMountainsLevel(),FALSE);
		SetDlgItemInt(hDlg,IDC_SEED,the_terrain->GetSeed(),FALSE);
		SetDlgItemInt(hDlg,IDC_MAXHEIGHT,the_terrain->GetMaximumHeight(),FALSE);
		SetDlgItemInt(hDlg,IDC_QUADSIZE,the_terrain->GetQuadrantSize(),FALSE);
		return TRUE ;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			the_terrain->SetWaterLevel(GetDlgItemInt(hDlg,IDC_WATER,NULL,FALSE));
			the_terrain->SetPlainsLevel(GetDlgItemInt(hDlg,IDC_PLAINS,NULL,FALSE));
			the_terrain->SetHillsLevel(GetDlgItemInt(hDlg,IDC_HILLS,NULL,FALSE));
			the_terrain->SetMountainsLevel(GetDlgItemInt(hDlg,IDC_MOUNTAINS,NULL,FALSE));
			the_terrain->SetSeed(GetDlgItemInt(hDlg,IDC_SEED,NULL,FALSE));
			the_terrain->SetMaximumHeight(GetDlgItemInt(hDlg,IDC_MAXHEIGHT,NULL,FALSE));
			the_terrain->SetQuadrantSize(GetDlgItemInt(hDlg,IDC_QUADSIZE,NULL,FALSE));
			// Validation
			if ((the_terrain->GetWaterLevel()+the_terrain->GetPlainsLevel()+
				the_terrain->GetHillsLevel()+the_terrain->GetMountainsLevel()) == 100)
			{
				EndDialog(hDlg,0);
			}
			else
			{
				MessageBox(hDlg,"Error: The Topography seetings\n must add up to 100!",szAppTitle,MB_OK|MB_ICONSTOP);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
		}
		break;

	}
	return FALSE;
}

void MakeMetaFile(HWND hWnd)
{
	HDC hdcRef, hdcMeta;
	int iWidthMM, iHeightMM, iWidthPels, iHeightPels;
	RECT rect;
	static OPENFILENAME Ofn;
	char szFilter[] = "MetaFiles\0*.EMF\0All Files\0*.*\0\0";
	char szTitle[255];
	char szFile[1024];
	char szFileTitle[255];
	char szDescription[255];
	HENHMETAFILE hemf;
	int nOldMapMode;

	/* Obtain a handle to a reference DC. */  
	hdcRef = GetDC(hWnd); 
 
	/* 
	* Determine the picture frame dimensions. 
	* iWidthMM is the display width in millimeters. 
	* iHeightMM is the display height in millimeters. 
	* iWidthPels is the display width in pixels. 
	* iHeightPels is the display height in pixels 
	*/ 
 
	iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE); 
	iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE); 
	iWidthPels = GetDeviceCaps(hdcRef, HORZRES); 
	iHeightPels = GetDeviceCaps(hdcRef, VERTRES); 
 
	/* 
	* Retrieve the coordinates of the client 
	* rectangle, in pixels. 
	*/ 
 		
	GetClientRect(hWnd, (LPRECT)&rect); 
	 
	/* 
	* Convert client coordinates to .01-mm units. 
	* Use iWidthMM, iWidthPels, iHeightMM, and 
	* iHeightPels to determine the number of 
	* .01-millimeter units per pixel in the x- 
	*  and y-directions. 
	*/ 
 
	rect.left = (rect.left * iWidthMM * 100)/iWidthPels; 
	rect.top = (rect.top * iHeightMM * 100)/iHeightPels; 
	rect.right = (rect.right * iWidthMM * 100)/iWidthPels; 
	rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels;
 
	/* Load the filename filter from the string table. */ 
 		
	wsprintf(szTitle,"Save MetaFile");
	wsprintf(szFileTitle,"\0");
	wsprintf(szFile,"\0");
 
	/* Initialize the OPENFILENAME members. */ 
 
	szFile[0] = '\0'; 
 
	Ofn.lStructSize = sizeof(OPENFILENAME); 
	Ofn.hwndOwner = hWnd; 
	Ofn.lpstrFilter = szFilter; 
	Ofn.lpstrFile= szFile; 
	Ofn.nMaxFile = sizeof(szFile); 
	Ofn.lpstrFileTitle = szFileTitle; 
	Ofn.nMaxFileTitle = sizeof(szFileTitle); 
	Ofn.lpstrInitialDir = (LPSTR)NULL; 
	Ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
	Ofn.nFilterIndex = 0;
	Ofn.lpstrCustomFilter = NULL;
	Ofn.lpstrTitle = "Create MetaFile"; 
 
	/* 
	* Display the Filename common dialog box. The 
	* filename specified by the user is passed 
	* to the CreateEnhMetaFile function and used to 
	* store the metafile on disk. 
	*/ 
 
	GetSaveFileName(&Ofn); 
 
	wsprintf(szDescription,"%s\0Terrain Sample\0\0",szAppTitle);
	
	/* Create the metafile DC. */ 
 
	hdcMeta = CreateEnhMetaFile(hdcRef, 
          (LPTSTR) Ofn.lpstrFile, 
          &rect, (LPSTR)szDescription); 
 
	if (!hdcMeta) 
	{
		MessageBox(hWnd,"Error : Unable to create MetaFile.",szAppTitle,MB_ICONSTOP|MB_OK);
		return;
	}
	
	SetMapMode(hdcMeta,MM_ANISOTROPIC);
	SetWindowExtEx(hdcMeta,rect.right,rect.left,NULL);
	GetClientRect(hWnd,&rect);
	SetViewportExtEx(hdcMeta,rect.right,rect.left,NULL);
	
	the_terrain->DisplayTerrain(hdcMeta,&rect);
	
	hemf = CloseEnhMetaFile(hdcMeta);
	DeleteEnhMetaFile(hemf);

	/* Release the reference DC. */ 
 		
	SetMapMode(hdcRef,nOldMapMode);
	ReleaseDC(hWnd, hdcRef);
	
}
