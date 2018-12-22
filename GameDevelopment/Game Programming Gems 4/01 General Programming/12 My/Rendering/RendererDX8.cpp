//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 RENDERER CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "RendererDX8.h"
#ifdef    _DEBUG
#include  "RendererDX8.inl"
#endif

#include  "IBufferDX8.h"
#include  "MeshDX8.h"
#include  "SwapChainDX8.h"
#include  "TextureDX8.h"
#include  "VBufferDX8.h"

#include  "VectorMatrix.h"
#include  "EngineMaterial.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CRendererDX8,CRenderer);

//--------------------------------------------------------------------------------------------------------------------//
//                                                BASIC OPS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create & init renderer
// in : window handle,flags(=_WINDOWED_|_ZBUFFER_),width(=0) & height(=0)
// out: OK?
// rem: if width==height==0 the client area of the window is used
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::Create(const HWND hWnd,const u32 u32Flags,const u32 u32Width,const u32 u32Height)
  {
  Destroy();

  bool boWindowed =(u32Flags & WINDOWED) != 0;
  bool boZBuffer  =(u32Flags & ZBUFFER ) != 0;
  u32  u32Width2  = u32Width;
  u32  u32Height2 = u32Height;

  if(boWindowed)
    {
    RECT rect;
    if(!GetClientRect(hWnd,&rect)) return false;
    if(!u32Width)  u32Width2  = rect.right;
    if(!u32Height) u32Height2 = rect.bottom;
    }

  //

  m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
  if(!m_pD3D) return false;

  D3DDISPLAYMODE d3ddm;
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm))) return false;

  DWORD    dwVtxProc = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  D3DCAPS8 d3dcaps;
  if(SUCCEEDED(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dcaps)))
    {
    if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) dwVtxProc = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp,sizeof(d3dpp));
  d3dpp.BackBufferWidth  = u32Width2;
  d3dpp.BackBufferHeight = u32Height2;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.BackBufferCount  = 1;
  d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow    = hWnd;
  d3dpp.Windowed         = boWindowed;
  d3dpp.EnableAutoDepthStencil = boZBuffer;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;        // for screenshots

  HRESULT hrErr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,dwVtxProc,&d3dpp,&m_pDevice);
  if(FAILED(hrErr))
    {
    TCHAR szError[1024];
    D3DXGetErrorString(hrErr,szError,1024);
    MY_ASSERT(false,"CreateDevice failed");
    MY_ASSERT(false,szError);
    return false;
    }

  D3DCAPS8 d3dDevDesc8;
  if(FAILED(m_pDevice->GetDeviceCaps(&d3dDevDesc8))) return false;

  //

  D3DVIEWPORT8 d3dViewport;
  d3dViewport.X      = 0;
  d3dViewport.Y      = 0;
  d3dViewport.Width  = u32Width2;
  d3dViewport.Height = u32Height2;
  d3dViewport.MinZ   = 0.f;
  d3dViewport.MaxZ   = 1.f;
  m_pDevice->SetViewport(&d3dViewport);

  D3DCOLORVALUE black_color = { 0.0f, 0.0f, 0.0f, 0.0f };
  D3DCOLORVALUE white_color = { 1.0f, 1.0f, 1.0f, 1.0f };
  ZeroMemory(&m_matCurrent,sizeof(m_matCurrent));
  m_matCurrent.Diffuse  = white_color;
  m_matCurrent.Ambient  = white_color;
  m_matCurrent.Specular = black_color;
  m_matCurrent.Emissive = black_color;
  m_matCurrent.Power    = 0.f;
  m_pDevice->SetMaterial(&m_matCurrent);

  InitStates();

  // trfs

  SetModel2World(GPG4::CMat4x4::Identity);
  SetWorld2View (GPG4::CMat4x4::Identity);
  SetPerspective(DEG2RAD(60.f),1.f,1000.f);

  // swap chain 0

  CSwapChainDX8* pSwapChain = new CSwapChainDX8(0,boWindowed,boZBuffer,hWnd,u32Width2,u32Height2);
  if(!pSwapChain) return false;
  AddSwapChain(pSwapChain);

  IDirect3DSurface8* pBackBuffer;
  IDirect3DSurface8* pZStencil;
  m_pDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
  m_pDevice->GetDepthStencilSurface(&pZStencil);

  pSwapChain->SetInterface (NULL);
  pSwapChain->SetBackBuffer(pBackBuffer);
  pSwapChain->SetZStencil  (pZStencil);

  SelectSwapChain(0);

// hardcoded light

  CVect3D v3Dir(1.f,-1.f,-1.f);
  CVect4D v4Col(1.f,1.f,1.f,1.f);
  float*  pfCol = (float*)&v4Col;

  D3DLIGHT8 d3dLight;
  d3dLight.Type         = D3DLIGHT_DIRECTIONAL;
  d3dLight.Diffuse      = *(D3DCOLORVALUE*)pfCol;
  d3dLight.Specular     = black_color;
  d3dLight.Ambient      = black_color;
  d3dLight.Direction    = *(D3DVECTOR*)&v3Dir;
  d3dLight.Range        = 1e6;                              // otherwise SetLight can fail :(

  hrErr = m_pDevice->SetLight   (0,&d3dLight);
  hrErr = m_pDevice->LightEnable(0,true);

  for(u32 L = 1; L < 8; L++) m_pDevice->LightEnable(L,false);

// hardcoded texture

  hrErr = D3DXCreateTexture(m_pDevice,32,32,1,0,D3DFMT_R8G8B8,D3DPOOL_DEFAULT,&m_pTexWhite);
  if(FAILED(hrErr)) MessageBox(NULL,"D3DXCreateTexture failed","CRendererDX8::Create",MB_OK|MB_ICONEXCLAMATION);
  else
    {
    CVect4D FillColor(1.f);
    hrErr = D3DXFillTexture(m_pTexWhite,&CRendererDX8::FillPlain,(LPVOID)&FillColor);
    if(FAILED(hrErr)) MessageBox(NULL,"D3DXFillTexture failed","CRendererDX8::Create",MB_OK|MB_ICONEXCLAMATION);
    }

  //

  m_boInit = true;
  return true;
  }

//

void CRendererDX8::FillPlain(D3DXVECTOR4* pOut,D3DXVECTOR2* pTexCoord,D3DXVECTOR2* pTexelSize,LPVOID pData)
  {
  CVect4D* pColor = (CVect4D*)pData;
  if(pColor)
    {
    pOut->x = pColor->r;
    pOut->y = pColor->g;
    pOut->z = pColor->b;
    pOut->w = pColor->a;
    }

  UNREFERENCED_PARAMETER(pTexCoord);
  UNREFERENCED_PARAMETER(pTexelSize);
  }

//----------------------------------------------- InitStates ---------------------------------------------------------//
// init render & texture stages states
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CRendererDX8::InitStates()
  {
  // render states

  float fZero = 0.f;
  float fOne  = 1.f;
  float f64   = 64.f;

  m_boLighted = true;
  m_boZTest   = true;
  m_boCulling = true;
  m_boTexture = false;

  m_colTexFactor = 0xff80ff80;

  m_pDevice->SetRenderState(D3DRS_FILLMODE,                 D3DFILL_SOLID);
  m_pDevice->SetRenderState(D3DRS_SHADEMODE,                D3DSHADE_GOURAUD);
  m_pDevice->SetRenderState(D3DRS_LINEPATTERN,              0);
  m_pDevice->SetRenderState(D3DRS_LASTPIXEL,                true);
  m_pDevice->SetRenderState(D3DRS_CULLMODE,                 m_boCulling ? D3DCULL_CW : D3DCULL_NONE);
  m_pDevice->SetRenderState(D3DRS_DITHERENABLE,             true);
  m_pDevice->SetRenderState(D3DRS_CLIPPING,                 true);
  m_pDevice->SetRenderState(D3DRS_VERTEXBLEND,              D3DVBF_DISABLE);
  m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,          0);
  m_pDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, false);
  m_pDevice->SetRenderState(D3DRS_DEBUGMONITORTOKEN,        D3DDMT_ENABLE);
  m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, false);
  m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,         0xf);
  m_pDevice->SetRenderState(D3DRS_TWEENFACTOR,              *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_BLENDOP,                  D3DBLENDOP_ADD);

  m_pDevice->SetRenderState(D3DRS_EDGEANTIALIAS,            false);
  m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,     false);
  m_pDevice->SetRenderState(D3DRS_MULTISAMPLEMASK,          0xffffffff);

  m_pDevice->SetRenderState(D3DRS_PATCHEDGESTYLE,           D3DPATCHEDGE_DISCRETE);
  m_pDevice->SetRenderState(D3DRS_PATCHSEGMENTS,            *(DWORD*)&fOne);
  m_pDevice->SetRenderState(D3DRS_POSITIONORDER,            D3DORDER_CUBIC);
  m_pDevice->SetRenderState(D3DRS_NORMALORDER,              D3DORDER_LINEAR);

  m_pDevice->SetRenderState(D3DRS_ZENABLE,                  m_boZTest);
  m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,             true);
  m_pDevice->SetRenderState(D3DRS_ZFUNC,                    D3DCMP_LESSEQUAL);
  m_pDevice->SetRenderState(D3DRS_ZBIAS,                    0);

  m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,          false);
  m_pDevice->SetRenderState(D3DRS_SRCBLEND,                 /*D3DBLEND_ONE*/D3DBLEND_SRCALPHA);
  m_pDevice->SetRenderState(D3DRS_DESTBLEND,                /*D3DBLEND_ZERO*/D3DBLEND_INVSRCALPHA);
  m_pDevice->SetRenderState(D3DRS_ALPHAREF,                 0);
  m_pDevice->SetRenderState(D3DRS_ALPHAFUNC,                D3DCMP_ALWAYS);
  m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,         true);

  m_pDevice->SetRenderState(D3DRS_FOGENABLE,                false);
  m_pDevice->SetRenderState(D3DRS_FOGCOLOR,                 0);
  m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE,             D3DFOG_NONE);
  m_pDevice->SetRenderState(D3DRS_FOGSTART,                 *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_FOGEND,                   *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_FOGDENSITY,               *(DWORD*)&fOne);
  m_pDevice->SetRenderState(D3DRS_RANGEFOGENABLE,           false);
  m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE,            D3DFOG_NONE);

  m_pDevice->SetRenderState(D3DRS_SPECULARENABLE,           false);
  m_pDevice->SetRenderState(D3DRS_LIGHTING,                 m_boLighted);
  m_pDevice->SetRenderState(D3DRS_AMBIENT,                  D3DCOLOR_XRGB(128,128,128));
  m_pDevice->SetRenderState(D3DRS_COLORVERTEX,              false);
  m_pDevice->SetRenderState(D3DRS_LOCALVIEWER,              true);
  m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS,         false);
  m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,    D3DMCS_MATERIAL);
  m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,   D3DMCS_MATERIAL);
  m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,    D3DMCS_MATERIAL);
  m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,   D3DMCS_MATERIAL);

  m_pDevice->SetRenderState(D3DRS_STENCILENABLE,            false);
  m_pDevice->SetRenderState(D3DRS_STENCILFAIL,              D3DSTENCILOP_KEEP);
  m_pDevice->SetRenderState(D3DRS_STENCILZFAIL,             D3DSTENCILOP_KEEP);
  m_pDevice->SetRenderState(D3DRS_STENCILPASS,              D3DSTENCILOP_KEEP);
  m_pDevice->SetRenderState(D3DRS_STENCILFUNC,              D3DCMP_ALWAYS);
  m_pDevice->SetRenderState(D3DRS_STENCILREF,               0);
  m_pDevice->SetRenderState(D3DRS_STENCILMASK,              0xffffffff);
  m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK,         0xffffffff);

  m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,            m_colTexFactor);
  m_pDevice->SetRenderState(D3DRS_WRAP0,                    0);//D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP1,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP2,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP3,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP4,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP5,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP6,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);
  m_pDevice->SetRenderState(D3DRS_WRAP7,                    D3DWRAPCOORD_0 | D3DWRAPCOORD_1 | D3DWRAPCOORD_2 | D3DWRAPCOORD_3);

  m_pDevice->SetRenderState(D3DRS_POINTSIZE,                *(DWORD*)&fOne);
  m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN,            *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,        false);
  m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,         false);
  m_pDevice->SetRenderState(D3DRS_POINTSCALE_A,             *(DWORD*)&fOne);
  m_pDevice->SetRenderState(D3DRS_POINTSCALE_B,             *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_POINTSCALE_C,             *(DWORD*)&fZero);
  m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX,            *(DWORD*)&f64);

  // texture stages states

	m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	m_pDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);

  m_pDevice->SetTexture(0,NULL);
  m_pDevice->SetTexture(1,NULL);

  m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
  m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

  m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
  m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

  m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
  m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

  m_pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
  m_pDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// destroy renderer
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CRendererDX8::Destroy()
  {
  CRenderer::Destroy();

  if(m_pDevice) m_pDevice->Release();
  if(m_pD3D)    m_pD3D   ->Release();

  m_pDevice = NULL;
  m_pD3D    = NULL;

  if(m_pTexWhite) m_pTexWhite->Release();
  m_pTexWhite = NULL;
  }

//----------------------------------------------- CreateSwapChain ----------------------------------------------------//
// create additional swap chain
// in : window handle,flags(=_WINDOWED_|_ZBUFFER_),width(=0) & height(=0)
// out: chain handle, 0==error
// rem: if width==height==0 the client area of the window is used
//--------------------------------------------------------------------------------------------------------------------//

u32 CRendererDX8::CreateSwapChain(const HWND hWnd,const u32 u32Flags,const u32 u32Width,const u32 u32Height)
  {
  bool boWindowed =(u32Flags & WINDOWED) != 0;
  bool boZBuffer  =(u32Flags & ZBUFFER ) != 0;
  u32  u32Width2  = u32Width;
  u32  u32Height2 = u32Height;

  if(boWindowed)
    {
    RECT rect;
    if(!GetClientRect(hWnd,&rect)) return 0;
    if(!u32Width)  u32Width2  = rect.right;
    if(!u32Height) u32Height2 = rect.bottom;
    }

  if(!u32Width2 || !u32Height2) return 0;

  // back buffer

  if(!m_pD3D | !m_pDevice) return 0;

  D3DDISPLAYMODE d3ddm;
  if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm))) return 0;

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp,sizeof(d3dpp));
  d3dpp.BackBufferWidth  = u32Width2;
  d3dpp.BackBufferHeight = u32Height2;
  d3dpp.BackBufferFormat = d3ddm.Format;
  d3dpp.BackBufferCount  = 1;
  d3dpp.MultiSampleType  = D3DMULTISAMPLE_NONE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow    = hWnd;
  d3dpp.Windowed         = boWindowed;
  d3dpp.EnableAutoDepthStencil = false;
  d3dpp.Flags            = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

  IDirect3DSwapChain8* pInterface;
  HRESULT hrErr = m_pDevice->CreateAdditionalSwapChain(&d3dpp,&pInterface);
  if(FAILED(hrErr))
    {
    TCHAR szError[1024];
    D3DXGetErrorString(hrErr,szError,1024);
    MY_ASSERT(false,"CreateAdditionalSwapChain failed");
    MY_ASSERT(false,szError);
    return 0;
    }

  // Z & stencil

  IDirect3DSurface8* pZStencil = NULL;

  if(boZBuffer)
    {
    hrErr = m_pDevice->CreateDepthStencilSurface(u32Width2,u32Height2,D3DFMT_D16,D3DMULTISAMPLE_NONE,&pZStencil);
    if(FAILED(hrErr))
      {
      TCHAR szError[1024];
      D3DXGetErrorString(hrErr,szError,1024);
      MY_ASSERT(false,"CreateDepthStencilSurface failed");
      MY_ASSERT(false,szError);
      pInterface->Release();
      return 0;
      }
    }

  //

  CSwapChainDX8* pSwapChain = new CSwapChainDX8(CSwapChain::GetFreeHandle(),boWindowed,boZBuffer,hWnd,u32Width2,u32Height2);
  if(!pSwapChain)
    {
    pInterface->Release();
    if(pZStencil) pZStencil->Release();
    return 0;
    }

  IDirect3DSurface8* pBackBuffer;
  pInterface->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

  AddSwapChain(pSwapChain);
  pSwapChain->SetInterface (pInterface);
  pSwapChain->SetBackBuffer(pBackBuffer);
  pSwapChain->SetZStencil  (pZStencil);

  return pSwapChain->GetHandle();
  }

//----------------------------------------------- ReleaseSwapChain ---------------------------------------------------//
// release additional swap chain
// in : chain handle
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CRendererDX8::ReleaseSwapChain(const u32 u32Chain)
  {
  CSwapChainDX8* pSwapChain = reinterpret_cast<CSwapChainDX8*>(GetSwapChain(u32Chain));
  if(!pSwapChain) return;

  RemoveSwapChain(pSwapChain);
  delete pSwapChain;
  }

//----------------------------------------------- SelectSwapChain ----------------------------------------------------//
// select as rendering target
// in : chain handle
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SelectSwapChain(const u32 u32Chain)
  {
  if(!m_pDevice) return false;
  if((u32Chain == m_u32CurrentChain) && m_pCurrentChain) return true;

  CSwapChainDX8* pSwapChain = reinterpret_cast<CSwapChainDX8*>(GetSwapChain(u32Chain));
  if(!pSwapChain) return false;

  IDirect3DSurface8* pBackBuffer = pSwapChain->GetBackBuffer();
  IDirect3DSurface8* pZStencil   = pSwapChain->GetZStencil();
  if(!pBackBuffer) return false;

  if(FAILED(m_pDevice->SetRenderTarget(pBackBuffer,pZStencil))) return false;
  m_pCurrentChain   = pSwapChain;
  m_u32CurrentChain = u32Chain;
  return true;
  }

//----------------------------------------------- BeginScene ---------------------------------------------------------//
// begin frame
// in :
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::BeginScene()
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->BeginScene());
  }

//----------------------------------------------- EndScene -----------------------------------------------------------//
// end frame
// in :
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::EndScene()
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->EndScene());
  }

//----------------------------------------------- Clear --------------------------------------------------------------//
// clear buffers
// in : buffers to clear(=IRenderer::_CLEAR_BACKDEPTH_)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::Clear(const u32 u32Flags)
  {
  if(!m_pDevice) return false;
  
  u32 u32Buffers = 0;
  if(u32Flags & IRenderer::CLEAR_BACK)    u32Buffers |= D3DCLEAR_TARGET;
  if(u32Flags & IRenderer::CLEAR_DEPTH)   u32Buffers |= D3DCLEAR_ZBUFFER;
  if(u32Flags & IRenderer::CLEAR_STENCIL) u32Buffers |= D3DCLEAR_STENCIL;

  D3DCOLOR ClearColor = D3DCOLOR_XRGB(255,128,128);
  if(m_pCurrentChain)
    {
    CVect3D v3Color = m_pCurrentChain->GetClearColor();
    ClearColor = D3DCOLOR_XRGB(u8(255*v3Color.r),u8(255*v3Color.g),u8(255*v3Color.b));
    }

  HRESULT hrErr = m_pDevice->Clear(0,NULL,u32Buffers,ClearColor,1.f,128);
  if(SUCCEEDED(hrErr)) return true;

  TCHAR szError[1024];
  D3DXGetErrorString(hrErr,szError,1024);
  MY_ASSERT(false,"Clear failed");
  MY_ASSERT(false,szError);
  return false;
  }

//----------------------------------------------- Swap ---------------------------------------------------------------//
// swap front & back buffers
// in : source(=NULL) & dest(=NULL) rects,nb vbls(=0) to wait (TBI)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::Swap(const RECT* pSrcRect,const RECT* pDestRect,const u32 u32Vbls)
  {
  if(!m_pDevice) return false;
  if(!m_u32CurrentChain) return SUCCEEDED(m_pDevice->Present(pSrcRect,pDestRect,NULL,NULL));

  CSwapChainDX8* pSwapChain = reinterpret_cast<CSwapChainDX8*>(m_pCurrentChain);
  if(!pSwapChain) return false;
  IDirect3DSwapChain8* pInterface = pSwapChain->GetInterface();
  if(!pInterface) return false;
  return SUCCEEDED(pInterface->Present(pSrcRect,pDestRect,NULL,NULL));

  UNREFERENCED_PARAMETER(u32Vbls);
  }

//----------------------------------------------- SetWireframe -------------------------------------------------------//
// enable/disable wireframe mode
// in : enable?
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetWireframe(const bool boON)
  {
  if(!m_pDevice) return false;
  return SUCCEEDED(m_pDevice->SetRenderState(D3DRS_FILLMODE,boON ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                IB,VB,MESH                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateIB -----------------------------------------------------------//
// create index buffer class instance
// in :
// out: instance
//--------------------------------------------------------------------------------------------------------------------//

CIBuffer* CRendererDX8::CreateIB()
  {
  return new CIBufferDX8(m_pDevice);
  }

//----------------------------------------------- CreateVB -----------------------------------------------------------//
// create vertex buffer class instance
// in :
// out: instance
//--------------------------------------------------------------------------------------------------------------------//

CVBuffer* CRendererDX8::CreateVB()
  {
  return new CVBufferDX8(m_pDevice);
  }

//----------------------------------------------- CreateMesh ---------------------------------------------------------//
// create mesh instance
// in :
// out: instance
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CRendererDX8::CreateMesh()
  {
  CMeshDX8* pMesh = new CMeshDX8(m_pDevice);
  if(!pMesh) return NULL;

  if(0 != pMesh->CreateIBVB(this))
    {
    delete pMesh;
    return NULL;
    }

  return pMesh;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TEXTURE                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateTexture ------------------------------------------------------//
// create texture instance
// in :
// out: instance
//--------------------------------------------------------------------------------------------------------------------//

CEngineTexture* CRendererDX8::CreateTexture()
  {
  return new CTextureDX8(m_pDevice);
  }

//----------------------------------------------- Shoot --------------------------------------------------------------//
// save frame buffer to texture
// in : directory
// out: 0=OK
// rem: FB 24b not tested
//--------------------------------------------------------------------------------------------------------------------//

u32 CRendererDX8::Shoot(const CStdString& strDir)
  {
  if(!m_pDevice) return -1;
  HRESULT hrErr;

  // get back buffer infos

  LPDIRECT3DSURFACE8 pSurface = NULL;
  //if(FAILED(hrErr = m_pDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pSurface)))
  if(FAILED(hrErr = m_pDevice->GetRenderTarget(&pSurface)))
    return -2;

  D3DSURFACE_DESC descSurface;
  if(FAILED(hrErr = pSurface->GetDesc(&descSurface)))
    {
    pSurface->Release();
    return -3;
    }
  u32 u32Width  = descSurface.Width;
  u32 u32Height = descSurface.Height;

  // choose format

  u32 u32Format  = CEngineTexture::TEXFORMAT_NONE;
  u32 u32PixelBytes;
  switch(descSurface.Format)
    {
    case D3DFMT_R5G6B5   : u32Format = CEngineTexture::TEXFORMAT_0565; u32PixelBytes = 2; break;
    case D3DFMT_R8G8B8   : u32Format = CEngineTexture::TEXFORMAT_0888; u32PixelBytes = 3; break;
    case D3DFMT_X8R8G8B8 : u32Format = CEngineTexture::TEXFORMAT_8888; u32PixelBytes = 4; break;
    }

  if(u32Format == CEngineTexture::TEXFORMAT_NONE) 
    {
    pSurface->Release();
    return -4;
    }

  // create temporary texture

  CEngineTexture* pTex = new CEngineTexture;
  if(!pTex) 
    {
    pSurface->Release();
    return -5;
    }

  hrErr = pTex->Create(u32Width,u32Height,u32Format);
  if(hrErr != 0)
    {
    pSurface->Release();
    delete pTex;
    return -6;
    }

  // get texture's RAW memory

  u8* pu8Dest = (u8*)pTex->GetSurfacePtr();
  if(!pu8Dest)
    {
    pSurface->Release();
    delete pTex;
    return -7;
    }

  // copy back buffer to texture

  D3DLOCKED_RECT lockRect;
  if(FAILED(hrErr = pSurface->LockRect(&lockRect,NULL,D3DLOCK_READONLY)))
    {
    pSurface->Release();
    delete pTex;
    return -8;
    }

  u8* pu8Buff      = (u8*)lockRect.pBits;
  u32 u32LineBytes = lockRect.Pitch;

  for(u32 u32Row = 0; u32Row < u32Height; u32Row++)
    {
    memcpy(pu8Dest,pu8Buff,u32Width*u32PixelBytes);
    pu8Dest += u32Width*u32PixelBytes;
    pu8Buff += u32LineBytes;
    }

  pSurface->UnlockRect();
  pSurface->Release();

  // save texture

  hrErr = pTex->Shoot(strDir);
  delete  pTex;

  return hrErr;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- DrawVBIB -----------------------------------------------------------//
// render VB & IB primitive
// in : primitive type,VB,IB
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CRendererDX8::DrawVBIB(RENDERER_PRIMITIVE Prim,CVBuffer* pVB,CIBuffer* pIB)
  {
  if(!m_pDevice) return -1;
  if(!pVB->GetActiveCount() || !pIB->GetActiveCount()) return 0;

  CVBufferDX8* pVBdx8 = DYNAMIC_CAST(CVBufferDX8,pVB);
  CIBufferDX8* pIBdx8 = DYNAMIC_CAST(CIBufferDX8,pIB);
  if(!pVBdx8 || !pIBdx8) return -2;

  D3DPRIMITIVETYPE Type;
  u32 u32PrimCount = pIBdx8->GetActiveCount();
  switch(Prim)
    {
    case TRILIST   : Type = D3DPT_TRIANGLELIST; u32PrimCount /= 3; break;
    case LINELIST  : Type = D3DPT_LINELIST;     u32PrimCount /= 2; break;
    case LINESTRIP : Type = D3DPT_LINESTRIP;    u32PrimCount -= 1; break;
    default        : return -3;
    }

  //

  HRESULT hrErr = S_OK;
  hrErr |= m_pDevice->SetVertexShader(  pVBdx8->GetFVF());
  hrErr |= m_pDevice->SetStreamSource(0,pVBdx8->GetInterface(),pVBdx8->GetVertexSize());
  hrErr |= m_pDevice->SetIndices     (  pIBdx8->GetInterface(),0);
  if(FAILED(hrErr)) return -4;

  hrErr  = m_pDevice->DrawIndexedPrimitive(Type,0,pVBdx8->GetActiveCount(),0,u32PrimCount);
  if(FAILED(hrErr)) return hrErr;
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRF                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SetModel2World -----------------------------------------------------//
// set model->world trf
// in : model->world matrix
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetModel2World(const CMat4x4& m4Matrix)
  {
  if(!m_pDevice) return false;
  bool boRes = SUCCEEDED(m_pDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m4Matrix));
  if(boRes) memcpy(&m_m4World,&m4Matrix,sizeof(CMat4x4));
  return boRes;
  }

//----------------------------------------------- SetWorld2View ------------------------------------------------------//
// set world->view trf
// in : world->view matrix
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetWorld2View(const CMat4x4& m4Matrix)
  {
  if(!m_pDevice) return false;
  bool boRes = SUCCEEDED(m_pDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&m4Matrix));
  if(boRes) memcpy(&m_m4View,&m4Matrix,sizeof(CMat4x4));
  return boRes;
  }

//----------------------------------------------- SetPerspective -----------------------------------------------------//
// set perspective projection
// in : fov,near & far distances,screen ratio (=4/3)
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetPerspective(const float fFov,const float fNear,const float fFar,const float fRatio)
  {
  if(!m_pDevice) return false;

  float fRight = (float)tan(fFov*0.5f)*fNear;
  float fTop   = fRight/fRatio;

  D3DXMATRIX m4Proj;
  D3DXMatrixPerspectiveRH(&m4Proj,fRight*2.f,fTop*2.f,fNear,fFar);
  bool boRes = SUCCEEDED(m_pDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m4Proj));

  if(boRes) memcpy(&m_m4Proj,&m4Proj,sizeof(CMat4x4));
  return boRes;
  }

//----------------------------------------------- SetOrtho -----------------------------------------------------------//
// set orthographic projection
// in : volume extents
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetOrtho(const CVect3D& v3Min,const CVect3D& v3Max)
  {
  if(!m_pDevice) return false;

  D3DXMATRIX m4Proj;
  D3DXMatrixOrthoOffCenterRH(&m4Proj,v3Min.x,v3Max.x,v3Min.y,v3Max.y,v3Min.z,v3Max.z);
  bool boRes = SUCCEEDED(m_pDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m4Proj));

  if(boRes) memcpy(&m_m4Proj,&m4Proj,sizeof(CMat4x4));
  return boRes;
  }

//----------------------------------------------- SetProjection ------------------------------------------------------//
// set projection
// in : proj matrix
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetProjection(const CMat4x4& m4Proj)
  {
  if(!m_pDevice) return false;
  bool boRes = SUCCEEDED(m_pDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m4Proj));

  if(boRes) m_m4Proj = m4Proj;
  return boRes;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                MATERIAL                                                            //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SetMaterial --------------------------------------------------------//
// modify current material
// in : new material
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRendererDX8::SetMaterial(const CEngineMaterial* pMaterial)
  {
  if(!m_pDevice) return false;
  if(!pMaterial) return false;

  // z buffer

  bool boZTest  = pMaterial->IsZEnabled();
  if(m_boZTest != boZTest)
    {
    m_pDevice->SetRenderState(D3DRS_ZFUNC,boZTest ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
    m_boZTest = boZTest;
    }

  // culling

  bool boCulling  = pMaterial->IsCulled();
  if(m_boCulling != boCulling)
    {
    m_pDevice->SetRenderState(D3DRS_CULLMODE,boCulling ? D3DCULL_CW : D3DCULL_NONE);
    m_boCulling = boCulling;
    }

  // lighting

  bool boLighted  = pMaterial->IsLighted();
  if(m_boLighted != boLighted)
    {
    if(boLighted)
      {
      m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
      m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
      }
    else
      {
      m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
      m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
      }

    m_pDevice->SetRenderState(D3DRS_LIGHTING,boLighted);
    m_boLighted = boLighted;
    }

  // texture

  bool boTexture  = (pMaterial->GetTexOp() != CEngineMaterial::NOP);

  if(m_boTexture != boTexture)
    {
    if(boTexture)
      {
      m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
      m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
      }
    else
      {
      m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
      m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
      m_pDevice->SetTexture          (0, NULL);
      }
    m_boTexture = boTexture;
    }

  if(boTexture)
    {
    CTextureDX8*       pTex    = DYNAMIC_CAST(CTextureDX8,pMaterial->GetTexture());
    LPDIRECT3DTEXTURE8 pTexDX8 = pTex ? pTex->GetInterface() : NULL;
    if(!pTexDX8)       pTexDX8 = m_pTexWhite;             // no texture => white
    m_pDevice->SetTexture(0, pTexDX8);
    }

  // diffuse color

  CVect4D  v4Diffuse  = pMaterial->GetDiffuse();
  D3DCOLOR d3dDiffuse = D3DCOLOR_ARGB(u8(255*v4Diffuse.a),u8(255*v4Diffuse.r),u8(255*v4Diffuse.g),u8(255*v4Diffuse.b));

  if(!m_boLighted && (m_colTexFactor != d3dDiffuse))
    {
    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,d3dDiffuse);
    m_colTexFactor = d3dDiffuse;
    return true;
    }

  m_matCurrent.Diffuse.r = v4Diffuse.r;
  m_matCurrent.Diffuse.g = v4Diffuse.g;
  m_matCurrent.Diffuse.b = v4Diffuse.b;
  m_matCurrent.Diffuse.a = v4Diffuse.a;

  m_matCurrent.Ambient.r = v4Diffuse.r;
  m_matCurrent.Ambient.g = v4Diffuse.g;
  m_matCurrent.Ambient.b = v4Diffuse.b;
  m_matCurrent.Ambient.a = v4Diffuse.a;

  return SUCCEEDED(m_pDevice->SetMaterial(&m_matCurrent));
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
