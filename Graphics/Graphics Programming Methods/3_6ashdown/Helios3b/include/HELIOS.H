////////////////////////////////////////////////////////////
//
//  HELIOS.H - Radiosity Renderer for MS-Windows Main Module
//             Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/07 - Added CheckAbort function
//                         prototype.
//              94/09/23 - Added AbortCalc function
//                         prototype.
//              94/11/25 - Added CenterDialog and
//                         FileOpenHook function
//                         prototypes.
//              94/12/04 - Added RenderMsg function
//                         prototype.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/26 - Added SetColor and DisplayBitmap
//                         function prototypes.
//              95/07/16 - Added DisAbleMenu and EnableMenu
//                         function prototypes.
//              95/07/21 - Version 1.02B release.
//              95/09/18 - Added GET_WM_HSCROLL_CODE and
//                         GET_WM_VSCROLL_CODE macros.
//              96/02/14 - Version 1.02C release.
//              96/03/03 - Added DisplayStats function
//                         prototype.
//              96/03/30 - Added H_LOCK, H_DOLLY, H_ORBIT,
//                         H_PAN, H_ROTATE, H_TILT and
//                         H_ZOOM definitions.
//                       - Added SetMovementType,
//                         UpdateDolly, UpdateOrbit,
//                         UpdatePan, UpdateRotate,
//                         UpdateTilt, UpdateZoom,
//                         UpdateCameraDialog and
//                         UpdateViewDialog function
//                         prototypes.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Added ResetMovementType function
//                         prototype.
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

#ifndef _HELIOS_H
#define _HELIOS_H

static const int MaxLen = 256;
static const int Offset = 2;

// Camera movement type
#define H_LOCK      0
#define H_DOLLY     1
#define H_ORBIT     2
#define H_PAN       3
#define H_ROTATE    4
#define H_TILT      5
#define H_ZOOM      6

// Display type
#define H_NONE      0   // None
#define H_WIRE      1   // Wireframe
#define H_BMAP      2   // Bitmap

#ifndef WIN32
// WIN16 message cracking macros
#define GET_WM_COMMAND_ID(wp, lp)   (wp)
#define GET_WM_HSCROLL_CODE(wp, lp) (wp)
#define GET_WM_VSCROLL_CODE(wp, lp) (wp)
#define GET_WM_HSCROLL_POS(wp, lp)  LOWORD(lp)
#define GET_WM_VSCROLL_POS(wp, lp)  LOWORD(lp)
#endif

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

static BOOL CheckAbort();
static BOOL DisplayBitmap( HWND, HWND, HMENU );
static BOOL InitApplication( HINSTANCE );
static BOOL InitInstance( HINSTANCE, int );
static double GetDlgItemFloat( HWND, int );
static void CalcWireDim( short, short, short *, short * );
static void CenterDialog( HWND );
static void DisableMenu( HMENU );
static void DoKeyDown( HWND, WPARAM );
static void EnableMenu( HMENU );
static void EnableMenu( HMENU );
static void ResetMovementType( HMENU );
static void SetDlgItemFloat( HWND, int, double );
static void SetMovementType( HMENU, UINT );
static void UpdateCameraDialog();
static void UpdateDolly( int );
static void UpdateOrbit (int, int );
static void UpdatePan (int, int );
static void UpdateRotate (int, int );
static void UpdateTilt( int );
static void UpdateZoom( int );
static void UpdateViewDialog();

extern "C"
{
  LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM,
      LPARAM );
  LRESULT WINAPI WireWndProc( HWND, UINT, WPARAM,
      LPARAM );
  BOOL CALLBACK About( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK AbortCalc( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK DisplayStats( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK RenderMsg( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetCamera( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetColor( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetConverge( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetDisplay( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetEntityDir( HWND, UINT, WPARAM, LPARAM );
  BOOL CALLBACK SetView( HWND, UINT, WPARAM, LPARAM );
  UINT CALLBACK FileOpenHook( HWND, UINT, WPARAM, LPARAM );
}

#endif

