// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "include/ZFXRenderer.h"            // our render interface
#include "include/ZFX.h"                    // return values and stuff
#include "include/main.h"                   // prototypes and stuff
#include "resource.h"
 
//include our library
#pragma comment(lib, "libs/ZFXRenderer.lib")
#pragma comment(lib, "libs/ZFXInput.lib")
#pragma comment(lib, "libs/ZFXAudio.lib")
#pragma comment(lib, "libs/ZFXNetwork.lib")
#pragma comment(lib, "libs/ZFXGeneral.lib")
#pragma comment(lib, "libs/zfx3d.lib")
 
 
// windows stuff
TCHAR  g_szAppClass[] = TEXT("FrameWorktest");
char   g_chDir[MAX_PATH];


ZFXNETMODE g_nmd=NMD_CLIENT;
char       g_chIP[16] = "";
char       g_nPort=0;

CGame  g_MyGame;


/**
 * WinMain function to get the thing started.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
   WNDCLASSEX	wndclass;
   HRESULT     hr;
   HWND		   hWnd;
   MSG			msg = { 0, 0, 0 };
   POINT       ptCur = { 10, 10 };

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
	   		                   "Pandoras Legacy - Alpha Version",
				                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 - 400,
                               GetSystemMetrics(SM_CYSCREEN)/2 - 300,
                               800, 600, 
                               NULL, NULL, hInst, NULL)))
      return 0;

   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);

   DialogBox(hInst, "ZFXSelectionDialog", hWnd, DlgProc);

   // init 3dmath lib
   ZFX3DInitCPU();

   // try to start the engine
   if (FAILED(hr = g_MyGame.Init(hWnd, hInst, g_nmd, g_nPort, g_chIP))) {
      if (hr == ZFX_FAIL)
         MessageBox(hWnd, 
                    "Sorry, CGame::Init failed.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
      else if (hr == ZFX_CANCELED)
         /* this is okay */;
      else if (hr == ZFX_NOTCOMPATIBLE) {
         MessageBox(hWnd, 
                    "Sorry, no suitable graphics hardware found.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         } 
      else if (hr == ZFX_NOSERVERFOUND) {
         MessageBox(hWnd, 
                    "Sorry, server was not found or is full.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         }
      g_MyGame.Done();
      } 
   // everything went smooth
   else {
      ShowWindow(hWnd, SW_SHOW);
      ShowCursor(false);
      g_MyGame.Active(true);
      }

   GetCurrentDirectory(MAX_PATH, g_chDir);

   // main loop
   while (!g_MyGame.IsDone()) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         }
      // do one frame
      if ( g_MyGame.IsActive() ) {

         if ( g_MyGame.IsWindowed() ) {
            ClientToScreen(hWnd, &ptCur);
            SetCursorPos(ptCur.x, ptCur.y);
            }

         g_MyGame.Update();
         g_MyGame.Render();
         }
      } // main loop

   // cleanup stuff     
   g_MyGame.Release();
   UnregisterClass(g_szAppClass, hInst);
   return (int)msg.wParam;
   } // WinMain
/*----------------------------------------------------------------*/


/**
 * MsgProc to deal with incoming windows messages.
 */
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   HRESULT hr=ZFX_OK;

   switch(msg) {
/*
      // our app has the focus
      case WM_ACTIVATE: {
         g_MyGame.Active((bool)wParam);
         } break;
*/
      // we are ordered to suicide
      case WM_DESTROY: {
         g_MyGame.Done();
         PostQuitMessage(0);
         return 1;
         } break;

      // network msg to be handled
      case WM_ZFXSERVER: case WM_ZFXCLIENT: {
         hr = g_MyGame.MsgProcNetwork(wParam, lParam);
         if (FAILED(hr)) {
            g_MyGame.Done();
            PostQuitMessage(0);
            }
         } break;

      default: break;
      } // switch
   
   return DefWindowProc(hWnd, msg, wParam, lParam);
   }
/*----------------------------------------------------------------*/


/**
 * Callback function to handle the selection dialog
 */
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
   char buf1[5], buf2[5], buf3[5], buf4[5], bufp[6];

   HWND hIP1    = GetDlgItem(hDlg, IDC_IP1);
   HWND hIP2    = GetDlgItem(hDlg, IDC_IP2);
   HWND hIP3    = GetDlgItem(hDlg, IDC_IP3);
   HWND hIP4    = GetDlgItem(hDlg, IDC_IP4);
   HWND hPort   = GetDlgItem(hDlg, IDC_PORT);
   HWND hServer = GetDlgItem(hDlg, IDSERVER);
   HWND hClient = GetDlgItem(hDlg, IDCLIENT);

   // get the text from IP and port input controls
   GetWindowText(hIP1,  buf1, 5);
   GetWindowText(hIP2,  buf2, 5);
   GetWindowText(hIP3,  buf3, 5);
   GetWindowText(hIP4,  buf4, 5);
   GetWindowText(hPort, bufp, 6);

   // for a server we need at least a port number
   if (strlen(bufp)>0) EnableWindow(hServer, true);
   else EnableWindow(hServer, false);

   // for a client we need both, port number and server IP
   if ( (strlen(buf1)>0) &&
        (strlen(buf2)>0) &&
        (strlen(buf3)>0) &&
        (strlen(buf4)>0) ) 
      EnableWindow(hClient, true);
   else EnableWindow(hClient, false);

   switch (msg) {
      case WM_INITDIALOG: {
         SetWindowText(hIP1,  "192");
         SetWindowText(hIP2,  "168");
         SetWindowText(hIP3,  "123");
         SetWindowText(hPort, "8000");
         } break;

      // control changed
      case WM_COMMAND: {
         switch (LOWORD(wParam)) {
            // take care that each IP field has max length of 3
            // and that the port number has only a length of 4
            case IDC_IP1: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP1);
               return 1;
               } break;
            case IDC_IP2: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP2);
               return 1;
               } break;
            case IDC_IP3: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP3);
               return 1;
               } break;
            case IDC_IP4: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP4);
               return 1;
               } break;
            case IDC_PORT: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(bufp, 4, hPort);
               return 1;
               } break;

            // button for client is pressed
            case IDCLIENT: {
               g_nmd = NMD_CLIENT;
               sprintf(g_chIP, "%s.%s.%s.%s", buf1,buf2,buf3,buf4);
               sscanf(bufp, "%d", &g_nPort);
               EndDialog(hDlg, 1);
               return 1;
               } break;

            // button for server is pressed
            case IDSERVER: {
               g_nmd = NMD_SERVER;
               memset(g_chIP, 0, sizeof(g_chIP));
               sscanf(bufp, "%d", &g_nPort);
               EndDialog(hDlg, 1);
               return 1;
               } break;
            } // switch [CMD]
         } break; // case [CMD]
      } // switch [MSG]
   return 0;
   } // DlgProc
/*----------------------------------------------------------------*/
   

/**
 * Checks whether the length of the string is not more than
 * allowed and cuts the string for the control if neccessary.
 */
void CheckInputLength(char *buf, UINT nLength, HWND hCtrl) {
   GetWindowText(hCtrl, buf, 6);
   if (strlen(buf)>nLength) {
      buf[nLength] = '\0';
      SetWindowText(hCtrl, buf);
      }
   } // CheckInputLength
/*----------------------------------------------------------------*/

