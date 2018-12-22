// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "windows.h"
#include "ZFXInput.h"  // our render interface
#include "ZFX.h"       // return values and stuff
#include "main.h"

//include our library
#pragma comment(lib, "ZFXInput.lib")
//#pragma comment(lib, "zfx3d.lib")


// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;

FILE *pLog=NULL;


// input objects
LPZFXINPUT       g_pInput  = NULL;
LPZFXINPUTDEVICE g_pDevice = NULL;


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
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -225,
                               GetSystemMetrics(SM_CYSCREEN)/2 -150,
				                   450, 300, NULL, NULL, hInst, NULL)))
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
      // do one frame
      if (g_bIsActive) {
         ProgramTick();
         if (g_pDevice->IsPressed(IDV_KEYBOARD, ZVK_ESCAPE)) {
            MSGBX("ESCAPE key was pressed, exiting program...");
            g_bDone = true;
            PostMessage(g_hWnd, WM_CLOSE, 0, 0);
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
         /*
         switch (wParam) {
            case VK_ESCAPE: {
               g_bDone = true;
               PostMessage(hWnd, WM_CLOSE, 0, 0);
               return 0;
               } break;
            }
         */
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
   RECT rcCage = { 0, 0, 0, 0 };

   // build mouse cage
   HDC hdc = GetDC(NULL);
   rcCage.right  = GetDeviceCaps(hdc, HORZRES) - 1;
   rcCage.bottom = GetDeviceCaps(hdc, VERTRES) - 1;
   ReleaseDC(NULL, hdc);

   // create an input objekt
   g_pInput = new ZFXInput(g_hInst);
   
   // create an input device object
   if (FAILED( g_pInput->CreateDevice() )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pInput->GetDevice();
   if(g_pDevice == NULL) return E_FAIL;

   // init input device
   if (FAILED( g_pDevice->Init(g_hWnd, rcCage, true) )) {
      fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
      return E_FAIL;
      }

   return ZFX_OK;
   } // ProgramStartup
/*----------------------------------------------------------------*/


/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {
   if (g_pInput) {
      delete g_pInput;
      g_pInput = NULL;
      }
   return S_OK;
   } // ProgramCleanup
/*----------------------------------------------------------------*/


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {
   HRESULT hr = ZFX_OK;
   POINT   pt;
   char    buf[400];
 
   // update input devices
   if (FAILED(g_pDevice->Update()))
      fprintf(pLog, "error: Update() failed\n");

   // print out mouse position
   g_pDevice->GetPosition(IDV_MOUSE, &pt);
   sprintf(buf, "Mouse: (%d/%d)", pt.x, pt.y);
   SetWindowText(hMousePos, buf);

   // print out joystick values
   g_pDevice->GetPosition(IDV_JOYSTICK, &pt);
   sprintf(buf, "Joystick: (%d/%d)", pt.x, pt.y);
   SetWindowText(hJoyPos, buf);

  
   // MOUSE BUTTONS /////////////////////////////////
   if (g_pDevice->IsPressed(IDV_MOUSE, 0))
      SetWindowText(hMouseBtn0, "1");
   else {
      SetWindowText(hMouseBtn0, "0");
      if (g_pDevice->IsReleased(IDV_MOUSE, 0))
         MSGBX("mouse button 0 released");
      }

   if (g_pDevice->IsPressed(IDV_MOUSE, 1))
      SetWindowText(hMouseBtn1, "1");
   else {
      SetWindowText(hMouseBtn1, "0");
      if (g_pDevice->IsReleased(IDV_MOUSE, 1))
         MSGBX("mouse button 1 released");
      }

   // JOYSTICK BUTTONS //////////////////////////////
   if (g_pDevice->IsPressed(IDV_JOYSTICK, 0))
      SetWindowText(hJoyBtn0, "1");
   else {
      SetWindowText(hJoyBtn0, "0");
      if (g_pDevice->IsReleased(IDV_JOYSTICK, 0))
         MSGBX("joystick button 0 released");
      }

   if (g_pDevice->IsPressed(IDV_JOYSTICK, 1))
      SetWindowText(hJoyBtn1, "1");
   else {
      SetWindowText(hJoyBtn1, "0");
      if (g_pDevice->IsReleased(IDV_JOYSTICK, 1))
         MSGBX("joystick button 1 released");
      }

   if (g_pDevice->IsPressed(IDV_JOYSTICK, 2))
      SetWindowText(hJoyBtn2, "1");
   else {
      SetWindowText(hJoyBtn2, "0");
      if (g_pDevice->IsReleased(IDV_JOYSTICK, 2))
         MSGBX("joystick button 2 released");
      }

   if (g_pDevice->IsPressed(IDV_JOYSTICK, 3))
      SetWindowText(hJoyBtn3, "1");
   else {
      SetWindowText(hJoyBtn3, "0");
      if (g_pDevice->IsReleased(IDV_JOYSTICK, 3))
         MSGBX("joystick button 3 released");
      }


   // KEYBOARD KEYS /////////////////////////////////
   if (g_pDevice->IsPressed(IDV_KEYBOARD, ZVK_RETURN))
      MSGBX("RETURN key was pressed");
   if (g_pDevice->IsPressed(IDV_KEYBOARD, ZVK_BACK))
      MSGBX("BACKSPACE key was pressed");
   if (g_pDevice->IsReleased(IDV_KEYBOARD, ZVK_SPACE))
      MSGBX("SPACEBAR was released");
   if (g_pDevice->IsReleased(IDV_KEYBOARD, ZVK_RSHIFT))
      MSGBX("RIGHTSHIFT key was released");

   return hr;
   } // Tick
/*----------------------------------------------------------------*/


/**
 * Build up GUI. Make sure to init Input.dll beforehand
 */
void CreateGUI(void) {
   DWORD dwChild = WS_CHILD | WS_VISIBLE;
   char  chName[256];
   char  buffer[270];

   STC("ZFX INPUT DEMO PROGRAM",10,10,400,20,IDC_hText1);
   STC("Keyboard and mouse online and ready",10,45,450,20,IDC_hText2);

   if (g_pDevice->HasJoystick(chName)) {
      sprintf(buffer, "Joystick: \"%s\"", chName);
      STC(buffer,10,70,450,20,IDC_hJoytext);
      }
   else
      STC("Joystick: no attached joystick found",10,70,450,20,IDC_hJoytext);

   hMousePos = STC("Mouse:",10,110,170,20,IDC_hMousePos);
   STC("Buttons: ",180,110,250,20, IDC_hDummy2);
   hMouseBtn0  = STC("0",260,110,250,20,IDC_hMouseBtn0);
   hMouseBtn1  = STC("0",275,110,250,20,IDC_hMouseBtn1);

   hJoyPos   = STC("Joystick:",10,135,170,20,IDC_hJoyPos);
   STC("Buttons: ",180,135,250,20, IDC_hDummy1);
   hJoyBtn0  = STC("0",260,135,250,20,IDC_hJoyBtn0);
   hJoyBtn1  = STC("0",275,135,250,20,IDC_hJoyBtn1);
   hJoyBtn2  = STC("0",290,135,250,20,IDC_hJoyBtn2);
   hJoyBtn3  = STC("0",305,135,250,20,IDC_hJoyBtn3);

   STC("Try and click mouse or joystick buttons",10,170,450,20,IDC_hText3);
   STC("Please also try RETURN and BACKSPACE on press",10,195,450,20,IDC_hText4);
   STC("And try SPACE and RIGHTSHIFT on release",10,220,450,20,IDC_hText5);
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
