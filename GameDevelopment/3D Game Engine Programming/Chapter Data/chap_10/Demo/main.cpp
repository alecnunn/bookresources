// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "windows.h"
#include "ZFXAudio.h"  // our audio interface
#include "ZFX.h"       // return values and stuff
#include "main.h"

//include our library
#pragma comment(lib, "ZFXAudio.lib")
//#pragma comment(lib, "zfx3d.lib")


// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;

FILE *pLog=NULL;


UINT g_iSoundID=0;


OPENFILENAME g_ofn;

// input objects
LPZFXAUDIO       g_pAudio  = NULL;
LPZFXAUDIODEVICE g_pDevice = NULL;


#define STC(a,x,y,xd,yd,id) CreateWindow("static",a,dwChild,x,y,xd,yd,g_hWnd,(HMENU)id,g_hInst,NULL);
#define MSGBX(a) MessageBox(NULL,a,"ZFXEngine report",MB_OK|MB_ICONINFORMATION)


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
	   		                   "ZFXEngine v2.0 - Demo Application",
				                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -115,
                               GetSystemMetrics(SM_CYSCREEN)/2 -60,
				                   230, 120, NULL, NULL, hInst, NULL)))
      return 0;
   
   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");

   // try to start the engine
   if (FAILED( hr = ProgramStartup() )) {
      fprintf(pLog, "error: ProgramStartup() failed\n");
      fflush(pLog);
      g_bDone = true;
      }
   // everything went smooth
   else {
      ShowWindow(hWnd, SW_SHOW);
      CreateGUI();
      }

   // main loop
   while (!g_bDone) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
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

      case WM_COMMAND: {
         switch ( LOWORD(wParam) ) {
            // play the last loaded sound
            case IDC_hPlay: {
               
               g_pDevice->PlaySound( g_iSoundID, false );
               
               } break;

            default: break;
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
HRESULT ProgramStartup(void) {
   char Buffer[MAX_PATH];

   if ( !GetCurrentDirectory( MAX_PATH-1, Buffer ) )
      {
      return ZFX_FAIL;
      }


   // create an audio objekt
   g_pAudio = new ZFXAudio(g_hInst);

   // create an audio device object
   if ( FAILED( g_pAudio->CreateDevice() )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pAudio->GetDevice();
   if(g_pDevice == NULL) return E_FAIL;

   // init audio device
   if (FAILED( g_pDevice->Init(g_hWnd, Buffer, true) )) {
      fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
      return E_FAIL;
      }

   g_pDevice->LoadSound("sound10.wav", &g_iSoundID);

   return ZFX_OK;
   } // ProgramStartup
/*----------------------------------------------------------------*/


/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {
   if (g_pAudio) {
      delete g_pAudio;
      g_pAudio = NULL;
      }
   return S_OK;
   } // ProgramCleanup
/*----------------------------------------------------------------*/


/**
 * Build up GUI.
 */
void CreateGUI(void) {
   DWORD dwChild = WS_CHILD | WS_VISIBLE;


   STC("ZFX AUDIO DEMO PROGRAM",15,10,400,20,IDC_hText);

   hPlay  = CreateWindow("BUTTON", "Play Sound", WS_CHILD|WS_VISIBLE,
                         65, 50, 100, 25, g_hWnd, 
                         (HMENU)IDC_hPlay, g_hInst, NULL);
   } // CreateGUI
/*----------------------------------------------------------------*/


/**
 * Convert error code to generic text string.
 */
TCHAR* HrToStr(HRESULT hr) {
   switch (hr) {
      case ZFX_OK:               return TEXT("ZFX_OK");
      case ZFX_FAIL:             return TEXT("ZFX_FAIL");
      case ZFX_CREATEAPI:        return TEXT("ZFX_CREATEAPI");
      case ZFX_CREATEDEVICE:     return TEXT("ZFX_CREATEDEVICE");
      case ZFX_INVALIDPARAM:     return TEXT("ZFX_INVALIDPARAM");
      case ZFX_INVALIDID:        return TEXT("ZFX_INVALIDID");
      case ZFX_BUFFERSIZE:       return TEXT("ZFX_BUFFERSIZE");
      case ZFX_BUFFERLOCK:       return TEXT("ZFX_BUFFERLOCK");
      case ZFX_NOTCOMPATIBLE:    return TEXT("ZFX_NOTCOMPATIBLE");
      case ZFX_OUTOFMEMORY:      return TEXT("ZFX_OUTOFMEMORY");
      case ZFX_FILENOTFOUND:     return TEXT("ZFX_FILENOTFOUND");
      case ZFX_INVALIDFILE:      return TEXT("ZFX_INVALIDFILE");
      default: return TEXT("unknown");
      }
   } // HrToStr
/*----------------------------------------------------------------*/


