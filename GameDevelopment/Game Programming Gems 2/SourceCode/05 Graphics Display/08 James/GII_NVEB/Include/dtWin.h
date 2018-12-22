/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Include
File:  dtWin.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#if !defined( AFX_DTWIN_H_INCLUDED_ )
#define AFX_DTWIN_H_INCLUDED_


#include <ul.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>
#include "assert.h"

#include "..\CommonSrc\dtWin\ErrorDebug.h"

#define DT_MESSAGE_LENGTH 512
#define DT_FILENAME_LENGTH 128



// Mouse callback structure
typedef struct _dtMouseEvent
{
  Float x, y;
  bool left_button;
  bool right_button;
  bool shift;
  bool control;
  bool middle_button;

} _dtMouseEvent;

typedef _dtMouseEvent * dtpMouseEvent;


typedef HRESULT (*dtKeyboardHandler)(int key); 
typedef HRESULT (*dtMouseHandler)(dtpMouseEvent);


typedef HRESULT (*dtShutdownHandler)(void);
typedef HRESULT(*dtMenuHandler)(int menuitem); 

typedef HRESULT (*dtFunctionHandler)(void);
typedef HRESULT (*dtMessageHandler)(WPARAM wParam, LPARAM lParam );

typedef LRESULT  (CALLBACK *dtWindowsMessageHandler)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Menu save structure

class _dtMenu: public ulLeaf <_dtMenu>
{
public:
  ~_dtMenu();
  HMENU m_hmenu;  // Parent Menu handles
  HMENU m_subhmenu;
  DWORD m_item;  // item associated with this menu entry
};

typedef _dtMenu * dtpMenu;
   
typedef ulTree<_dtMenu> _dtMenuTree;
typedef _dtMenuTree * dtpMenuTree;

typedef struct _dtCursor
{

  HCURSOR stack[256];
  int top;
} _dtCursor;






typedef struct _dtWindowData
{
  HACCEL  hAccel;
  HWND    hWnd;
  bool    bReady;
  
  RECT    rcScreenRect;     // Screen rect for window
  RECT    rcWindow;			// window coordinates
  RECT    rcViewportRect;   // Offscreen rect for VPort
  bool resize_window;
  
  dtKeyboardHandler KeyUpHandler;
  dtKeyboardHandler KeyDownHandler;
  
  
  dtWindowsMessageHandler WindowsMessageHandler;
  _dtMouseEvent mouse_event;
  dtMenuHandler MenuHandler;
  dtMouseHandler MouseMoveHandler;
  dtMouseHandler MouseLeftUpHandler;
  dtMouseHandler MouseLeftDownHandler;
  dtMouseHandler MouseLeftDblHandler;
  
  dtMouseHandler MouseRightUpHandler;
  dtMouseHandler MouseRightDownHandler;
  dtMouseHandler MouseRightDblHandler;
  
  dtMouseHandler MouseMiddleUpHandler;
  dtMouseHandler MouseMiddleDownHandler;
  dtMouseHandler MouseMiddleDblHandler;
 
  dtMessageHandler ResizeHandler;

  dtFunctionHandler CloseHandler;
  dtFunctionHandler QuitHandler;
  dtFunctionHandler DestroyHandler;
  dtFunctionHandler PaintHandler;
  //dtFunctionHandler Handler;
  bool    bWindowCreated;
  
  _dtCursor Cursor;
  _dtMenuTree PulldownMenu;
  _dtMenuTree PopupMenu;
  
  dtpMenuTree current_tree;
  dtpMenu current_menu;
  HMENU current_hmenu;
  HMENU current_hmenu_stack[256];
  
  // app name
  char name[DT_FILENAME_LENGTH];
  ulColor textcolor;
  
  
  
} _dtWindowData;




// 2d mouse and Menus class
class dtWindow
{
public:

  dtWindow();
  virtual ~dtWindow();
  

  static dtWindow * m_pWindowInstance;
  static dtWindow * GetpWindowInstance() { return m_pWindowInstance;}
  static _dtWindowData * pWindowData() { return &m_pWindowInstance->m;}
  static _dtWindowData & WindowData() { return m_pWindowInstance->m;}


   static void SetResizeWindow(bool b) { WindowData().resize_window = b;}

  // INIT
   static void Open2DWindow( const HINSTANCE &hInst,  char * str,
    const int &x, const int &y, const int &w, const int &h, const DWORD &winFlags, LPCSTR lpszMenuName = 0,
     bool resize = true, bool show = true);

   static void Open2DWindow( char * str,
    const int &x, const int &y, const int &w, const int &h, const DWORD &winFlags, LPCSTR lpszMenuName = 0,
     bool resize = true, bool show = true);


   static void ShutdownWindow();

   static void SetWindowReady( const bool &b ) {WindowData().bReady = b;}
   static bool IsWindowReady() { return WindowData().bReady;}
   static BOOL SetWindowTitle(const char * title);
   static LRESULT CALLBACK WindowsProcedureCallback( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
   static void SetScreenParametersFromWnd(); // update window properties (not h and w) from window
   static void ConvertToScreen(int &x, int &y);
   static void SetWindowPosition(); // Make window poition based on parameters (x,y,h,w)
   static void SetSizeFromWindow(); // make h w match window

   static void ResetWindow();


 
  
  // menus
   static dtpMenu CreateMainPopupMenu();
   static dtpMenu CreatePopupMenu(const char * str, const DWORD &flags = 0);
   static dtpMenu AddMenuItem(const int &id, const char * str, const DWORD &flags = 0);
   static bool DeleteMenuItem(const DWORD &id);
   static bool DeleteMenuItem(dtpMenu);
   static void ExitMenu();
   static int AddMenuSeparator();
   static int EnableMenuItem(const DWORD &iSelection, const DWORD & tf);
   static int CheckMenuItem(const DWORD &iSelection, const DWORD & tf);
   static bool InsertMenuItem(const int &before, 
                                     const int &id, const char * str, const DWORD &flags = 0);

   static dtpMenu MakeMenu(int id, HMENU hmenu);

   static dtpMenu CreateMainPulldownMenu();
   static dtpMenu CreatePulldownMenu(const char * str, const DWORD &flags = 0);

   static void DeleteAllMenus();


  // Menus
   static void SetMenuHandler (dtMenuHandler handler);
   static void SetWindowsMessageHandler(dtWindowsMessageHandler handler){WindowData().WindowsMessageHandler = handler;}

   static dtpMenuTree FindTree(DWORD item, dtpMenu * menu = 0);

   static HRESULT SetCursor(LPSTR);
   static HRESULT ReleaseCursor();



  // Handlers

   static void SetKeyDownHandler (dtKeyboardHandler Handler );
   static void SetKeyUpHandler (dtKeyboardHandler Handler );

  static void MakeMouseEvent(const int &x, const int &y, DWORD flags);

   static void SetMouseMoveHandler (dtMouseHandler Handler );
   static void SetMouseLeftDblHandler (dtMouseHandler Handler );
   static void SetMouseLeftDownHandler (dtMouseHandler Handler );
   static void SetMouseLeftUpHandler (dtMouseHandler Handler );
   static void SetMouseRightDblHandler (dtMouseHandler Handler );
   static void SetMouseRightDownHandler (dtMouseHandler Handler );
   static void SetMouseRightUpHandler (dtMouseHandler Handler );
   static void SetMouseMiddleDblHandler (dtMouseHandler Handler );
   static void SetMouseMiddleDownHandler (dtMouseHandler Handler );
   static void SetMouseMiddleUpHandler (dtMouseHandler Handler );

   static dtpMouseEvent GetMouseEvent() { return &WindowData().mouse_event;}

   static void SetResizeHandler(dtMessageHandler Handler) { WindowData().ResizeHandler = Handler;}
   static void SetQuitHandler(dtFunctionHandler Handler) { WindowData().QuitHandler = Handler;}
   static void SetCloseHandler(dtFunctionHandler Handler) { WindowData().CloseHandler = Handler;}
   static void SetDestroyHandler(dtFunctionHandler Handler) { WindowData().DestroyHandler = Handler;}
   static void SetPaintHandler(dtFunctionHandler Handler) { WindowData().PaintHandler = Handler;}


   static void SetWindowHandle(const HWND &hWnd);
   static VOID Move(const int &x, const int &y );

 // Functions for managing screen and viewport bounds


   static RECT * GetpWindowRect() { return &WindowData().rcWindow;}
   static int GetWindowX() { return WindowData().rcWindow.left; }
   static int GetWindowY() { return WindowData().rcWindow.top; }
   static int GetWindowWidth() { return WindowData().rcWindow.right - WindowData().rcWindow.left; }
   static int GetWindowHeight() { return WindowData().rcWindow.bottom - WindowData().rcWindow.top; }

  
   static RECT*   GetpViewportRect()               { return &WindowData().rcViewportRect; }
   static RECT*   GetpScreenRect()               { return &WindowData().rcScreenRect; }
   static DWORD GetViewportWidth() { return WindowData().rcViewportRect.right; }
   static DWORD GetViewportHeight() { return WindowData().rcViewportRect.bottom; }
   static void SetViewportRect(int x1, int y1, int x2, int y2) 
  {
    WindowData().rcViewportRect.left = x1; 
    WindowData().rcViewportRect.right = x2;
    
    WindowData().rcViewportRect.top = y1;
    WindowData().rcViewportRect.bottom = y2;
  }
   static void SetScreenRect(int x1, int y1, int x2, int y2) 
  {
    WindowData().rcScreenRect.left = x1; 
    WindowData().rcScreenRect.right = x2;
    
    WindowData().rcScreenRect.top = y1;
    WindowData().rcScreenRect.bottom = y2;
  }


   static HWND GethWnd() {return WindowData().hWnd; }

  // because its 
   static const char * GetName() { return WindowData().name;}
   static void SetName(const char * name) 
  { 
    strncpy(WindowData().name, name, DT_FILENAME_LENGTH); 
  }

   static bool IsName(const char * name)
  {
    if (name == 0)
      return false;
    if (strncmp(WindowData().name, name, DT_FILENAME_LENGTH) == 0)
      return true;

    return false;

  }

   static int ServiceMessages ( const int &service_all = 0);
   static HACCEL WINAPI LoadAccelerators(LPCSTR lpTableName);
   static char * PopupFileRequestor(const char *fileMask, const char * initialdir = 0,
    const char * title = 0);

   static char * PopupSaveFileRequestor(const char *fileMask, 
       const char * name = 0, const char * initialdir = 0,
       const char * title = 0);

   static char * PopupDirectoryRequestor(const char * initialdir = 0, const char * title = 0);

   char * PopupImageFileRequestor(const char * initialdir = 0, const char * title = 0);
   static void Msg( const char * fmt, ... );

  static void OutputText( const DWORD &x, const DWORD &y, const CHAR* str );
  static  void SetTextColor(const int &r, const int &g, const int &b) 
  { 
    WindowData().textcolor.r = r;
    WindowData().textcolor.g = g;
    WindowData().textcolor.b = b;
  }
   static void GetTextColor(int &r, int &g, int &b)
  {
    r = WindowData().textcolor.r;
    g = WindowData().textcolor.g;
    b = WindowData().textcolor.b;
  }



private:
    _dtWindowData m;
};



#ifdef MULTI_THREAD
  #pragma comment(lib, "DTWinMT.lib")
#else
  #pragma comment(lib, "DTWin.lib")
#endif


#endif  // #if !defined( AFX_DTWIN_H_INCLUDED_ )