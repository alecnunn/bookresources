/**************************************************
WinMain.cpp
Chapter 2 Particle Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  WINMM.LIB, D3D9.LIB, and D3DX9.LIB
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>
#include "d3d9.h"
#include "d3dx9.h"

// Window handles, class and caption text
HWND          g_hWnd;
HINSTANCE     g_hInst;
static char   g_szClass[]   = "ParticleClass";
static char   g_szCaption[] = "Particle Demo by Jim Adams";

// The Direct3D and Device object
IDirect3D9       *g_pD3D       = NULL;
IDirect3DDevice9 *g_pD3DDevice = NULL;

// The particle vertex format and descriptor
typedef struct {
  FLOAT x, y, z;    // 3D coordinates
  D3DCOLOR Diffuse; // Color
  FLOAT u, v;       // Texture coordinates
} sVertex;
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// The Particle vertex buffer and texture
IDirect3DVertexBuffer9 *g_pParticleVB      = NULL;
IDirect3DTexture9      *g_pParticleTexture = NULL;

// Create a structure for tracking particles
typedef struct sParticle {
  float    XPos, YPos, ZPos;  // Coordinates
  float    XAdd, YAdd, ZAdd;  // Movement values
  float    Red, Green, Blue;  // Colors
  long     Timer, Counter;    // Current and update counter

  sParticle()
  {
    // Position particle at origin
    XPos = YPos = ZPos = 0.0f;

    // Get a random update counter
    Counter = rand() % 50 + 10;
    Timer = 0;

    // Get a random speed
    XAdd = (float)(rand() % 11) - 5.0f;
    YAdd = (float)(rand() % 11) - 5.0f;
    ZAdd = (float)(rand() % 11) - 5.0f;

    // Get a random color
    Red   = (float)(rand() % 101) / 100.0f;
    Green = (float)(rand() % 101) / 100.0f;
    Blue  = (float)(rand() % 101) / 100.0f;
  }
} sParticle;

sParticle *g_pParticles = NULL;

// Function prototypes
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL DoInit();
BOOL DoShutdown();
BOOL DoFrame();
BOOL SetupParticles();

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
  g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
  g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

  // Set ambient light to highest level (to see particles)
  g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);

  // Create and set the projection transformation
  D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f);
  g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

  // Create and set the view transformation
  D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, 0.0f, -500.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
  g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

  // Create the vertices
  SetupParticles();

  return TRUE;
}

BOOL DoShutdown()
{
  // Free particles
  if(g_pParticles != NULL)
    delete [] g_pParticles;

  // Release textures and vertex buffers
  if(g_pParticleTexture != NULL)
    g_pParticleTexture->Release();

  if(g_pParticleVB != NULL)
    g_pParticleVB->Release();

  // Release device and 3D objects
  if(g_pD3DDevice != NULL)
    g_pD3DDevice->Release();

  if(g_pD3D != NULL)
    g_pD3D->Release();

  return TRUE;
}

BOOL DoFrame()
{
  D3DXMATRIX matView, matWorld, matTransposed, matTrans;
  static D3DMATERIAL9 Mat;
  static BOOL InitMat = TRUE;
  static DWORD Counter = timeGetTime();

  // Limit to 30fps
  if(timeGetTime() < Counter+33)
    return TRUE;
  Counter = timeGetTime();

  // Configure the material if first time called
  if(InitMat == TRUE) {
    InitMat = FALSE;
    ZeroMemory(&Mat, sizeof(Mat));
    Mat.Diffuse.a = Mat.Ambient.a = 0.5f;
  }

  // Clear device backbuffer
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,64,128,255), 1.0f, 0);

  if(SUCCEEDED(g_pD3DDevice->BeginScene())) {

    // Set the particle source, shader, and texture
    g_pD3DDevice->SetStreamSource(0, g_pParticleVB, 0, sizeof(sVertex));
    g_pD3DDevice->SetFVF(VERTEXFVF);
    g_pD3DDevice->SetTexture(0, g_pParticleTexture);

    // Get and set the transposed view matrix (billboard technique)
    g_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixTranspose(&matTransposed, &matView);

    // Enable alpha blending
    g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // Loop through all particles and draw them
    for(short i=0;i<512;i++) {

      // Move particle first
      g_pParticles[i].XPos += g_pParticles[i].XAdd;
      g_pParticles[i].YPos += g_pParticles[i].YAdd;
      g_pParticles[i].ZPos += g_pParticles[i].ZAdd;

      // Reverse movements if past counter
      if((g_pParticles[i].Timer += 1) >= g_pParticles[i].Counter) {
        g_pParticles[i].Timer = 0;
        g_pParticles[i].XAdd *= -1.0f;
        g_pParticles[i].YAdd *= -1.0f;
        g_pParticles[i].ZAdd *= -1.0f;
      }

      // Setup the particle's world transformation
      D3DXMatrixTranslation(&matTrans, g_pParticles[i].XPos, \
                                       g_pParticles[i].YPos, \
                                       g_pParticles[i].ZPos);
      D3DXMatrixMultiply(&matWorld, &matTrans, &matTransposed);
      g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

      // Set the particle's material
      Mat.Diffuse.r = Mat.Ambient.r = g_pParticles[i].Red;
      Mat.Diffuse.g = Mat.Ambient.g = g_pParticles[i].Green;
      Mat.Diffuse.b = Mat.Ambient.b = g_pParticles[i].Blue;
      g_pD3DDevice->SetMaterial(&Mat);

      // Draw the particle
      g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }

    // Clear the texture usage
    g_pD3DDevice->SetTexture(0, NULL);

    // End the scene
    g_pD3DDevice->EndScene();
  }

  // Display the scene
  g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

  return TRUE;
}

BOOL SetupParticles()
{
  BYTE *Ptr;
  sVertex Verts[4] = {
    { -50.0f, 50.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f },
    {  50.0f, 50.0f, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f },
    { -50.0f,  0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 1.0f },
    {  50.0f,  0.0f, 0.0f, 0xFFFFFFFF, 1.0f, 1.0f }
  };

  // Create vertex buffers and set data
  g_pD3DDevice->CreateVertexBuffer(sizeof(Verts)*4, 0,        \
                  VERTEXFVF, D3DPOOL_DEFAULT,                 \
                  &g_pParticleVB, NULL);
  g_pParticleVB->Lock(0,0, (void**)&Ptr, 0);
  memcpy(Ptr, Verts, sizeof(Verts));
  g_pParticleVB->Unlock();

  // Get textures 
  D3DXCreateTextureFromFile(g_pD3DDevice, "Particle.bmp", &g_pParticleTexture);

  // Create some particles
  g_pParticles = new sParticle[512]();

  return TRUE;
}
