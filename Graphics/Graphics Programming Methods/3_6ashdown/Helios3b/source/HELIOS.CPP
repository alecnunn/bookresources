////////////////////////////////////////////////////////////
//
//  HELIOS.CPP - Radiosity Renderer for MS-Windows Main
//               Module
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/07 - Added CheckAbort and user-
//                         requested abort logic.
//                       - Added WM_QUERYENDSESSION and
//                         WM_CLOSE message handlers.
//                       - Added AbortCalc modeless dialog
//                         box.
//              94/09/23 - Added elapsed calculation time to
//                         Convergence Statistics message
//                         box.
//              94/09/27 - Modified IDM_SETCONVERGE case
//                         logic to prevent Rendering menu
//                         option from being enabled when
//                         display type is H_NONE.
//              94/10/02 - Made Set Camera and Specify
//                         View dialog boxes modeless.
//                       - Revised cursor display logic.
//              94/10/14 - Added ContrastError string.
//                       - Modified SetDisplay dialog box
//                         function.
//              94/11/19 - Added OutOfMemory error reporting
//                         for SynCamera::Preview,
//                         SynCamera::Shoot, WinBitmap::Open
//                         and WinBitmap::Display function
//                         call failures.
//              94/11/25 - Added CenterDialog and
//                         FileOpenHook functions.
//                       - Modified SetEntityDir, SetCamera,
//                         SetView, SetConverge, AbortCalc,
//                         SetDisplay, and About to center
//                         dialog boxes.
//              94/12/01 - Added palette flag and
//                         color resolution initialization
//                         to InitInstance.
//                       - Added color resolution logic to
//                         SetDisplay dialog box.
//              94/12/04 - Added bitmap color quantization
//                         capability to IDM_RENDER,
//                         IDM_REDISPLAY and IDD_UPD_DISPLAY
//                         case logic.
//                       - Added modeless RenderMsg dialog
//                         box.
//              94/12/06 - Added WM_QUERYPALETTE and
//                         WM_PALETTECHANGED message
//                         handlers.
//                       - Deleted <windows.h>, <stdio.h>
//                         and <math.h> include directives.
//                       - Added "general.h" include
//                         directive.
//                       - Made SetDisplay dialog box
//                         modeless.
//                       - Added check for invalid display
//                         type to InitInstance.
//                       - Added AppTitle string.
//              94/12/08 - Added IDM_HELP_CONTENTS and
//                         IDM_HELP_HELP case logic.
//                       - Added Windows Help cancel to
//                         WM_DESTROY case logic.
//                       - Added HelpFileName string.
//                       - Added "win_help.h" include
//                         directive.
//              94/12/11 - Revised FrontDistErr string.
//              94/12/16 - Version 1.01A release.
//              94/12/19 - Changed OutputFilterSpec string
//                         to support TARGA files.
//                       - Changed IDM_SAVEAS case logic to
//                         support TARGA files.
//                       - Added static WinTarga object.
//                       - Added "win_tga.h" include
//                         directive.
//              94/12/24 - Modified SetCamera to handle
//                         window scale and projection type.
//                       - Added WindowScaleError string.
//              95/02/05 - Version 1.02A release.
//              95/03/09 - Modified IDM_FILEOPEN case logic
//                         to prevent GetSaveFileName from
//                         overwriting hook function pointer.
//              95/03/26 - Revised IDM_RENDER case logic to
//                         support antialiasing.
//                       - Added hourglass cursor to
//                         IDM_FILEOPEN case logic.
//                       - Deleted RenderInProgress and
//                         QuantInProgress strings.
//                       - Added IDM_SETCOLOR case.
//                       - Added SetColor function.
//                       - Made pScroll and DispType global.
//                       - Added DisplayBitmap function.
//              95/06/26 - Modified SetCamera, SetView,
//                         SetDisplay and SetColor to
//                         update display when OK button
//                         selected.
//              95/07/16 - Revised menu item graying logic.
//                       - Added DisableMenu and EnableMenu
//                         functions.
//                       - Revised IDM_REDISPLAY case logic
//                         to properly handle redisplay when
//                         no bitmap is currently displayed.
//                       - Added RadCalcDone flag.
//              95/07/21 - Version 1.02B release.
//              95/09/30 - Revised About function to
//                         properly respond to WM_COMMAND
//                         messages under Windows NT.
//              95/10/10 - Revised WM_HSCROLL and WM_VSCROLL
//                         case statements to process scroll
//                         bar messages under Windows NT.
//              96/02/14 - Version 1.02C release.
//              96/02/21 - Modified DisplayBitmap to call
//                         SynCamera::Abort.
//              96/02/29 - Modified IDM_FILEOPEN case logic
//                         to initialize camera view system.
//              96/03/30 - Added camera focus position to
//                         SetView dialog box handler.
//                       - Added IDM_STATISTICS case.
//                       - Added DisplayStats function.
//                       - Modified DisableMenu and
//                         EnableMenu functions.
//                       - Deleted Environment Statistics
//                         message box function call from
//                         IDM_FILEOPEN case statement.
//                       - Modified IDM_FILEOPEN case logic
//                         to display default wireframe view
//                         of environment.
//                       - Added IDM_SETDOLLY, IDM_SETORBIT,
//                         IDM_SETPAN, IDM_SETROTATE,
//                         IDM_SETTILT, IDM_SETZOOM,
//                         IDM_DEFVIEW, WM_LBUTTONDOWN,
//                         WM_LBUTTONUP and WM_RBUTTONDOWN
//                         case statements.
//                       - Made menu handle in MainWndProc
//                         static and initialized from
//                         from WM_CREATE case statement.
//                       - Added MoveType global variable.
//                       - Added SetMovementType,
//                         UpdateDolly, UpdateOrbit,
//                         UpdatePan, UpdateRotate,
//                         UpdateTilt, UpdateZoom,
//                         UpdateCameraDialog and
//                         UpdateViewDialog functions.
//                       - Modified DisableMenu and
//                         EnableMenu functions.
//                       - Added camera tilt field to
//                         SetCamera.
//                       - Removed View Direction and
//                         View-Up Vector fields from
//                         SetView.
//                       - Deleted HorzError, VertError,
//                         ViewDirName, ViewUpName and
//                         ViewUpError static strings.
//                       - Added FocusError and TiltError
//                         static strings.
//                       - Deleted "spheric3.h" include
//                         directive.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Modified IDM_FILEOPEN case logic
//                         to disable camera movement and to
//                         delete extraneous InitViewSystem
//                         function call.
//                       - Added ResetMovementType function.
//              96/11/23 - Revised SynCamera constructor
//                         call to delete unused parameters.
//                       - Added WPARAM and LPARAM typecasts
//                         to SendDlgItemMessage function
//                         calls in SetDisplay.
//              96/11/29 - Modified IDM_FILEOPEN case
//                         statement to call SetCursor only
//                         if file was successfully opened.
//              97/03/25 - Modified DisplayStats to display
//                         number of luminaires.
//                       - Added LPARAM typecasts to
//                         SendMessage function calls in
//                         DoDownKey.
//				96/12/07 - Remove RenderMsg dialog box call
//						   from DisplayBitmap for WIN32
//						   compilation.
//				96/12/30 - Added EraseFlag static variable.
//						 - Added WM_ERASEBKGND case
//                         statement.
//						 - Modified IDD_UPD_DISPLAY case
//						   statement to reset EraseFlag.
//              97/01/25 - Replaced MAKEPOINT macro in
//                         WM_RBUTTONDOWN case statement
//                         with equivalent LOWORD and HIWORD
//                         macros.
//              97/02/25 - Removed unreferenced local
//                         variables from SetView.
//              97/03/01 - Removed initialization parameters
//                         from SynCamera declaration.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
// 
//////////////////////////////////////////////////////////// 

#include "general.h"
#include <windowsx.h>
#include <commdlg.h>
#include "error.h"
#include "parse.h"
#include "syn_cam.h"
#include "win_bmap.h"
#include "win_tga.h"
#include "win_meta.h"
#include "win_sbar.h"
#include "win_help.h"

#if (defined(_HEMI_CUBE) || defined(_CUBIC_TETRA))
#include "prog_rad.h"
#elif defined(_RAY_CAST)
#include "ray_rad.h"
#else
#include "rad_eqn.h"
#endif

#include "resource.h"
#include "helios.h"

static BOOL AbortFlag = FALSE;  // User-requested abort
static BOOL InCalc = FALSE;     // Calculations in progress
static BOOL EraseFlag = TRUE;   // Erase background flag

// Radiosity calculations completed
static BOOL RadCalcDone = FALSE;

static char EntityDir[MaxLen];  // Entity directory
static char WorldName[MaxLen];  // World file name buffer
static char BitmapName[MaxLen]; // Bitmap file name buffer
static char FileTitle[MaxLen];  // File title buffer
static char StrBuffer[MaxLen];  // Temporary string buffer

static int DispType = H_NONE;   // Display type
static int MoveType = H_LOCK;   // Camera movement type

static HINSTANCE hInst;         // Current instance handle

// Dialog box handles
static HWND hAbortDlg;          // Abort calculations
static HWND hQuantDlg;          // Quantization message
static HWND hRenderDlg;         // Rendering message
static HWND hSetCameraDlg;      // Camera parameters
static HWND hSetColorDlg;       // Color parameters
static HWND hSetDisplayDlg;     // Display parameters
static HWND hSetViewDlg;        // View parameters

// Dialog box exported function prolog pointers
static DLGPROC pcfunc;          // Camera parameters
static DLGPROC pdfunc;          // Display parameters
static DLGPROC pofunc;          // Coloor parameters
static DLGPROC pvfunc;          // View parameters

static OPENFILENAME Ofn;        // Open filename structure

// Synthetic camera
static SynCamera Camera(640, 480);

static Environ Environment;     // Environment
static Parse Parser;            // World file parser
static WinBitmap Bitmap;        // Bitmap manager
static WinMetaFile Wire;        // Metafile manager
static WinScroll *pScroll;      // Scroll bar manager ptr
static WinTarga Targa;          // Targa file manager

// Radiosity equation solver
#if (defined(_HEMI_CUBE) || defined(_CUBIC_TETRA))
static ProgRad Radiosity;       // Progressive radiosity
#elif defined(_RAY_CAST)
static RayRad Radiosity;        // Ray cast radiosity
#else
static RadEqnSolve Radiosity;   // Dummy equation solver
#endif

static const char AppName[] = "HELIOS";
static const char AppTitle[] = "HELIOS Radiosity Renderer";
static const char BitmapSection[] = "Bitmap";
static const char CalcInProgress[] = "Radiosity "
    "calculations in progress ...";
static const char ContrastError[] = "Contrast factor must "
    "be greater than zero";
static const char EyeDistError[] = "Front distance is "
    "behind eye position";
static const char FocusError[] = "Focus position cannot be"
    "equal to eye position";
static const char FrontDistError[] = "Front distance must "
    "be greater than back distance";
static const char GammaError[] = "Gamma value must be "
    "greater than zero";
static const char HeightEntry[] = "Height";
static const char HelpFileName[] = "HELIOS.HLP";
static const char InitFileName[] = "HELIOS.INI";
static const char InvalidDisplay[] = "HELIOS requires a "
    "display with at least 256 colors";
static const char MaxStepError[] = "Maximum number of "
    "steps must be between 1 and 2000";
static const char NoiseError[] = "Noise level must be "
    "between 0 and 8";
static const char PixelError[] = "Pixel values must be "
    "between 32 and 1024";
static const char StopError[] = "Stopping criterion must be"
    "between 0.0 and 1.0";
static const char TiltError[] = "Tilt angle must be between"
    " 0.0 and 360.0 degrees";
static const char WidthEntry[] = "Width";
static const char ViewDistError[] = "View distance must be "
    "greater than zero";
static const char WindowScaleError[] = "Window scaling "
    "factor must be greater than zero";
static const char WireClass[] = "WIRE";

// File type filters
static const char InputFilterSpec[128] =
    "World Files (*.WLD)\0*.WLD\0All Files (*.*)\0*.*\0";
static const char OutputFilterSpec[128] =
    "BMP Files (*.BMP)\0*.BMP\0TARGA Files (*.TGA)\0*.TGA\0";

static const int MaxStep = 2000;
static const int MinPixel = 32;
static const int MaxPixel = 1024;

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hpinst,
    LPSTR pcmdline, int cmdshow )
{
  MSG msg;      // Window message

  // Initialize user-request abort flag
  AbortFlag = FALSE;

  // Other instances of application running ?
  if (!hpinst)
    if (!InitApplication(hinstance))
      return FALSE;

  // Initialize current instance
  if (!InitInstance(hinstance, cmdshow))
      return FALSE;

  // Process window messages
  while (GetMessage(&msg, NULL, NULL, NULL))
  {
    // Check for "Set Camera" dialog message
    if (hSetCameraDlg != NULL)
      if (IsDialogMessage(hSetCameraDlg, &msg) == TRUE)
        continue;

    // Check for "Set Color Parameters" dialog message
    if (hSetColorDlg != NULL)
      if (IsDialogMessage(hSetColorDlg, &msg) == TRUE)
        continue;

    // Check for "Set Display Parameters" dialog message
    if (hSetDisplayDlg != NULL)
      if (IsDialogMessage(hSetDisplayDlg, &msg) == TRUE)
        continue;

    // Check for "Specify View" dialog message
    if (hSetViewDlg != NULL)
      if (IsDialogMessage(hSetViewDlg, &msg) == TRUE)
        continue;

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int) msg.wParam;
}

// Initialize window data and register window classes
static BOOL InitApplication( HINSTANCE hinstance )
{
  WNDCLASS wc;  // Window class

  // Register main window class
  wc.style = CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc = (WNDPROC) MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hinstance;
#if defined(_CUBIC_TETRA)
  wc.hIcon = LoadIcon(hinstance, "HELIOS_C");
#elif defined(_HEMI_CUBE)
  wc.hIcon = LoadIcon(hinstance, "HELIOS_H");
#elif defined(_RAY_CAST)
  wc.hIcon = LoadIcon(hinstance, "HELIOS_R");
#else
  wc.hIcon = LoadIcon(hinstance, "HELIOS_S");
#endif
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  wc.lpszMenuName =  "HeliosMenu";
  wc.lpszClassName = AppName;

  if (!RegisterClass(&wc))
    return FALSE;

  // Register wireframe window class
  wc.lpfnWndProc = (WNDPROC) WireWndProc;
  wc.hIcon = NULL;
  wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszClassName = WireClass;

  return (RegisterClass(&wc) ? TRUE : FALSE);
}

// Save instance handle and create main window
static BOOL InitInstance( HINSTANCE hinstance, int
    cmdshow )
{
  HDC hdc;      // Device context
  HWND hwnd;    // Main window handle

  hInst = hinstance;    // Save current instance handle

  // Create main window for current instance
  hwnd = CreateWindow(AppName, AppTitle,
      WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, NULL, NULL, hinstance, NULL);
      
  if (hwnd == 0)
    return FALSE;

  // Initialize open filename structure
  Ofn.lStructSize = sizeof(OPENFILENAME);
  Ofn.hwndOwner = hwnd;
  Ofn.lpstrFilter = NULL;
  Ofn.lpstrCustomFilter = NULL;
  Ofn.nMaxCustFilter = 0;
  Ofn.nFilterIndex = 1;
  Ofn.lpstrFile = NULL;
  Ofn.nMaxFile = MaxLen;
  Ofn.lpstrInitialDir = NULL;
  Ofn.lpstrFileTitle = FileTitle;
  Ofn.nMaxFileTitle = MaxLen;
  Ofn.lpstrTitle = NULL;
  Ofn.lpstrDefExt = NULL;

  // Initialize modeless dialog box handles
  hSetCameraDlg = NULL;
  hSetColorDlg = NULL;
  hSetDisplayDlg = NULL;
  hSetViewDlg = NULL;

  hdc = GetDC(NULL);    // Get screen device context

  // Determine display device capabilities
  if ((GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) == 0)
  {
    // Check for 15-bit or better display
    if (GetDeviceCaps(hdc, BITSPIXEL) < 15)
    {
      MessageBox(NULL, InvalidDisplay, AppTitle, MB_OK);
      ReleaseDC(NULL, hdc);
      return FALSE;
    }

    // No palette
    Bitmap.SetPaletteFlag(FALSE);
    Bitmap.SetRGBFlag(TRUE);
  }
  else
  {
    // Check for 256-color display
    if (GetDeviceCaps(hdc, COLORRES) < 8)
    {
      MessageBox(NULL, InvalidDisplay, AppTitle, MB_OK);
      ReleaseDC(NULL, hdc);
      return FALSE;
    }

    // Palette-mapped display
    Bitmap.SetPaletteFlag(TRUE);
    Bitmap.SetRGBFlag(FALSE);
  }

  ReleaseDC(NULL, hdc);         // Release device context

  ShowWindow(hwnd, cmdshow);    // Show the window
  UpdateWindow(hwnd);           // Paint the client area

  return TRUE;
}

// Main window message handler
LRESULT WINAPI MainWndProc( HWND hwnd, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  static short client_x;        // Client area width
  static short client_y;        // Client area height
  static short mouse_x;         // Mouse x-axis position
  static short mouse_y;         // Mouse y-axis position
  static short wire_x;          // Wireframe window width
  static short wire_y;          // Wireframe window height
  static HWND hwnd_wire;        // Wireframe window handle
  static HMENU hmenu;           // Menu handle
  int mouse_dx;                 // Mouse x-axis delta
  int mouse_dy;                 // Mouse y-axis delta
  BOOL redraw;                  // Redraw flag
  BOOL status;                  // Status flag
  DLGPROC pfunc;                // Exported fcn prolog ptr
  HCURSOR hcursor;              // Cursor handle
  HDC hdc;                      // Device context handle
  PAINTSTRUCT ps;               // Window paint structure
  POINT pos;                    // Point co-ordinates
  RECT rc;                      // Rectangle co-ordinates

  switch (msg)
  {
    case WM_CREATE:     // Create window
      // Instantiate scroll bar manager
      pScroll = new WinScroll(hwnd);

      hmenu = GetMenu(hwnd);        // Get menu handle

      // Create dialog box procedure instance thunks
      pcfunc = (DLGPROC) MakeProcInstance((FARPROC)
          SetCamera, hInst);
      pdfunc = (DLGPROC) MakeProcInstance((FARPROC)
          SetDisplay, hInst);
      pofunc = (DLGPROC) MakeProcInstance((FARPROC)
          SetColor, hInst);
      pvfunc = (DLGPROC) MakeProcInstance((FARPROC)
          SetView, hInst);
      break;
    case WM_SIZE:       // Get client area dimensions
      client_x = LOWORD(lparam);
      client_y = HIWORD(lparam);
      switch (DispType)
      {
        case H_WIRE:
          // Update wireframe display
          CalcWireDim(client_x, client_y, &wire_x, &wire_y);
          MoveWindow(hwnd_wire, Offset, Offset, wire_x,
              wire_y, TRUE);
          break;
        case H_BMAP:
          // Set scroll bar manager
          pScroll->Set(Camera.GetWidth(),
              Camera.GetHeight());
          break;
        default:
          break;
      }
      break;
    case WM_PAINT:      // Paint client area 
      hdc = BeginPaint(hwnd, &ps);
      if (DispType == H_BMAP)   // Display bitmap ?
      {
        GetClientRect(hwnd, &rc);
        pos = pScroll->Pos();
        if (Bitmap.Display(hdc, pos, rc) == FALSE)
        {
          DispType = H_NONE;
          pScroll->Hide();
          OutOfMemory();
        }
      }
      EndPaint(hwnd, &ps);
      break;
	case WM_ERASEBKGND:	// Erase background
	  if (EraseFlag == TRUE)
	  {
		// Erase background
		return DefWindowProc(hwnd, msg, wparam, lparam);
	  }
	  else
	  {
		// Set erase background flag
		EraseFlag = TRUE;

	    return TRUE;
	  }
    case WM_HSCROLL:    // Process horz scroll bar message
      pScroll->Horz(GET_WM_HSCROLL_CODE(wparam, lparam),
          GET_WM_HSCROLL_POS(wparam, lparam));
      break;
    case WM_VSCROLL:    // Process vertical scroll bar msg
      pScroll->Vert(GET_WM_VSCROLL_CODE(wparam, lparam),
          GET_WM_VSCROLL_POS(wparam, lparam));
      break;
    case WM_KEYDOWN:    // Process key down message
      DoKeyDown(hwnd, wparam);
      break;
    case WM_LBUTTONDOWN:        // Process left button down
      if (MoveType != H_LOCK && InCalc == FALSE)
      {
        // Save current mouse coordinates
        mouse_x = LOWORD(lparam);
        mouse_y = HIWORD(lparam);

        SetCapture(hwnd);       // Capture mouse input
      }
      break;
    case WM_LBUTTONUP:          // Process left button up
      if (MoveType != H_LOCK && InCalc == FALSE)
      {
        ReleaseCapture();   // Release mouse

        // Calculate mouse movement
        mouse_dx = (int) LOWORD(lparam) - (int) mouse_x;
        mouse_dy = (int) mouse_y - (int) HIWORD(lparam);

        switch (MoveType)   // Process camera movement
        {
          case H_DOLLY:
            UpdateDolly(mouse_dy);
            break;
          case H_ORBIT:
            UpdateOrbit(mouse_dx, mouse_dy);
            break;
          case H_PAN:
            UpdatePan(mouse_dx, mouse_dy);
            break;
          case H_ROTATE:
            UpdateRotate(mouse_dx, mouse_dy);
            break;
          case H_TILT:
            UpdateTilt(mouse_dx);
            break;
          case H_ZOOM:
            UpdateZoom(mouse_dy);
            break;
        }

        // Display wireframe
        SendMessage(hwnd, WM_COMMAND, IDD_UPD_DISPLAY,
            (LPARAM) FALSE);
      }
      break;
    case WM_RBUTTONDOWN:        // Process right button down
      if (InCalc == FALSE)
      {
        // Get current mouse cursor coordinates
        pos.x = LOWORD(lparam);
		pos.y = HIWORD(lparam);

        // Check whether mouse cursor is in client area
        GetClientRect(hwnd, &rc);
        if (PtInRect(&rc, pos) == TRUE)
        {
          // Convert mouse position to screen coordinates
          ClientToScreen(hwnd, &pos);

          // Display Interactive popup menu
          TrackPopupMenu(GetSubMenu(GetSubMenu(hmenu,
              MENU_CAMERA_POS), MENU_INTERACTIVE_POS),
              TPM_LEFTALIGN | TPM_LEFTBUTTON, pos.x, pos.y,
              0, hwnd, NULL);
        }
      }
      break;
    case WM_QUERYENDSESSION:    // Terminate Windows session
      if (InCalc == TRUE)
      {
        MessageBox(hwnd, CalcInProgress, AppName,
            MB_ICONINFORMATION | MB_OK);
        return NULL;
      }
      else
        return 1L;
    case WM_CLOSE:      // Terminate application
      if (InCalc == TRUE)
        MessageBox(hwnd, CalcInProgress, AppName,
            MB_ICONINFORMATION | MB_OK);
      else
        DestroyWindow(hwnd);    // Close main window
      break;
    case WM_QUERYNEWPALETTE:    // New palette required ?
    case WM_PALETTECHANGED:     // System palette changed
      if (DispType == H_BMAP)
        Bitmap.Redisplay(hwnd); // Redisplay bitmap
      break;
    case WM_COMMAND:    // Process window message
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDM_FILEOPEN:      // Open file
          Ofn.lpstrDefExt = "WLD";
          Ofn.lpstrFilter = InputFilterSpec;
          Ofn.lpstrFile = WorldName;
          Ofn.Flags = OFN_HIDEREADONLY | OFN_READONLY |
              OFN_ENABLEHOOK;
          Ofn.lpfnHook = (UINT (CALLBACK *)(HWND, UINT,
              WPARAM, LPARAM)) MakeProcInstance((FARPROC)
              FileOpenHook, hInst);
          if (GetOpenFileName((LPOPENFILENAME) &Ofn))
          {
            switch (DispType)
            {
              case H_WIRE:
                // Erase wireframe metafile
                Wire.Erase();
                DispType = H_NONE;

                // Close wireframe window
                DestroyWindow(hwnd_wire);
                break;
              case H_BMAP:
                Bitmap.Close();     // Close bitmap
                DispType = H_NONE;
                pScroll->Hide();    // Hide scroll bars

                // Disable Save As menu item
                EnableMenuItem(hmenu, IDM_SAVEAS,
                    MF_GRAYED);

                InvalidateRect(hwnd, NULL, TRUE);
                break;
              default:
                break;
            }

            // Parse environment file
            if ((Parser.ParseFile(Ofn.lpstrFile, EntityDir,
                &Environment)) == TRUE)
            {
              // Update window title
              // Initialize camera view system
              Camera.InitViewSystem(&Environment);

              wsprintf(StrBuffer, "HELIOS - %s",
                  Ofn.lpstrFile);
              SetWindowText(hwnd, StrBuffer);

              // Display hourglass cursor
              hcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

              // Disable camera movement
              ResetMovementType(hmenu);
            
              // Display wireframe
              SendMessage(hwnd, WM_COMMAND, IDM_WIREFRAME,
                  (LPARAM) 0L);

              // Enable display statistics menu item
              EnableMenuItem(hmenu, IDM_STATISTICS,
                  MF_ENABLED);

              // Enable rendering menu items
              EnableMenuItem(hmenu, IDM_WIREFRAME,
                  MF_ENABLED);
              EnableMenuItem(hmenu, IDM_SHADED,
                  MF_ENABLED);
              EnableMenuItem(hmenu, IDM_RENDER,
                  MF_ENABLED);

              // Disable Redisplay menu item
              EnableMenuItem(hmenu, IDM_REDISPLAY,
                  MF_GRAYED);

              // Reset radiosity calculations done flag
              RadCalcDone = FALSE;

              SetCursor(hcursor);   // Redisplay old cursor
            }
          }
          FreeProcInstance((FARPROC) Ofn.lpfnHook);
          break;
        case IDM_SAVEAS:        // Save BMP file
          Ofn.lpstrDefExt = "BMP";
          Ofn.lpstrFilter = OutputFilterSpec;
          Ofn.lpstrFile = BitmapName;
          Ofn.Flags = OFN_OVERWRITEPROMPT |
              OFN_HIDEREADONLY;
          if (GetSaveFileName((LPOPENFILENAME) &Ofn))
          {
            // Write bitmap file
            switch (Ofn.nFilterIndex)
            {
              case 1:   // BMP format
                status = Bitmap.Write(Ofn.lpstrFile);
                break;
              case 2:   // TARGA format
                status = Targa.Write(Bitmap.GetBitmapPtr(),
                    Bitmap.GetPalettePtr(),
                    Bitmap.GetWidth(), Bitmap.GetHeight(),
                    Bitmap.GetScanWidth(),
                    Bitmap.GetNumColors(),
                    Ofn.lpstrFile);
                break;
              default:  // Unrecognized format
                status = FALSE;
                break;
            }

            if (status == FALSE)        // Check for error
            {
              sprintf(StrBuffer, "Could not save file %s",
                  Ofn.lpstrFile);
                ReportError(StrBuffer);
            }
          }
          break;
        case IDM_EXIT:          // Exit application
          DestroyWindow(hwnd);
          break;
        case IDM_SETCAMERA:     // Set camera parameters
          if (hSetCameraDlg == NULL)
            hSetCameraDlg = CreateDialog(hInst, "SetCamera",
                hwnd, (DLGPROC) pcfunc);
          break;
        case IDM_SETVIEW:       // Specify view direction
          if (hSetViewDlg == NULL)
            hSetViewDlg = CreateDialog(hInst, "SetView",
                hwnd, (DLGPROC) pvfunc);
          break;
        case IDM_SETDOLLY:      // Toggle camera dolly
        case IDM_SETORBIT:      // Toggle camera orbit
        case IDM_SETPAN:        // Toggle camera pan
        case IDM_SETROTATE:     // Toggle camera rotate
        case IDM_SETTILT:       // Toggle camera tilt
        case IDM_SETZOOM:       // Toggle camera zoom
          SetMovementType(hmenu, GET_WM_COMMAND_ID(wparam,
              lparam));
          break;
        case IDM_DEFVIEW:       // Display default view
          // Initialize camera view system
          Camera.InitViewSystem(&Environment);

          // Update modeless Camera Parameters dialog box
          UpdateCameraDialog();

          // Update modeless View Parameters dialog box
          UpdateViewDialog();

          // Display wireframe
          SendMessage(hwnd, WM_COMMAND, IDD_UPD_DISPLAY,
              (LPARAM) FALSE);
          break;  
        case IDM_WIREFRAME:     // Wireframe display
          if (DispType != H_WIRE)
          {
            if (DispType == H_BMAP)
            {
              Bitmap.Close();       // Close bitmap
              DispType = H_NONE;
              pScroll->Hide();      // Hide scroll bars

              // Disable Save As menu item
              EnableMenuItem(hmenu, IDM_SAVEAS, MF_GRAYED);

              InvalidateRect(hwnd, NULL, TRUE);
            }

            // Create wireframe window
            CalcWireDim(client_x, client_y, &wire_x,
                &wire_y);
            hwnd_wire = CreateWindow(WireClass, NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER |
                WS_DISABLED, Offset, Offset, wire_x, wire_y,
                    hwnd, NULL, hInst, NULL);
            DispType = H_WIRE;

            // Record wireframe display
            if (Camera.Preview(&Environment, &Wire) ==
                FALSE)
              OutOfMemory();
          }
          break;
        case IDM_SHADED:        // Shaded display
        case IDM_RENDER:        // Radiosity rendering
          if (wparam == IDM_RENDER)
          {
            DisableMenu(hmenu);     // Disable menu items

            // Reset radiosity calculations done flag
            RadCalcDone = FALSE;

            // Confirm radiosity equation solver status
            if (Radiosity.GetStatus() == FALSE)
            {
              OutOfMemory();
              EnableMenu(hmenu);    // Enable menu items
              break;
            }

            // Initialize equation solver
            if (Radiosity.Open(&Environment) == FALSE)
            {
              OutOfMemory();
              EnableMenu(hmenu);    // Enable menu items
              break;
            }

            // Create modeless radiosity calculations
            // dialog box
            InCalc = TRUE;
            pfunc = (DLGPROC) MakeProcInstance((FARPROC)
                AbortCalc, hInst);
            hAbortDlg = CreateDialog(hInst, "AbortRad",
                hwnd, (DLGPROC) pfunc);
            SetDlgItemInt(hAbortDlg, IDC_MAXSTEP,
                Radiosity.GetMaxStep(), FALSE);
            SetDlgItemFloat(hAbortDlg, IDC_MINSTOP,
                Radiosity.GetStopCriterion());

            // Perform radiosity calculations
            while (Radiosity.Calculate() == FALSE)
            {
              // Check for user-requested abort
              if (CheckAbort() == TRUE)
              {
                AbortFlag = FALSE;
                break;
              }

              // Display radiosity calculations progress
              SetDlgItemInt(hAbortDlg, IDC_NUMSTEP,
                  Radiosity.GetStepCount(), FALSE);
              SetDlgItemFloat(hAbortDlg, IDC_CONVERGE,
                  Radiosity.GetConvergence());
              SetDlgItemText(hAbortDlg, IDC_ELAPSED,
                  Radiosity.GetElapsedTime());
            }

            // Close radiosity calculations dialog box
            DestroyWindow(hAbortDlg);
            FreeProcInstance((FARPROC) pfunc);
            UpdateWindow(hwnd);

            // Close radiosity equation solver
            Radiosity.Close();

            InCalc = FALSE;
            if (AbortFlag == FALSE)
            {
              // Set radiosity calculations done flag
              RadCalcDone = TRUE;
            }

            EnableMenu(hmenu);  // Enable menu items
          }
          else
          {
            // Set vertice exitances to parent surface
            // reflectances
            Radiosity.Shade(&Environment);

            // Reset radiosity calculations done flag
            RadCalcDone = FALSE;
          }
  
          // Open new bitmap
          if (Bitmap.Open(Camera.GetWidth(),
              Camera.GetHeight()) == TRUE)
          {
            // Display bitmap
            if (DisplayBitmap(hwnd, hwnd_wire, hmenu) ==
                TRUE)
            {
              if (wparam == IDM_RENDER)
              {
                MessageBeep(MB_OK);     // Signal completion

                // Display convergence statistics
                sprintf(StrBuffer, "Number of Steps = %d\n"
                    "Convergence = %f\nElapsed Time = %s",
                Radiosity.GetStepCount(),
                Radiosity.GetConvergence(),
                Radiosity.GetElapsedTime());

                MessageBox(hwnd, StrBuffer, "Convergence "
                    "Statistics", MB_OK |
                    MB_ICONINFORMATION);

                // Disable Rendering menu item
                EnableMenuItem(hmenu, IDM_RENDER,
                    MF_GRAYED);
              }
            }
            else
              Bitmap.Close();   // Close bitmap
          }
          else
            OutOfMemory();      // Report error
          break;
        case IDM_REDISPLAY:     // Redisplay bitmap
          if (DispType == H_BMAP)
            Bitmap.Close();     // Close bitmap

          // Open new bitmap
          if (Bitmap.Open(Camera.GetWidth(),
              Camera.GetHeight()) == TRUE)
          {
            // Display bitmap
            if (DisplayBitmap(hwnd, hwnd_wire, hmenu) ==
                FALSE)
              Bitmap.Close();   // Close bitmap
          }
          else
            OutOfMemory();
          break;
        case IDD_UPD_DISPLAY:   // Update display
          if (DispType == H_BMAP)
          {
			// Inhibit background erasure
			EraseFlag = (BOOL) lparam;

            Bitmap.Close();     // Close bitmap

            // Open new bitmap
            if (Bitmap.Open(Camera.GetWidth(),
                Camera.GetHeight()) == TRUE)
            {
              // Display bitmap
              if (DisplayBitmap(hwnd, hwnd_wire, hmenu) ==
                  FALSE)
                Bitmap.Close(); // Close bitmap
            }
            else
              OutOfMemory();    // Report error
          }
          else if (DispType == H_WIRE)
          {
            // Record wireframe display
            if (Camera.Preview(&Environment, &Wire) ==
                TRUE)
            {
              // Resize and redraw wireframe window
              InvalidateRect(hwnd_wire, NULL, TRUE);
              CalcWireDim(client_x, client_y, &wire_x,
                  &wire_y);
              MoveWindow(hwnd_wire, Offset, Offset, wire_x,
                  wire_y, TRUE);
            }
            else
              OutOfMemory();
          }
          break;
        case IDM_DIRECTORY:     // Set entity directory
          pfunc = (DLGPROC) MakeProcInstance((FARPROC)
              SetEntityDir, hInst);
          DialogBox(hInst, "SetEntityDir", hwnd, pfunc);
          FreeProcInstance((FARPROC) pfunc);
          break;
        case IDM_STATISTICS:    // Display statistics
          pfunc = (DLGPROC) MakeProcInstance((FARPROC)
              DisplayStats, hInst);
          DialogBox(hInst, "DisplayStats", hwnd, pfunc);
          FreeProcInstance((FARPROC) pfunc);
          break;
        case IDM_SETCONVERGE:   // Set convergence
          pfunc = (DLGPROC) MakeProcInstance((FARPROC)
              SetConverge, hInst);
          redraw = DialogBox(hInst, "SetConverge", hwnd,
              pfunc);
          FreeProcInstance((FARPROC) pfunc);
          if (redraw == TRUE)
          {
            // Reset radiosity calculations done flag
            RadCalcDone = FALSE;

            // Enable Rendering menu item
            EnableMenuItem(hmenu, IDM_RENDER, MF_ENABLED);
          }
          break;
        case IDM_SETDISPLAY:    // Set display parameters
          if (hSetDisplayDlg == NULL)
            hSetDisplayDlg = CreateDialog(hInst,
                "SetDisplay", hwnd, (DLGPROC) pdfunc);
          break;
        case IDM_SETCOLOR:      // Set color parameters
          if (hSetColorDlg == NULL)
            hSetColorDlg = CreateDialog(hInst, "SetColor",
                hwnd, (DLGPROC) pofunc);
          break;
        case IDM_HELP_CONTENTS: // Display Help contents
           WinHelp(hwnd, HelpFileName, HELP_CONTENTS, 0L);
           break;
        case IDM_HELP_HELP:     // Display Using Help
          WinHelp(hwnd, "WINHELP.HLP", HELP_CONTENTS, 0L);
          break;
        case IDM_HELP_BOOK:     // Display book info
          WinHelp(hwnd, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_BOOK);
          break;
        case IDM_ABOUT:         // Display About box
          pfunc = (DLGPROC) MakeProcInstance((FARPROC)
              About, hInst);
          DialogBox(hInst, "AboutBox", hwnd, pfunc);
          FreeProcInstance((FARPROC) pfunc);
          break;
        default:
          break;
      }
      break;
    case WM_DESTROY:    // Destroy window
      delete pScroll;   // Delete scroll bar manager
      Wire.Erase();     // Erase wireframe metafile
      Bitmap.Close();   // Release bitmap memory

      // Cancel Windows Help
      WinHelp(hwnd, HelpFileName, HELP_QUIT, 0L);

      // Release dialog box procedure instance thunks
      FreeProcInstance((FARPROC) pcfunc);
      FreeProcInstance((FARPROC) pdfunc);
      FreeProcInstance((FARPROC) pvfunc);

      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  return NULL;
}

// File Open dialog box hook function
UINT CALLBACK FileOpenHook( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lParam)
{
  if (msg == WM_INITDIALOG)
  {
    CenterDialog(hdlg);
    return TRUE;
  }
  else
    return FALSE;
}

// Wireframe window message handler
LRESULT WINAPI WireWndProc( HWND hwnd, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  static short client_x;    // Client area width
  static short client_y;    // Client area height

  switch (msg)
  {
    case WM_SIZE:
      client_x = LOWORD(lparam);
      client_y = HIWORD(lparam);
      break;
    case WM_PAINT:      // Paint client area
      Wire.Play(hwnd, Camera.GetWidth(), Camera.GetHeight(),
          client_x, client_y);
      break;
    default:
      return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  return NULL;
}

// Set entities directory
BOOL CALLBACK SetEntityDir( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  switch (msg)
  {
    case WM_INITDIALOG:
      SetDlgItemText(hdlg, IDC_ENTITY, EntityDir);
      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_HELP_SETDIR:   // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETDIR);
          break;
        case IDOK:
          GetDlgItemText(hdlg, IDC_ENTITY, EntityDir,
              sizeof(EntityDir));
          EndDialog(hdlg, TRUE);
          return TRUE;
        case IDCANCEL:
          EndDialog(hdlg, FALSE);
          return TRUE;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return FALSE;
}

// Display environment statistics
BOOL CALLBACK DisplayStats( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  switch (msg)
  {
    case WM_INITDIALOG:
      // Initialize environment statistics
      SetDlgItemInt(hdlg, IDC_NUM_INSTANCES,
          Environment.GetNumInst(), FALSE);
      SetDlgItemInt(hdlg, IDC_NUM_SURFACES,
          Environment.GetNumSurf(), FALSE);
      SetDlgItemInt(hdlg, IDC_NUM_PATCHES,
          Environment.GetNumPatch(), FALSE);
      SetDlgItemInt(hdlg, IDC_NUM_ELEMENTS,
          Environment.GetNumElem(), FALSE);
      SetDlgItemInt(hdlg, IDC_NUM_VERTICES,
          Environment.GetNumVert(), FALSE);

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDOK:
          EndDialog(hdlg, TRUE);
          return TRUE;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return FALSE;
}

// Set camera parameters
BOOL CALLBACK SetCamera( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  double vpd, fpd, bpd;     // Camera distances
  double scale;             // Window scaling factor
  double tilt;              // Camera tilt angle
  int w, h;                 // Bitmap window dimensions
  BOOL dummy;               // Dummy parameter
  static BOOL par_flag;     // Parallel projection flag

  switch (msg)
  {
    case WM_INITDIALOG:
      // Get camera distances
      SetDlgItemFloat(hdlg, IDC_VDIST,
          Camera.GetViewDist());
      SetDlgItemFloat(hdlg, IDC_FDIST,
          Camera.GetFrontDist());
      SetDlgItemFloat(hdlg, IDC_BDIST,
          Camera.GetBackDist());

      // Get window scaling factor
      SetDlgItemFloat(hdlg, IDC_SCALE,
          Camera.GetWindowScale());

      // Get bitmap window dimensions
      SetDlgItemInt(hdlg, IDC_HPIXSZ, Camera.GetWidth(),
          FALSE);
      SetDlgItemInt(hdlg, IDC_VPIXSZ, Camera.GetHeight(),
          FALSE);

      // Get camera tilt angle
      SetDlgItemFloat(hdlg, IDC_CAMTILT,
          RadToDeg(Camera.GetTiltAngle()));

      // Get projection type
      par_flag = Camera.GetParProjFlag();

      if (par_flag == TRUE)
        CheckRadioButton(hdlg, IDC_PERSPECT, IDC_PARALLEL,
            IDC_PARALLEL);
      else
        CheckRadioButton(hdlg, IDC_PERSPECT, IDC_PARALLEL,
            IDC_PERSPECT);

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_PERSPECT:
        case IDC_PARALLEL:
          par_flag = (wparam - IDC_PERSPECT != 0) ? TRUE :
              FALSE;

          CheckRadioButton(hdlg, IDC_PERSPECT, IDC_PARALLEL,
              wparam);
          break;
        case IDC_HELP_SETCAMERA:        // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETCAMERA);
          break;
        case IDOK:
        case IDC_UPDATE:
          // Validate distances
          vpd = GetDlgItemFloat(hdlg, IDC_VDIST);
          fpd = GetDlgItemFloat(hdlg, IDC_FDIST);
          bpd = GetDlgItemFloat(hdlg, IDC_BDIST);

          if (vpd <= 0.0)
          {
            MessageBox(hdlg, ViewDistError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          if (fpd >= bpd)
          {
            MessageBox(hdlg, FrontDistError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          if (fpd < -(vpd - MIN_VALUE))
          {
            MessageBox(hdlg, EyeDistError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Validate window scaling factor
          scale = GetDlgItemFloat(hdlg, IDC_SCALE);

          if (scale <= 0.0)
          {
            MessageBox(hdlg, WindowScaleError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Validate bitmap window dimensions
          w = (int) GetDlgItemInt(hdlg, IDC_HPIXSZ, &dummy,
              FALSE);
          h = (int) GetDlgItemInt(hdlg, IDC_VPIXSZ, &dummy,
              FALSE);

          if (w < MinPixel || w > MaxPixel || h < MinPixel
              || h > MaxPixel)
          {
            MessageBox(hdlg, PixelError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Validate tilt angle
          tilt = GetDlgItemFloat(hdlg, IDC_CAMTILT);

          if (tilt < 0.0 || tilt > 360.0)
          {
            MessageBox(hdlg, TiltError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set distances
          Camera.SetViewDist(vpd);
          Camera.SetFrontDist(fpd);
          Camera.SetBackDist(bpd);

          // Set window scaling factor
          Camera.SetWindowScale(scale);

          // Set bitmap window dimensions
          Camera.SetWidth(w);
          Camera.SetHeight(h);

          // Set camera tilt angle
          Camera.SetTiltAngle(DegToRad(tilt));

          // Set projection type
          Camera.SetParProjFlag(par_flag);

          // Update view system parameters
          Camera.UpdateViewSystem();

          // Instruct parent window to update client area
          SendMessage(GetParent(hdlg), WM_COMMAND,
              IDD_UPD_DISPLAY, (LPARAM) 0L);

          if (GET_WM_COMMAND_ID(wparam, lparam) == IDOK)
          {
            DestroyWindow(hdlg);        // Close dialog box
            hSetCameraDlg = NULL;
          }

          return TRUE;
        case IDCANCEL:
          DestroyWindow(hdlg);          // Close dialog box
          hSetCameraDlg = NULL;
          return TRUE;
        default:
          break;
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hdlg);      // Close dialog box
      hSetCameraDlg = NULL;
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Set camera view parameters
BOOL CALLBACK SetView( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  Point3 eye_posn;      // Camera eye position
  Point3 focus_posn;    // Camera focus position
  Vector3 temp;         // Temporary vector

  switch (msg)
  {
    case WM_INITDIALOG:
      // Get camera eye position
      eye_posn = Camera.GetEyePosn();
      SetDlgItemFloat(hdlg, IDC_EYE_X, eye_posn.GetX());
      SetDlgItemFloat(hdlg, IDC_EYE_Y, eye_posn.GetY());
      SetDlgItemFloat(hdlg, IDC_EYE_Z, eye_posn.GetZ());

      // Get camera focus position
      focus_posn = Camera.GetFocusPosn();
      SetDlgItemFloat(hdlg, IDC_FOCUS_X, focus_posn.GetX());
      SetDlgItemFloat(hdlg, IDC_FOCUS_Y, focus_posn.GetY());
      SetDlgItemFloat(hdlg, IDC_FOCUS_Z, focus_posn.GetZ());

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_HELP_SETVIEW:  // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETVIEW);
          break;
        case IDOK:
        case IDC_UPDATE:
          // Get eye position
          eye_posn.SetX(GetDlgItemFloat(hdlg, IDC_EYE_X));
          eye_posn.SetY(GetDlgItemFloat(hdlg, IDC_EYE_Y));
          eye_posn.SetZ(GetDlgItemFloat(hdlg, IDC_EYE_Z));

          // Get focus position
          focus_posn.SetX(GetDlgItemFloat(hdlg,
              IDC_FOCUS_X));
          focus_posn.SetY(GetDlgItemFloat(hdlg,
              IDC_FOCUS_Y));
          focus_posn.SetZ(GetDlgItemFloat(hdlg,
              IDC_FOCUS_Z));

          // Validate eye and focus positions
          temp = Vector3(eye_posn, focus_posn);
          if (fabs(temp.Length()) < MIN_VALUE)
          {
            MessageBox(hdlg, FocusError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set camera view parameters
          Camera.SetEyeFocusPosn(eye_posn, focus_posn);

          // Update view system parameters
          Camera.UpdateViewSystem();

          // Instruct parent window to update client area
          SendMessage(GetParent(hdlg), WM_COMMAND,
              IDD_UPD_DISPLAY, (LPARAM) 0L);
              
          if (GET_WM_COMMAND_ID(wparam, lparam) == IDOK)
          {
            DestroyWindow(hdlg);        // Close dialog box
            hSetViewDlg = NULL;
          }

          return TRUE;
        case IDCANCEL:
          DestroyWindow(hdlg);          // Close dialog box
          hSetViewDlg = NULL;
          return TRUE;
        default:
          break;
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hdlg);      // Close dialog box
      hSetViewDlg = NULL;
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Set radiosity rendering convergence parameters
BOOL CALLBACK SetConverge( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  int mp;       // Maximum number of steps
  double sc;    // Stopping criterion
  BOOL dummy;   // Dummy parameter

  switch (msg)
  {
    case WM_INITDIALOG:
      SetDlgItemInt(hdlg, IDC_MSTEP, Radiosity.GetMaxStep(),
          FALSE);
      SetDlgItemFloat(hdlg, IDC_STOPC,
          Radiosity.GetStopCriterion());
      CheckDlgButton(hdlg, IDC_AMBIENT_EN,
          Radiosity.AmbientFlag());

      if (Radiosity.OverShootFlag() == TRUE)
        CheckDlgButton(hdlg, IDC_OVER_EN, TRUE);

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_HELP_SETCONVERGE:      // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETCONVERGE);
          break;
        case IDOK:
          // Validate parameters
          mp = GetDlgItemInt(hdlg, IDC_MSTEP, &dummy, TRUE);
          sc = GetDlgItemFloat(hdlg, IDC_STOPC);

          if (mp < 1 || mp > MaxStep)
          {
            MessageBox(hdlg, MaxStepError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          if (sc <= 0.0 || sc > 1.0)
          {
            MessageBox(hdlg, StopError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set convergence parameters
          Radiosity.SetMaxStep(mp);
          Radiosity.SetStopCriterion(sc);

          if (IsDlgButtonChecked(hdlg, IDC_AMBIENT_EN) != 0)
            Radiosity.EnableAmbient();
          else
            Radiosity.DisableAmbient();

          if (IsDlgButtonChecked(hdlg, IDC_OVER_EN) != 0)
            Radiosity.EnableOverShoot();
          else
            Radiosity.DisableOverShoot();

          EndDialog(hdlg, TRUE);
          return TRUE;
        case IDCANCEL:
          EndDialog(hdlg, FALSE);
          return TRUE;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return FALSE;
}

// Set display parameters
BOOL CALLBACK SetDisplay( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  int i;                // Loop index
  int noise;            // Noise parameter
  double contrast;      // Contrast value
  double gamma;         // Gamma parameter
  double intensity;     // Intensity value
  BOOL dummy;           // Dummy parameter

  switch (msg)
  {
    case WM_INITDIALOG:
      SetDlgItemFloat(hdlg, IDC_INTENS,
          Camera.GetIntensity());
      SetDlgItemFloat(hdlg, IDC_CONTRAST,
          Camera.GetContrast());
      SetDlgItemFloat(hdlg, IDC_GAMMA, Camera.GetGamma());
      SetDlgItemInt(hdlg, IDC_JITTER,
          Camera.GetNoiseLevel(), FALSE);
      CheckDlgButton(hdlg, IDC_INTENS_EN,
          Camera.IntensityFlag());
      CheckDlgButton(hdlg, IDC_CONTRAST_EN,
          Camera.ContrastFlag());
      CheckDlgButton(hdlg, IDC_GAMMA_EN,
          Camera.GammaFlag());
      CheckDlgButton(hdlg, IDC_JITTER_EN,
          Camera.JitterFlag());

      // Initialize filter selection combo box
      for (i = 0; i < Camera.GetNumFilters(); i++)
        SendDlgItemMessage(hdlg, IDC_FILTERSEL,
            CB_INSERTSTRING, (WPARAM) i, (LPARAM)
            Camera.GetFilterSize(i));

      // Select current antialiasing filter
      (void) SendDlgItemMessage(hdlg, IDC_FILTERSEL,
          CB_SELECTSTRING, (WPARAM) -1, (LPARAM)
          Camera.GetCurrFilterSize());

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_HELP_SETDISPLAY:       // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETDISPLAY);
          break;
        case IDOK:
        case IDC_UPDATE:
          intensity = GetDlgItemFloat(hdlg, IDC_INTENS);

          // Set intensity parameter
          Camera.SetIntensity(intensity);

          contrast = GetDlgItemFloat(hdlg, IDC_CONTRAST);

          if (contrast <= 0.0)
          {
            MessageBox(hdlg, ContrastError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set contrast parameter
          Camera.SetContrast(contrast);

          gamma = GetDlgItemFloat(hdlg, IDC_GAMMA);

          if (gamma <= 0.0)
          {
            MessageBox(hdlg, GammaError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set gamma correction parameter
          Camera.SetGamma(gamma);

          noise = GetDlgItemInt(hdlg, IDC_JITTER, &dummy,
              TRUE);

          if (noise < 0 || noise > 8)
          {
            MessageBox(hdlg, NoiseError, AppName,
                MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Set noise level parameter
          Camera.SetNoiseLevel(noise);

          // Set antialiasing filter type
          Camera.SetFilterType((int)
              SendDlgItemMessage(hdlg, IDC_FILTERSEL,
              CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0L));

          if (IsDlgButtonChecked(hdlg, IDC_INTENS_EN) != 0)
            Camera.EnableIntensity();
          else
            Camera.DisableIntensity();

          if (IsDlgButtonChecked(hdlg, IDC_CONTRAST_EN) != 0)
            Camera.EnableContrast();
          else
            Camera.DisableContrast();

          if (IsDlgButtonChecked(hdlg, IDC_GAMMA_EN) != 0)
            Camera.EnableGamma();
          else
            Camera.DisableGamma();

          if (IsDlgButtonChecked(hdlg, IDC_JITTER_EN) != 0)
            Camera.EnableJitter();
          else
            Camera.DisableJitter();

          if (DispType == H_BMAP)
          {
            // Instruct parent window to update client area
            SendMessage(GetParent(hdlg), WM_COMMAND,
                IDD_UPD_DISPLAY, (LPARAM) 0L);
          }

          if (GET_WM_COMMAND_ID(wparam, lparam) == IDOK)
          {
            DestroyWindow(hdlg);        // Close dialog box
            hSetDisplayDlg = NULL;
          }

          return TRUE;
        case IDCANCEL:
          DestroyWindow(hdlg);          // Close dialog box
          hSetDisplayDlg = NULL;
          return TRUE;
        default:
          break;
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hdlg);      // Close dialog box
      hSetDisplayDlg = NULL;
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Set color parameters
BOOL CALLBACK SetColor( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  static int c_type;    // Color type
  static BOOL r_flag;   // Color resolution flag

  switch (msg)
  {
    case WM_INITDIALOG:
      c_type = Camera.GetColorType();
      r_flag = Bitmap.GetRGBFlag();

      CheckRadioButton(hdlg, IDC_RGB, IDC_PSEUDO, c_type +
          IDC_RGB);

      if (Bitmap.GetPaletteFlag() == FALSE)
      {
        if (r_flag == TRUE)
          CheckRadioButton(hdlg, IDC_COLRES_24, IDC_COLRES_8,
              IDC_COLRES_24);
        else
          CheckRadioButton(hdlg, IDC_COLRES_24, IDC_COLRES_8,
              IDC_COLRES_8);
      }
      else
      {
        // Disable 24-bit color resolution button
        EnableWindow(GetDlgItem(hdlg, IDC_COLRES_24),
            FALSE);

        CheckRadioButton(hdlg, IDC_COLRES_24, IDC_COLRES_8,
            IDC_COLRES_8);
      }

      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDC_RGB:
        case IDC_MONO:
        case IDC_PSEUDO:
          switch (wparam - IDC_RGB)
          {
            case 0:
              c_type = SC_RGB;
              break;
            case 1:
              c_type = SC_MONO;
              break;
            case 2:
              c_type = SC_PSEUDO;
              break;
            default:
              break;
          }

          CheckRadioButton(hdlg, IDC_RGB, IDC_PSEUDO,
              wparam);
          break;
        case IDC_COLRES_8:
        case IDC_COLRES_24:
          r_flag = (wparam - IDC_COLRES_24 == 0) ? TRUE :
              FALSE;

          CheckRadioButton(hdlg, IDC_COLRES_24,
              IDC_COLRES_8, wparam);
          break;
        case IDC_HELP_SETDISPLAY:       // Display Help
          WinHelp(hdlg, HelpFileName, HELP_CONTEXT,
              (DWORD) HELP_SETCOLOR);
          break;
        case IDOK:
        case IDC_UPDATE:
          // Set display color type
          Camera.SetColorType(c_type);

          // Set display color resolution
          Bitmap.SetRGBFlag(r_flag);

          if (DispType == H_BMAP)
          {
            // Instruct parent window to update client area
            SendMessage(GetParent(hdlg), WM_COMMAND,
                IDD_UPD_DISPLAY, (LPARAM) 0L);
          }

          if (GET_WM_COMMAND_ID(wparam, lparam) == IDOK)
          {
            DestroyWindow(hdlg);        // Close dialog box
            hSetColorDlg = NULL;
          }

          return TRUE;
        case IDCANCEL:
          DestroyWindow(hdlg);          // Close dialog box
          hSetColorDlg = NULL;
          return TRUE;
        default:
          break;
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hdlg);      // Close dialog box
      hSetColorDlg = NULL;
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Abort calculations dialog box
BOOL CALLBACK AbortCalc( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  switch (msg)
  {
    case WM_INITDIALOG:
      SetFocus(GetDlgItem(hdlg, IDCANCEL));
      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      AbortFlag = TRUE;
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Rendering calculations dialog box
BOOL CALLBACK RenderMsg( HWND hdlg, UINT msg, WPARAM
    wparam, LPARAM lparam )
{
  switch (msg)
  {
    case WM_INITDIALOG:
      CenterDialog(hdlg);
      return TRUE;
    default:
      break;
  }
  return FALSE;
}

// Display About dialog box
BOOL CALLBACK About( HWND hdlg, UINT msg, WPARAM wparam,
    LPARAM lparam )
{
  switch (msg)
  {
    case WM_INITDIALOG:
      CenterDialog(hdlg);
      return TRUE;
    case WM_COMMAND:
      switch (GET_WM_COMMAND_ID(wparam, lparam))
      {
        case IDOK:
        case IDCANCEL:
          EndDialog(hdlg, TRUE);
          return TRUE;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return FALSE;
}

// Reset camera movement type
static void ResetMovementType( HMENU hmenu )
{
  HMENU hintm;  // Interactive menu handle

  // Get interactive menu handle
  hintm = GetSubMenu(GetSubMenu(hmenu, MENU_CAMERA_POS),
      MENU_INTERACTIVE_POS);

  // Clear menu item check marks
  CheckMenuItem(hintm, IDM_SETDOLLY, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETORBIT, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETPAN, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETROTATE, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETTILT, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETZOOM, MF_UNCHECKED);

  MoveType = H_LOCK;  // Disable camera movement
}

// Set camera movement type
static void SetMovementType( HMENU hmenu, UINT menu_id )
{
  HMENU hintm;  // Interactive menu handle
  BOOL check;   // Check mark status

  // Get interactive menu handle
  hintm = GetSubMenu(GetSubMenu(hmenu, MENU_CAMERA_POS),
      MENU_INTERACTIVE_POS);

  // Save current menu item state
  check = GetMenuState(hintm, menu_id, MF_BYCOMMAND) &
      MF_CHECKED;

  // Clear menu item check marks
  CheckMenuItem(hintm, IDM_SETDOLLY, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETORBIT, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETPAN, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETROTATE, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETTILT, MF_UNCHECKED);
  CheckMenuItem(hintm, IDM_SETZOOM, MF_UNCHECKED);

  if (check == FALSE)
  {
    // Check current menu item
    CheckMenuItem(hintm, menu_id, MF_CHECKED);

    // Update camera movement type
    switch (menu_id)
    {
      case IDM_SETDOLLY:
        MoveType = H_DOLLY;
        break;
      case IDM_SETORBIT:
        MoveType = H_ORBIT;
        break;
      case IDM_SETPAN:
        MoveType = H_PAN;
        break;
      case IDM_SETROTATE:
        MoveType = H_ROTATE;
        break;
      case IDM_SETTILT:
        MoveType = H_TILT;
        break;
      case IDM_SETZOOM:
        MoveType = H_ZOOM;
        break;
      default:
        break;
    }
  }
  else
    MoveType = H_LOCK;  // Disable camera movement
}

// Disable all menu items
static void DisableMenu( HMENU hmenu )
{
  EnableMenuItem(hmenu, IDM_FILEOPEN, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SAVEAS, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_DIRECTORY, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_STATISTICS, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_EXIT, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SETCAMERA, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SETVIEW, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_DEFVIEW, MF_GRAYED);
  EnableMenuItem(GetSubMenu(hmenu, MENU_CAMERA_POS),
      MENU_INTERACTIVE_POS, MF_GRAYED | MF_BYPOSITION);
  EnableMenuItem(hmenu, IDM_WIREFRAME, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SHADED, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_RENDER, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_REDISPLAY, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SETDISPLAY, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SETCOLOR, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_SETCONVERGE, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_HELP_CONTENTS, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_HELP_HELP, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_HELP_BOOK, MF_GRAYED);
  EnableMenuItem(hmenu, IDM_ABOUT, MF_GRAYED);
}

// Enable menu items
static void EnableMenu( HMENU hmenu )
{
  EnableMenuItem(hmenu, IDM_FILEOPEN, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_DIRECTORY, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_STATISTICS, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_EXIT, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SETCAMERA, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SETVIEW, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_DEFVIEW, MF_ENABLED);
  EnableMenuItem(GetSubMenu(hmenu, MENU_CAMERA_POS),
      MENU_INTERACTIVE_POS, MF_ENABLED | MF_BYPOSITION);
  EnableMenuItem(hmenu, IDM_WIREFRAME, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SHADED, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_REDISPLAY, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SETDISPLAY, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SETCOLOR, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_SETCONVERGE, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_HELP_CONTENTS, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_HELP_HELP, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_HELP_BOOK, MF_ENABLED);
  EnableMenuItem(hmenu, IDM_ABOUT, MF_ENABLED);

  if (RadCalcDone == FALSE)
    EnableMenuItem(hmenu, IDM_RENDER, MF_ENABLED);

  if (DispType == H_BMAP)
    EnableMenuItem(hmenu, IDM_SAVEAS, MF_ENABLED);
}

// Display bitmap
static BOOL DisplayBitmap( HWND hwnd, HWND hwnd_wire, HMENU
    hmenu )
{
  BOOL r_abort;     // Rendering abort flag
  BOOL repeat;      // Repeat flag
  BOOL status;      // Status flag
  DLGPROC pfunc;    // Exported fcn prolog ptr

  DisableMenu(hmenu);   // Disable menu items

  // Create modeless rendering calculations dialog box
  if (Camera.AntiAliasFlag() == TRUE)
  {
    InCalc = TRUE;
    pfunc = (DLGPROC) MakeProcInstance((FARPROC) AbortCalc,
        hInst);
    hAbortDlg = CreateDialog(hInst, "AbortRender", hwnd,
        (DLGPROC) pfunc);
    SetDlgItemInt(hAbortDlg, IDC_MAXPASS,
        Camera.GetMaxPass(), FALSE);
    SetDlgItemInt(hAbortDlg, IDC_NUMPASS,
        Camera.GetNumPass(), FALSE);
    SetDlgItemText(hAbortDlg, IDC_FILTERTYPE,
        Camera.GetCurrFilterName());
  }
#if defined(WIN32) == 0
  else
  {
    pfunc = (DLGPROC) MakeProcInstance((FARPROC) RenderMsg,
        hInst);
    hRenderDlg = CreateDialog(hInst, "RenderMsg", hwnd,
        (DLGPROC) pfunc);
  }
#endif

  // Record shaded display
  r_abort = FALSE;
  do
  {
    if ((status = Camera.Shoot(&Environment, &Bitmap,
        &repeat)) == FALSE)
      break;

    if (Camera.AntiAliasFlag() == TRUE)
    {
      // Update dialog box
      SetDlgItemInt(hAbortDlg, IDC_NUMPASS,
          Camera.GetNumPass(), FALSE);

      // Check for user-requested abort
      if (CheckAbort() == TRUE)
      {
        r_abort = TRUE;
        AbortFlag = FALSE;
        Camera.Abort();
        break;
      }
    }
  }
  while (repeat == TRUE);

  // Close rendering calculations dialog box
  if (Camera.AntiAliasFlag() == TRUE)
  {
    InCalc = FALSE;
    DestroyWindow(hAbortDlg);
  }
  else
    DestroyWindow(hRenderDlg);

  FreeProcInstance((FARPROC) pfunc);
  UpdateWindow(hwnd);

  if (status == TRUE && r_abort == FALSE)
  {
    // 256-color display required ?
    if (Bitmap.GetRGBFlag() == FALSE)
    {
      // Create modeless color quantization dialog box
      pfunc = (DLGPROC) MakeProcInstance((FARPROC)
          RenderMsg, hInst);
      hRenderDlg = CreateDialog(hInst, "QuantizeMsg", hwnd,
          (DLGPROC) pfunc);

      // Perform color quantization
      status = Bitmap.QuantizeColors();

      // Close color quantization dialog box
      DestroyWindow(hRenderDlg);
      FreeProcInstance((FARPROC) pfunc);
      UpdateWindow(hwnd);
    }
  }

  if (status == TRUE)   // Check for error
  {
    if (DispType == H_WIRE)
    {
      // Erase wireframe metafile
      Wire.Erase();

      // Destroy wireframe window
      DestroyWindow(hwnd_wire);
    }

    if (r_abort == FALSE)
    {
      DispType = H_BMAP;

      // Initialize scroll bar manager
      pScroll->Init(Camera.GetWidth(), Camera.GetHeight());

      // Enable Save As menu item
      EnableMenuItem(hmenu, IDM_SAVEAS, MF_ENABLED);
    }
    else
    {
      DispType = H_NONE;
      pScroll->Hide();      // Hide scroll bars
    }
  }
  else
    OutOfMemory();      // Report error

  // Redraw display (via WM_PAINT)
  InvalidateRect(hwnd, NULL, TRUE);

  EnableMenu(hmenu);    // Enable menu items

  if (status == TRUE && r_abort == FALSE)
    return TRUE;
  else
    return FALSE;
}

// Update camera dolly
static void UpdateDolly( int mdy )
{
  double dist_mult;     // Dolly distance multiplier

  // Calculate dolly distance (range -1.0 to 1.0)
  dist_mult = (double) mdy / (double)
      GetSystemMetrics(SM_CYSCREEN);

  Camera.Dolly(dist_mult);  // Dolly camera

  // Update modeless View Parameters dialog box
  UpdateViewDialog();
}

// Update camera orbit
static void UpdateOrbit( int mdx, int mdy )
{
  double h_rotate;  // Horizontal rotation angle
  double v_rotate;  // Vertical rotation angle

  // Calculate horizontal rotation angle (range:
  // -aspect_ratio to aspect_ratio)
  h_rotate = PI * (double) mdx / (4.0 * (double)
      GetSystemMetrics(SM_CYSCREEN));

  // Calculate vertical rotation angle (range: -1 to 1)
  v_rotate = PI * (double) mdy / (4.0 * (double)
      GetSystemMetrics(SM_CYSCREEN));

  // Orbit camera
  Camera.Orbit(h_rotate, v_rotate);

  // Update modeless View Parameters dialog box
  UpdateViewDialog();
}

// Update camera pan
static void UpdatePan( int mdx, int mdy )
{
  double horz_mult;     // Horizontal pan multiplier
  double vert_mult;     // Vertical pan multiplier

  // Calculate horizontal pan multiplier (range -1.0 *
  // aspect_ratio to 1.0 * aspect_ratio)
  horz_mult = (double) mdx / (double)
      GetSystemMetrics(SM_CYSCREEN);

  // Calculate vertical pan multiplier (range -1.0 to
  // 1.0)
  vert_mult = (double) mdy / (double)
      GetSystemMetrics(SM_CYSCREEN);

  Camera.Pan(horz_mult, vert_mult);     // Pan camera

  // Update modeless View Parameters dialog box
  UpdateViewDialog();
}

// Update camera rotate
static void UpdateRotate( int mdx, int mdy )
{
  double h_rotate;  // Horizontal rotation angle
  double v_rotate;  // Vertical rotation angle

  // Calculate horizontal rotation angle (range:
  // -aspect_ratio to aspect_ratio)
  h_rotate = PI * (double) mdx / (4.0 * (double)
      GetSystemMetrics(SM_CYSCREEN));

  // Calculate vertical rotation angle (range: -1 to 1)
  v_rotate = PI * (double) mdy / (4.0 * (double)
      GetSystemMetrics(SM_CYSCREEN));

  // Rotate camera
  Camera.Rotate(h_rotate, v_rotate);

  // Update modeless View Parameters dialog box
  UpdateViewDialog();
}

// Update camera tilt
static void UpdateTilt( int mdx )
{
  double tilt_angle;    // Tilt angle

  // Calculate rotation angle (range: -1 to 1)
  tilt_angle = PI * (double) mdx / (4.0 * (double)
      GetSystemMetrics(SM_CXSCREEN));

  // Tilt camera
  Camera.Tilt(tilt_angle);

  // Update modeless Camera Parameters dialog box
  UpdateCameraDialog();
}

// Update camera zoom
static void UpdateZoom( int mdy )
{
  double vd_mult;   // View distance multiplier

  // Calculate view distance multiplier (range 0.25 to 4.0)
  vd_mult = pow(2.0, (2.0 + (double) (2 * mdy) / (double)
      GetSystemMetrics(SM_CYSCREEN))) / 4.0;

  Camera.Zoom(vd_mult);     // Zoom camera

  // Update modeless Camera Parameters dialog box
  UpdateCameraDialog();
}

// Update modeless Camera Parameters dialog box
static void UpdateCameraDialog()
{
  if (hSetCameraDlg != NULL)
  {
    // Update view distance edit control
    SetDlgItemFloat(hSetCameraDlg, IDC_VDIST,
        Camera.GetViewDist());

    // Update front distance edit control
    SetDlgItemFloat(hSetCameraDlg, IDC_FDIST,
        Camera.GetFrontDist());

    // Update back distance edit control
    SetDlgItemFloat(hSetCameraDlg, IDC_BDIST,
        Camera.GetBackDist());

    // Update camera tilt angle
    SetDlgItemFloat(hSetCameraDlg, IDC_CAMTILT,
        RadToDeg(Camera.GetTiltAngle()));
  }
}

// Update modeless View Parameters dialog box
static void UpdateViewDialog()
{
  if (hSetViewDlg != NULL)
  {
    // Update eye position edit controls
    SetDlgItemFloat(hSetViewDlg, IDC_EYE_X,
        Camera.GetEyePosn().GetX());
    SetDlgItemFloat(hSetViewDlg, IDC_EYE_Y,
        Camera.GetEyePosn().GetY());
    SetDlgItemFloat(hSetViewDlg, IDC_EYE_Z,
        Camera.GetEyePosn().GetZ());

    // Update focus position edit controls
    SetDlgItemFloat(hSetViewDlg, IDC_FOCUS_X,
        Camera.GetFocusPosn().GetX());
    SetDlgItemFloat(hSetViewDlg, IDC_FOCUS_Y,
        Camera.GetFocusPosn().GetY());
    SetDlgItemFloat(hSetViewDlg, IDC_FOCUS_Z,
        Camera.GetFocusPosn().GetZ());
  }
}

// Display Interactive popup menu
static void DispInteractMenu( HWND hwnd, HMENU hmenu, POINT
    point )
{
  ClientToScreen(hwnd, (LPPOINT) &point);
  TrackPopupMenu(hmenu, 0, point.x, point.y, 0, hwnd, NULL);
}

// Get floating point dialog item
static double GetDlgItemFloat( HWND hdlg, int id )
{
  (void) GetDlgItemText(hdlg, id, StrBuffer,
      sizeof(StrBuffer));
  return atof(StrBuffer);
}

// Set floating point dialog item
static void SetDlgItemFloat( HWND hdlg, int id, double num )
{
  sprintf(StrBuffer, "%6.5f", num);
  SetDlgItemText(hdlg, id, StrBuffer);
}

// Calculate wireframe window dimensions
static void CalcWireDim( short client_x, short client_y,
    short *pxchild, short *pychild )
{
  double client_aspect;
  double child_aspect;

  if (client_y > 0)
  {
    client_aspect = (double) client_x / (double) client_y;
    child_aspect = (double) Camera.GetWidth() / (double)
        Camera.GetHeight();
    if (client_aspect >= child_aspect)
    {
      *pychild = (short) max(client_y - Offset * 2, Offset);
      *pxchild = (short) ((double) *pychild * child_aspect);
    }
    else
    {
      *pxchild = (short) max(client_x - Offset * 2, Offset);
      *pychild = (short) ((double) *pxchild / child_aspect);
    }
  }
  else
    *pxchild = *pychild = Offset;
}

// Process WM_KEYDOWN message
static void DoKeyDown( HWND hwnd, WPARAM wparam )
{
  switch (GET_WM_COMMAND_ID(wparam, lparam))
  {
    case VK_HOME:
      SendMessage(hwnd, WM_VSCROLL, SB_TOP, (LPARAM) 0L);
      break;
    case VK_END:
      SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, (LPARAM) 0L);
      break;
    case VK_PRIOR:
      SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, (LPARAM) 0L);
      break;
    case VK_NEXT:
      SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, (LPARAM)
          0L);
      break;
    case VK_UP:
      SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, (LPARAM) 0L);
      break;
    case VK_DOWN:
      SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)
          0L);
      break;
    case VK_LEFT:
      SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, (LPARAM) 0L);
      break;
    case VK_RIGHT:
      SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
      break;
  }
}

// Check for user-requested abort
static BOOL CheckAbort()
{
  MSG msg;      // Window message

  // Process application message queue
  while ((AbortFlag == FALSE) && (PeekMessage(&msg, NULL,
      0, 0, PM_REMOVE)))
  {
    if (IsDialogMessage(hAbortDlg, &msg) == FALSE)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return AbortFlag;
}

// Center dialog box
static void CenterDialog( HWND hdlg )
{
  int height;   // Dialog box height
  int width;    // Dialog box width
  RECT drc;     // Dialog box coordinates
  RECT wrc;     // Parent window coordinates

  // Get dialog box coordinates
  GetWindowRect(hdlg, &drc);
  height = drc.bottom - drc.top;
  width = drc.right - drc.left;

  // Get parent window coordinates
  GetWindowRect(GetWindow(hdlg, GW_OWNER), &wrc);

  // Center dialog box in parent window
  MoveWindow(hdlg, max(0, (wrc.left + wrc.right - width) /
      2), max(0, (wrc.top + wrc.bottom - height) / 2),
     width, height, FALSE);
}
