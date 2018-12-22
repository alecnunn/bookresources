// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "ZFXRenderer.h"  // our render interface
#include "ZFX.h"          // return values and stuff
#include "main.h"         // prototypes and stuff

//include our library
#pragma comment(lib, "ZFXRenderer.lib")


// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;

FILE *pLog=NULL;


// renderer object
LPZFXRENDERER     g_pRenderer = NULL;
LPZFXRENDERDEVICE g_pDevice   = NULL;


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
	   		                   "Crancking up ZFXEngine...",
				                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -190,
                               GetSystemMetrics(SM_CYSCREEN)/2 -140,
				                   380, 280, NULL, NULL, hInst, NULL)))
      return 0;
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");

   // try to start the engine
   if (FAILED( hr = ProgramStartup("Direct3D"))) {
      fprintf(pLog, "error: ProgramStartup() failed\n");
      g_bDone = true;
      }
   else if (hr == ZFX_CANCELED) {
      fprintf(pLog, "error: ProgramStartup() canceled by user \n");
      g_bDone = true;
      }
   else 
      g_pDevice->SetClearColor(0.1f, 0.3f, 0.1f);

   while (!g_bDone)
      {
      while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	      {
		 	TranslateMessage(&msg);
			DispatchMessage(&msg);
         }

      if (g_bIsActive)
         {  
         if (g_pDevice->IsRunning()) {
            g_pDevice->BeginRendering(true,true,true);
            g_pDevice->EndRendering();
            }
         }
      }
   
   // cleanup stuff
   ProgramCleanup();

   UnregisterClass(g_szAppClass, hInst);
   
   // return back to windows
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
         g_bIsActive = (BOOL)wParam;
         } break;
         
      // key was pressed
      case WM_KEYDOWN: {
         switch (wParam) {
            case VK_ESCAPE: {
               g_bDone = true;
               PostMessage(hWnd, WM_CLOSE, 0, 0);
               return 0;
               } break;
            }
         } break;

      // we are ordered to suicide
      case WM_DESTROY: {
         g_bDone = true;
         PostQuitMessage(0);
         return 1;
         } break;

      default: break;
      }
   
   return DefWindowProc(hWnd, msg, wParam, lParam);
   }
/*----------------------------------------------------------------*/
 
/**
 * Create a render device and stuff. 
 */
HRESULT ProgramStartup(char *chAPI) {
   HWND hWnd3D[4];
   RECT rcWnd;
   int  x=0,y=0;
   
   // no opengl render device yet...
   if (strcmp(chAPI, "OpenGL")==0) return S_OK;

   // create a render objekt
   g_pRenderer = new ZFXRenderer(g_hInst);
   
   // create a device for the chosen api
   if (FAILED( g_pRenderer->CreateDevice(chAPI) )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pRenderer->GetDevice();
   if(g_pDevice == NULL) return E_FAIL;

   // build for child windows
   GetClientRect(g_hWnd, &rcWnd);

   for (int i=0; i<4; i++) {
      if ( (i==0) || (i==2) ) x = 10;
      else x = rcWnd.right/2 + 10;

      if ( (i==0) || (i==1) ) y = 10;
      else y = rcWnd.bottom/2 + 10;

      hWnd3D[i] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("static"), 
                           NULL, WS_CHILD | SS_BLACKRECT | WS_VISIBLE,
                           x, y, rcWnd.right/2-20, rcWnd.bottom/2-20, 
                           g_hWnd, NULL, g_hInst, NULL);
      }

   // init render device
   return g_pDevice->Init(g_hWnd, hWnd3D, 4, 16, 0, false);
   } // ProgramStartup
/*----------------------------------------------------------------*/

/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {
   if (g_pRenderer) {
      delete g_pRenderer;
      g_pRenderer = NULL;
      }

   if (pLog)
      fclose(pLog);

   return S_OK;
   } // ProgramCleanup
/*----------------------------------------------------------------*/

