/**************************************************
WinMain.cpp
Chapter 2 Billboard Demo

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
static char   g_szClass[]   = "BillboardClass";
static char   g_szCaption[] = "Billboard Demo by Jim Adams";

// The Direct3D and Device object
IDirect3D9       *g_pD3D       = NULL;
IDirect3DDevice9 *g_pD3DDevice = NULL;

// The 3D vertex format and descriptor
typedef struct {
  FLOAT x, y, z;   // 3D coordinates
  FLOAT u, v;      // Texture coordinates
} s3DVertex;
#define FVF3D (D3DFVF_XYZ | D3DFVF_TEX1)

// The billboard vertex buffer and texture
IDirect3DVertexBuffer9 *g_pBillboardVB      = NULL;
IDirect3DTexture9      *g_pBillboardTexture = NULL;

// The floor vertex buffer and texture
IDirect3DVertexBuffer9 *g_pFloorVB      = NULL;
IDirect3DTexture9      *g_pFloorTexture = NULL;

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL DoInit();
BOOL DoShutdown();
BOOL DoFrame();
BOOL SetupMeshes();

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
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

long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
  D3DXMATRIX            matView, matProj;

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

  // Set the render states
  g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
  g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  g_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
  g_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

  // Create and set the projection transformation
  D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f);
  g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

  // Create the meshes
  SetupMeshes();

  return TRUE;
}

BOOL DoShutdown()
{
  // Release textures and vertex buffers
  if(g_pBillboardTexture != NULL)
    g_pBillboardTexture->Release();

  if(g_pBillboardVB != NULL)
    g_pBillboardVB->Release();

  if(g_pFloorTexture != NULL)
    g_pFloorTexture->Release();

  if(g_pFloorVB != NULL)
    g_pFloorVB->Release();

  // Release device and 3D objects
  if(g_pD3DDevice != NULL)
    g_pD3DDevice->Release();

  if(g_pD3D != NULL)
    g_pD3D->Release();

  return TRUE;
}

BOOL DoFrame()
{
  D3DXMATRIX matView, matWorld;
  float      Angle;
  short      i, j;

  // Clear device backbuffer
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,64,128,255), 1.0f, 0);

  // Update the view position
  Angle = (float)timeGetTime() / 2000.0f;
  D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3((float)cos(Angle) * 200.0f, 200.0f, (float)sin(Angle) * 200.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
  g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

  if(SUCCEEDED(g_pD3DDevice->BeginScene())) {
    // Draw the floor
    g_pD3DDevice->SetStreamSource(0, g_pFloorVB, 0, sizeof(s3DVertex));
    g_pD3DDevice->SetFVF(FVF3D);
    g_pD3DDevice->SetTexture(0, g_pFloorTexture);
    D3DXMatrixIdentity(&matWorld);
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    // Draw the billboards
    g_pD3DDevice->SetStreamSource(0, g_pBillboardVB, 0, sizeof(s3DVertex));
    g_pD3DDevice->SetTexture(0, g_pBillboardTexture);

    g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    D3DXMatrixTranspose(&matWorld, &matView);
    for(i=0;i<3;i++) {
      for(j=0;j<3;j++) {
        matWorld._41 = (float)i *  80.0f - 80.0f;
        matWorld._42 = 0.0f;
        matWorld._43 = (float)j *  80.0f - 80.0f;

        g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
        g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
      }
    }
    g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // Clear the texture usage
    g_pD3DDevice->SetTexture(0, NULL);

    // End the scene
    g_pD3DDevice->EndScene();
  }

  // Display the scene
  g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

  return TRUE;
}

BOOL SetupMeshes()
{
  BYTE *Ptr;
  s3DVertex BillboardVerts[4] = {
      { -42.0f, 80.0f, 0.0f, 0.0f, 0.0f },
      {  40.0f, 80.0f, 0.0f, 1.0f, 0.0f },
      { -40.0f,  0.0f, 0.0f, 0.0f, 1.0f },
      {  40.0f,  0.0f, 0.0f, 1.0f, 1.0f }
    };

  s3DVertex FloorVerts[4] = {
      { -100.0f, 0.0f,  100.0f, 0.0f, 0.0f },
      {  100.0f, 0.0f,  100.0f, 1.0f, 0.0f },
      { -100.0f, 0.0f, -100.0f, 0.0f, 1.0f },
      {  100.0f, 0.0f, -100.0f, 1.0f, 1.0f }
    };

  // Create vertex buffers and stuff in data
  // Billboard
  if(FAILED(g_pD3DDevice->CreateVertexBuffer(                 \
            sizeof(BillboardVerts), 0, FVF3D,                 \
            D3DPOOL_DEFAULT, &g_pBillboardVB, NULL)))
    return FALSE;
  if(FAILED(g_pBillboardVB->Lock(0,0, (void**)&Ptr, 0)))
    return FALSE;
  memcpy(Ptr, BillboardVerts, sizeof(BillboardVerts));
  g_pBillboardVB->Unlock();

   // Floor
  if(FAILED(g_pD3DDevice->CreateVertexBuffer(                 \
            sizeof(FloorVerts), 0, FVF3D,                     \
            D3DPOOL_DEFAULT, &g_pFloorVB, NULL)))
    return FALSE;
  if(FAILED(g_pFloorVB->Lock(0,0, (void**)&Ptr, 0)))
    return FALSE;
  memcpy(Ptr, FloorVerts, sizeof(FloorVerts));
  g_pFloorVB->Unlock();

  // Get textures 
  D3DXCreateTextureFromFile(g_pD3DDevice, "Floor.bmp", &g_pFloorTexture);
  D3DXCreateTextureFromFileEx(g_pD3DDevice, "Billboard.bmp", 
    D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5, 
    D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
    D3DCOLOR_RGBA(0,0,0,255), NULL, NULL, &g_pBillboardTexture);

  return TRUE;
}
