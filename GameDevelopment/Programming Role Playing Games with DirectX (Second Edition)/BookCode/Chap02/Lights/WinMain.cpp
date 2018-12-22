/**************************************************
WinMain.cpp
Chapter 2 Lights Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  WINMM.LIB, D3D9.LIB and D3DX9.LIB
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>
#include "d3d9.h"
#include "d3dx9.h"

// Window handles, class and caption text
HWND          g_hWnd;
HINSTANCE     g_hInst;
static char   g_szClass[]   = "LightClass";
static char   g_szCaption[] = "Light Demo by Jim Adams";

// The Direct3D and Device object
IDirect3D9       *g_pD3D       = NULL;
IDirect3DDevice9 *g_pD3DDevice = NULL;

// The 3-D vertex format and descriptor
typedef struct {
  FLOAT x, y, z;     // 3-D coordinates
  FLOAT nx, ny, nz;  // Normals
  D3DCOLOR Diffuse;  // Color
} sVertex;
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

// Vertex buffer
IDirect3DVertexBuffer9 *g_pVB = NULL;

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
  D3DXMATRIX matProj, matView;
  D3DLIGHT9 Light;
  BYTE *Ptr;
  sVertex Verts[16] = {
    { -100.0f,  100.0f, -100.0f, 0.0f,0.0f,-1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f,  100.0f, -100.0f, 0.0f,0.0f,-1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f, -100.0f, -100.0f, 0.0f,0.0f,-1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f, -100.0f, -100.0f, 0.0f,0.0f,-1.0f, D3DCOLOR_RGBA(255,255,255,255) },

    {  100.0f,  100.0f, -100.0f, 1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f,  100.0f,  100.0f, 1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f, -100.0f, -100.0f, 1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f, -100.0f,  100.0f, 1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },

    {  100.0f,  100.0f,  100.0f, 0.0f,0.0f,1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f,  100.0f,  100.0f, 0.0f,0.0f,1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    {  100.0f, -100.0f,  100.0f, 0.0f,0.0f,1.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f, -100.0f,  100.0f, 0.0f,0.0f,1.0f, D3DCOLOR_RGBA(255,255,255,255) },

    { -100.0f,  100.0f,  100.0f, -1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f,  100.0f, -100.0f, -1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f, -100.0f,  100.0f, -1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) },
    { -100.0f, -100.0f, -100.0f, -1.0f,0.0f,0.0f, D3DCOLOR_RGBA(255,255,255,255) }
  };

  // Do a windowed mode initialization of Direct3D
  if((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return FALSE;
  if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    return FALSE;
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                  &d3dpp, &g_pD3DDevice)))
    return FALSE;

  // Set the rendering states
  g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
  g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

  // Create and set the projection matrix
  D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4.0f, 1.33333f, 1.0f, 1000.0f);
  g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

  // Create and set the view matrix
  D3DXMatrixLookAtLH(&matView,                                \
                     &D3DXVECTOR3(0.0f, 0.0f, -500.0f),       \
                     &D3DXVECTOR3(0.0f, 0.0f, 0.0f),          \
                     &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
  g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

  // Create the vertex buffer and set data
  g_pD3DDevice->CreateVertexBuffer(sizeof(sVertex)*16, 0,     \
                          VERTEXFVF, D3DPOOL_DEFAULT,         \
                          &g_pVB, NULL);
  g_pVB->Lock(0,0, (void**)&Ptr, 0);
  memcpy(Ptr, Verts, sizeof(Verts));
  g_pVB->Unlock();

  // Set light data, color, position, and range
  ZeroMemory(&Light, sizeof(Light));
  Light.Type = D3DLIGHT_POINT;
  Light.Diffuse.r = Light.Ambient.r = 0.5f;
  Light.Diffuse.g = Light.Ambient.g = 0.5f;
  Light.Diffuse.b = Light.Ambient.b = 0.0f;
  Light.Diffuse.a = Light.Ambient.a = 1.0f;
  Light.Range = 1000.0f;
  Light.Attenuation0 = 0.5f;
  Light.Position.x = 300.0f;
  Light.Position.y = 0.0f;
  Light.Position.z = -600.0f;

  // Set and enable the light
  g_pD3DDevice->SetLight(0, &Light);
  g_pD3DDevice->LightEnable(0, TRUE);

  return TRUE;
}

BOOL DoShutdown()
{
  // Release vertex buffer
  if(g_pVB != NULL)
    g_pVB->Release();

  // Release device and 3D objects
  if(g_pD3DDevice != NULL)
    g_pD3DDevice->Release();

  if(g_pD3D != NULL)
    g_pD3D->Release();

  return TRUE;
}

BOOL DoFrame()
{
  D3DXMATRIX matWorld;

  // Clear device backbuffer
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                        D3DCOLOR_RGBA(0,0,0,255), 1.0f, 0);

  // Begin scene
  if(SUCCEEDED(g_pD3DDevice->BeginScene())) {

    // Create and set the world transformation matrix
    // Rotate object along Y-axis
    D3DXMatrixRotationY(&matWorld, (float)timeGetTime() / 1000.0f);
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // Set the vertex stream and shader
    g_pD3DDevice->SetStreamSource(0, g_pVB, 0, sizeof(sVertex));
    g_pD3DDevice->SetFVF(VERTEXFVF);

    // Draw the vertex buffer
    for(short i=0;i<4;i++)
      g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);

    // End the scene
    g_pD3DDevice->EndScene();
  }

  // Display the scene
  g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

  return TRUE;
}
