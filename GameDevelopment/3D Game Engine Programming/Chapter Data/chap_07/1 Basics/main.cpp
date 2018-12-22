// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "ZFXRenderer.h"  // our render interface
#include "ZFX.h"          // return values and stuff
#include "main.h"         // prototypes and stuff


//include our library
#pragma comment(lib, "ZFXRenderer.lib")
#pragma comment(lib, "zfx3d.lib")
 

// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;

FILE *pLog=NULL;

UINT g_nFontID=0;

// renderer object
LPZFXRENDERER     g_pRenderer = NULL;
LPZFXRENDERDEVICE g_pDevice   = NULL;

UINT g_sRoom=0;

UINT g_Base[2] = { 0, 0 };



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
	   		                   "ZFXEngine v2.0 - Shader Demo Application",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                               WS_MINIMIZEBOX | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -250,
                               GetSystemMetrics(SM_CYSCREEN)/2 -187,
				                   500, 375, NULL, NULL, hInst, NULL)))
      return 0;
   
   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");

   // init 3dmath lib
   ZFX3DInitCPU();

   // try to start the engine
   if (FAILED( hr = ProgramStartup("Direct3D") )) {
      if (hr == ZFX_FAIL)
         fprintf(pLog, "error: ProgramStartup() failed\n");
      else if (hr == ZFX_CANCELED)
         fprintf(pLog, "error: ProgramStartup() canceled by user \n");
      else if (hr == ZFX_NOTCOMPATIBLE) {
         MessageBox(g_hWnd, 
                    "Sorry, no suitable graphics hardware found.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         fprintf(pLog, "error: no compatible graphics adapter \n");
         }
      fflush(pLog);
      g_bDone = true;
      }
   // everything went smooth
   else {
      ShowWindow(hWnd, SW_SHOW);

      if (FAILED(BuildAndSetShader())) {
         MessageBox(g_hWnd, 
                    "Sorry, pixel shaders 1.1 required to run this demo.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         g_bDone = true;
         }

      if (FAILED( BuildGeometry() ))
         g_bDone = true;
      }

   ZFXVector vR(1,0,0), vU(0,1,0), vD(0,0,1), vP(0,0,-15);
   g_pDevice->SetView3D(vR,vU,vD,vP);
   g_pDevice->SetClearColor(0.7f,0.7f,1.0f);

   // main loop
   while (!g_bDone) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         } 
      // do one frame
      if (g_bIsActive) ProgramTick();
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
   // no opengl render device yet...
   if (strcmp(chAPI, "OpenGL")==0) return S_OK;

   // create a render objekt
   g_pRenderer = new ZFXRenderer(g_hInst);
   
   // create a device for the chosen api
   if (FAILED( g_pRenderer->CreateDevice(chAPI) )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pRenderer->GetDevice();
   if (g_pDevice == NULL) return E_FAIL;

   // init render device
   if (FAILED( g_pDevice->Init(g_hWnd, NULL, 0, 16, 8, true) )) {
      fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
      return E_FAIL;
      }

   g_pDevice->UseWindow(0);


   POINT ptRes; int fs=0;
   g_pDevice->GetResolution(&ptRes);

   // font size
   fs  = ptRes.x / 40;

   // prepare viewport stage
   g_pDevice->InitStage(0.8f, NULL, 0);

   if (FAILED( g_pDevice->CreateFont("Arial", 0, false, false,
                                     false, fs, &g_nFontID) )) {
      fprintf(pLog, "error: ZFXRenderDevice::CreateFont failed\n");
      return ZFX_FAIL;
      }

   return ZFX_OK;
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


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {
   HRESULT hr = ZFX_FAIL;
   ZFXMatrix mat;
   mat.Identity();

   // clear buffers and start scene
   g_pDevice->BeginRendering(true,true,true);

   Render(-1);  // ambient

   g_pDevice->UseShaders(false);
   g_pDevice->DrawText(g_nFontID, 10, 10, 255, 255, 0, 
                       "Basic Shader Demo");

   // flip backbuffer to front
   g_pDevice->EndRendering();
   return hr;
   } // Tick
/*----------------------------------------------------------------*/


/**
 * Renders the scene.
 */
HRESULT Render(int n) {
   ZFXMatrix mat;
   mat.RotaY(-0.4f);
   mat._42 -= 0.5f;
   mat._41 -= 1.5f;

   // circle ambient light color
   float fT = GetTickCount() / 1000.0f;
   float fR = 0.5f + 0.5f * sinf(fT*1.2f);
   float fG = 0.5f + 0.5f * sinf(fT*2.0f);
   float fB = 0.5f + 0.5f * sinf(fT*1.7f);

   g_pDevice->SetAmbientLight(fR, fG, fB);
   g_pDevice->SetWorldTransform(&mat);

   // ambient pass
   g_pDevice->ActivateVShader(g_Base[0], VID_TV);
   g_pDevice->ActivatePShader(g_Base[1]);

   return g_pDevice->GetVertexManager()->Render(g_sRoom);
   } // Render
/*----------------------------------------------------------------*/



HRESULT BuildAndSetShader(void) {
   if (!g_pDevice->CanDoShaders()) return S_OK;

   g_pDevice->CreateVShader("base.vsh", 0,
                            true, false, 
                            &g_Base[0]);

   g_pDevice->CreatePShader("base.psh", 0,
                            true, false, 
                            &g_Base[1]);
   return ZFX_OK;
   } // BuildAndSetShader
/*----------------------------------------------------------------*/


/**
 * Create geometry needed for the scene along with textures and stuff.
 */
HRESULT BuildGeometry(void) {
   HRESULT hr=ZFX_OK;
   TVERTEX v[24];
   WORD    i[36];
   UINT    s=0;

   memset(v, 0, sizeof(TVERTEX)*24);
   memset(i, 0, sizeof(WORD)*36);
   
   ZFXCOLOR c = { 1.0f, 1.0f, 1.0f, 1.0f };
   ZFXCOLOR d = { 0.0f, 0.0f, 0.0f, 1.0f };

   g_pDevice->GetSkinManager()->AddSkin(&c, &c, &d, &c, 1, &s);
   g_pDevice->GetSkinManager()->AddTexture(s, "texture.bmp", false, 0, NULL, 0);

   // geometry for the room
   CreateCube(ZFXVector(0,0,0), 10.0f, 7.0f, 10.0f, v, i, true);
   return g_pDevice->GetVertexManager()->CreateStaticBuffer(
                           VID_TV, 0, 24, 36,
                           v, i, &g_sRoom);
   } // BuildGeometry
/*----------------------------------------------------------------*/


/**
 * Create geometry for a cube, invert face orientation if needed.
 */
void CreateCube(ZFXVector vcP, float fW, float fH, float fD,
                TVERTEX *pVerts, WORD *pIndis, bool bInv) {
   WORD     i[36] = {  3,  0,  1,   3,  1,  2,   // top
                       6,  7,  4,   6,  4,  5,   // right
                      10,  9,  8,  11, 10,  8,   // left
                      12, 13, 14,  14, 15, 12,   // back
                      18, 19, 16,  17, 18, 16,   // front
                      22, 23, 20,  20, 21, 22 }; // bottom

   WORD     iinv[36] = {  1,  0,  3,   2,  1,  3,   // top
                          4,  7,  6,   5,  4,  6,   // right
                          8,  9, 10,   8, 10, 11,   // left
                         14, 13, 12,  12, 15, 14,   // back
                         16, 19, 18,  16, 18, 17,   // front
                         20, 23, 22,  22, 21, 20 }; // bottom

   if (bInv) memcpy(pIndis, iinv, sizeof(WORD)*36);
   else      memcpy(pIndis,    i, sizeof(WORD)*36);

   memset(pVerts, 0, sizeof(TVERTEX)*24);

   // top rectangle
   pVerts[0].x = vcP.x - (fW / 2.0f);
   pVerts[0].y = vcP.y + (fH / 2.0f);
   pVerts[0].z = vcP.z - (fD / 2.0f);
   pVerts[0].tu = 0.0f;  pVerts[0].tv = 0.0f;

   pVerts[1].x = vcP.x - (fW / 2.0f);
   pVerts[1].y = vcP.y + (fH / 2.0f);
   pVerts[1].z = vcP.z + (fD / 2.0f);
   pVerts[1].tu = 0.0f;  pVerts[1].tv = 1.0f;

   pVerts[2].x = vcP.x + (fW / 2.0f);
   pVerts[2].y = vcP.y + (fH / 2.0f);
   pVerts[2].z = vcP.z + (fD / 2.0f);
   pVerts[2].tu = 1.0f;  pVerts[2].tv = 1.0f;

   pVerts[3].x = vcP.x + (fW / 2.0f);
   pVerts[3].y = vcP.y + (fH / 2.0f);
   pVerts[3].z = vcP.z - (fD / 2.0f);
   pVerts[3].tu = 1.0f;  pVerts[3].tv = 0.0f;

   if (bInv) pVerts[0].vcN[1] = -1.0f;
   else      pVerts[0].vcN[1] =  1.0f;
   pVerts[1].vcN[1] = pVerts[0].vcN[1];
   pVerts[2].vcN[1] = pVerts[0].vcN[1];
   pVerts[3].vcN[1] = pVerts[0].vcN[1];


   // right side rectanlge
   pVerts[4]    = pVerts[3];
   pVerts[4].tu = 1.0f;  
   pVerts[4].tv = 0.0f;
   pVerts[5]    = pVerts[2];
   pVerts[5].tu = 0.0f;  
   pVerts[5].tv = 0.0f;

   pVerts[6].x  = vcP.x + (fW / 2.0f);
   pVerts[6].y  = vcP.y - (fH / 2.0f);
   pVerts[6].z  = vcP.z + (fD / 2.0f);
   pVerts[6].tu = 0.0f;  pVerts[6].tv = 1.0f;

   pVerts[7].x  = vcP.x + (fW / 2.0f);
   pVerts[7].y  = vcP.y - (fH / 2.0f);
   pVerts[7].z  = vcP.z - (fD / 2.0f);
   pVerts[7].tu = 1.0f;  pVerts[7].tv = 1.0f;

   if (bInv) pVerts[4].vcN[0] = -1.0f;
   else      pVerts[4].vcN[0] =  1.0f;
   pVerts[5].vcN[0] = pVerts[4].vcN[0];
   pVerts[6].vcN[0] = pVerts[4].vcN[0];
   pVerts[7].vcN[0] = pVerts[4].vcN[0];


   // left side rectangle
   pVerts[8]    = pVerts[0];
   pVerts[8].tu = 0.0f;  
   pVerts[8].tv = 0.0f;
   pVerts[9]    = pVerts[1];
   pVerts[9].tu = 1.0f;  
   pVerts[9].tv = 0.0f;

   pVerts[10].x = vcP.x - (fW / 2.0f);
   pVerts[10].y = vcP.y - (fH / 2.0f);
   pVerts[10].z = vcP.z + (fD / 2.0f);
   pVerts[10].tu = 1.0f;  pVerts[10].tv = 1.0f;

   pVerts[11].x = vcP.x - (fW / 2.0f);
   pVerts[11].y = vcP.y - (fH / 2.0f);
   pVerts[11].z = vcP.z - (fD / 2.0f);
   pVerts[11].tu = 0.0f;  pVerts[11].tv = 1.0f;

   if (bInv) pVerts[8].vcN[0] =  1.0f;
   else      pVerts[8].vcN[0] = -1.0f;
   pVerts[9].vcN[0]  = pVerts[8].vcN[0];
   pVerts[10].vcN[0] = pVerts[8].vcN[0];
   pVerts[11].vcN[0] = pVerts[8].vcN[0];


   // back side rectanlge
   pVerts[12]    = pVerts[2];
   pVerts[12].tu = 1.0f; 
   pVerts[12].tv = 0.0f;
   pVerts[13]    = pVerts[1];
   pVerts[13].tu = 0.0f; 
   pVerts[13].tv = 0.0f;
   pVerts[14]    = pVerts[10];
   pVerts[14].tu = 0.0f; 
   pVerts[14].tv = 1.0f;
   pVerts[15]    = pVerts[6];
   pVerts[15].tu = 1.0f; 
   pVerts[15].tv = 1.0f;

   if (bInv) pVerts[12].vcN[2] = -1.0f;
   else      pVerts[12].vcN[2] =  1.0f;
   pVerts[13].vcN[2] = pVerts[12].vcN[2];
   pVerts[14].vcN[2] = pVerts[12].vcN[2];
   pVerts[15].vcN[2] = pVerts[12].vcN[2];


   // front side rectangle
   pVerts[16]    = pVerts[0];
   pVerts[16].tu = 1.0f; 
   pVerts[16].tv = 0.0f;
   pVerts[17]    = pVerts[3];
   pVerts[17].tu = 0.0f;  
   pVerts[17].tv = 0.0f;
   pVerts[18]    = pVerts[7];
   pVerts[18].tu = 0.0f; 
   pVerts[18].tv = 1.0f;
   pVerts[19]    = pVerts[11];
   pVerts[19].tu = 1.0f; 
   pVerts[19].tv = 1.0f;

   if (bInv) pVerts[16].vcN[2] =  1.0f;
   else      pVerts[16].vcN[2] = -1.0f;
   pVerts[17].vcN[2] = pVerts[16].vcN[2];
   pVerts[18].vcN[2] = pVerts[16].vcN[2];
   pVerts[19].vcN[2] = pVerts[16].vcN[2];


   // bottom side rectangle
   pVerts[20]    = pVerts[7];
   pVerts[20].tu = 1.0f; 
   pVerts[20].tv = 1.0f;
   pVerts[21]    = pVerts[6];
   pVerts[21].tu = 1.0f; 
   pVerts[21].tv = 0.0f;
   pVerts[22]    = pVerts[10];
   pVerts[22].tu = 0.0f;  
   pVerts[22].tv = 0.0f;
   pVerts[23]    = pVerts[11];
   pVerts[23].tu = 0.0f; 
   pVerts[23].tv = 1.0f;

   if (bInv) pVerts[20].vcN[1] =  1.0f;
   else      pVerts[20].vcN[1] = -1.0f;
   pVerts[21].vcN[1] = pVerts[20].vcN[1];
   pVerts[22].vcN[1] = pVerts[20].vcN[1];
   pVerts[23].vcN[1] = pVerts[20].vcN[1];
   } // CreateCube
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


