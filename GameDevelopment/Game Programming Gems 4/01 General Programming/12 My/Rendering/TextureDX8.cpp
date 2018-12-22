//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 TEXTURE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "TextureDX8.h"
#ifdef    _DEBUG
#include  "TextureDX8.inl"
#endif

#include  <d3dx8tex.h>

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CTextureDX8,CEngineTexture);

//--------------------------------------------------------------------------------------------------------------------//
//                                                DOWNLOAD/UNLOAD                                                     //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- DownloadInit -------------------------------------------------------//
// inits before DL
// in :
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTextureDX8::DownloadInit()
  {
  if(!m_pDevice)  return -1;
  if(m_boInVideo) return -6;

  // BGRA

  if(m_u32RGBAOrder != ORDER_BGRA)
    ConvertRAW2RAW(m_u32Format,ORDER_BGRA);

  // format

  D3DFORMAT dx8TexFormat = D3DFMT_UNKNOWN;

  switch(m_u32Format)
    {
    // no alpha => 0565 or 0888

    case TEXFORMAT_0565 :	dx8TexFormat = D3DFMT_R5G6B5;   break;
    case TEXFORMAT_0888 : dx8TexFormat = D3DFMT_R8G8B8;   break;

    // alpha    => 1555 or 4444 or 8888

    case TEXFORMAT_1555 : dx8TexFormat = D3DFMT_A1R5G5B5; break;
    case TEXFORMAT_4444 : dx8TexFormat = D3DFMT_A4R4G4B4; break;
    case TEXFORMAT_8888 : dx8TexFormat = D3DFMT_A8R8G8B8; break;
    }

  if(dx8TexFormat == D3DFMT_UNKNOWN) return -2;
  m_dx8TexFormat = dx8TexFormat;

  // create surface

  HRESULT hrErr;
  u32     u32MipMap = 1;

  hrErr = D3DXCreateTexture(m_pDevice,
                            m_u32Width,
                            m_u32Height,
                            u32MipMap,
                            0,                              // otherwise D3DUSAGE_RENDERTARGET
                            m_dx8TexFormat,
                            D3DPOOL_MANAGED,
                            &m_pTexInterface);
  if(FAILED(hrErr)) return -3;

  // check format of returned texture
  
    // TBI

  // copy to system

  LPDIRECT3DSURFACE8 pSurface = GetSurface();
  if(!pSurface) return -4;
  u32  u32Pitch = m_u32MemBytes / m_u32Height;
  RECT rectSrc  = { 0, 0, m_u32Width, m_u32Height };

  hrErr = D3DXLoadSurfaceFromMemory(pSurface,
                                    NULL,                   // palette
                                    NULL,                   // entire surface
                                    m_pu8RAM,
                                    m_dx8TexFormat,
                                    u32Pitch,
                                    NULL,
                                    &rectSrc,
                                    D3DX_FILTER_NONE,
                                    0);                     // color key
  pSurface->Release();
  if(FAILED(hrErr)) return -5;

  //

  m_boInVideo     = true;
  m_u32UsedVideo += m_u32MemBytes;
  return 0;
  }

//----------------------------------------------- Unload -------------------------------------------------------------//
// UL texture from video memory
// in :
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTextureDX8::Unload()
  {
  if(!m_pDevice)   return -1;
  if(!m_boInVideo) return -2;

  if(m_pTexInterface) m_pTexInterface->Release();
  m_pTexInterface = NULL;

  m_boInVideo     = false;
  m_u32UsedVideo -= m_u32MemBytes;
  return 0;
  }

//----------------------------------------------- Reload -------------------------------------------------------------//
// DL already created texture
// in :
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTextureDX8::Reload()
  {
  if(!m_pTexInterface) return -1;
  if(!m_boInVideo)     return -2;

  LPDIRECT3DSURFACE8 pSurface = GetSurface();
  if(!pSurface) return -4;

  u32  u32Pitch  = m_u32MemBytes / m_u32Height;
  RECT rectSrc   = { 0, 0, m_u32Width, m_u32Height };
  u32  u32MipMap = 1;

  // copy to system

  HRESULT hrErr = D3DXLoadSurfaceFromMemory(pSurface,
                                            NULL,           // palette
                                            NULL,           // entire surface
                                            m_pu8RAM,
                                            m_dx8TexFormat,
                                            u32Pitch,
                                            NULL,
                                            &rectSrc,
                                            D3DX_FILTER_POINT,
                                            0);             // color key
  pSurface->Release();
  if(FAILED(hrErr)) return -5;
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
