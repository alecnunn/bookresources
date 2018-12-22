// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "include/ZFXRenderer.h"            // our render interface
#include "include/ZFX.h"                    // return values and stuff
#include "include/main.h"                   // prototypes and stuff
#include "include/Globals.h"                // GLOBALS struct

 
//include our library
#pragma comment(lib, "libs/ZFXRenderer.lib")
#pragma comment(lib, "libs/zfx3d.lib")
 
 
// windows stuff
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

CApplication g_MyApp;



/**
 * WinMain function to get the thing started.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
   WNDCLASSEX	wndclass;
   HRESULT     hr;
   HWND		   hWnd;
   MSG			msg;

   // Set up window attributes
   wndclass.cbSize         = sizeof(wndclass);
   wndclass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
   wndclass.lpfnWndProc    = MsgProc;
   wndclass.cbClsExtra     = 0;
   wndclass.cbWndExtra     = 0;
   wndclass.hInstance      = hInst;
   wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
   wndclass.lpszMenuName   = NULL;
   wndclass.lpszClassName  = g_szAppClass;
   wndclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    
   if(RegisterClassEx(&wndclass) == 0)
      return 0;

   // create window
   if (!(hWnd = CreateWindowEx(NULL, g_szAppClass,
	   		                   "PanBox Edit - New Level...",
				                   WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,
			 	                   GetSystemMetrics(SM_CXSCREEN),
                               GetSystemMetrics(SM_CYSCREEN),
				                   NULL, NULL, hInst, NULL)))
      return 0;

   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);

   // init 3dmath lib
   ZFX3DInitCPU();

   // try to start the engine
   if (FAILED(hr = g_MyApp.Init(hWnd, hInst))) {
      if (hr == ZFX_FAIL)
         fprintf(CApplication::m_G.pLog, "error: ProgramStartup() failed\n");
      else if (hr == ZFX_CANCELED)
         fprintf(CApplication::m_G.pLog, "error: ProgramStartup() canceled by user \n");
      else if (hr == ZFX_NOTCOMPATIBLE) {
         MessageBox(CApplication::m_G.hWnd, 
                    "Sorry, no suitable graphics hardware found.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         fprintf(CApplication::m_G.pLog, "error: no compatible graphics adapter \n");
         } 
      fprintf(CApplication::m_G.pLog, "error:: g_MyApp.Init() failed\n");
      fflush(CApplication::m_G.pLog);
      g_MyApp.Done();
      } 
   // everything went smooth
   else ShowWindow(hWnd, SW_SHOW);

   GetCurrentDirectory(MAX_PATH, CApplication::m_G.Dir);

   // main loop
   while (!g_MyApp.IsDone()) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         if (  (CApplication::m_G.hTexDlg == 0) ||
              !(IsDialogMessage(CApplication::m_G.hTexDlg, &msg)) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
         }
      // do one frame
      if ( g_MyApp.IsActive() ) {

         g_MyApp.Update();
         
         // tick the program
         ProgramTick();
         }
      } // main loop
   
   // cleanup stuff     
   g_MyApp.Release();
   UnregisterClass(g_szAppClass, hInst);
   return (int)msg.wParam;
   } // WinMain
/*----------------------------------------------------------------*/


/**
 * MsgProc to deal with incoming windows messages.
 */
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   switch(msg) {

      // our app has the focus
      case WM_ACTIVATE: {
         g_MyApp.Active((bool)wParam);
         } break;

      // track the mouse guy
      case WM_MOUSEMOVE: {
         g_MyApp.OnMouseMove(wParam, lParam);
         return 0;
         } break;

      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN: {
         g_MyApp.OnMouseClick(true, wParam, lParam);
         return 0;
         } break;

      case WM_LBUTTONUP: {
         g_MyApp.OnMouseClick(false, MK_LBUTTON, lParam);
         return 0;
         } break;

      case WM_KEYDOWN: {
         if (LOWORD(wParam) == VK_DELETE)
            g_MyApp.OnDeleteKey();
         else if (LOWORD(wParam) == VK_SPACE)
            SendMessage(CApplication::m_G.hWnd, 
                  WM_COMMAND, IDM_CAM_TO_SEL, 0);
         return 0;
         } break;

      // user input commands
      case WM_COMMAND: {
         // only handle quit message here
         switch (LOWORD(wParam)) {
            case IDM_FILE_EXIT: {
               g_MyApp.Done();
               PostMessage(hWnd, WM_CLOSE, 0, 0);
               return 0;
               } break;
            }
 
         return ProgramProc(hWnd, wParam, lParam);
         } break;

      // catch wm_close for security check
      case WM_CLOSE: {
         g_MyApp.OnClose();
         return 1;
         } break;

      // we are ordered to suicide
      case WM_DESTROY: {
         g_MyApp.Done();
         PostQuitMessage(0);
         return 1;
         } break;

      default: break;
      } // switch
   
   return DefWindowProc(hWnd, msg, wParam, lParam);
   }
/*----------------------------------------------------------------*/

   
/**
 * Do one frame.
 */
HRESULT ProgramTick(void) { 



   return S_OK;
   } // Tick
/*----------------------------------------------------------------*/


/**
 * Handle WM_COMMAND messages from menu, toolbar and GUI. 
 */
LRESULT ProgramProc(HWND hWnd, WPARAM wParam, LPARAM lParam) {
   HMENU hm = GetMenu(hWnd);

   // first try to handle message as toolbar event
   if ( g_MyApp.MsgProcToolbar( LOWORD(wParam) ) ) {
      
      // unfortunately there is no workaround for this
      if ( LOWORD(wParam) == IDTB_MAKE_PORTAL)
         g_MyApp.OnSelBufChange();

      if ( LOWORD(wParam) == IDTB_MAKE_ENTITY)
         g_MyApp.OnSelBufChange();

      return 0;
      }

   // second we give the GUI a change
   if ( g_MyApp.MsgProcGUI( LOWORD(wParam) ) ) {
      // take the focus back after combox was closed
      // otherwise we won't get WM_KEYDOWN messages
      if ( (LOWORD(wParam) == IDC_SECTOR) &&
           (HIWORD(wParam) == CBN_CLOSEUP)  )
           SetFocus(hWnd);
      if ( (LOWORD(wParam) == IDC_SKLIST) &&
           (HIWORD(wParam) == CBN_CLOSEUP)  )
           SetFocus(hWnd);
      return 0;
      }

   // now try to handle message as menue event
   if ( g_MyApp.MsgProcMenu( LOWORD(wParam) ) ) return 0;

   return 0;
   } // ProgramProc
/*----------------------------------------------------------------*/


