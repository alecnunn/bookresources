// File: ZFXD3D_init.cpp

//#include <windows.h>        // type definitions

#include "ZFX.h"              // return values and stuff
#include "ZFXD3D.h"           // class definition
#include "ZFXD3D_skinman.h"   // material manager
#include "ZFXD3D_vcache.h"    // vertex cache manager
#include <vfw.h>              // show bmp's
#include "resource.h"         // control id's


bool g_bLF=false;


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
   m_bUseShaders       = false;
   m_bCanDoShaders     = false;
   m_bAdditive         = false;
   m_bColorBuffer      = true;
   m_bTextures         = true;
   m_pSkinMan          = NULL;
   m_pVertexMan        = NULL;

   m_pDeclVertex       = NULL;
   m_pDeclPVertex      = NULL;
   m_pDeclLVertex      = NULL;
   m_pDeclCVertex      = NULL;
   m_pDecl3TVertex     = NULL;
   m_pDeclTVertex      = NULL;

   m_pFont             = NULL;

   // dont use swapchain at first
   m_nActivehWnd       = 0;
   m_nNumVShaders      = 0;
   m_nNumPShaders      = 0;
   m_nNumFonts         = 0;

   g_ZFXD3D = this;

   m_pLog = fopen("Log_ZFXRenderDevice.txt", "w");
   Log("online (waiting for initialization call)");
   }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXD3D::~ZFXD3D() {
   Release();
   }
/*----------------------------------------------------------------*/


/**
 * Release all of our own and the Direct3D COM stuff.
 */
void ZFXD3D::Release() {
   // our own stuff
   if (m_pEnum) {
      delete m_pEnum;
      m_pEnum = NULL;
      }
   if (m_pSkinMan) {
      delete m_pSkinMan;
      m_pSkinMan = NULL;
      }
   if (m_pVertexMan) {
      delete m_pVertexMan;
      m_pVertexMan = NULL;
      }

   // shader stuff
   if (m_pDeclVertex) {
      m_pDeclVertex->Release();
      m_pDeclVertex = NULL;
      }
   if (m_pDeclPVertex) {
      m_pDeclPVertex->Release();
      m_pDeclPVertex = NULL;
      }
   if (m_pDeclLVertex) {
      m_pDeclLVertex->Release();
      m_pDeclLVertex = NULL;
      }

   if (m_pDeclCVertex) {
      m_pDeclCVertex->Release();
      m_pDeclCVertex = NULL;
      }

   if (m_pDecl3TVertex) {
      m_pDecl3TVertex->Release();
      m_pDecl3TVertex = NULL;
      }

   if (m_pDeclTVertex) {
      m_pDeclTVertex->Release();
      m_pDeclTVertex = NULL;
      }

   for (UINT i=0; i<m_nNumVShaders; i++) {
      if (m_pVShader[i]) {
         m_pVShader[i]->Release();
         m_pVShader[i] = NULL;
         }
      }
   for (UINT j=0; j<m_nNumPShaders; j++) {
      if (m_pPShader[j]) {
         m_pPShader[j]->Release();
         m_pPShader[j] = NULL;
         }
      }

   for (UINT k=0; k<m_nNumFonts; k++) {
      if (m_pFont[k]) {
         m_pFont[k]->Release();
         m_pFont[k] = NULL;
         }
      }

   if (m_pFont) {
      free(m_pFont);
      m_pFont = NULL;
      }

   // main objects
   if(m_pDevice) {
      m_pDevice->Release();
      m_pDevice = NULL;
      }
   if(m_pD3D) {
      m_pD3D->Release();
      m_pD3D = NULL;
      }
   Log("offline (ok)");
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

   g_bLF = bSaveLog;
   
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

   // create enum object
   m_pEnum = new ZFXD3DEnum(nMinDepth, nMinStencil);

   Log("calling dialog");

   // load bmp logo
   g_hBMP = (HBITMAP)LoadImage(NULL, "zfx.bmp",
                               IMAGE_BITMAP,0,0,
                               LR_LOADFROMFILE |
                               LR_CREATEDIBSECTION);

   // call device selection dialog
   nResult = DialogBox(m_hDLL, "dlgChangeDevice", hWnd, DlgProcWrap);
   
   // delete bmp if any
   if (g_hBMP) DeleteObject(g_hBMP);

   Log("dialog finished");

   // dialog failed somehow
   if (nResult == -1) {
      Log("error: selection dialog error");
      return ZFX_FAIL;
      }
   // dialog was canceled by user
   else if (nResult == 0) {
      Log("warning: selection dialog canceled by user");
      return ZFX_CANCELED;
      }
   else if (nResult == -2) {
      Log("error: no compatible graphics adapter");
      return ZFX_NOTCOMPATIBLE;
      }
   // dialog ended with ok button
   else {
      Log("firing up MS Direct3D");
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
   HRESULT  hr;
   D3DCAPS9 caps;
   RECT     rc;

   g_bLF = bSaveLog;
   
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

   m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,
                         D3DDEVTYPE_HAL,
                         &caps);

   if ( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0) {
      Log("error: no hardware vertexprocessing");
      m_pD3D->Release();
      m_pD3D = NULL;
      return ZFX_NOTCOMPATIBLE;
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
   GetClientRect(m_hWnd[0], &rc);
   m_d3dpp.hDeviceWindow     = m_hWnd[0];
   m_d3dpp.BackBufferWidth   = rc.right;
   m_d3dpp.BackBufferHeight  = rc.bottom;

   // create direct3d device
   hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
                             D3DDEVTYPE_HAL,
                             m_hWnd[0], 
                             D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                             &m_d3dpp, 
                             &m_pDevice);

   
   if(FAILED(hr)) {
      Log("error: IDirect3D::CreateDevice()");
      if (hr == D3DERR_NOTAVAILABLE)
         Log("D3DERR_NOTAVAILABLE");
      else if (hr == D3DERR_INVALIDCALL)
         Log("D3DERR_INVALIDCALL");
      else if (hr == D3DERR_OUTOFVIDEOMEMORY)
         Log("D3DERR_OUTOFVIDEOMEMORY");
      else
         Log("unknown error");

      return ZFX_CREATEDEVICE;
      }

   // create additional swap chains if wished and possible
   if ( (m_nNumhWnd > 0) && m_bWindowed) {
      for (UINT i=0; i<m_nNumhWnd; i++) {
         GetClientRect(m_hWnd[i], &rc);
         m_d3dpp.hDeviceWindow    = m_hWnd[i];
         m_d3dpp.BackBufferWidth  = rc.right;
         m_d3dpp.BackBufferHeight = rc.bottom;

         hr = m_pDevice->CreateAdditionalSwapChain(&m_d3dpp, &m_pChain[i]);
         if (FAILED(hr)) break;
         }
      }

   Log("initialized (online and ready)");
   m_pDevice->GetDeviceCaps(&g_xDevice.d3dCaps);
   LogDeviceCaps(&g_xDevice.d3dCaps);

   m_bRunning        = true;
   m_bIsSceneRunning = false;
   m_dwWidth         = m_d3dpp.BackBufferWidth;
   m_dwHeight        = m_d3dpp.BackBufferHeight;
   
   return OneTimeInit();
   } // InitWnd
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
      Log("error: Direct3DCreate8()");
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
   m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
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
      m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
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
      Log("error: IDirect3D::CreateDevice()");
      return ZFX_CREATEDEVICE;
      }

   Log("initialized (online and ready)");
   LogDeviceCaps(&g_xDevice.d3dCaps);

   m_bRunning        = true;
   m_bIsSceneRunning = false;
   m_dwWidth         = m_d3dpp.BackBufferWidth;
   m_dwHeight        = m_d3dpp.BackBufferHeight;

   return OneTimeInit();
   } // Go
/*----------------------------------------------------------------*/


/**
 * Called by ZFXD3D::Go to initialize renderstates and stuff.
 */
HRESULT ZFXD3D::OneTimeInit(void) {

   if (!ZFX3DInitCPU()) Log("no SIMD compatible CPU detected");
   else Log("SIMD compatible CPU detected => using fast math");

   // solid rendering
   m_ShadeMode = RS_SHADE_SOLID;

   // bring material and vertex cache managers online
   m_pSkinMan = new ZFXD3DSkinManager(m_pDevice, m_pLog);

   m_pVertexMan = new ZFXD3DVCManager((ZFXD3DSkinManager*)m_pSkinMan, 
                                      m_pDevice, this, 8192, 8192, m_pLog);

   // activate renderstates
   m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
   m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
   m_pDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE);

   // create and set standard material
   memset(&m_StdMtrl, 0, sizeof(D3DMATERIAL9));
   m_StdMtrl.Ambient.r  = 1.0f;
   m_StdMtrl.Ambient.g  = 1.0f;
   m_StdMtrl.Ambient.b  = 1.0f;
   m_StdMtrl.Ambient.a  = 1.0f;

   if (FAILED(m_pDevice->SetMaterial(&m_StdMtrl))) {
      Log("error: set material (OneTimeInit)");
      return ZFX_FAIL;
      }

   // set texture filtering
   m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

   // goto 3D mode in first stage
   ZFXVIEWPORT vpView = { 0, 0, m_dwWidth, m_dwHeight };
   m_Mode   = EMD_PERSPECTIVE;
   m_nStage = -1;
   SetActiveSkinID(MAX_ID);

   // identity matrix for view
   IDENTITY(m_mView3D);

   // set clipping plane values
   SetClippingPlanes(0.1f, 1000.0f);

   // check and init shaders
   PrepareShaderStuff();

   // create a default shader. Note: its ID is 0
   if (m_bCanDoShaders) {
      const char BaseShader[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oD0, c4            \n"\
         "mov oT0, v6            \n";

      if (FAILED(CreateVShader((void*)BaseShader, sizeof(BaseShader),
                               false, false, NULL))) {
         Log("CreateVShader() default failed");
         return ZFX_FAIL;
         }
      /*
      if (FAILED(ActivateVShader(0, VID_UU))) {
         Log("ActivateVShader() default failed");
         return ZFX_FAIL;
         }
      */
      } // default shader

   // Set Ambient light level
   SetAmbientLight(1.0f, 1.0f, 1.0f);

   SetTextureStage(0, RS_TEX_MODULATE);
   SetTextureStage(1, RS_NONE);
   SetTextureStage(2, RS_NONE);
   SetTextureStage(3, RS_NONE);
   SetTextureStage(4, RS_NONE);
   SetTextureStage(5, RS_NONE);
   SetTextureStage(6, RS_NONE);
   SetTextureStage(7, RS_NONE);

   // set perspective projection stage 0
   if (FAILED(InitStage(0.8f, &vpView, 0)))
      return ZFX_FAIL;

   // set perspective projection
   if (FAILED(SetMode(EMD_PERSPECTIVE, 0)))
      return ZFX_FAIL;

   // world matrix to identity, this is needed to set
   // correct values to vertex shader registers also
   SetWorldTransform(NULL);

   Log("one time scene init complete");
   return ZFX_OK;
   } // OneTimeInit
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: char - format string to output
 *        ...  - output values
 */
void ZFXD3D::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXD3DDevice]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/


/**
 * Callback function to handle the device/mode selection dialog
 */
BOOL CALLBACK ZFXD3D::DlgProc(HWND hDlg, UINT message, 
                              WPARAM wParam, LPARAM lParam) {
   DIBSECTION dibSection;
   HRESULT    hr;
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
         hr = m_pEnum->Enum(hADAPTER, hMODE, hDEVICE, 
                            hADAPTERFMT, hBACKFMT,
                            hWND, hFULL, m_pLog);

         if (hr == ZFX_NOTCOMPATIBLE)
            EndDialog(hDlg, -2);
         else if (hr == ZFX_FAIL)
            EndDialog(hDlg, -1);
        
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
               m_bWindowed = !SendMessage(hFULL, BM_GETCHECK, 0, 0);
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


/**
 * Log all nice features of a device for eye candy
 */
void ZFXD3D::LogDeviceCaps(D3DCAPS9 *pCaps) {

   Log("ADAPTERINFO: {");

   if (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
      Log("   adapter features hardware TnL");
      if (pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE)
         Log("   (pure device possible)");
      else Log("   (no pure device possible)");
      }
   else
      Log("   adapter only features software TnL");

   // TEXTURE STUFF
   Log("   max. texture stages: %d" , pCaps->MaxTextureBlendStages);
   Log("   max. textures for single pass: %d" , pCaps->MaxSimultaneousTextures);
   Log("   max. texture width: %d" , pCaps->MaxTextureWidth);
   Log("   max. texture height: %d" , pCaps->MaxTextureHeight);

   // VERTEX SHADER VERSION
   if (pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
      Log("   Vertex Shader Version 1.0");
   else if (pCaps->VertexShaderVersion < D3DVS_VERSION(2,0) )
      Log("   Vertex Shader Version 1.1");
   else  
      Log("   Vertex Shader Version 2.0 or better");

   // PIXEL SHADER VERSION
   if (pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
      Log("   Pixel Shader Version 1.0");
   else if (pCaps->PixelShaderVersion < D3DPS_VERSION(1,2) )
      Log("   Pixel Shader Version 1.1");
   else if (pCaps->PixelShaderVersion < D3DPS_VERSION(1,3) )
      Log("   Pixel Shader Version 1.2");
   else if (pCaps->PixelShaderVersion < D3DPS_VERSION(1,4) )
      Log("   Pixel Shader Version 1.3");
   else if (pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
      Log("   Pixel Shader Version 1.4");
   else
      Log("   Pixel Shader Version 2.0 or better");
   Log("   } ENDINFO");


   // SCREEN RESOLUTION, FORMAT AND STUFF
   Log("DISPLAYINFO {");
   LPDIRECT3DSURFACE9 pDepthStencil=NULL;
   D3DSURFACE_DESC desc;
   D3DFORMAT Format = D3DFMT_UNKNOWN;
   D3DDISPLAYMODE mode = {0,0,0,D3DFMT_UNKNOWN};
   
   if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode)))
      Log("Error: IDirect3D::GetAdapterDisplayMode failed");

   if (FAILED(m_pDevice->GetDepthStencilSurface(&pDepthStencil)))
      Log("Error: IDirect3DDevice::GetDepthStencilSurface failed");
   else {
      if (FAILED(pDepthStencil->GetDesc(&desc))) {
         Log("Error: IDirect3DSurface::GetDesc failed");
         Format = D3DFMT_UNKNOWN;
         }
      }

   Log("   Resolution: %dx%d", mode.Width, mode.Height);
   Log("   Refreshrate: %d", mode.RefreshRate);

   // backbuffer format
   switch(mode.Format) {
      case D3DFMT_A2R10G10B10: Log("   Pixelformat: D3DFMT_A2R10G10B10"); break;
      case D3DFMT_A8R8G8B8:    Log("   Pixelformat: D3DFMT_A8R8G8B8"); break;
      case D3DFMT_X8R8G8B8:    Log("   Pixelformat: D3DFMT_X8R8G8B8"); break;
      case D3DFMT_A1R5G5B5 :   Log("   Pixelformat: D3DFMT_A1R5G5B5"); break;
      case D3DFMT_X1R5G5B5:    Log("   Pixelformat: D3DFMT_X1R5G5B5"); break;
      case D3DFMT_R5G6B5:      Log("   Pixelformat: D3DFMT_R5G6B5");   break;
      default: break;
      } // switch

   // depth buffer format
   switch(desc.Format) {
      case D3DFMT_D16_LOCKABLE:  Log("   Depth/Stencil: D3DFMT_D16_LOCKABLE");  break;
      case D3DFMT_D32F_LOCKABLE: Log("   Depth/Stencil: D3DFMT_D32F_LOCKABLE"); break;
      case D3DFMT_D32:     Log("   Depth/Stencil: D3DFMT_D32");     break;
      case D3DFMT_D15S1:   Log("   Depth/Stencil: D3DFMT_D15S1");   break;
      case D3DFMT_D24S8:   Log("   Depth/Stencil: D3DFMT_D24S8");   break;
      case D3DFMT_D24X8:   Log("   Depth/Stencil: D3DFMT_D24X8");   break;
      case D3DFMT_D24X4S4: Log("   Depth/Stencil: D3DFMT_D24X4S4"); break;
      case D3DFMT_D24FS8:  Log("   Depth/Stencil: D3DFMT_D24FS8");  break;
      case D3DFMT_D16:     Log("   Depth/Stencil: D3DFMT_D16");     break;
      default: break;
      } // switch
   
   pDepthStencil->Release();
   Log("   } ENDINFO");
   } // LogDeviceCaps
/*----------------------------------------------------------------*/

