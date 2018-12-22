/**************************************************
WinMain.cpp
Chapter 2 Font Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB and D3DX9.LIB
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>
#include "d3d9.h"
#include "d3dx9.h"

// Window handles, class and caption text
HWND          g_hWnd;
HINSTANCE     g_hInst;
static char   g_szClass[]   = "FontClass";
static char   g_szCaption[] = "Font Demo by Jim Adams";

// The Direct3D and Device object
IDirect3D9       *g_pD3D       = NULL;
IDirect3DDevice9 *g_pD3DDevice = NULL;

// The Font object
ID3DXFont *g_pFont = NULL;

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg,              \
                           WPARAM wParam, LPARAM lParam);

BOOL DoInit();
BOOL DoShutdown();
BOOL DoFrame();
BOOL SetupMeshes();

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASSEX wcex;
  MSG        Msg;

  g_hInst = hInst;

  // Create the window class here and register it
  wcex.cbSize        = sizeof(wcex);
  wcex.style         = CS_CLASSDC;
  wcex.lpfnWndProc   = WindowProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInst;
  wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = NULL;
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = g_szClass;
  wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
  if(!RegisterClassEx(&wcex))
    return FALSE;

  // Create the Main Window
  g_hWnd = CreateWindow(g_szClass, g_szCaption,
        WS_CAPTION | WS_SYSMENU,
        0, 0, 400, 400,
        NULL, NULL,
        hInst, NULL );
  if(!g_hWnd)
    return FALSE;
  ShowWindow(g_hWnd, SW_NORMAL);
  UpdateWindow(g_hWnd);

  // Run init function and return on error
  if(DoInit() == FALSE)
    return FALSE;

  // Start message pump, waiting for signal to quit
  ZeroMemory(&Msg, sizeof(MSG));
  while(Msg.message != WM_QUIT) {
    if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    if(DoFrame() == FALSE)
      break;
  }

  // Run shutdown function
  DoShutdown();
  
  UnregisterClass(g_szClass, hInst);

  return Msg.wParam;
}

long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg,              \
                           WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL DoInit()
{
  D3DPRESENT_PARAMETERS d3dpp;
  D3DDISPLAYMODE        d3ddm;
  LOGFONT               Font;

  // Do a windowed mode initialization of Direct3D
  if((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return FALSE;
  if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    return FALSE;
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.EnableAutoDepthStencil = FALSE;
  if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                  &d3dpp, &g_pD3DDevice)))
    return FALSE;

  // Create the font
  ZeroMemory(&Font, sizeof(Font));
  strcpy(Font.lfFaceName, "Arial");
  Font.lfHeight = -32;
  D3DXCreateFontIndirect(g_pD3DDevice, &Font, &g_pFont);

  return TRUE;
}

BOOL DoShutdown()
{
  // Release font
  if(g_pFont != NULL)
    g_pFont->Release();

  // Release device and 3D objects
  if(g_pD3DDevice != NULL)
    g_pD3DDevice->Release();

  if(g_pD3D != NULL)
    g_pD3D->Release();

  return TRUE;
}

BOOL DoFrame()
{
  RECT Rect = { 0,0,400,400 };

  // Clear device backbuffer
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,               \
                        D3DCOLOR_RGBA(0,64,128,255), 1.0f, 0);

  // Begin scene
  if(SUCCEEDED(g_pD3DDevice->BeginScene())) {

    // Draw some text
    g_pFont->Begin();
    g_pFont->DrawText("Programming is Fun!", -1, &Rect, DT_CENTER | DT_VCENTER, 0xFFFFFFFF);
    g_pFont->End();

    // End the scene
    g_pD3DDevice->EndScene();
  }

  // Display the scene
  g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

  return TRUE;
}
