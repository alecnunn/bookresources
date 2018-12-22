// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "windows.h"     // windows stuff
#include "ZFXNetwork.h"  // our network interface
#include "ZFX.h"         // return values and stuff
#include "main.h"        // header for this file
#include "resource.h"    // dialogbox resource

//include our library
#pragma comment(lib, "ZFXNetwork.lib")
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
LPZFXNETWORK       g_pNetwork  = NULL;
LPZFXNETWORKDEVICE g_pDevice   = NULL;

ZFXNETMODE g_nmd=NMD_CLIENT;
char       g_chIP[16];
UINT       g_nPort;
UINT       g_ID=0;

char g_chServerTitle[] = "ZFXEngine v2.0 - Demo Application (Server)";
char g_chClientTitle[] = "ZFXEngine v2.0 - Demo Application (Client)";


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
   
   // now show off
   ShowWindow(hWnd, SW_SHOW);
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");

   CreateGUI();

   // call the dialog box to ask about client or server
   DialogBox(hInst, "ZFXSelectionDialog", hWnd, DlgProc);
   UpdateWindow(g_hWnd);

   // try to start the engine
   if (FAILED( hr = ProgramStartup(g_nmd) )) {
      fprintf(pLog, "error: ProgramStartup() failed\n");
      fflush(pLog);
      if (g_nmd==NMD_CLIENT) 
         MSGBX("Could not find a server.\nExiting application now.");
      else 
         MSGBX("Server initialization failed.\nExiting application now.");
      g_bDone = true;
      }
   // everything went smooth
   else {
      EnableWindow(hText, true);
      EnableWindow(hBtn,  true);
      SetWindowText(hText, "");
      }

   // place to store data
   ZFXPACKAGE     msgPkg;
   char           buffer[5000];
   char           temp[300];
   msgPkg.pData = buffer;


   // main loop
   while (!g_bDone) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         }
      
      /* RUN FORREST, RUN!!! */;
      while (g_pDevice->IsPkgWaiting()) {
         if (FAILED(g_pDevice->GetNextPkg(&msgPkg))) {
            fprintf(pLog, "GetNextPkg() failed\n");
            fflush(pLog);
            }
         else {
            // receiving our ID from server
            if (msgPkg.nType == 0) {
               g_ID = *((UINT*)msgPkg.pData);
               }
            // be advised that a new client has connected
            else if (msgPkg.nType == 1) {
               sprintf(temp, "new client has connected <%d>", *((UINT*)msgPkg.pData));
               SendMessage(hLbx,LB_ADDSTRING,0,(LPARAM)temp);
               }
            // be advised that a client has disconnected
            else if (msgPkg.nType == 2) {
               sprintf(temp, "client <%d> has disconnected", *((UINT*)msgPkg.pData));
               SendMessage(hLbx,LB_ADDSTRING,0,(LPARAM)temp);
               }
            // receiving chat message
            else {
               SendMessage(hLbx,LB_ADDSTRING,0,(LPARAM)msgPkg.pData);
               if (g_nmd==NMD_SERVER) {
                  g_pDevice->SendToClients(&msgPkg);
                  }
               }
            int n=SendMessage(hLbx,LB_GETCOUNT,0,0);
            SendMessage(hLbx,LB_SETCURSEL,(WPARAM)n-1,0);
            }
         } // while msg incoming
      } // main loop
   
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

      case WM_ZFXSERVER: case WM_ZFXCLIENT: {
         g_pDevice->MsgProc(wParam, lParam);
         } break;

      // check if "send" button was pressed
      case WM_COMMAND: {
         switch (LOWORD(wParam)) {
            case IDC_hBtn: {
               ZFXPACKAGE     msgPkg;
               char           buffer[128];
               char           buf[200];
               msgPkg.pData = buf;

               GetWindowText(hText, buffer, 128);
               SetWindowText(hText, "");
               if (strcmp(buffer, "exit")==0) {
                  g_bDone = true;
                  PostMessage(hWnd, WM_CLOSE, 0, 0);
                  }
               else {
                  sprintf((char*)msgPkg.pData, "<%d> %s", g_ID, buffer);
                  // add 1 for terminating 0 character
                  msgPkg.nLength   = strlen((char*)msgPkg.pData)+1;
                  msgPkg.nType     = 3;
                  msgPkg.nSender   = g_ID;

                  // SERVER-CODE
                  if (g_nmd==NMD_SERVER) {
                     SendMessage(hLbx,LB_ADDSTRING,0,(LPARAM)msgPkg.pData);
                     int n=SendMessage(hLbx,LB_GETCOUNT,0,0);
                     SendMessage(hLbx,LB_SETCURSEL,(WPARAM)n-1,0);
                     if (FAILED(g_pDevice->SendToClients(&msgPkg)))
                        fprintf(pLog, "error: SendToCLients() failed");
                     }

                  // CLIENT CODE
                  else {
                     if (FAILED(g_pDevice->SendToServer(&msgPkg)))
                        fprintf(pLog, "error: SendToServer() failed");
                     }
                  }
               return 0;
               } break;
            } // switch [CMD]
         return 0;
         } break; // WM_COMMAND

      default: break;
      }
   
   return DefWindowProc(hWnd, msg, wParam, lParam);
   }
/*----------------------------------------------------------------*/
 

/**
 * Create a network device and stuff. 
 */
HRESULT ProgramStartup(ZFXNETMODE nmd) {
   // create a network object
   g_pNetwork = new ZFXNetwork(g_hInst);
   
   // create a network device object to work with
   if (FAILED( g_pNetwork->CreateDevice() )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pNetwork->GetDevice();
   if(g_pDevice == NULL) return E_FAIL;

   // init input device
   if (FAILED( g_pDevice->Init(g_hWnd, nmd, g_nPort, g_chIP, 5000, 64, true) )) {
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
   if (g_pNetwork) {
      delete g_pNetwork;
      g_pNetwork = NULL;
      }
   return S_OK;
   } // ProgramCleanup
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
      // control changed
      case WM_COMMAND: {
         switch (LOWORD(wParam)) {
            // take care that each IP field has max length of 3
            // and that the port number has only a length of 4
            case IDC_IP1: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP1);
               return TRUE;
               } break;
            case IDC_IP2: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP2);
               return TRUE;
               } break;
            case IDC_IP3: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP3);
               return TRUE;
               } break;
            case IDC_IP4: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(buf1, 3, hIP4);
               return TRUE;
               } break;
            case IDC_PORT: {
               if (HIWORD(wParam)==EN_UPDATE)
                  CheckInputLength(bufp, 4, hPort);
               return TRUE;
               } break;

            // button for client is pressed
            case IDCLIENT: {
               g_nmd = NMD_CLIENT;
               sprintf(g_chIP, "%s.%s.%s.%s", buf1,buf2,buf3,buf4);
               sscanf(bufp, "%d", &g_nPort);
               SetWindowText(hText, "Searching for Server...");
               SetWindowText(g_hWnd, g_chClientTitle);
               EndDialog(hDlg, 1);
               return TRUE;
               } break;

            // button for server is pressed
            case IDSERVER: {
               g_nmd = NMD_SERVER;
               memset(g_chIP, 0, sizeof(g_chIP));
               sscanf(bufp, "%d", &g_nPort);
               SetWindowText(hText, "Waiting for client to connect...");
               SetWindowText(g_hWnd, g_chServerTitle);
               EndDialog(hDlg, 1);
               return TRUE;
               } break;
            } // switch [CMD]
         } break; // case [CMD]
      } // switch [MSG]
   return FALSE;
   }
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
   }
/*----------------------------------------------------------------*/


/**
 * Build up GUI to input some text.
 */
void CreateGUI(void) {
   DWORD dwChild = WS_CHILD | WS_VISIBLE;
   hLbx   = LBX("",10,10,430,185,IDC_hLbx);
   hBtn   = BTN("Send",360,230,80,30,IDC_hBtn);
   hLabel = STC("Write up to 128 chars and click on send:",10,205,340,20,IDC_hLabel);
   hText  = EDT("", 10, 232, 340, 26, IDC_hText);
   EnableWindow(hText, false);
   EnableWindow(hBtn,  false);
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
