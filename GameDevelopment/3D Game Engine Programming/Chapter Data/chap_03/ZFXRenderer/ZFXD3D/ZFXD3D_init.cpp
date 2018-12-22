// File: ZFXD3D_init.cpp

#include <windows.h>       // type definitions
#include <vfw.h>           // show bmp's
#include "resource.h"      // control id's
#include "ZFX.h"           // return values and stuff
#include "ZFXD3D.h"        // class definition

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")


// we need this for dialog callback only
ZFXD3D *g_ZFXD3D=NULL;

// store result when dialog is closed
ZFXDEVICEINFO  g_xDevice;
D3DDISPLAYMODE g_Dspmd;
D3DFORMAT      g_fmtA;
D3DFORMAT      g_fmtB;
HBITMAP        g_hBMP;



/*-----------------------------------------------------------*/
/* DLL stuff implementation                                  *
/*-----------------------------------------------------------*/

/**
 * DLL Entry Point similar to WinMain()/main()
 */
BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
                          DWORD     fdwReason,
                          LPVOID    lpvReserved) {
   switch(fdwReason) {
      // called when we attach to the DLL
      case DLL_PROCESS_ATTACH:
         /* dll init/setup stuff */
         break;
      case DLL_PROCESS_DETACH:
         /* dll shutdown/release stuff */
         break;
      default:
         break;
      };
   
   return TRUE;
   } // DllEntryPoint
/*----------------------------------------------------------------*/

/**
 * Exported create function: Creates a new ZFXRenderDevice object.
 */
HRESULT CreateRenderDevice(HINSTANCE hDLL, ZFXRenderDevice **pDevice) {
   if(!*pDevice) {
      *pDevice = new ZFXD3D(hDLL);
      return ZFX_OK;
      }
   return ZFX_FAIL;
   }
/*----------------------------------------------------------------*/

/**
 * Exported release function: Realeses the given ZFXRenderDevice object.
 */
HRESULT ReleaseRenderDevice(ZFXRenderDevice **pDevice) {
   if(!*pDevice) {
      return ZFX_FAIL;
      }
   delete *pDevice;
   *pDevice = NULL;
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/



// give helping hand to class implementation of callback function
BOOL CALLBACK DlgProcWrap(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
   { return g_ZFXD3D->DlgProc(hDlg, message, wParam, lParam); }
/*----------------------------------------------------------------*/



/*-----------------------------------------------------------*/
/* ZFXD3D class implementation                               *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXD3D::ZFXD3D(HINSTANCE hDLL) {
   m_hDLL              = hDLL;
   m_pEnum             = NULL;
   m_pD3D              = NULL;
   m_pDevice           = NULL;
   m_pLog              = NULL;
   m_ClearColor        = D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f);
   m_bRunning          = false;
   m_bIsSceneRunning   = false;

   // dont use swapchain at first
   m_nActivehWnd       = 0;

   g_ZFXD3D = this;
   }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXD3D::~ZFXD3D() {
   Log("shutting down direct3d \n");
   if (m_pLog) fflush(m_pLog);
   this->Release();
   }
/*----------------------------------------------------------------*/


/**
 * Release all the Direct3D COM stuff.
 */
void ZFXD3D::Release() {
   if (m_pEnum) {
      delete m_pEnum;
      m_pEnum = NULL;
      }
   if(m_pDevice) {
      m_pDevice->Release();
      m_pDevice = NULL;
      }
   if(m_pD3D) {
      m_pD3D->Release();
      m_pD3D = NULL;
      }
   Log("shutdown completed \n");
   fclose(m_pLog);
   }
/*----------------------------------------------------------------*/


/**
 * Initialize dialogbox to select device and format. Dialog must
 * have ID using -"- signs and must have combobox "IDC_ADAPTER",
 * listbox "IDC_MODES", radiobuttons "IDC_FULL" and "IDC_WND",
 * and Buttons "IDOK" and "IDCANCEL".
 * -> IN: HWND      - handle of application window
 *        HWND      - array to render child-window HWNDs or NULL
 *        int       - number of child HWNDs in array
 *        int       - minimum number of depth bits
 *        int       - minimum number of stencil bits
 *        bool      - create a save but slow log?
 */
HRESULT ZFXD3D::Init(HWND hWnd, const HWND *hWnd3D, 
                     int nNumhWnd, int nMinDepth, 
                     int nMinStencil, bool bSaveLog) {
   int nResult;

   m_pLog = fopen("log_renderdevice.txt", "w");
   if (!m_pLog) return ZFX_FAIL;
   
   // are there any child windows to use?
   if (nNumhWnd > 0) {
      if (nNumhWnd > MAX_3DHWND) nNumhWnd = MAX_3DHWND;
      memcpy(&m_hWnd[0], hWnd3D, sizeof(HWND)*nNumhWnd);
      m_nNumhWnd = nNumhWnd;
      }
   // else store handle to main window at least
   else {
      m_hWnd[0] = hWnd;
      m_nNumhWnd = 0;
      }
   m_hWndMain = hWnd;

   if (nMinStencil > 0)
      m_bStencil = true;

   Log("ZFXEngine ZFXD3D-RenderDevice Log File:\n\n");

   // create enum object
   m_pEnum = new ZFXD3DEnum(nMinDepth, nMinStencil);

   Log("calling dialog... \n");

   // load bmp logo
   g_hBMP = (HBITMAP)LoadImage(NULL, "zfx.bmp",
                               IMAGE_BITMAP,0,0,
                               LR_LOADFROMFILE |
                               LR_CREATEDIBSECTION);

   // call device selection dialog
   nResult = DialogBox(m_hDLL, "dlgChangeDevice", hWnd, DlgProcWrap);
   
   // delete bmp if any
   if (g_hBMP) DeleteObject(g_hBMP);

   Log("returning from dialog... \n");

   // dialog failed somehow
   if (nResult == -1) {
      Log("selection dialog error \n");
      return ZFX_FAIL;
      }
   // dialog was canceled by user
   else if (nResult == 0) {
      Log("selection dialog canceled by user\n");
      return ZFX_CANCELED;
      }
   // dialog ended with ok button
   else {
      Log("selection dialog ok\nfiring up direct3d \n");
      return Go();
      }
   }
/*----------------------------------------------------------------*/


/**
 * -> IN: HWND      - handle of application window
 *        HWND      - array to render child-window HWNDs or NULL
 *        int       - number of child HWNDs in array
 *        bool      - create a save but slow log?
 */
HRESULT ZFXD3D::InitWindowed(HWND hWnd, const HWND *hWnd3D, 
                             int nNumhWnd, bool bSaveLog) {
   HRESULT hr;
   HWND    hwnd;
   
   // are there any child windows to use?
   if (nNumhWnd > 0) {
      if (nNumhWnd > MAX_3DHWND) nNumhWnd = MAX_3DHWND;
      memcpy(&m_hWnd[0], hWnd3D, sizeof(HWND)*nNumhWnd);
      m_nNumhWnd = nNumhWnd;
      }
   // else store handle to main window at least
   else {
      m_hWnd[0] = hWnd;
      m_nNumhWnd = 0;
      }
 
   m_hWndMain  = hWnd;
   m_bWindowed = true;

   // build main direct3d object
   if (m_pD3D) {
      m_pD3D->Release();
      m_pD3D = NULL;
      }
   m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
   if(!m_pD3D) {
      Log("error: Direct3DCreate8()");
      return ZFX_CREATEAPI;
      }

   // prepare present parameters structure
   ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
   m_d3dpp.Windowed               = m_bWindowed;
   m_d3dpp.BackBufferCount        = 1;
   m_d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
   m_d3dpp.EnableAutoDepthStencil = TRUE;
   m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
   m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
   m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
   m_bStencil = false;

   // windowed mode
   m_d3dpp.hDeviceWindow     = hwnd = m_hWnd[0];
   m_d3dpp.BackBufferWidth   = GetSystemMetrics(SM_CXSCREEN);
   m_d3dpp.BackBufferHeight  = GetSystemMetrics(SM_CYSCREEN);

   // create direct3d device
   hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
                             D3DDEVTYPE_HAL,
                             hwnd, 
                             D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                             &m_d3dpp, 
                             &m_pDevice);
   
   // create additional swap chains if wished and possible
   if ( (m_nNumhWnd > 0) && m_bWindowed) {
      for (UINT i=0; i<m_nNumhWnd; i++) {
         m_d3dpp.hDeviceWindow = m_hWnd[i];
         m_pDevice->CreateAdditionalSwapChain(&m_d3dpp, &m_pChain[i]);
         }
      }

   if(FAILED(hr)) {
      Log("error: IDirect3D::CreateDevice()");
      return ZFX_CREATEDEVICE;
      }

   Log("initialized (online and ready)");
   m_pDevice->GetDeviceCaps(&g_xDevice.d3dCaps);

   m_bRunning        = true;
   m_bIsSceneRunning = false;
   m_dwWidth         = m_d3dpp.BackBufferWidth;
   m_dwHeight        = m_d3dpp.BackBufferHeight;
   
   return ZFX_OK;
   } // InitWindowed
/*----------------------------------------------------------------*/


/**
 * Start the API with values from dialogbox.
 */
HRESULT ZFXD3D::Go(void) {
   ZFXCOMBOINFO   xCombo;
   HRESULT        hr;
   HWND           hwnd;

   // build main direct3d object
   if (m_pD3D) {
      m_pD3D->Release();
      m_pD3D = NULL;
      }
   m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
   if(!m_pD3D) {
      Log("error: Direct3DCreate8()\n");
      return ZFX_CREATEAPI;
      }

   // find the appropriate combo
   for (UINT i=0; i<g_xDevice.nNumCombo; i++) {
      if ( (g_xDevice.d3dCombo[i].bWindowed  == m_bWindowed)
        && (g_xDevice.d3dCombo[i].d3dDevType == g_xDevice.d3dDevType)
        && (g_xDevice.d3dCombo[i].fmtAdapter == g_fmtA)
        && (g_xDevice.d3dCombo[i].fmtBackBuffer == g_fmtB) ) 
         {
         xCombo = g_xDevice.d3dCombo[i];
         break;
         }
      }

   // prepare present parameters structure
   ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
   m_d3dpp.Windowed               = m_bWindowed;
   m_d3dpp.BackBufferCount        = 1;
   m_d3dpp.BackBufferFormat       = g_Dspmd.Format;
   m_d3dpp.EnableAutoDepthStencil = TRUE;
   m_d3dpp.AutoDepthStencilFormat = xCombo.fmtDepthStencil;
   m_d3dpp.MultiSampleType        = xCombo.msType;
   m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

   // stencil buffer active?
   if (  (xCombo.fmtDepthStencil == D3DFMT_D24S8)
      || (xCombo.fmtDepthStencil == D3DFMT_D24X4S4)
      || (xCombo.fmtDepthStencil == D3DFMT_D15S1) )
      m_bStencil = true;
   else
      m_bStencil = false;

   // fullscreen mode
   if (!m_bWindowed) {
      m_d3dpp.hDeviceWindow     = hwnd = m_hWndMain;
      m_d3dpp.BackBufferWidth   = g_Dspmd.Width;
      m_d3dpp.BackBufferHeight  = g_Dspmd.Height;
      ShowCursor(FALSE);
      }
   // windowed mode
   else {
      m_d3dpp.hDeviceWindow     = hwnd = m_hWnd[0];
      m_d3dpp.BackBufferWidth   = GetSystemMetrics(SM_CXSCREEN);
	   m_d3dpp.BackBufferHeight  = GetSystemMetrics(SM_CYSCREEN);
      }

   // create direct3d device
   hr = m_pD3D->CreateDevice(g_xDevice.nAdapter, g_xDevice.d3dDevType,
                             hwnd, xCombo.dwBehavior, 
                             &m_d3dpp, &m_pDevice);

   // create additional swap chains if wished and possible
   if ( (m_nNumhWnd > 0) && m_bWindowed) {
      for (UINT i=0; i<m_nNumhWnd; i++) {
         m_d3dpp.hDeviceWindow = m_hWnd[i];
         m_pDevice->CreateAdditionalSwapChain(&m_d3dpp, &m_pChain[i]);
         }
      }

   m_pEnum->~ZFXD3DEnum();
   m_pEnum = NULL;

   if(FAILED(hr)) {
      Log("error: IDirect3D::CreateDevice() \n");
      return ZFX_CREATEDEVICE;
      }

   Log("everything initialized smoothly... \n");

   m_bRunning        = true;
   m_bIsSceneRunning = false;

   return ZFX_OK;
   } // Go
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: FILE - pointer to logfile stream or NULL
 */
void ZFXD3D::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs) ;
   fprintf(m_pLog, ch);
   
   #ifdef _DEBUGFLUSH_
   fflush(m_pLog);
   #endif
   }
/*----------------------------------------------------------------*/
 

/**
 * Callback function to handle the device/mode selection dialog
 */
BOOL CALLBACK ZFXD3D::DlgProc(HWND hDlg, UINT message, 
                              WPARAM wParam, LPARAM lParam) {
   DIBSECTION dibSection;
   BOOL       bWnd=FALSE;
   
   // get handles
   HWND hFULL       = GetDlgItem(hDlg, IDC_FULL);
   HWND hWND        = GetDlgItem(hDlg, IDC_WND);
   HWND hADAPTER    = GetDlgItem(hDlg, IDC_ADAPTER);
   HWND hMODE       = GetDlgItem(hDlg, IDC_MODE);
   HWND hADAPTERFMT = GetDlgItem(hDlg, IDC_ADAPTERFMT);
   HWND hBACKFMT    = GetDlgItem(hDlg, IDC_BACKFMT);
   HWND hDEVICE     = GetDlgItem(hDlg, IDC_DEVICE);
   
   switch (message) {
      
      // preselect windowed
      case WM_INITDIALOG: {
         SendMessage(hWND, BM_SETCHECK, BST_CHECKED, 0);
         this->m_pEnum->Enum(hADAPTER, hMODE, hDEVICE, 
                             hADAPTERFMT, hBACKFMT,
                             hWND, hFULL, m_pLog);
         
         return TRUE;
         }
         
      // draw zfx logo
      case WM_PAINT: {
         if (g_hBMP) {
            GetObject(g_hBMP, sizeof(DIBSECTION), &dibSection);
            HDC      hdc = GetDC(hDlg);
            HDRAWDIB hdd = DrawDibOpen();
            DrawDibDraw(hdd, hdc, 50, 10, 95, 99, 
                        &dibSection.dsBmih,
                        dibSection.dsBm.bmBits, 0, 0, 
                        dibSection.dsBmih.biWidth, 
                        dibSection.dsBmih.biHeight, 0);
            DrawDibClose(hdd);
            ReleaseDC(hDlg, hdc);
            }
         } break;

      // control changed
      case WM_COMMAND: {
         switch (LOWORD(wParam)) {
            
            // okay button => read selections
            case IDOK: {
               m_bWindowed = SendMessage(hFULL, BM_GETCHECK, 0, 0) != BST_CHECKED;
               m_pEnum->GetSelections(&g_xDevice, &g_Dspmd, &g_fmtA, &g_fmtB);
               GetWindowText(hADAPTER, m_chAdapter, 256);
               EndDialog(hDlg, 1);
               return TRUE;
               } break;
               
            // cancel button
            case IDCANCEL: {
               EndDialog(hDlg, 0);
               return TRUE;
               } break;

               
            case IDC_ADAPTER: {
               if(HIWORD(wParam)==CBN_SELCHANGE)
                  m_pEnum->ChangedAdapter();
               } break;


            case IDC_DEVICE: {
               if(HIWORD(wParam)==CBN_SELCHANGE) {
                  m_pEnum->ChangedDevice();
                  }
               } break;

            case IDC_ADAPTERFMT: {
               if(HIWORD(wParam)==CBN_SELCHANGE)
                  m_pEnum->ChangedAdapterFmt();
               } break;


            case IDC_FULL: case IDC_WND: {
               m_pEnum->ChangedWindowMode();
               } break;
             
            } // switch [CMD]
         } break; // case [CMD]
      } // switch [MSG]
   return FALSE;
   }
/*----------------------------------------------------------------*/




